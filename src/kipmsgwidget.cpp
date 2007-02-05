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
#include <kprocess.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <knotifyclient.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

#include "kipmsgwidget.h"
#include "senddialog.h"
#include "recievedialog.h"
#include "kipmsgconfig.h"
#include "absenceconfig.h"
#include "about.h"
#include "kipmsgsettings.h"
#include "kipmsglogger.h"
#include "kipmsgevent.h"
#include "openconfirm.h"
#include "networkconfig.h"

#define POLLING_INTERVAL_MSEC 500

/**
 * IP Messnger for KDE用のソート比較オブジェクト
 * ・設定によりソート順序を操作する。
 */
class KIpMsgHostListComparator: public HostListComparator{
	public:
		virtual int compare( vector<HostListItem>::iterator host1, vector<HostListItem>::iterator host2 ){
			int ret = 0;
			if ( KIpMsgSettings::sortUserName() ){
				ret = strCompare( host1->Nickname().c_str(), host2->Nickname().c_str() );
			} else if ( KIpMsgSettings::sortIpAddress() ) {
				ret = ipCompare( host1->IpAddress().c_str(), host2->IpAddress().c_str() );
			} else if ( KIpMsgSettings::sortHostName() ) {
				ret = strCompare( host1->HostName().c_str(), host2->HostName().c_str() );
			}
			if ( KIpMsgSettings::sort2ndGroupingDesc() ) {
				ret *= -1;
			}
			if ( KIpMsgSettings::sortGroupName() ){
				int retGroup = strCompare( host1->GroupName().c_str(), host2->GroupName().c_str() );
				//反転設定の場合は評価を逆転する
				if ( KIpMsgSettings::sort1stGroupingDesc() ){
					retGroup *= -1;
				}
				if ( retGroup != 0 ) {
					ret = retGroup;
				}
			}
			return ret;
		};
	private:
		/**
		 * IPアドレスの比較
		 * @param s1 IPアドレスの文字列表現1
		 * @param s2 IPアドレスの文字列表現2
		 * @retval 0:同じ、1:s1が大きい、-1:s2が大きい。
		 */
		int ipCompare( const char *s1, const char *s2 ){
			in_addr_t hip1 = inet_addr( s1 );
			in_addr_t hip2 = inet_addr( s2 );
			if ( hip1 == hip2 ){
				return 0;
			} else if ( hip1 < hip2 ){
				return -1;
			} else {
				return 1;
			}
			return 0;
		}
		/**
		 * 文字列の比較
		 * @param s1 文字列1
		 * @param s2 文字列2
		 * @retval 0:同じ、1:s1が大きい、-1:s2が大きい。
		 */
		int strCompare( const char *s1, const char *s2 ){
			char *p1 = (char *)(s1);
			char *p2 = (char *)(s2);

			if ( *p1 == '\0' && *p2 != '\0' ){
				return -1;
			}
			if ( *p1 != '\0' && *p2 == '\0' ){
				return 1;
			}
			if ( !KIpMsgSettings::priorityMultiBytes() && !KIpMsgSettings::ignoreCaseAsSingleByte() ){
				return strcmp( s1, s2 );
			} else if ( !KIpMsgSettings::priorityMultiBytes() && KIpMsgSettings::ignoreCaseAsSingleByte() ){
				return strcasecmp( s1, s2 );
			} else if ( KIpMsgSettings::priorityMultiBytes() && !KIpMsgSettings::ignoreCaseAsSingleByte() ){
				while( *p1 != '\0' && *p2 != '\0' ) {
					//おそらくp1がマルチバイト文字(ダメなやりかた)
					if ( !isprint( *p1 ) && isprint( *p2 ) ) {
						return -1;
					//おそらくp2がマルチバイト文字(ダメなやりかた)
					} else if ( isprint( *p1 ) && !isprint( *p2 ) ) {
						return 1;
					}
					//両方シングルバイト、両方マルチバイトならそのまま比較を続ける。
					if ( *p1 < *p2 ) {
						return -1;
					} else if ( *p1 < *p2 ) {
						return 1;
					}
					p1++;
					p2++;
				}
			} else if ( KIpMsgSettings::priorityMultiBytes() && KIpMsgSettings::ignoreCaseAsSingleByte() ){
				while( *p1 != '\0' && *p2 != '\0' ){
					//おそらくp1がマルチバイト文字(ダメなやりかた)
					if ( !isprint( *p1 ) && isprint( *p2 ) ) {
						return -1;
					//おそらくp2がマルチバイト文字(ダメなやりかた)
					} else if ( isprint( *p1 ) && !isprint( *p2 ) ) {
						return 1;
					}
					//両方シングルバイト、両方マルチバイトならそのまま比較を続ける。
					if ( toupper( *p1 ) < toupper( *p2 ) ) {
						return -1;
					} else if ( toupper( *p1 ) < toupper( *p2 ) ) {
						return 1;
					}
					p1++;
					p2++;
				}
			}
			return 0;
		};
};

/**
 * コンストラクタ
 * ・IPメッセンジャーエージェントの設定
 * ・設定に応じた初期化（アプリ）
 * ・メニューの生成
 * ・タイマの開始
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
kipmsgWidget::kipmsgWidget(QWidget* parent, const char* name, WFlags fl)
        : kipmsgWidgetBase(parent,name,fl)
{
	IpMsgAgent = IpMessengerAgent::GetInstance();
	IpMsgAgent->SetEventObject( new KIpMsgEvent() );
	IpMsgAgent->SetSortHostListComparator( new KIpMsgHostListComparator() );
	IpMsgAgent->setAbortDownloadAtFileChanged( KIpMsgSettings::notPermitedIfModified() );
	IpMsgAgent->setIsDialup( KIpMsgSettings::connectDialup() );
	IpMsgAgent->setSaveRecievedMessage( false );

	QStringList broadcastNetworkAddress = KIpMsgSettings::broadcastNetworkAddress();
	for( QStringList::iterator it = broadcastNetworkAddress.begin(); it != broadcastNetworkAddress.end(); it++){
		QString broadcastAddress = *it;
		IpMsgAgent->AddBroadcastAddress( broadcastAddress.data() );
	}

	IpMsgAgent->StartNetwork( NetworkConfig::getSpecifyNics() );

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

/**
 * デストラクタ
 * ・IPメッセンジャーエージェントの解放
 */
kipmsgWidget::~kipmsgWidget()
{
	IpMessengerAgent::Release();
}

/**
 * メニューの再構築
 * ・不在などの状態に応じたメニューを再構築する。
 */
void kipmsgWidget::rebuildMenu()
{
	MainPopup->clear();
	MainPopup->insertTitle( SmallIcon("kipmsg"), "KIpMessenger" );
	MainPopup->insertItem( SmallIcon("kipmsg_download"), tr2i18n("Download Monitor"), this, SLOT( slotDownloadMonitorClicked( void ) ) );
	MainPopup->insertSeparator();
	MainPopup->insertItem( SmallIcon("window_suppressed"),tr2i18n("Hide all open confirm windows"), this, SLOT( slotHideAllOpenConfirmClicked( void ) ) );
	MainPopup->insertItem( SmallIcon("top"),tr2i18n("All windows stay on top"), this, SLOT( slotStayOnTopAllWindowsClick( void ) ) );
	MainPopup->insertSeparator();
	MainPopup->insertItem( SmallIcon("filenew"), tr2i18n("New message..."), this, SLOT( slotNewMessageClicked( void ) ) );
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
	for( int i = (int)titles.count() - 1; i >= 0 ; i-- ){
		QStringList item = QStringList::split( "\a", titles[i] );
		if ( item[1] == curEnc ) {
			int menu_item = AbsencePopup->insertItem( item[2] == NULL ? item[0] : item[2], this, SLOT( slotAbsenceModeSelect( int ) ) );
			absence_mode_menu.insert( menu_item, new QString( item[0] ) );
			if ( item[0] == currentAbsenceMode ) {
				modeName = item[2];
			}
		}
	}
	AbsencePopup->insertSeparator();

	absence_mode_config_menu_item = AbsencePopup->insertItem( SmallIcon("configure"),tr2i18n("Detail Configuration..."), this, SLOT( slotAbsenceModeConfigClicked( void ) ) );

	QPixmap iconImage;
	if ( KIpMsgSettings::absenceIconFileName() == "") {
		iconImage = SmallIcon("kipmsg_rev");
	} else {
		iconImage.load( KIpMsgSettings::absenceIconFileName() );
	}
	MainPopup->insertItem( iconImage, tr2i18n("Absence mode"), AbsencePopup );
	if ( IpMsgAgent->IsAbsence() ){
		MainPopup->insertSeparator();
		MainPopup->insertItem( tr2i18n("Reset absence mode") + "(" + modeName + ")", this, SLOT( slotResetAbsenceModeClicked( void ) ) );
	}
	MainPopup->insertSeparator();
	MainPopup->insertItem( SmallIcon( "exit"), tr2i18n("Quit"), this, SLOT( slotExitClicked( void ) ) );
}

/**
 * 不在モード選択
 * ・キーから不在モードを取得し、不在モードに遷移。
 * @param menu_item メニュー項目
 */
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
	QStringList titles = KIpMsgSettings::absenceTitles();
	QStringList details = KIpMsgSettings::absenceDetails();

	vector<AbsenceMode>absence;
	for( unsigned int i = 0; i < titles.count(); i++ ){
		QStringList item = QStringList::split( "\a", titles[i] );
		QStringList ditem = QStringList::split( "\a", details[i] );
		if ( item[0] == key && item[1] != "" ) {
			QTextCodec *codec = QTextCodec::codecForName( item[1] );
			AbsenceMode config;
			config.setEncodingName( codec->fromUnicode( item[1] ).data() );
			config.setAbsenceName( item[2] == NULL ? "" : codec->fromUnicode( item[2] ).data() );
			config.setAbsenceDescription( ditem[2] == NULL ? "" : codec->fromUnicode( ditem[2] ).data() );
			absence.push_back( config );
		}
	}
	currentAbsenceMode=key;
	IpMsgAgent->SetAbsence( KIpMsgSettings::messageEncoding().data(), absence );
}

/**
 * アイコンロード
 * ・アイコンと不在アイコンをロードしラベルに設定
 */
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

/**
 * マウス押下イベント
 * ・右ボタンならメインメニューをポップアップ
 * ・左ボタンならシングルクリックで送信画面表示の設定であれば、送信ダイアログ表示。
 * ・また、不在時にノンポップアップ受信していたメッセージを一括表示。
 * @param e マウスイベント
 */
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
		if ( kipmsgWidget::isRecievedOnNonePopup() ) {
			KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMsgAgent->GetEventObject());
			if ( evt != NULL && evt->GetRecievedMessageCount() > 0 ) {
				evt->ShowHiddenRecieveMsg();
				return;
			}
		}
		if ( KIpMsgSettings::openBySingleClick() ) {
			KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMsgAgent->GetEventObject());
			if ( evt != NULL ) {
				evt->ShowSendDlg();
			}
		}
	}
}

/**
 * 受信音再生
 * ・設定されているファイルがあればサウンドファイルから再生。
 * ・以外はシステムビープ
 */
void kipmsgWidget::playSound()
{
	QString soundFile = KIpMsgSettings::recieveSoundFileName();
	if ( KIpMsgSettings::noSound() ) {
		return;
	}
	if ( soundFile == "" ) {
		KNotifyClient::beep();
	} else {
		KProcess recv;
		recv << "artsplay";
		recv << soundFile;
		recv.start(KProcess::DontCare);
	}
}

/*$SPECIALIZATION$*/
/**
 * マウスダブルクリックイベント
 * ・送信画面表示の設定でなければ、送信ダイアログ表示。
 * @param e マウスイベント
 */
void kipmsgWidget::mouseDoubleClickEvent (QMouseEvent * /*e*/)
{
	if ( !KIpMsgSettings::openBySingleClick() ) {
		KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMsgAgent->GetEventObject());
		if ( evt != NULL ) {
			evt->ShowSendDlg();
		}
	}
}

/**
 * 新しいメッセージメニュークリックイベント
 * ・送信ダイアログを開く。
 */
void kipmsgWidget::slotNewMessageClicked()
{
	KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMsgAgent->GetEventObject());
	if ( evt != NULL ) {
		evt->ShowSendDlg();
	}
}

/**
 * 設定メニュークリックイベント
 * ・設定ダイアログを表示。
 * ・アイコンを再ロード。
 */
void kipmsgWidget::slotConfigureClicked()
{
	KIPMsgConfigDialog *configWin = new KIPMsgConfigDialog(this,0,TRUE);
	configWin->exec();
	loadIcon();
}

/**
 * ダウンロードモニターメニュークリックイベント
 * ・ダウンロードモニターが未表示ならダウンロードモニターを表示。
 */
void kipmsgWidget::slotDownloadMonitorClicked()
{
	if ( downMonitor == NULL ) {
		downMonitor = new KIpMsgDownloadMonitor();
	}
	downMonitor->show();
}

/**
 * 全ての開封を消去メニュークリックイベント
 * ・イベントオブジェクトの全ての開封を消去メソッドを実行
 */
void kipmsgWidget::slotHideAllOpenConfirmClicked()
{
	KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMsgAgent->GetEventObject());
	if ( evt != NULL ) {
		evt->HideAllOpenConfirm();
	}
}

/**
 * 全てのウインドウを前面にメニュークリックイベント
 * ・イベントオブジェクトの全てのウインドウを前面にメソッドを実行
 */
void kipmsgWidget::slotStayOnTopAllWindowsClick()
{
	KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMsgAgent->GetEventObject());
	if ( evt != NULL ) {
		evt->StayOnTopAllWindows();
	}
}

/**
 * 不在設定メニュークリックイベント
 * ・不在設定ダイアログを表示
 */
void kipmsgWidget::slotAbsenceModeConfigClicked()
{
	KIpMsgAbsenceModeConfigDialog *absencewin = new KIpMsgAbsenceModeConfigDialog(this,0,TRUE);
	absencewin->show();
	rebuildMenu();
}

/**
 * KIpMessengerについてメニュークリックイベント
 * ・アバウトダイアログを表示
 */
void kipmsgWidget::slotAboutClicked()
{
	KIpMessengerAboutDialog *aboutWin = new KIpMessengerAboutDialog();
	aboutWin->show();
}

/**
 * ログ参照メニュークリックイベント
 * ・kwriteでログを表示
 */
void kipmsgWidget::slotViewLogClicked()
{
	if ( KIpMsgSettings::useExternalLogViewer() ) {
		KRun::run( KIpMsgSettings::logViewerPath() + " " + KIpMsgSettings::logViewerOption(), QStringList( KIpMsgSettings::logFileName() ) );
	} else {
		KRun::run( "kwrite --encoding utf8", QStringList( KIpMsgSettings::logFileName() ) );
	}
}

/**
 * 終了メニュークリックイベント
 * ・アプリケーションを終了する。
 */
void kipmsgWidget::slotExitClicked()
{
	IpMsgAgent->Logout();
	PollingTimer->stop();
	close();
	exit(0);
}

/**
 * 不在解除メニュークリックイベント
 * ・不在を解除する。
 */
void kipmsgWidget::slotResetAbsenceModeClicked()
{
	IpMsgAgent->ResetAbsence();
	currentAbsenceMode="";
}

/**
 * 不在解除メニュークリックイベント
 * ・不在を解除する。
 */
bool kipmsgWidget::isRecievedOnNonePopup()
{
	return ( IpMessengerAgent::GetInstance()->IsAbsence() && KIpMsgSettings::nonePopupOnAbsence() ) || KIpMsgSettings::noPopup();
}


/**
 * タイマーイベント
 * ・パケット受信。
 * ・アイコン点滅／不在に変更。
 * ・ダウンロードモニタのファイル一覧を更新。
 * ・タイムオーバーなら自動不在への移行。
 * ・参加者数の更新。
 */
void kipmsgWidget::slotPollingTimeout()
{
	static long c = 0;
	static long moved_c = 0;

	//送受信キューを処理します。
	IpMsgAgent->Process();

	//メッセージが到着している
	KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMsgAgent->GetEventObject());
	if ( evt != NULL && evt->GetRecievedMessageCount() > 0 ) {
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
	if ( evt != NULL ) {
		evt->TimerEvent();
	}

	int userSize = IpMsgAgent->GetHostList().size();
	if ( userSize != prevUserSize ) {
		QToolTip::remove( this );
		QToolTip::add( this, QString("KIpMsg(%1)").arg( userSize ) );
	}
	prevUserSize = userSize;
}

#include "kipmsgwidget.moc"
