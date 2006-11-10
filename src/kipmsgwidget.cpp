/***************************************************************************
 *   Copyright (C) 2006 by 仁木 邦信                                       *
 *   nikikuni@yahoo.co.jp                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <qpixmap.h>
#include <qlabel.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>
#include <qtimer.h>
#include <qtextcodec.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qtooltip.h>
#include <kwin.h>
#include <kstandarddirs.h>
#include <kservice.h>
#include <krun.h>
#include <kaudioplayer.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <knotifyclient.h>
#include <time.h>
#include <stdlib.h>

#include "kipmsgwidget.h"
#include "senddialog.h"
#include "recievedialog.h"
#include "kipmsgconfig.h"
#include "absenceconfig.h"
#include "about.h"
#include "kipmsgsettings.h"
#include "kipmsglogger.h"
#include "openconfirm.h"

QPtrList<SendDialog> sendDialogs;
QPtrList<RecieveDialog> recieveDialogs;

#define POLLING_INTERVAL_MSEC 500

kipmsgWidget::kipmsgWidget(QWidget* parent, const char* name, WFlags fl)
        : kipmsgWidgetBase(parent,name,fl)
{
	IpMsgAgent = IpMessengerAgent::GetInstance();
    MainPopup = new KPopupMenu(this);
	AbsencePopup = new KPopupMenu(this);
	rebuildMenu();
	PollingTimer = new QTimer( this );
	connect( PollingTimer, SIGNAL( timeout() ), this, SLOT( slotPollingTimeout() ) );
	PollingTimer->start( POLLING_INTERVAL_MSEC, FALSE );
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
	IpMsgAgent->Login( codec->fromUnicode( KIpMsgSettings::userName() ).data(), codec->fromUnicode( KIpMsgSettings::groupName() ).data() );
	downMonitor = NULL;
	_OrigEnableIconImage = *m_EnabledIconLabel->pixmap();
	_OrigDisableIconImage = *m_DisabledIconLabel->pixmap();
	loadIcon();
	prevUserSize = 0;
	QToolTip::add( this, "KIpMsg(0)" );
}

kipmsgWidget::~kipmsgWidget()
{
	IpMessengerAgent::Release();
}

void kipmsgWidget::rebuildMenu()
{
	MainPopup->clear();
	MainPopup->insertTitle( SmallIcon("kipmsg"), "KIpMessenger" );
	MainPopup->insertItem( SmallIcon("kipmsg_download"), tr2i18n("Download Monitor"), this, SLOT( slotDownloadMonitorClicked( void ) ) );
	MainPopup->insertSeparator();
	MainPopup->insertItem( SmallIcon("window_suppressed"),tr2i18n("Hide all open confirm windows"), this, SLOT( slotHideAllOpenConfirmClicked( void ) ) );
	MainPopup->insertItem( SmallIcon("top"),tr2i18n("All windows stay on top"), this, SLOT( slotStayOnTopAllWindowsClick( void ) ) );
	MainPopup->insertSeparator();
	MainPopup->insertItem( SmallIcon("configure"), tr2i18n("Configuration..."), this, SLOT( slotConfigureClicked( void ) ) );
	MainPopup->insertItem( SmallIcon("kipmsg_about"), tr2i18n("About KIpMessenger"), this, SLOT( slotAboutClicked( void ) ) );
	MainPopup->insertItem( SmallIcon("view_text"),tr2i18n("View logs"), this, SLOT( slotViewLogClicked( void ) ) );
	MainPopup->insertSeparator();
	AbsencePopup->clear();
	QStringList keys = KIpMsgSettings::absenceKeys();
	QStringList encodings = KIpMsgSettings::absenceEncodings();
	QStringList titles = KIpMsgSettings::absenceTitles();
	QString curEnc = KIpMsgSettings::messageEncoding();
	absence_mode_menu.clear();
	QString modeName="";
	for( unsigned int i = 0; i < keys.count(); i++ ){
		if ( encodings[i] == curEnc ) {
			int menu_item = AbsencePopup->insertItem( keys[i] + ":" + titles[i], this, SLOT( slotAbsenceModeSelect( int ) ) );
			absence_mode_menu.insert( menu_item, new QString( keys[i] ) );
			if ( keys[i] == currentAbsenceMode ) {
				modeName = titles[i];
			}
		}
	}
	AbsencePopup->insertSeparator();

	absence_mode_config_menu_item = AbsencePopup->insertItem( SmallIcon("configure"),tr2i18n("Detail Configuration..."), this, SLOT( slotAbsenceModeConfigClicked( void ) ) );

	QPixmap iconImage;
	if ( KIpMsgSettings::absenceIconFileName() == "") {
		iconImage = SmallIcon("kipmsg_absence");
	} else {
		iconImage.load( KIpMsgSettings::absenceIconFileName() );
	}
	MainPopup->insertItem( iconImage, tr2i18n("Absense mode"), AbsencePopup );
	if ( IpMsgAgent->IsAbsence() ){
		MainPopup->insertSeparator();
		MainPopup->insertItem( tr2i18n("Reset absence mode") + "(" + modeName + ")", this, SLOT( slotResetAbsenceModeClicked( void ) ) );
	}
	MainPopup->insertSeparator();
	MainPopup->insertItem( SmallIcon( "exit"), tr2i18n("Quit"), this, SLOT( slotExitClicked( void ) ) );
}

void kipmsgWidget::slotAbsenceModeSelect( int menu_item )
{
	static int prev_menu = 0;
	if ( prev_menu == menu_item ) {
		prev_menu = 0;
		return;
	}
	if ( absence_mode_config_menu_item == menu_item ){
		slotAbsenceModeConfigClicked();
		return;
	}
	QString key = (*absence_mode_menu[menu_item]).data();
	QStringList keys = KIpMsgSettings::absenceKeys();
	QStringList encodings = KIpMsgSettings::absenceEncodings();
	QStringList titles = KIpMsgSettings::absenceTitles();
	QStringList details = KIpMsgSettings::absenceDetails();

	vector<AbsenceMode>absence;
	for( unsigned int i = 0; i < keys.count(); i++ ){
		if ( keys[i] == key && encodings[i] != "" ) {
			QTextCodec *codec = QTextCodec::codecForName( encodings[i] );
			AbsenceMode config;
			config.setAbsenceName( codec->fromUnicode( titles[i] ).data() );
			config.setAbsenceDescription( codec->fromUnicode( details[i] ).data() );
			config.setEncodingName( codec->fromUnicode( encodings[i] ).data() );
			absence.push_back( config );
		}
	}
	currentAbsenceMode=key;
	IpMsgAgent->SetAbsence( KIpMsgSettings::messageEncoding().data(), absence );
}

void kipmsgWidget::loadIcon()
{
	if ( KIpMsgSettings::iconFileName() == "" ) {
    	m_EnabledIconLabel->setPixmap( _OrigEnableIconImage );
	} else {
		QPixmap iconImage;
		iconImage.load( KIpMsgSettings::iconFileName() );
	    m_EnabledIconLabel->setPixmap( iconImage );
	}
	if ( KIpMsgSettings::absenceIconFileName() == "" ) {
    	m_DisabledIconLabel->setPixmap( _OrigDisableIconImage );
	} else {
		QPixmap iconImage;
		iconImage.load( KIpMsgSettings::absenceIconFileName() );
	    m_DisabledIconLabel->setPixmap( iconImage );
	}
}


void kipmsgWidget::mousePressEvent( QMouseEvent *e )
{
	if(e->button() == RightButton ){
		rebuildMenu();

		QSize size = MainPopup->sizeHint();
		KWin::WindowInfo i = KWin::windowInfo( winId(), NET::WMGeometry );
		QRect g = i.geometry();
		QRect screen = KGlobalSettings::desktopGeometry(g.center());

		if ( g.x()-screen.x() > screen.width()/2 &&
			 g.y()-screen.y() + size.height() > screen.height() ) {
			MainPopup->popup(QPoint( g.x(), g.y() - size.height()));
		} else {
			MainPopup->popup(QPoint( g.x() + width(), g.y() + height()));
		}
	}
	if(e->button() == LeftButton ){
		if ( isRecievedOnNonePopup() ) {
			if ( popupRecieve() ) {
				playSound();
				return;
			}
		}
		if ( KIpMsgSettings::openBySingleClick() ) {
			SendDialog *sendWin = new SendDialog();
			sendWin->show();
			sendDialogs.append( sendWin );
		}
	}
}

void kipmsgWidget::playSound()
{
	QString soundFile = KIpMsgSettings::recieveSoundFileName();
	if ( KIpMsgSettings::noSound() ) {
		return;
	}
	if ( soundFile == "" ) {
		KNotifyClient::beep();
	} else {
		KAudioPlayer::play( soundFile );
	}
}

/*$SPECIALIZATION$*/
void kipmsgWidget::mouseDoubleClickEvent (QMouseEvent *e)
{
	if ( !KIpMsgSettings::openBySingleClick() ) {
		SendDialog *sendWin = new SendDialog();
		sendWin->show();
		sendDialogs.append( sendWin );
	}
}
void kipmsgWidget::slotConfigureClicked()
{
	KIPMsgConfigDialog *configWin = new KIPMsgConfigDialog(this,0,TRUE);
	configWin->exec();
	loadIcon();
}

void kipmsgWidget::slotDownloadMonitorClicked()
{
	if ( downMonitor == NULL ) {
		downMonitor = new KIpMsgDownloadMonitor();
	}
	downMonitor->show();
}
void kipmsgWidget::slotHideAllOpenConfirmClicked()
{
	QPtrListIterator<OpenConfirmDialog> itp(confirmDialogs);
	OpenConfirmDialog *dlg;
	while( ( dlg = itp.current() ) != 0 ) {
		if ( dlg->isShown() ) {
			dlg->setHidden(TRUE);
		}
		++itp;
	}
	while( confirmDialogs.count() > 0 ){
		confirmDialogs.remove( 0U );
	}
}
void kipmsgWidget::slotStayOnTopAllWindowsClick()
{
	QPtrListIterator<OpenConfirmDialog> confirmIt(confirmDialogs);
	OpenConfirmDialog *confirmDlg;
	while( ( confirmDlg = confirmIt.current() ) != 0 ) {
		if ( confirmDlg->isShown() ) {
			KWin::activateWindow( confirmDlg->winId() );
		}
		++confirmIt;
	}
	QPtrListIterator<RecieveDialog> recieveIt(recieveDialogs);
	RecieveDialog *recieveDlg;
	while( ( recieveDlg = recieveIt.current() ) != 0 ) {
		if ( recieveDlg->isShown() ) {
			KWin::activateWindow( recieveDlg->winId() );
		}
		++recieveIt;
	}
	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( sendDlg->isShown() ) {
			KWin::activateWindow( sendDlg->winId() );
		}
		++sendIt;
	}
}
void kipmsgWidget::slotAbsenceModeConfigClicked()
{
	KIpMsgAbsenceModeConfigDialog *absencewin = new KIpMsgAbsenceModeConfigDialog(this,0,TRUE);
	absencewin->show();
	rebuildMenu();
}

void kipmsgWidget::slotAboutClicked()
{
	KIpMessengerAboutDialog *aboutWin = new KIpMessengerAboutDialog();
	aboutWin->show();
}
void kipmsgWidget::slotViewLogClicked()
{
	KRun::run( "kwrite", QStringList( KIpMsgSettings::logFileName()));
}
void kipmsgWidget::slotExitClicked()
{
	IpMsgAgent->Logout();
	PollingTimer->stop();
	close();
	exit(0);
}

void kipmsgWidget::slotResetAbsenceModeClicked()
{
	IpMsgAgent->ResetAbsence();
	currentAbsenceMode="";
}

bool kipmsgWidget::isRecievedOnNonePopup()
{
	return ( IpMsgAgent->IsAbsence() && KIpMsgSettings::nonePopupOnAbsence() ) || KIpMsgSettings::noPopup();
}

bool kipmsgWidget::popupRecieve()
{
	int hasMsg = 0;
	while( IpMsgAgent->GetRecievedMessageCount() > 0 ){
		RecieveDialog *recv = new RecieveDialog();
		RecievedMessage msg = IpMsgAgent->PopRecievedMessage();
		recv->setMessage( msg );
		recv->setDownloadFiles();
		recv->show();
		if ( !KIpMsgSettings::recordAfterUnlock() ){
			KIpMessengerLogger::GetInstance()->PutRecivedMessage( msg );
		}
		KWin::activateWindow( recv->winId() );
		recieveDialogs.append( recv );
		hasMsg++;
	}
	return hasMsg > 0;
}

void kipmsgWidget::slotPollingTimeout()
{
	static long c = 0;
	static long moved_c = 0;

	//送受信キューを処理します。
	IpMsgAgent->Process();

	//ポップアップ受信
	if ( !isRecievedOnNonePopup() ) {
		if ( popupRecieve() ) {
			playSound();
		}
	}
	//開封を表示。
	time_t t = time( NULL );
	char timebuf[30];
	ctime_r( &t, timebuf );
	timebuf[strlen(timebuf) - 1] = 0;
	vector<SentMessage> *msg = IpMsgAgent->GetSentMessages();
	for(vector<SentMessage>::iterator it = msg->begin(); it != msg->end(); it++ ){
		if ( it->IsSecret() && it->IsConfirmed() && !it->IsConfirmAnswered() ) {
			QString encode = "";
			if ( it->Host().EncodingName() == "" ) {
				encode = it->Host().EncodingName().c_str();
			}else {
				encode = KIpMsgSettings::messageEncoding();
			}
			QStringList encodings = KIpMsgSettings::encodingSettings();
			QString IpAddr = it->Host().IpAddress().c_str();
			QString UserName = it->Host().UserName().c_str();
			for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
				QStringList fields = QStringList::split( ":", *ite );
				if ( IpAddr == fields[0] && UserName == fields[1] ) {
					encode = fields[2];
					break;
				}
			}
			QTextCodec *codec = QTextCodec::codecForName( encode );
			OpenConfirmDialog *opendlg = new OpenConfirmDialog();
			IpMsgAgent->AcceptConfirmNotify( *it );
			opendlg->m_NicknameLabel->setText( codec->toUnicode( it->Host().Nickname().c_str() ) );
			opendlg->m_TimeLabel->setText( "(" + QString(timebuf) + ")" );
			opendlg->show();
			if ( KIpMsgSettings::confirmIcon() ) {
				KWin::iconifyWindow( opendlg->winId() );
			} else {
				KWin::activateWindow( opendlg->winId() );
			}
			confirmDialogs.append(opendlg);
		}
	}
	//メッセージが到着している
	if ( IpMsgAgent->GetRecievedMessageCount() > 0 ) {
		//点滅させる。
		if ( m_EnabledIconLabel->isHidden() ) {
			m_EnabledIconLabel->setHidden( FALSE );
			m_DisabledIconLabel->setHidden( TRUE );
		} else {
			m_EnabledIconLabel->setHidden( TRUE );
			m_DisabledIconLabel->setHidden( FALSE );
		}
	} else {
		//不在時のアイコン切替え
		if ( IpMsgAgent->IsAbsence() ) {
			m_EnabledIconLabel->setHidden( TRUE );
			m_DisabledIconLabel->setHidden( FALSE );
		} else {
			m_EnabledIconLabel->setHidden( FALSE );
			m_DisabledIconLabel->setHidden( TRUE );
		}
	}
	//ダウンロードモニタが表示中ならファイル一覧をリフレッシュ
	if ( downMonitor != NULL && downMonitor->isShown() ) {
		downMonitor->refreshDownloadFileList();
	}

	//設定された分数をこえたら自動的に不在モードに移行。
	c++;
	static bool automatic_absence = false;
	static QPoint prevpos = QCursor::pos();
	QPoint cpos = QCursor::pos();
	if ( cpos.x() == prevpos.x() && cpos.y() == prevpos.y() ) {
		//cの1は0.5秒なので2倍すると1秒。120で1分。
		if ( c - moved_c > 120 * KIpMsgSettings::autoAbsenceMinutes() ) {
			QIntDictIterator<QString> idt( absence_mode_menu );
			if ( absence_mode_menu.count() > 0 && !automatic_absence ) {
				slotAbsenceModeSelect( idt.currentKey() );
				automatic_absence = true;
			}
		}
	} else {
		moved_c = c;
		prevpos = cpos;
		if ( automatic_absence ){
			automatic_absence = false;
			IpMsgAgent->ResetAbsence();
		}
	}
	//表示されていない開封をリストから後始末。
	QPtrListIterator<OpenConfirmDialog> confirmIt(confirmDialogs);
	OpenConfirmDialog *confirmDlg;
	while( ( confirmDlg = confirmIt.current() ) != 0 ) {
		if ( !confirmDlg->isShown() ) {
			confirmDialogs.remove(confirmDlg);
			continue;
		}
		++confirmIt;
	}
	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( !sendDlg->isShown() ) {
			sendDialogs.remove(sendDlg);
			continue;
		}
		++sendIt;
	}
	QPtrListIterator<RecieveDialog> recieveIt(recieveDialogs);
	RecieveDialog *recieveDlg;
	while( ( recieveDlg = recieveIt.current() ) != 0 ) {
		if ( !recieveDlg->isShown() ) {
			recieveDialogs.remove(recieveDlg);
			continue;
		}
		++recieveIt;
	}
	int userSize = IpMsgAgent->GetHostList().size();
	if ( userSize != prevUserSize ) {
		QToolTip::remove( this );
		QToolTip::add( this, QString("KIpMsg(%1)").arg( userSize ) );
	}
	prevUserSize = userSize;
}

#include "kipmsgwidget.moc"
