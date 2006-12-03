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


//#include <sys/types.h>
//#include <sys/stat.h>

#include <qlabel.h>
#include <qrect.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qtextcodec.h>
#include <qstringlist.h>
#include <qgroupbox.h>
#include <qfileinfo.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <krun.h>
#include <kiconloader.h>

#include "senddialog.h"
#include "passworddialog.h"
#include "recievedialog.h"
#include "downloadcompletedialog.h"
#include "downloaderrordialog.h"
#include "kipmsgsettings.h"
#include "kipmsglogger.h"

extern QPtrList<RecieveDialog> recieveDialogs;

RecieveDialog::RecieveDialog(QWidget* parent, const char* name, WFlags fl)
        : RecieveDialogBase(parent,name,fl)
{
	defaultX = x();
	defaultY = y();
	defaultWidth = width();
	defaultHeight = height();

    RecvPopup = new KPopupMenu(this);
	SizePopup = new KPopupMenu(this);
	save_size_menu_item = SizePopup->insertItem(SmallIcon("fileexport"),tr2i18n("Save Size"), this, SLOT( slotSaveSizeClicked( void ) ) );
	SizePopup->insertItem(SmallIcon("undo"),tr2i18n("Restore default(temporary)"), this, SLOT( slotRestoreSizeTempClicked( void ) ) );
	RecvPopup->insertItem(SmallIcon("configure"),tr2i18n("Size Configuration"), SizePopup );

	SizePopup->setItemChecked( save_size_menu_item, KIpMsgSettings::recieveDialogSaveSize() );

	fixize_pos_menu_item = RecvPopup->insertItem(tr2i18n("Fixize Position"), this, SLOT( slotFixizePositionClicked( void ) ) );

	RecvPopup->setItemChecked( fixize_pos_menu_item, KIpMsgSettings::recieveDialogFixizePosition() );

    m_RecievedMessageHTMLPart = new KHTMLPart( m_RecvAreaFrame, "m_RecievedHTMLPart" );
    m_RecievedMessageHTMLPart->view()->setGeometry( QRect( 10, 54, 216, 100 ) );
    m_RecievedMessageHTMLPart->view()->setMinimumSize( QSize( 0, 100 ) );
    m_RecievedMessageHTMLPart->view()->setMarginWidth( 0 );
    m_RecievedMessageHTMLPart->view()->setMarginHeight( 0 );
    connect( m_RecievedMessageHTMLPart->browserExtension(),
			 SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ), this,
			 SLOT( slotOpenURL( const KURL & ) ) );
//    m_RecievedMessageHTMLPart->view()->resize( 216 - 10, 100 - 54 );
//    m_RecievedMessageHTMLPart->setGeometry( QRect( 10, 54, 216, 100 ) );
//    m_RecievedMessageHTMLPart->setMinimumSize( QSize( 0, 100 ) );


	m_DownloadButton->setHidden( TRUE );
//	m_RecievedMessageEditbox->setHidden( TRUE );
    m_RecievedMessageHTMLPart->view()->setHidden( TRUE );

	if ( KIpMsgSettings::fireIntercept()  ) {
		m_ReplyButton->setText(tr2i18n( "Intercept") );
	}
	m_QuoteCheckbox->setChecked( KIpMsgSettings::quoteDefault() );


	if ( KIpMsgSettings::recieveDialogFixizePosition() ) {
		move( QPoint( KIpMsgSettings::recieveDialogLeft(), KIpMsgSettings::recieveDialogTop() ) );
	}else{
		move( QPoint( defaultX, defaultY ) );
	}
    if ( KIpMsgSettings::recieveDialogSaveSize() ) {
		resize( KIpMsgSettings::recieveDialogWidth(), KIpMsgSettings::recieveDialogHeight() );
//		setMinimumSize( KIpMsgSettings::recieveDialogWidth(), KIpMsgSettings::recieveDialogHeight() );
	}else{
		resize( defaultWidth, defaultHeight );
	}
}

RecieveDialog::~RecieveDialog()
{}

/*$SPECIALIZATION$*/

void RecieveDialog::synchronizeMenu()
{
	QPtrListIterator<RecieveDialog> recieveIt(recieveDialogs);
	RecieveDialog *recieveDlg;
	while( ( recieveDlg = recieveIt.current() ) != 0 ) {
		if ( recieveDlg->isShown() && recieveDlg->winId() != winId() ) {
			recieveDlg->setFixsizePotisionMenu();
			recieveDlg->setSaveSizeMenu();
		}
		++recieveIt;
	}
}
void RecieveDialog::setSaveSizeMenu()
{
	SizePopup->setItemChecked( save_size_menu_item, KIpMsgSettings::recieveDialogSaveSize() );
}
/*
 * サイズ保存。
 */
void RecieveDialog::slotSaveSizeClicked()
{
	KIpMsgSettings::setRecieveDialogSaveSize( !KIpMsgSettings::recieveDialogSaveSize() );
	setSaveSizeMenu();
	KIpMsgSettings::setRecieveDialogHeight( width() );
	KIpMsgSettings::setRecieveDialogWidth( height() );
	KIpMsgSettings::writeConfig();
	synchronizeMenu();
}

/*
 * サイズを一時的に戻す。
 */
void RecieveDialog::slotRestoreSizeTempClicked()
{
	resize( defaultWidth, defaultHeight );
}

void RecieveDialog::setFixsizePotisionMenu()
{
	RecvPopup->setItemChecked( fixize_pos_menu_item, KIpMsgSettings::recieveDialogFixizePosition() );
}

/*
 * 位置固定。
 */
void RecieveDialog::slotFixizePositionClicked()
{
	KIpMsgSettings::setRecieveDialogFixizePosition( !KIpMsgSettings::recieveDialogFixizePosition() );
	setFixsizePotisionMenu();
	KIpMsgSettings::setRecieveDialogTop( x() );
	KIpMsgSettings::setRecieveDialogLeft( y() );
	KIpMsgSettings::writeConfig();
	synchronizeMenu();
}

void RecieveDialog::slotOpenURL( const KURL &url ){
//	printf("URL on [%s]\n", url.url().data() );
//	fflush(stdout);
	/*
    bool mUseDefaultBrowser;
    QStringList mExecutePrograms;
    bool mExecuteDoubleClick;
	+*/
	QMap<QString,QString> protocols;
	QStringList pro = KIpMsgSettings::executePrograms();

	for( QStringList::iterator p = pro.begin(); p != pro.end(); p++ ){
		QString item = *p;
		int pos = item.find(':');
		if ( pos < 0 ) {
			protocols[item] = "";
		} else {
			QString key = item.left( pos );
			QString val = item.mid( pos + 1 );
			val.stripWhiteSpace();
			protocols[key] = val;
//printf("for[%s][%s]\n", key.data(), val.data());
//fflush(stdout);
		}
	}
	if ( protocols.contains( url.protocol().upper() ) && protocols[url.protocol().upper()].isEmpty() ) {
		if ( KIpMsgSettings::useDefaultBrowser() ) {
//printf("KRun\n");
fflush(stdout);
			( new KRun( url ) )->setAutoDelete( true );
		}
	} else {
		QFileInfo exe = QFileInfo( protocols[url.protocol().upper()] );
		if ( !exe.exists() ){
			KMessageBox::sorry( 0, tr2i18n( "Specified application was not found.\n" ) + "[" + exe.fileName() + "]", "KIpMessenger" );
		} else if ( !exe.isExecutable() ){
			KMessageBox::sorry( 0, tr2i18n( "Specified application is not executable.\n" ) + "[" + exe.fileName() + "]", "KIpMessenger" );
		} else {
			QString cmd = protocols[url.protocol().upper()] + " \"" + url.url() + "\"";
//printf("RunCommand[%s]\n", cmd.data());
fflush(stdout);
			KRun::runCommand( cmd );
		}
	}
}


void RecieveDialog::slotDialogCloseClicked()
{
	close();
}

void RecieveDialog::slotMessageOpenClicked()
{
	if ( msg.IsPasswordLock() ) {
		KIpMessengerPasswordDialog *password = new KIpMessengerPasswordDialog(this,0,TRUE);
		if ( password->exec() != QDialog::Accepted ){
			return;
		}
		if ( KIpMsgSettings::recordAfterUnlock() ){
			KIpMessengerLogger::GetInstance()->PutRecivedMessage( msg );
		}
	}
	if ( msg.HasAttachFile() ) {
		m_DownloadButton->setHidden( FALSE );
		//ファイル名設定
		setDownloadFiles();
	} else {
		m_DownloadButton->setHidden( TRUE );
	}
	m_OpenButton->setHidden( TRUE );
//	m_RecievedMessageEditbox->setHidden( FALSE );
    m_RecievedMessageHTMLPart->view()->setHidden( FALSE );
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	agent->ConfirmMessage( msg );
}

void RecieveDialog::slotEncodingChange( int index )
{
	/* エンコーディングを変更したのでメッセージを再表示 */
	QTextCodec *codec = QTextCodec::codecForName( m_EncodingCombobox->text( index ) );
//	m_RecievedMessageEditbox->setText( codec->toUnicode( msg.Message().c_str() ) );
	renderMessage( codec->toUnicode( msg.Message().c_str() ) );
	/* エンコーディングを変更したので設定保存 */
	QStringList encodings = KIpMsgSettings::encodingSettings();
	QString IpAddr = msg.Host().IpAddress().c_str();
	QString UserName = msg.Host().UserName().c_str();
	for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
		QStringList fields = QStringList::split( ":", *ite );
		if ( IpAddr == fields[0] && UserName == fields[1] ) {
			encodings.remove( ite );
			break;
		}
	}
	encodings << IpAddr + ":" + UserName + ":" + m_EncodingCombobox->text( index );
	KIpMessengerLogger::GetInstance()->PutRecivedMessage( msg, TRUE );
}

void RecieveDialog::slotReplyClicked()
{
    SendDialog *send = new SendDialog();
	if ( m_QuoteCheckbox->isChecked() ) {
//		QStringList RecvMsg = QStringList::split("\n", m_RecievedMessageEditbox->text() );
		if ( !m_RecievedMessageHTMLPart->hasSelection() ) {
			m_RecievedMessageHTMLPart->selectAll();
		}
		QStringList RecvMsg = QStringList::split("\n", m_RecievedMessageHTMLPart->selectedText() );
		QString quoteStr;
		QString quoteChar = KIpMsgSettings::quoteChar();
		for( QStringList::iterator quote = RecvMsg.begin(); quote != RecvMsg.end(); quote++) {
			quoteStr.append( quoteChar );
			quoteStr.append(  " " );
			QString quotePart = *quote;
			for( unsigned int i=0; i < quotePart.length();i++){
				//&nbsp;なら空白に
				if ( quotePart.at( i ).latin1() == 0xffffffa0 ) {
					quoteStr.append( " " );
				} else {
					quoteStr.append( quotePart.at( i ) );
				}
			}
			quoteStr.append( "\n" );
		}
		send->setMessageText( quoteStr );
	}
	QString IpAddr = msg.Host().IpAddress().c_str();
	QListViewItemIterator its( send->m_HostListView );
	while ( its.current() != NULL ) {
		QListViewItem *item = its.current();
		item->setSelected( FALSE );
		if ( item->text( SendDialog::ColumnIpAddress ).find( IpAddr ) >= 0 ) {
			send->m_HostListView->setSelected( item, TRUE );
//printf("found\n");
			break;
		}
		++its;
	}
	send->setRecieveDialog( this );
    send->show();
}

void RecieveDialog::setDownloadFiles()
{
	string fileNames;
	for( vector<AttachFile>::iterator ix = msg.Files().begin(); ix != msg.Files().end(); ix++ ) {
		fileNames += ix->FileName() + " ";
	}
	QTextCodec *codec = QTextCodec::codecForName( m_EncodingCombobox->currentText() );
	m_DownloadButton->setText( codec->toUnicode( fileNames.c_str() ) );
}

void RecieveDialog::slotDownloadClicked()
{
	vector<AttachFile>::iterator ix = msg.Files().begin();
	KIpMsgFileNameConverter *codec = new KIpMsgFileNameConverter();
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QString networkFileName = fsCodec->toUnicode( codec->ConvertNetworkToLocal( ix->FileName().c_str() ).c_str() );
	DownloadInfo info;
	//保存対象がディレクトリ
	if ( ix->IsDirectory() ){
		while( 1 ) {
			QString saveFileName = getSaveFileName( networkFileName, static_cast<KFile::Mode>( KFile::Directory | KFile::LocalOnly ) );
			if ( saveFileName != "" ) {
				int pos = saveFileName.findRev( "/" );
				if ( pos >= 0 ) {
					QString saveDirName = fsCodec->fromUnicode( saveFileName.left( pos + 1 ) );
					QString saveBaseFileName = fsCodec->fromUnicode( saveFileName.mid( pos + 1 ) );
					AttachFile file( *ix );
					if ( msg.DownloadDir( file, saveBaseFileName.data(), saveDirName.data(), info, codec ) ){
						DownloadCompleteDialog *dlg = new DownloadCompleteDialog(this, 0, TRUE);
						dlg->setDownloadInfo( info, fsCodec->fromUnicode( saveFileName ).data(), *ix );
						dlg->exec();
						delete dlg;
						msg.Files().erase( ix );
					} else {
						DownloadErrorDialog *dlg = new DownloadErrorDialog(this, 0, TRUE);
						dlg->setDownloadInfo( info );
						if ( dlg->exec() == QDialog::Accepted ) {
							continue;
						}
					}
				}
			}
			break;
		}
	//保存対象が一般ファイル
	} else if ( ix->IsRegularFile() ){
		while( 1 ) {
			QString saveFileName = getSaveFileName( networkFileName, static_cast<KFile::Mode>( KFile::File | KFile::LocalOnly ) );
			if ( saveFileName != "" ) {
				int pos = saveFileName.findRev( "/" );
				if ( pos >= 0 ) {
					AttachFile file( *ix );
					if ( msg.DownloadFile( file, saveFileName.data(), info ) ) {
						DownloadCompleteDialog *dlg = new DownloadCompleteDialog(this, 0, TRUE);
						dlg->setDownloadInfo( info, fsCodec->fromUnicode( saveFileName ).data(), *ix );
						dlg->exec();
						delete dlg;
						msg.Files().erase( ix );
					} else {
						DownloadErrorDialog *dlg = new DownloadErrorDialog(this, 0, TRUE);
						dlg->setDownloadInfo( info );
						if ( dlg->exec() == QDialog::Accepted ) {
							continue;
						}
					}
				}
			}
			break;
		}
	}
	delete codec;
	setDownloadFiles();
	doResize();
}

QString RecieveDialog::getSaveFileName( QString fileName, KFile::Mode mode )
{
	KFileDialog dlg( QString::null, QString::null, 0L, "save_as", true);
	dlg.setCaption( tr2i18n( "Save As" ) );
	dlg.setOperationMode( KFileDialog::Saving );
	dlg.setSelection( fileName );
	dlg.setMode( mode );
	if ( dlg.exec() == QDialog::Rejected ) {
		return "";
	}
	return dlg.selectedFile();
}

/**
 *	リサイズイベント
 */
void RecieveDialog::resizeEvent( QResizeEvent *e )
{
	doResize( e );
}

/**
 *	リサイズ実行
 */
void RecieveDialog::doResize( QResizeEvent *e )
{
	char buf[30];

	if ( msg.IsMulticast() ) {
		m_MessageGroup->setTitle("Multicast from ...");
	}
	if ( msg.IsBroadcast() ) {
		m_MessageGroup->setTitle("Broadcast from ...");
	}
	if ( msg.IsPasswordLock() ) {
		m_OpenButton->setText(tr2i18n("Open(Password Locked)"));
	}
	time_t recv = msg.Recieved();
	ctime_r( &recv, buf );
	buf[strlen(buf) - 1] = 0;
	m_MessageRecievedAtLabel->setText( "at " + QString( buf ) );
	QRect rectGroup = m_MessageGroup->geometry();

	QRect rectFrame = m_RecvAreaFrame->geometry();
	QRect rectOpenButton = rectGroup;
	rectOpenButton.setTop( 0 );
	rectOpenButton.setLeft( 0 );
	rectOpenButton.setRight( rectOpenButton.right() - 4 );
	rectOpenButton.setBottom( rectFrame.bottom() - rectFrame.top() - 4 );
	m_OpenButton->setGeometry( rectOpenButton );

	if ( !msg.IsSecret() && !m_OpenButton->isHidden() ) {
		slotMessageOpenClicked();
	}
	if ( msg.Files().size() > 0 ) {
	//if ( msg.Files().size() > 0 ) {
		QRect rectSplitter = m_DownloadSplitterLabel->geometry();
		//ダウンロードボタン位置設定
		QRect rectDownloadButton = rectOpenButton;
		rectDownloadButton.setBottom( rectSplitter.top() - 2 );
		m_DownloadButton->setGeometry( rectDownloadButton );

		rectSplitter.setLeft(rectDownloadButton.left());
		rectSplitter.setRight(rectDownloadButton.right());
		m_DownloadSplitterLabel->setGeometry( rectSplitter );
		//メッセージ位置設定
		QRect rectEditMessage = rectOpenButton;
		rectEditMessage.setTop( rectSplitter.bottom() + 2 );
//		m_RecievedMessageEditbox->setGeometry( rectEditMessage );
    	m_RecievedMessageHTMLPart->view()->setGeometry( rectEditMessage );
	} else {
		//メッセージ位置設定
//		m_RecievedMessageEditbox->setGeometry( rectOpenButton );
    	m_RecievedMessageHTMLPart->view()->setGeometry( rectOpenButton );
	}
	QTextCodec *codec = QTextCodec::codecForName( m_EncodingCombobox->currentText() );
	string fromUser = msg.Host().Nickname() + "(" + msg.Host().HostName() + "/" + msg.Host().GroupName() + ")";
	m_MessageFromLabel->setText( codec->toUnicode( fromUser.c_str() ) );
//	m_RecievedMessageEditbox->setText( codec->toUnicode( msg.Message().c_str() ) );
	m_RecievedMessageHTMLPart->setJScriptEnabled(false);
	m_RecievedMessageHTMLPart->setJavaEnabled(false);
	m_RecievedMessageHTMLPart->setMetaRefreshEnabled(false);
	m_RecievedMessageHTMLPart->setPluginsEnabled(false);
//    m_RecievedMessageHTMLPart->write( codec->toUnicode( msg.Message().c_str() ) );
	renderMessage( codec->toUnicode( msg.Message().c_str() ) );
}

QString RecieveDialog::convertMessageToHTML( QString msg )
{
	QString htmlBody = msg
						.replace('&', "&amp;")
						.replace('\'', "&apos;")
						.replace('"', "&quot;")
						.replace('<', "&lt;")
						.replace('>',"&gt;")
						.replace(' ', "&nbsp;");
	QString url = "";
	QString returnHtmlBody = "";
	bool isInURLString = false;
	//Process htmlBody
	for( unsigned int i = 0; i < htmlBody.length(); i++){
		if ( htmlBody.mid( i, 7 ) == "http://" ||
			 htmlBody.mid( i, 8 ) == "https://" ||
			 htmlBody.mid( i, 6 ) == "ftp://" ||
			 htmlBody.mid( i, 7 ) == "file://" ||
			 htmlBody.mid( i, 9 ) == "telnet://" ) {
			returnHtmlBody += url == "" ? "" : ( "<a href=\"" + url.replace("&amp;", "&") + "\">" + url + "</a>" );
			url = "";
			isInURLString = true;
		}
		if ( isInURLString ) {
			if ( isUrlCharWithoutAmp( htmlBody[i] ) ){
				url += htmlBody[i];
				//&は修正済なので特別扱い
			} else if ( htmlBody.mid( i, 5 ) == "&amp;" ) {
				url += "&amp;";
				i += 4;
			} else {
				returnHtmlBody += url == "" ? "" : ( "<a href=\"" + url.replace("&amp;", "&") + "\">" + url + "</a>" );
				url = "";
				isInURLString = false;
				returnHtmlBody += htmlBody[i];
			}
		} else {
			returnHtmlBody += htmlBody[i];
		}
	}
	returnHtmlBody += url == "" ? "" : ( "<a href=\"" + url.replace("&amp;", "&") + "\">" + url + "</a>" );
	//return Fully HTML Message
    return "<html><body>" + returnHtmlBody.replace('\n', "<br>") + "</body></html>";
}

bool RecieveDialog::isUrlCharWithoutAmp( QChar c )
{
	const bool isUrlCharResult[] = {
			/* 0x00 */ false, /* 0x01 */ false, /* 0x02 */ false, /* 0x03 */ false,
			/* 0x04 */ false, /* 0x05 */ false, /* 0x06 */ false, /* 0x07 */ false,
			/* 0x08 */ false, /* 0x09 */ false, /* 0x0a */ false, /* 0x0b */ false,
			/* 0x0c */ false, /* 0x0d */ false, /* 0x0d */ false, /* 0x0f */ false,
			/* 0x10 */ false, /* 0x11 */ false, /* 0x12 */ false, /* 0x13 */ false,
			/* 0x14 */ false, /* 0x15 */ false, /* 0x16 */ false, /* 0x17 */ false,
			/* 0x18 */ false, /* 0x19 */ false, /* 0x1a */ false, /* 0x1b */ false,
			/* 0x1c */ false, /* 0x1d */ false, /* 0x1d */ false, /* 0x1f */ false,
			/* 0x20 */ false, /* 0x21 */ true , /* 0x22 */ true , /* 0x23 */ true ,
			/* 0x24 */ true , /* 0x25 */ true , /* 0x26 */ false , /* 0x27 */ true ,
			/* 0x28 */ true , /* 0x29 */ true , /* 0x2a */ true , /* 0x2b */ true ,
			/* 0x2c */ true , /* 0x2d */ true , /* 0x2d */ true , /* 0x2f */ true ,
			/* 0x30 */ true , /* 0x31 */ true , /* 0x32 */ true , /* 0x33 */ true ,
			/* 0x34 */ true , /* 0x35 */ true , /* 0x36 */ true , /* 0x37 */ true ,
			/* 0x38 */ true , /* 0x39 */ true , /* 0x3a */ true , /* 0x3b */ true ,
			/* 0x3c */ true , /* 0x3d */ true , /* 0x3d */ true , /* 0x3f */ true ,
			/* 0x40 */ true , /* 0x41 */ true , /* 0x42 */ true , /* 0x43 */ true ,
			/* 0x44 */ true , /* 0x45 */ true , /* 0x46 */ true , /* 0x47 */ true ,
			/* 0x48 */ true , /* 0x49 */ true , /* 0x4a */ true , /* 0x4b */ true ,
			/* 0x4c */ true , /* 0x4d */ true , /* 0x4d */ true , /* 0x4f */ true ,
			/* 0x50 */ true , /* 0x51 */ true , /* 0x52 */ true , /* 0x53 */ true ,
			/* 0x54 */ true , /* 0x55 */ true , /* 0x56 */ true , /* 0x57 */ true ,
			/* 0x58 */ true , /* 0x59 */ true , /* 0x5a */ true , /* 0x5b */ true ,
			/* 0x5c */ true , /* 0x5d */ true , /* 0x5d */ true , /* 0x5f */ true ,
			/* 0x60 */ true , /* 0x61 */ true , /* 0x62 */ true , /* 0x63 */ true ,
			/* 0x64 */ true , /* 0x65 */ true , /* 0x66 */ true , /* 0x67 */ true ,
			/* 0x68 */ true , /* 0x69 */ true , /* 0x6a */ true , /* 0x6b */ true ,
			/* 0x6c */ true , /* 0x6d */ true , /* 0x6d */ true , /* 0x6f */ true ,
			/* 0x70 */ true , /* 0x71 */ true , /* 0x72 */ true , /* 0x73 */ true ,
			/* 0x74 */ true , /* 0x75 */ true , /* 0x76 */ true , /* 0x77 */ true ,
			/* 0x78 */ true , /* 0x79 */ true , /* 0x7a */ true , /* 0x7b */ true ,
			/* 0x7c */ true , /* 0x7d */ true , /* 0x7d */ true , /* 0x7f */ false,
			/* 0x80 */ false, /* 0x81 */ false, /* 0x82 */ false, /* 0x83 */ false,
			/* 0x84 */ false, /* 0x85 */ false, /* 0x86 */ false, /* 0x87 */ false,
			/* 0x88 */ false, /* 0x89 */ false, /* 0x8a */ false, /* 0x8b */ false,
			/* 0x8c */ false, /* 0x8d */ false, /* 0x8d */ false, /* 0x8f */ false,
			/* 0x90 */ false, /* 0x91 */ false, /* 0x92 */ false, /* 0x93 */ false,
			/* 0x94 */ false, /* 0x95 */ false, /* 0x96 */ false, /* 0x97 */ false,
			/* 0x98 */ false, /* 0x99 */ false, /* 0x9a */ false, /* 0x9b */ false,
			/* 0x9c */ false, /* 0x9d */ false, /* 0x9d */ false, /* 0x9f */ false,
			/* 0xa0 */ false, /* 0xa1 */ false, /* 0xa2 */ false, /* 0xa3 */ false,
			/* 0xa4 */ false, /* 0xa5 */ false, /* 0xa6 */ false, /* 0xa7 */ false,
			/* 0xa8 */ false, /* 0xa9 */ false, /* 0xaa */ false, /* 0xab */ false,
			/* 0xac */ false, /* 0xad */ false, /* 0xad */ false, /* 0xaf */ false,
			/* 0xb0 */ false, /* 0xb1 */ false, /* 0xb2 */ false, /* 0xb3 */ false,
			/* 0xb4 */ false, /* 0xb5 */ false, /* 0xb6 */ false, /* 0xb7 */ false,
			/* 0xb8 */ false, /* 0xb9 */ false, /* 0xba */ false, /* 0xbb */ false,
			/* 0xbc */ false, /* 0xbd */ false, /* 0xbd */ false, /* 0xbf */ false,
			/* 0xc0 */ false, /* 0xc1 */ false, /* 0xc2 */ false, /* 0xc3 */ false,
			/* 0xc4 */ false, /* 0xc5 */ false, /* 0xc6 */ false, /* 0xc7 */ false,
			/* 0xc8 */ false, /* 0xc9 */ false, /* 0xca */ false, /* 0xcb */ false,
			/* 0xcc */ false, /* 0xcd */ false, /* 0xcd */ false, /* 0xcf */ false,
			/* 0xd0 */ false, /* 0xd1 */ false, /* 0xd2 */ false, /* 0xd3 */ false,
			/* 0xd4 */ false, /* 0xd5 */ false, /* 0xd6 */ false, /* 0xd7 */ false,
			/* 0xd8 */ false, /* 0xd9 */ false, /* 0xda */ false, /* 0xdb */ false,
			/* 0xdc */ false, /* 0xdd */ false, /* 0xdd */ false, /* 0xdf */ false,
			/* 0xe0 */ false, /* 0xe1 */ false, /* 0xe2 */ false, /* 0xe3 */ false,
			/* 0xe4 */ false, /* 0xe5 */ false, /* 0xe6 */ false, /* 0xe7 */ false,
			/* 0xe8 */ false, /* 0xe9 */ false, /* 0xea */ false, /* 0xeb */ false,
			/* 0xec */ false, /* 0xed */ false, /* 0xed */ false, /* 0xef */ false,
			/* 0xf0 */ false, /* 0xf1 */ false, /* 0xf2 */ false, /* 0xf3 */ false,
			/* 0xf4 */ false, /* 0xf5 */ false, /* 0xf6 */ false, /* 0xf7 */ false,
			/* 0xf8 */ false, /* 0xf9 */ false, /* 0xfa */ false, /* 0xfb */ false,
			/* 0xfc */ false, /* 0xfd */ false, /* 0xfd */ false, /* 0xff */ false,
	};
	return isUrlCharResult[(int)c.latin1()];
}

void RecieveDialog::renderMessage( QString msg )
{
	//Clear Message
    m_RecievedMessageHTMLPart->begin();
    m_RecievedMessageHTMLPart->write( "" );
    m_RecievedMessageHTMLPart->end();

    m_RecievedMessageHTMLPart->begin();
    m_RecievedMessageHTMLPart->write( convertMessageToHTML( msg ) );
    m_RecievedMessageHTMLPart->end();
}

void RecieveDialog::closeEvent ( QCloseEvent * e )
{
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	agent->DeleteNotify( msg );
	e->accept();
}

void RecieveDialog::mousePressEvent (QMouseEvent *e)
{
	if(e->button() == RightButton ){
		RecvPopup->popup( QCursor::pos() );
	} else if(e->button() == LeftButton ){
		QRect rectSplitter = m_DownloadSplitterLabel->geometry();
		if ( !m_DownloadSplitterLabel->isHidden() ) {
			QRect rectRecvAreaFrame = m_RecvAreaFrame->geometry();
			if ( rectSplitter.top() <= e->y() - rectRecvAreaFrame.top() && rectSplitter.bottom() >= e->y() - rectRecvAreaFrame.top() &&
				rectSplitter.left() <= e->x() && rectSplitter.right() >= e->x() ) {
				isDownloadSplitterDragging = true;
			}
		}
	}
}

void RecieveDialog::mouseReleaseEvent (QMouseEvent *e)
{
	mouseMoveEvent(e);
	isDownloadSplitterDragging = false;
}

void RecieveDialog::mouseMoveEvent (QMouseEvent *e)
{
	if ( isDownloadSplitterDragging ){
		QSize sizeMessageGroup = m_MessageGroup->size();
		int localY = e->y() - sizeMessageGroup.height();

		QRect rectSplitter = m_DownloadSplitterLabel->geometry();
		QSize sizeSplitter = m_DownloadSplitterLabel->size();
		QSize sizeDownloadButton = m_DownloadButton->size();

		QSize minsizeDownloadButton = m_DownloadButton->minimumSize();
//		QSize minsizeRecievedMessage = m_RecievedMessageEditbox->minimumSize();
		QSize minsizeRecievedMessage = m_RecievedMessageHTMLPart->view()->minimumSize();

		QSize sizeRecvAreaFrame = m_RecvAreaFrame->size();
		int msgHeight = sizeRecvAreaFrame.height() - sizeDownloadButton.height() - sizeSplitter.height();
		int msgAfterHeight = sizeRecvAreaFrame.height() - localY - sizeSplitter.height();
		int prevTop = rectSplitter.top();
		if ( prevTop < minsizeDownloadButton.height() || localY < minsizeDownloadButton.height() ) {
			;
		} else if ( msgAfterHeight < minsizeRecievedMessage.height() || msgHeight < minsizeRecievedMessage.height() ) {
			;
		} else {
			rectSplitter.setTop( localY );
		}
		m_DownloadSplitterLabel->setGeometry( rectSplitter );
	}
	doResize();
}

#include "recievedialog.moc"

