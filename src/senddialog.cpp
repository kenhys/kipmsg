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

#include <qevent.h>
#include <qcursor.h>
#include <qrect.h>
#include <qsize.h>
#include <qfont.h>
#include <qheader.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>
#include <qdragobject.h>
#include <qpoint.h>
#include <qurl.h>
#include <kmessagebox.h>
#include <kfontdialog.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <klistview.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kfiledialog.h>
#include <kiconloader.h>

#include "senddialog.h"
#include "searchbox.h"
#include "kipmsgsettings.h"
#include "kipmsglogger.h"
#include "showconfig.h"
#include "attachedfiledialog.h"
#include "encodingconfig.h"

extern QPtrList<SendDialog> sendDialogs;

string
KIpMsgFileNameConverter::ConvertNetworkToLocal( string original_file_name ){
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QTextCodec *msgCodec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
	return fsCodec->fromUnicode( msgCodec->toUnicode( original_file_name.c_str() ) ).data();
}
string
KIpMsgFileNameConverter::ConvertLocalToNetwork( string original_file_name ){
	QTextCodec *msgCodec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	return msgCodec->fromUnicode( fsCodec->toUnicode( original_file_name.c_str() ) ).data();
}
	
KTextEditNoDnD::KTextEditNoDnD(QWidget *parent, const char *name) : KTextEdit(parent, name){
	DnDPopup = new KPopupMenu(this);
	DnDPopup->insertItem(SmallIcon("text"),tr2i18n("Add as Text"), this, SLOT( slotAddAsText( void ) ) );
	DnDPopup->insertItem(SmallIcon("attach"), tr2i18n("Add as Attachment"), this, SLOT( slotAddAsFile( void ) ) );
};

KTextEditNoDnD::~KTextEditNoDnD(){
	delete DnDPopup;
};

void KTextEditNoDnD::contentsDragEnterEvent(QDragEnterEvent *e){
	printf("ContentsDragEnterEvent\n");
	fflush(stdout);
};

void KTextEditNoDnD::contentsDropEvent(QDropEvent *e){
	printf("ContentsDropEvent\n");
	fflush(stdout);

	if ( QTextDrag::decode( e, dropText ) ){
		DnDPopup->popup( QCursor::pos() );
	} else {
		dropText = "";
	}
};

void KTextEditNoDnD::slotAddAsText( void ){
	insert(dropText.replace("\r\n", "\n"));
	dropText = "";
}
void KTextEditNoDnD::slotAddAsFile( void ){
	SendDialog *top = dynamic_cast<SendDialog*>(topLevelWidget());
	if ( top != NULL ){
		top->addDnDFiles(dropText);
		top->refreshFiles();
	}
	dropText = "";
}

/*
 * コンストラクタ。ポップアップメニューを初期化。
 */
SendDialog::SendDialog(QWidget* parent, const char* name, WFlags fl)
        : SendDialogBase(parent,name,fl)
{
	defaultX = x();
	defaultY = y();
	defaultWidth = width();
	defaultHeight = height();

	recvdialog = NULL;
	setAcceptDrops( TRUE );

    m_MessageEditbox = new KTextEditNoDnD( m_MessageFrame, "m_MessageEditbox" );
    m_MessageEditbox->setGeometry( QRect( 0, 40, 526, 190 ) );
    m_MessageEditbox->setMinimumSize( QSize( 0, 140 ) );
    m_MessageEditbox->setCursor( QCursor( 4 ) );
    m_MessageEditbox->setMouseTracking( FALSE );
    m_MessageEditbox->setAcceptDrops( FALSE );


	m_MessageEditbox->setAcceptDrops( TRUE );
	//m_MessageEditbox->setAcceptDrops( FALSE );

	m_HostListView->setFont( KIpMsgSettings::listFont() );
	m_MessageEditbox->setFont( KIpMsgSettings::editFont() );

    SendPopup = new KPopupMenu(this);

	SortPopup = new KPopupMenu(this);
	move_priority1_menu_item = SortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 1"), this, SLOT( slotMoveToPriority1Clicked( void ) ) );
	move_priority2_menu_item = SortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 2"), this, SLOT( slotMoveToPriority2Clicked( void ) ) );
	move_priority3_menu_item = SortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 3"), this, SLOT( slotMoveToPriority3Clicked( void ) ) );
	move_priority4_menu_item = SortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 4"), this, SLOT( slotMoveToPriority4Clicked( void ) ) );
	move_default_menu_item = SortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to default"), this, SLOT( slotMoveToDefaultClicked( void ) ) );
	move_hidden_menu_item = SortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to hidden"), this, SLOT( slotMoveToHiddenClicked( void ) ) );
	SortPopup->insertSeparator();
	show_hidden_menu_item = SortPopup->insertItem(tr2i18n("Show Hidden(temporary)"), this, SLOT( slotShowHiddenTempClicked( void ) ) );
	SortPopup->setItemChecked( show_hidden_menu_item, FALSE );
	SortPopup->insertItem(SmallIcon("undo"),tr2i18n("Restore All to default"), this, SLOT( slotRestoreAllClicked( void ) ) );
	SendPopup->insertItem(SmallIcon("filter"),tr2i18n("Sort Filter"), SortPopup );

	GroupPopup = new KPopupMenu(this);
	SendPopup->insertItem(tr2i18n("Group Select"), GroupPopup );

	EncodingPopup = new KPopupMenu(this);
	SendPopup->insertItem(SmallIcon("charset"), tr2i18n("Encoding Select"), EncodingPopup );

	SendPopup->insertItem(tr2i18n("Configure Encoding..."), this, SLOT( slotEncodingConfigClicked( void ) ) );

	SendPopup->insertItem(SmallIcon("search_user"),tr2i18n("Search User..."), this, SLOT( slotSearchUserClicked( void ) ) );
	SendPopup->insertItem(SmallIcon("attach"),tr2i18n("Attach file..."), this, SLOT( slotAttachFileClicked( void ) ) );
	SendPopup->insertItem(SmallIcon("attach"),tr2i18n("Attach directory..."), this, SLOT( slotAttachDirectoryClicked( void ) ) );
	SendPopup->insertSeparator();

	SendPopup->insertItem(SmallIcon("fileexport"),tr2i18n("Save Header"), this, SLOT( slotSaveListHeaderClicked( void ) ) );

	FontPopup = new KPopupMenu(this);
	FontPopup->insertItem(tr2i18n("List..."), this, SLOT( slotFontSelectListClicked( void ) ) );
	FontPopup->insertItem(tr2i18n("Edit..."), this, SLOT( slotFontSelectEditClicked( void ) ) );
	FontPopup->insertItem(SmallIcon("undo"),tr2i18n("Restore default"), this, SLOT( slotFontRestoreToDefaultClicked( void ) ) );
	SendPopup->insertItem(SmallIcon("fonts"),tr2i18n("Font Select"), FontPopup );

	SizePopup = new KPopupMenu(this);
	save_size_menu_item = SizePopup->insertItem(SmallIcon("fileexport"),tr2i18n("Save Size"), this, SLOT( slotSaveSizeClicked( void ) ) );
	SizePopup->insertItem(SmallIcon("undo"),tr2i18n("Restore default(temporary)"), this, SLOT( slotRestoreSizeTempClicked( void ) ) );
	SendPopup->insertItem(SmallIcon("configure"),tr2i18n("Size Configuration"), SizePopup );

	SizePopup->setItemChecked( save_size_menu_item, KIpMsgSettings::sendDialogSaveSize() );

	fixize_pos_menu_item = SendPopup->insertItem(tr2i18n("Fixize Position"), this, SLOT( slotFixizePositionClicked( void ) ) );
	SendPopup->insertItem(SmallIcon("configure"),tr2i18n("View Detail Configuration"), this, SLOT( slotViewDetailConfigurationClicked( void ) ) );

	SendPopup->setItemChecked( fixize_pos_menu_item, KIpMsgSettings::sendDialogFixizePosition() );

	if ( KIpMsgSettings::fireIntercept()  ) {
		m_SendButton->setText(tr2i18n( "Fire") );
	}
	m_SecretCheckbox->setChecked( KIpMsgSettings::secretDefault() );

	if ( KIpMsgSettings::sendDialogFixizePosition() ) {
		move( QPoint( KIpMsgSettings::sendDialogLeft(), KIpMsgSettings::sendDialogTop() ) );
	}else{
		move( QPoint( defaultX, defaultY ) );
	}
    if ( KIpMsgSettings::sendDialogSaveSize() ) {
		resize( KIpMsgSettings::sendDialogWidth(), KIpMsgSettings::sendDialogHeight() );
//		setMinimumSize( KIpMsgSettings::sendDialogWidth(), KIpMsgSettings::sendDialogHeight() );
	}else{
		resize( defaultWidth, defaultHeight );
	}
	
	slotSecretClicked();

	refreshHostList();
}

SendDialog::~SendDialog()
{}

void SendDialog::setMessageText(QString text){
	m_MessageEditbox->setText( text );
}

/*
 * ソートメニューの移動メニューの有効／無効を設定
 */
void SendDialog::setMenuStatus()
{
	int sel_count = 0;
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item->isSelected() ) {
			sel_count++;
		}
		it++;
	}
	SortPopup->setItemEnabled( move_priority1_menu_item, sel_count != 0 );
	SortPopup->setItemEnabled( move_priority2_menu_item, sel_count != 0 );
	SortPopup->setItemEnabled( move_priority3_menu_item, sel_count != 0 );
	SortPopup->setItemEnabled( move_priority4_menu_item, sel_count != 0 );
	SortPopup->setItemEnabled( move_default_menu_item, sel_count != 0 );
	SortPopup->setItemEnabled( move_hidden_menu_item, sel_count != 0 );
}

/*
 * マウス押下時の処理。スプリッターフラグオン
 */
void SendDialog::mousePressEvent( QMouseEvent *e )
{
	if(e->button() == RightButton ){
		setMenuStatus();
		SendPopup->popup(QCursor::pos() );
	} else if(e->button() == LeftButton ){
		QRect rectSplitter = m_MainSplitter->geometry();
		if ( rectSplitter.top() <= e->y() && rectSplitter.bottom() >= e->y() &&
			rectSplitter.left() <= e->x() && rectSplitter.right() >= e->x() ) {
			isMainSplitterDragging = true;
		}
	}
}

/*
 * マウス解放時の処理。スプリッターを動かす。フラグリセット
 */
void SendDialog::mouseReleaseEvent(QMouseEvent *e){
	mouseMoveEvent(e);
	isMainSplitterDragging = false;
	isDownloadSplitterDragging = false;
}

/*
 * マウス移動時の処理。スプリッターを動かす。
 */
void SendDialog::mouseMoveEvent(QMouseEvent *e){
	if ( isMainSplitterDragging ){
		QRect rectSplitter = m_MainSplitter->geometry();
		QSize sizeSplitter = m_MainSplitter->size();
		QSize sizeFrameOperation = m_OperationFrame->size();
		QSize sizeHostFrame = m_HostFrame->size();

		QSize minsizeHostFrame = m_HostFrame->minimumSize();
		QSize minsizeMessageFrame = m_MessageFrame->minimumSize();

		QSize sizeDlg = size();
		int msgHeight = sizeDlg.height() - sizeHostFrame.height() - sizeSplitter.height() - sizeFrameOperation.height();
		int msgAfterHeight = sizeDlg.height() - e->y() - sizeSplitter.height() - sizeFrameOperation.height();
		int prevTop = rectSplitter.top();
		if ( prevTop < minsizeHostFrame.height() || e->y() < minsizeHostFrame.height() ) {
			;
		} else if ( msgAfterHeight < minsizeMessageFrame.height() || msgHeight < minsizeMessageFrame.height() ) {
			;
		} else {
			rectSplitter.setTop( e->y() );
		}
		m_MainSplitter->setGeometry( rectSplitter );
	}
	doResize( size() );
}

/*
 * ドラッグ開始時の処理。受け付けることをシステムに通知する。
 */
void SendDialog::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept( QTextDrag::canDecode(e) );
}

const char *url_decode( string url )
{
	string ret;
	char hex[3];
	for( int i = 0 ; i < url.size(); ){
		unsigned char c = url.at( i );
		if ( c == '%' ) {
			memset( hex, 0, sizeof( hex ) );
			i++;
			c = url.at( i );
			if ( ( c >= '0' && c <= '9' ) || ( c >= 'A' && c <= 'F' ) || ( c >= 'a' && c <= 'f' ) ) {
				hex[0] = c;
				i++;
				c = url.at( i );
				if ( ( c >= '0' && c <= '9' ) || ( c >= 'A' && c <= 'F' ) || ( c >= 'a' && c <= 'f' ) ) {
					hex[1] = c;
					i++;
					char *dmypt;
					c = strtol( hex, &dmypt, 16 );
					ret += c;
					continue;
				}
				continue;
			}
			continue;
		}
		ret += c;
		i++;
	}
	return ret.c_str();

}

/*
 * ドロップ時の処理。URLの先頭が"file://"以外は受け付けない。CR+LFで複数ファイルが区切られることが有る
 */
void SendDialog::dropEvent(QDropEvent *e)
{
	QString dropText;

	if ( QTextDrag::decode( e, dropText ) ){
		addDnDFiles( dropText );
	}
	refreshFiles();
}

void SendDialog::addDnDFiles(QString fileUrl){
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QStringList dropFileList = QStringList::split("\r\n", fileUrl.data() );
	QString dropFileNames;
	bool isDropObjectNotAFile = false;
	for( QStringList::iterator it = dropFileList.begin(); it != dropFileList.end(); it++ ){
		AttachFile file;
		QString fileUrl = *it;
		fileUrl = url_decode( string( fileUrl.data() ) );
		if ( fileUrl.left( 7 ) != "file://" ) {
			isDropObjectNotAFile = true;
			dropFileNames += dropFileNames + "\n[" + fileUrl + "]";
		} else {
			file.setFullPath( fileUrl.mid( 7 ).data() );
			file.GetLocalFileInfo();
			files.AddFile( file );
		}
	}
	if ( isDropObjectNotAFile ) {
		KMessageBox::sorry( 0, tr2i18n( "Dropped object is not a file or directory." ) + dropFileNames, "KIpMessenger" );
		return;
	}
}

/*$SPECIALIZATION$*/
/*
 * メッセージ送信が押された。複数ホストが選択されているなら複数ホストに送信する。
 * ファイルも添付することが有る。
 */
void SendDialog::slotMessageSendClicked()
{
	int send_count = 0;
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	QListViewItemIterator it( m_HostListView );
	QStringList encodings = KIpMsgSettings::encodingSettings();
	vector<HostListItem> targets;
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item != NULL && item->isSelected() ) {
			HostListItem host;
			host.setEncodingName( string( m_EncodingCombobox->currentText().data() ) );
			QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
			host.setNickname( codec->fromUnicode( item->text( ColumnUser ) ).data() );
			host.setGroupName( codec->fromUnicode( item->text( ColumnGroup ) ).data() );
			host.setHostName( codec->fromUnicode( item->text( ColumnHost ) ).data() );
			host.setIpAddress( codec->fromUnicode( item->text( ColumnIpAddress ) ).data() );
			host.setUserName( codec->fromUnicode( item->text( ColumnLogin ) ).data() );
			host.setIpAddress( codec->fromUnicode( item->text( ColumnIpAddress ) ).data() );
			char *dmyptr;
			host.setEncryptionCapacity( strtoul( codec->fromUnicode( item->text( ColumnEncryptionCapacity ) ).data(), &dmyptr, 16 ) );
			host.setEncryptMethodHex( codec->fromUnicode( item->text( ColumnRsaMethod ) ).data() );
			host.setPubKeyHex( codec->fromUnicode( item->text( ColumnRsaPublicKey ) ).data() );

			item->setText( ColumnEncoding, m_EncodingCombobox->currentText() );
			host.setPortNo( 2425 );
			targets.push_back( host );
		}
		++it;
	}

	agent->SetFileNameConverter( new KIpMsgFileNameConverter() );
	for( vector<HostListItem>::iterator host = targets.begin(); host != targets.end(); host++ ) {
		QTextCodec *codec = QTextCodec::codecForName( host->EncodingName().c_str() );
		string msg = codec->fromUnicode( m_MessageEditbox->text() ).data();
		KIpMessengerLogger::GetInstance()->PutSentMessage( 
			agent->SendMsg( m_LockCheckbox->isChecked(), targets.size(), *host, msg, m_SecretCheckbox->isChecked(), files ) );
		QString ip = codec->toUnicode( host->IpAddress().c_str() );
		QString login = codec->toUnicode( host->UserName().c_str() );
		for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
			QStringList fields = QStringList::split( ":", *ite );
			if ( ip == fields[0] && 
				login == fields[1] ) {
				encodings.remove( ite );
				break;
			}
		}
		encodings << ip + ":" + login + ":" + m_EncodingCombobox->currentText();
		send_count++;
	}
	if ( send_count > 0 ) {
		KIpMsgSettings::setEncodingSettings( encodings );
		KIpMsgSettings::writeConfig();
		if ( recvdialog != NULL && recvdialog->isShown() && !KIpMsgSettings::noHide() ) {
			recvdialog->close();
		}
		close();
	}
}

/*
 * 封書チェックが押された。オフにされたときは錠をクリアして無効にする。
 */
void SendDialog::slotSecretClicked()
{
	m_LockCheckbox->setEnabled( m_SecretCheckbox->isChecked() );
	if ( !m_LockCheckbox->isEnabled() ){
		m_LockCheckbox->setChecked( FALSE );
	}
}

/*
 * 更新ボタンが押された。
 */
void SendDialog::slotHostListUpdateClicked()
{
	refreshHostList();
}

/*
 * ホストリストをリフレッシュする。
 */
void SendDialog::refreshHostList()
{
	//選択状態を保存して
	QListViewItemIterator it( m_HostListView );
	QStringList saveSelectedValues;
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item->isSelected() ) {
			saveSelectedValues << item->text( ColumnIpAddress ) + ":" + item->text( ColumnLogin );
		}
		++it;
	}

	//ホストリストを再取得
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	hosts = agent->UpdateHostList();
	//ホストリストを再設定
	m_HostListView->clear();
	m_HostListView->setRootIsDecorated(FALSE);
	m_HostListView->setMultiSelection(TRUE);
	m_HostListView->setSelectionMode(QListView::Extended);

	if ( m_HostListView->columns() > 0 ) {
		while( m_HostListView->columns() > 0 ){
			m_HostListView->removeColumn(0);
		}
	}
#if 0
	//設定によって表示内容を変更する
#endif
	m_HostListView->addColumn( tr2i18n( "User" ), KIpMsgSettings::userNameColumnWidth() );
	if ( KIpMsgSettings::showGroupName() ) {
		m_HostListView->addColumn( tr2i18n("Group"), KIpMsgSettings::groupNameColumnWidth() );
	} else {
		m_HostListView->addColumn( tr2i18n("Group"), 0 );
	}
	if ( KIpMsgSettings::showHostName() ) {
		m_HostListView->addColumn( tr2i18n("Host"), KIpMsgSettings::hostNameColumnWidth() );
	} else {
		m_HostListView->addColumn( tr2i18n("Host"), 0 );
	}
	if ( KIpMsgSettings::showIpAddress() ) {
		m_HostListView->addColumn( tr2i18n("IP address"), KIpMsgSettings::ipAddressColumnWidth() );
	} else {
		m_HostListView->addColumn( tr2i18n("IP address"), 0 );
	}
	if ( KIpMsgSettings::showLoginName() ) {
		m_HostListView->addColumn( tr2i18n("Login"), KIpMsgSettings::loginNameColumnWidth() );
	} else {
		m_HostListView->addColumn( tr2i18n("Login"), 0 );
	}
	if ( KIpMsgSettings::showPriority() ) {
		m_HostListView->addColumn( tr2i18n("Priority"), KIpMsgSettings::priorityColumnWidth() );
	} else {
		m_HostListView->addColumn( tr2i18n("Priority"), 0 );
	}
	if ( KIpMsgSettings::showEncoding() ) {
		m_HostListView->addColumn(tr2i18n("Encoding"), KIpMsgSettings::encodingColumnWidth() );
	} else {
		m_HostListView->addColumn(tr2i18n("Encoding"), 0 );
	}
	m_HostListView->addColumn(tr2i18n("Encryption Flags"), 0 );
	m_HostListView->addColumn(tr2i18n("RSA Method"), 0 );
	m_HostListView->addColumn(tr2i18n("RSA Public Key"), 0 );

	//ヘッダー順序を設定
	QHeader *header = m_HostListView->header();
	header->moveSection( ColumnUser, KIpMsgSettings::userNameColumnIndex() );
	header->moveSection( ColumnGroup, KIpMsgSettings::groupNameColumnIndex() );
	header->moveSection( ColumnHost, KIpMsgSettings::hostNameColumnIndex() );
	header->moveSection( ColumnIpAddress, KIpMsgSettings::ipAddressColumnIndex() );
	header->moveSection( ColumnLogin, KIpMsgSettings::loginNameColumnIndex() );
	header->moveSection( ColumnPriority, KIpMsgSettings::priorityColumnIndex() );
	header->moveSection( ColumnEncoding, KIpMsgSettings::encodingColumnIndex() );

	QStringList encodings = KIpMsgSettings::encodingSettings();
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
		//設定によって表示内容を変更する
		QStringList values;
		QTextCodec *codec;
		for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
			QStringList fields = QStringList::split( ":", *ite );
			if ( QString( ix->IpAddress().c_str() ) == fields[0] && 
				QString( ix->UserName().c_str() ) == fields[1] ) {
				ix->setEncodingName( string( fields[2].data() ) );
				break;
			}
		}
		if ( ix->EncodingName() != "" ) {
			codec = QTextCodec::codecForName( ix->EncodingName().c_str() );
		} else {
			codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
		}
		QStringList pList = KIpMsgSettings::priorityLevel1();
		QString pitem = ix->IpAddress() + ":" + ix->UserName();
		for( QStringList::Iterator it = pList.begin(); it != pList.end(); ++it ){
			if ( pitem == *it ) {
				ix->setPriority( "1" );
				break;
			}
		}
		pList = KIpMsgSettings::priorityLevel2();
		for( QStringList::Iterator it = pList.begin(); it != pList.end(); ++it ){
			if ( pitem == *it ) {
				ix->setPriority( "2" );
				break;
			}
		}
		pList = KIpMsgSettings::priorityLevel3();
		for( QStringList::Iterator it = pList.begin(); it != pList.end(); ++it ){
			if ( pitem == *it ) {
				ix->setPriority( "3" );
				break;
			}
		}
		pList = KIpMsgSettings::priorityLevel4();
		for( QStringList::Iterator it = pList.begin(); it != pList.end(); ++it ){
			if ( pitem == *it ) {
				ix->setPriority( "4" );
				break;
			}
		}
		pList = KIpMsgSettings::priorityDefault();
		for( QStringList::Iterator it = pList.begin(); it != pList.end(); ++it ){
			if ( pitem == *it ) {
				ix->setPriority( "-" );
				break;
			}
		}
		pList = KIpMsgSettings::priorityHidden();
		for( QStringList::Iterator it = pList.begin(); it != pList.end(); ++it ){
			if ( pitem == *it ) {
				ix->setPriority( "X" );
				break;
			}
		}
		if ( ix->Priority() == "" ) {
			ix->setPriority( "-" );
		}
		if ( ix->Priority() != "X" || SortPopup->isItemChecked( show_hidden_menu_item ) ) {
			QListViewItem *item = new QListViewItem( m_HostListView,
													codec->toUnicode( ix->Nickname().c_str() ),
													codec->toUnicode( ix->GroupName().c_str() ),
													codec->toUnicode( ix->HostName().c_str() ),
													codec->toUnicode( ix->IpAddress().c_str() ),
													codec->toUnicode( ix->UserName().c_str() ),
													codec->toUnicode( ix->Priority().c_str() ),
													codec->toUnicode( ix->EncodingName().c_str() ) );

			char buf[100];
			snprintf( buf, sizeof( buf ), "%lx", ix->EncryptionCapacity() );
			item->setText( ColumnEncryptionCapacity, codec->toUnicode( buf ) );
			item->setText( ColumnRsaMethod, codec->toUnicode( ix->EncryptMethodHex().c_str() ) );
			item->setText( ColumnRsaPublicKey, codec->toUnicode( ix->PubKeyHex().c_str() ) );
		}
	}
	//件数を表示
	m_UserCountLCD->display( (int)hosts.size() );
	//選択状態をリストア
	QListViewItemIterator its( m_HostListView );
	while ( its.current() != NULL ) {
		QListViewItem *item = its.current();
		for( unsigned int i = 0; i < saveSelectedValues.count(); i++ ) {
			if ( item->text( ColumnIpAddress ) + ":" + item->text( ColumnLogin ) == saveSelectedValues[i] ) {
				item->setSelected( TRUE );
				break;
			}
		}
		++its;
	}
	
	//グループ選択メニュー再構築
	GroupPopup->clear();
	group_menu.clear();
	group_menu.setAutoDelete( TRUE );
	vector<string> groups = agent->GetGroupList();
	for( vector<string>::iterator ixgr = groups.begin(); ixgr != groups.end();ixgr++ ){
		if ( *ixgr != "" ) {
			QTextCodec *codec;
//			if ( ix->EncodingName != "" ) {
//				codec = QTextCodec::codecForName( ix->EncodingName.c_str() );
//			} else {
				codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
//			}
			int menu_item = GroupPopup->insertItem( codec->toUnicode( QString( ixgr->c_str() ) ) );
			connect( GroupPopup, SIGNAL( activated(int) ), this, SLOT( slotGroupSelect( int ) ) );
			group_menu.insert( menu_item, new QString( codec->toUnicode( QString( ixgr->c_str() ) ) ) );
		}
	}
	//エンコーディング選択メニュー再構築
	EncodingPopup->clear();
	encoding_menu.clear();
	encoding_menu.setAutoDelete( TRUE );
	it = QListViewItemIterator( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		QString enc = item->text( ColumnEncoding );
		if ( enc != "" ) {
			bool isFound = false;
			QIntDictIterator<QString> ite( encoding_menu );
			while ( ite.current() != NULL ) {
				if ( *ite == enc ){
					isFound = true;
					break;
				}
				++ite;
			}
			if ( !isFound ) {
				int menu_item = EncodingPopup->insertItem( enc );
				connect( EncodingPopup, SIGNAL( activated(int) ), this, SLOT( slotEncodingSelect( int ) ) );
				encoding_menu.insert( menu_item, new QString( enc ) );
			}
		}
		it++;
	}
}

/*
 * 添付ファイルリスト表示。
 */
void SendDialog::slotAttacheFileListButtonClicked()
{
	KIpMessengerAttachedFileDialog *afdialog = new KIpMessengerAttachedFileDialog(this,0,TRUE);
	afdialog->setFiles( files );
printf("afdialog->show()before\n");
	afdialog->exec();
printf("afdialog->show()after\n");
	files = afdialog->getFiles();
	refreshFiles();
}

/*
 * グループ選択メニューからグループを選択した。
 */
void SendDialog::slotGroupSelect( int menu_item )
{
	static int prev_menu = 0;
	if ( prev_menu == menu_item ) {
		prev_menu = 0;
		return;
	}
	printf("menu_item %d[%s]\n", menu_item, (*group_menu[menu_item]).data() );
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		printf("item [%s]\n", item->text( ColumnGroup ).data() );
		if ( *group_menu[menu_item] == item->text( ColumnGroup ) ) {
			printf("item->setSelected( TRUE )[%s]\n", item->text( ColumnGroup ).data() );
			m_HostListView->setSelected( item, TRUE );
		}
		it++;
	}
	prev_menu = menu_item;
}

/*
 * エンコーディング選択メニューからエンコーディングを選択した。
 */
void SendDialog::slotEncodingSelect( int menu_item )
{
	static int prev_menu = 0;
	if ( prev_menu == menu_item ) {
		prev_menu = 0;
		return;
	}
	printf("menu_item %d[%s]\n", menu_item, (*encoding_menu[menu_item]).data() );
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		printf("item [%s]\n", item->text( ColumnEncoding ).data() );
		if ( *encoding_menu[menu_item] == item->text( ColumnEncoding ) ) {
			printf("item->setSelected( TRUE )[%s]\n", item->text( ColumnEncoding ).data() );
			m_HostListView->setSelected( item, TRUE );
		}
		it++;
	}
	prev_menu = menu_item;
}

void SendDialog::deleteFromList( QStringList &base, QStringList items )
{
	for( QStringList::iterator it = base.begin(); it != base.end(); it++ ) {
		for( QStringList::iterator pit = items.begin(); pit != items.end(); pit++ ) {
			if ( *it == *pit ) {
				it = base.remove( it );
				break;
			}
		}
	}
}

/*
 * 優先度を設定する。
 */
void SendDialog::setPriority( string pri, QStringList &priList )
{
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item->isSelected() ) {
			item->setText( ColumnPriority, pri.c_str() );
			priList << item->text( ColumnIpAddress ) + ":" + item->text( ColumnLogin );
		}
		++it;
	}

	QStringList tmpList = KIpMsgSettings::priorityLevel1();
	deleteFromList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel1( tmpList );

	tmpList = KIpMsgSettings::priorityLevel2();
	deleteFromList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel2( tmpList );

	tmpList = KIpMsgSettings::priorityLevel3();
	deleteFromList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel3( tmpList );

	tmpList = KIpMsgSettings::priorityLevel4();
	deleteFromList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel4( tmpList );

	tmpList = KIpMsgSettings::priorityLevel4();
	deleteFromList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel4( tmpList );

	tmpList = KIpMsgSettings::priorityDefault();
	deleteFromList( tmpList, priList );
	KIpMsgSettings::setPriorityDefault( tmpList );

	tmpList = KIpMsgSettings::priorityHidden();
	deleteFromList( tmpList, priList );
	KIpMsgSettings::setPriorityHidden( tmpList );
}

/*
 * 優先度1に移動する。
 */
void SendDialog::slotMoveToPriority1Clicked()
{
	QStringList priList;
	setPriority( "1", priList );
	priList = KIpMsgSettings::priorityLevel1() + priList;
	KIpMsgSettings::setPriorityLevel1( priList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
}

/*
 * 優先度2に移動する。
 */
void SendDialog::slotMoveToPriority2Clicked()
{
	QStringList priList;
	setPriority( "2", priList );
	priList = KIpMsgSettings::priorityLevel2() + priList;
	KIpMsgSettings::setPriorityLevel2( priList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
}

/*
 * 優先度3に移動する。
 */
void SendDialog::slotMoveToPriority3Clicked()
{
	QStringList priList;
	setPriority( "3", priList );
	priList = KIpMsgSettings::priorityLevel3() + priList;
	KIpMsgSettings::setPriorityLevel3( priList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
}

/*
 * 優先度1に移動する。
 */
void SendDialog::slotMoveToPriority4Clicked()
{
	QStringList priList;
	setPriority( "4", priList );
	priList = KIpMsgSettings::priorityLevel4() + priList;
	KIpMsgSettings::setPriorityLevel4( priList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
}

/*
 * デフォルトに移動する。
 */
void SendDialog::slotMoveToDefaultClicked()
{
	QStringList priList;
	setPriority( "-", priList );
	priList = KIpMsgSettings::priorityDefault() + priList;
	KIpMsgSettings::setPriorityDefault( priList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
}

/*
 * 非表示に移動する。
 */
void SendDialog::slotMoveToHiddenClicked()
{
	QStringList priList;
	setPriority( "X", priList );
	priList = KIpMsgSettings::priorityHidden() + priList;
	KIpMsgSettings::setPriorityHidden( priList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
}

/*
 * 非表示を一時解除。
 */
void SendDialog::slotShowHiddenTempClicked()
{
	SortPopup->setItemChecked( show_hidden_menu_item, !SortPopup->isItemChecked( show_hidden_menu_item ) );
	refreshHostList();
}

/*
 * 全てデフォルトに戻す。
 */
void SendDialog::slotRestoreAllClicked()
{
	if ( KMessageBox::questionYesNo( 0, tr2i18n( "Change to default.Are you sure?" ), "KIpMessenger" ) != KMessageBox::Yes ){
		return;
	} 

	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		item->setText( ColumnPriority, "-" );
		++it;
	}
	QStringList emptyList;
	KIpMsgSettings::setPriorityLevel1( emptyList );
	KIpMsgSettings::setPriorityLevel2( emptyList );
	KIpMsgSettings::setPriorityLevel3( emptyList );
	KIpMsgSettings::setPriorityLevel4( emptyList );
	KIpMsgSettings::setPriorityDefault( emptyList );
	KIpMsgSettings::setPriorityHidden( emptyList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
}

/*
 * 検索。
 */
void SendDialog::slotSearchUserClicked()
{
	KIpMsgSearchBox *searchbox = new KIpMsgSearchBox();
	searchbox->m_HostListView = m_HostListView;
	searchbox->show();
}

/*
 * ファイル添付。
 */
void SendDialog::slotAttachFileClicked()
{
	QString attachFileName = KFileDialog::getOpenFileName();
	if ( attachFileName != "" ) {
		AttachFile file;
		file.setFullPath( attachFileName.data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	refreshFiles();
}

/*
 * ディレクトリ添付。
 */
void SendDialog::slotAttachDirectoryClicked()
{
	QString attachDirName = KFileDialog::getExistingDirectory();
	if ( attachDirName != "" ) {
		AttachFile file;
		file.setFullPath( attachDirName.data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	refreshFiles();
}

/*
 * ヘッダ保存。
 */
void SendDialog::slotSaveListHeaderClicked()
{
	QHeader *header = m_HostListView->header();

	KIpMsgSettings::setUserNameColumnIndex( 0 );
	if ( m_HostListView->columnWidth( 0 ) > 0 ) {
		KIpMsgSettings::setUserNameColumnWidth( m_HostListView->columnWidth( 0 ) );
	}
	KIpMsgSettings::setGroupNameColumnIndex( header->mapToIndex( 1 ) );
	if ( m_HostListView->columnWidth( 1 ) > 0 ) {
		KIpMsgSettings::setGroupNameColumnWidth( m_HostListView->columnWidth( 1 ) );
	}
	KIpMsgSettings::setHostNameColumnIndex( header->mapToIndex( 2 ) );
	if ( m_HostListView->columnWidth( 2 ) > 0 ) {
		KIpMsgSettings::setHostNameColumnWidth( m_HostListView->columnWidth( 2 ) );
	}
	KIpMsgSettings::setIpAddressColumnIndex( header->mapToIndex( 3 ) );
	if ( m_HostListView->columnWidth( 3 ) > 0 ) {
		KIpMsgSettings::setIpAddressColumnWidth( m_HostListView->columnWidth( 3 ) );
	}
	KIpMsgSettings::setLoginNameColumnIndex( header->mapToIndex( 4 ) );
	if ( m_HostListView->columnWidth( 4 ) > 0 ) {
		KIpMsgSettings::setLoginNameColumnWidth( m_HostListView->columnWidth( 4 ) );
	}
	KIpMsgSettings::setPriorityColumnIndex( header->mapToIndex( 5 ) );
	if ( m_HostListView->columnWidth( 5 ) > 0 ) {
		KIpMsgSettings::setPriorityColumnWidth( m_HostListView->columnWidth( 5 ) );
	}
	KIpMsgSettings::setEncodingColumnIndex( header->mapToIndex( 6 ) );
	if ( m_HostListView->columnWidth( 6 ) > 0 ) {
		KIpMsgSettings::setEncodingColumnWidth( m_HostListView->columnWidth( 6 ) );
	}
	KIpMsgSettings::writeConfig();
}

/*
 * ホストリストのフォントを選択。
 */
void SendDialog::slotFontSelectListClicked()
{
	QFont font = m_HostListView->font();
	int result = KFontDialog::getFont(font);
	if ( result == KFontDialog::Accepted ) {
		m_HostListView->setFont(font);
		KIpMsgSettings::setListFont( m_HostListView->font() );
		KIpMsgSettings::writeConfig();
	}
}

/*
 * 本文のフォントを選択。
 */
void SendDialog::slotFontSelectEditClicked()
{
	QFont font = m_MessageEditbox->font();
	int result = KFontDialog::getFont(font);
	if ( result == KFontDialog::Accepted ) {
		m_MessageEditbox->setFont(font);
		KIpMsgSettings::setListFont( m_MessageEditbox->font() );
		KIpMsgSettings::writeConfig();
	}
}

/*
 * フォントをデフォルトに設定。
 */
void SendDialog::slotFontRestoreToDefaultClicked()
{
	m_HostListView->unsetFont();
	m_MessageEditbox->unsetFont();
	KIpMsgSettings::setListFont( m_HostListView->font() );
	KIpMsgSettings::setEditFont( m_MessageEditbox->font() );
	KIpMsgSettings::writeConfig();
}

/*
 * サイズ保存。
 */
void SendDialog::synchronizeMenu()
{
	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( sendDlg->isShown() && sendDlg->winId() != winId() ) {
			sendDlg->setFixsizePotisionMenu();
			sendDlg->setSaveSizeMenu();
		}
		++sendIt;
	}
}
void SendDialog::setSaveSizeMenu()
{
	SizePopup->setItemChecked( save_size_menu_item, KIpMsgSettings::sendDialogSaveSize() );
}
void SendDialog::slotSaveSizeClicked()
{
	KIpMsgSettings::setSendDialogSaveSize( !KIpMsgSettings::sendDialogSaveSize() );
	setSaveSizeMenu();
	KIpMsgSettings::setSendDialogHeight( width() );
	KIpMsgSettings::setSendDialogWidth( height() );
	KIpMsgSettings::writeConfig();
	synchronizeMenu();
}

/*
 * サイズを一時的に戻す。
 */
void SendDialog::slotRestoreSizeTempClicked()
{
	resize( defaultWidth, defaultHeight );
}

void SendDialog::setFixsizePotisionMenu()
{
	SendPopup->setItemChecked( fixize_pos_menu_item, KIpMsgSettings::sendDialogFixizePosition() );
}
/*
 * 位置固定。
 */
void SendDialog::slotFixizePositionClicked()
{
	KIpMsgSettings::setSendDialogFixizePosition( !KIpMsgSettings::sendDialogFixizePosition() );
	setFixsizePotisionMenu();
	KIpMsgSettings::setSendDialogTop( x() );
	KIpMsgSettings::setSendDialogLeft( y() );
	KIpMsgSettings::writeConfig();
	synchronizeMenu();
}

/*
 * 表示詳細設定。
 */
void SendDialog::slotViewDetailConfigurationClicked()
{
	KIPMsgShowConfigDialog *showconfig = new KIPMsgShowConfigDialog();
	showconfig->setSendDialog( this );
	showconfig->show();
}

/*
 * ホストリストでのコンテキストメニュー表示要求。
 */
void SendDialog::slotListContextMenuRequested( QListViewItem *item, const QPoint &pos, int col )
{
	setMenuStatus();
	SendPopup->popup( QCursor::pos() );
}

/*
 * エンコーディング設定表示。
 */
void SendDialog::slotEncodingConfigClicked()
{
	KIPMsgEncodingConfigDialog *encconfig = new KIPMsgEncodingConfigDialog(this,0,TRUE);
	encconfig->exec();
}

/*
 * リサイズイベント。
 */
void SendDialog::resizeEvent( QResizeEvent *e )
{
	doResize( e->size() );
}

/*
 * リサイズ実行（共通）。
 */
void SendDialog::doResize( QSize size )
{
	QSize sizeFrameOperation = m_OperationFrame->size();
	QRect rectMainSplitter = m_MainSplitter->geometry();
	rectMainSplitter.setLeft( 2 );
	rectMainSplitter.setRight( size.width() - 2 );
	m_MainSplitter->setGeometry( rectMainSplitter );

	m_HostFrame->setGeometry( 2, 1, size.width() - 2, rectMainSplitter.top() );
	m_OperationFrame->setGeometry( 2, size.height() - sizeFrameOperation.height(), size.width() - 2, sizeFrameOperation.height() );
	QRect rectOperationFrame = m_OperationFrame->geometry();
	QSize sizeMainSplitter = m_MainSplitter->size();
	
	QSize sizeHostFrame = m_HostFrame->size();
	m_MessageFrame->setGeometry( 2, rectMainSplitter.bottom(), size.width() - 2, size.height() - sizeHostFrame.height() - sizeMainSplitter.height() - rectOperationFrame.height() );
	QRect rectm_MessageFrame = m_MessageFrame->geometry();
	refreshFiles();
}

/*
 * ボタンにファイル名を設定。
 */
void SendDialog::refreshFiles()
{
	QTextCodec *codec;
//	if ( ix->EncodingName != "" ) {
//		codec = QTextCodec::codecForName( ix->EncodingName.c_str() );
//	} else {
		codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
//	}

 	m_AttachFileButton->setHidden( TRUE );
	if ( files.size() > 0 ) {
		QSize sizem_MessageFrame = m_MessageFrame->size();
		//ダウンロードボタン位置設定
		QRect rectAttachedFileList;
		rectAttachedFileList.setTop( 4 );
		rectAttachedFileList.setLeft( 4 );
		rectAttachedFileList.setRight( sizem_MessageFrame.width() - 2 );
		rectAttachedFileList.setBottom( 32 );
		m_AttachFileButton->setGeometry( rectAttachedFileList );

		//メッセージ位置設定
		QRect rectMessageEditbox;
		rectMessageEditbox.setTop( 32 + 2 );
		rectMessageEditbox.setLeft( 4 );
		rectMessageEditbox.setRight( sizem_MessageFrame.width() - 8 );
		rectMessageEditbox.setBottom( sizem_MessageFrame.height() - 8 );
		m_MessageEditbox->setGeometry( rectMessageEditbox );
		m_AttachFileButton->setHidden( FALSE );
		QString AttachedFileListCaption = "";
		for( vector<AttachFile>::iterator it = files.begin(); it != files.end(); it++ ){
			AttachedFileListCaption += codec->toUnicode( it->FileName().c_str() ) + " ";
		}
		m_AttachFileButton->setText( AttachedFileListCaption );
	} else {
		//メッセージ位置設定
		QRect rectMessageEditbox;
		QSize sizem_MessageFrame = m_MessageFrame->size();

		rectMessageEditbox.setTop( 4 );
		rectMessageEditbox.setLeft( 4 );
		rectMessageEditbox.setRight( sizem_MessageFrame.width() - 8 );
		rectMessageEditbox.setBottom( sizem_MessageFrame.height() - 8 );
		m_MessageEditbox->setGeometry( rectMessageEditbox );
	}
}

#include "senddialog.moc"
