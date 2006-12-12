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
#include "kipmsgevent.h"

extern QPtrList<SendDialog> sendDialogs;

class KIpMsgHostListViewItem : public QListViewItem {
  private:
    HostListItem m_host;
  public:
    KIpMsgHostListViewItem( QListView *parent, QTextCodec *codec, HostListItem host ) :
			QListViewItem( parent, 
					codec->toUnicode( host.Nickname().c_str() ),
					codec->toUnicode( host.GroupName().c_str() ),
					codec->toUnicode( host.HostName().c_str() ),
					codec->toUnicode( host.IpAddress().c_str() ),
					codec->toUnicode( host.UserName().c_str() ),
					codec->toUnicode( host.Priority().c_str() ),
					codec->toUnicode( host.EncodingName().c_str() ) )
	{
		m_host = host;
		
	};
    HostListItem host(){ return m_host; }
};

/**
 * ファイル名コンバータのネットワークエンコーディングからファイルシステムエンコーディングへの変換メソッド。
 * @param ネットワークエンコーディングのファイル名
 * @retval ファイルシステムエンコーディングに変換されたファイル名
 */
string
KIpMsgFileNameConverter::ConvertNetworkToLocal( string original_file_name ){
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QTextCodec *msgCodec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
	return fsCodec->fromUnicode( msgCodec->toUnicode( original_file_name.c_str() ) ).data();
}
/**
 * ファイル名コンバータのファイルシステムエンコーディングからネットワークエンコーディングへの変換メソッド。
 * @param ファイルシステムエンコーディングのファイル名
 * @retval ネットワークエンコーディングに変換されたファイル名
 */
string
KIpMsgFileNameConverter::ConvertLocalToNetwork( string original_file_name ){
	QTextCodec *msgCodec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	return msgCodec->fromUnicode( fsCodec->toUnicode( original_file_name.c_str() ) ).data();
}

/**
 * 編集領域Widgetのコンストラクタ。
 * ・ポップアップメニューの生成
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KTextEditNoDnD::KTextEditNoDnD(QWidget *parent, const char *name) : KTextEdit(parent, name){
	DnDPopup = new KPopupMenu(this);
	DnDPopup->insertItem(SmallIcon("text"),tr2i18n("Add as Text"), this, SLOT( slotAddAsText( void ) ) );
	DnDPopup->insertItem(SmallIcon("attach"), tr2i18n("Add as Attachment"), this, SLOT( slotAddAsFile( void ) ) );
};

/**
 * 編集領域Widgetのデストラクタ。
 * ・ポップアップメニューの削除
 */
KTextEditNoDnD::~KTextEditNoDnD(){
	delete DnDPopup;
};

/**
 * 編集領域Widgetのコンテンツドロップイベント。
 * ・ポップアップメニューを表示
 * @param e ドロップイベント
 */
void KTextEditNoDnD::contentsDropEvent(QDropEvent *e){
	if ( QTextDrag::decode( e, dropText ) ){
		DnDPopup->popup( QCursor::pos() );
	} else {
		dropText = "";
	}
};

/**
 * 編集領域WidgetのDND対象をテキストとして追加。
 * ・テキストとして追加
 */
void KTextEditNoDnD::slotAddAsText( void ){
	insert(dropText.replace("\r\n", "\n"));
	dropText = "";
}

/**
 * 編集領域WidgetのDND対象をファイルとして追加。
 * ・ファイルとして追加
 */
void KTextEditNoDnD::slotAddAsFile( void ){
	SendDialog *top = dynamic_cast<SendDialog*>(topLevelWidget());
	if ( top != NULL ){
		top->addDnDFiles(dropText);
		top->refreshFiles();
	}
	dropText = "";
}

/**
 * コンストラクタ。
 * ・表示初期設定
 * ・ポップアップメニューの生成
 * ・リサイズ
 * ・封書設定
 * ・ホストリストの更新
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
SendDialog::SendDialog(QWidget* parent, const char* name, WFlags fl)
        : SendDialogBase(parent,name,fl)
{
	defaultX = x();
	defaultY = y();
	defaultWidth = width();
	defaultHeight = height();

	recvDialog = NULL;
	setAcceptDrops( TRUE );

    m_MessageEditbox = new KTextEditNoDnD( m_MessageFrame, "m_MessageEditbox" );
    m_MessageEditbox->setGeometry( QRect( 0, 40, 526, 190 ) );
    m_MessageEditbox->setMinimumSize( QSize( 0, 140 ) );
    m_MessageEditbox->setCursor( QCursor( 4 ) );
    m_MessageEditbox->setMouseTracking( FALSE );
    m_MessageEditbox->setAcceptDrops( FALSE );


	m_MessageEditbox->setAcceptDrops( TRUE );

	m_HostListView->setFont( KIpMsgSettings::listFont() );
	m_MessageEditbox->setFont( KIpMsgSettings::editFont() );

    sendPopup = new KPopupMenu(this);

	sortPopup = new KPopupMenu(this);
	moveToPriority1MenuId = sortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 1"), this, SLOT( slotMoveToPriority1Clicked( void ) ) );
	moveToPriority2MenuId = sortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 2"), this, SLOT( slotMoveToPriority2Clicked( void ) ) );
	moveToPriority3MenuId = sortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 3"), this, SLOT( slotMoveToPriority3Clicked( void ) ) );
	moveToPriority4MenuId = sortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to Priority 4"), this, SLOT( slotMoveToPriority4Clicked( void ) ) );
	moveToDefaultMenuId = sortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to default"), this, SLOT( slotMoveToDefaultClicked( void ) ) );
	moveToHiddenMenuId = sortPopup->insertItem(SmallIcon("move"),tr2i18n("Move to hidden"), this, SLOT( slotMoveToHiddenClicked( void ) ) );
	sortPopup->insertSeparator();
	showHiddenMenuId = sortPopup->insertItem(tr2i18n("Show Hidden(temporary)"), this, SLOT( slotShowHiddenTempClicked( void ) ) );
	sortPopup->setItemChecked( showHiddenMenuId, FALSE );
	sortPopup->insertItem(SmallIcon("undo"),tr2i18n("Restore All to default"), this, SLOT( slotRestoreAllClicked( void ) ) );
	sendPopup->insertItem(SmallIcon("filter"),tr2i18n("Sort Filter"), sortPopup );

	groupPopup = new KPopupMenu(this);
	sendPopup->insertItem(tr2i18n("Group Select"), groupPopup );

	encodingPopup = new KPopupMenu(this);
	sendPopup->insertItem(SmallIcon("charset"), tr2i18n("Encoding Select"), encodingPopup );

	sendPopup->insertItem(tr2i18n("Configure Encoding..."), this, SLOT( slotEncodingConfigClicked( void ) ) );

	sendPopup->insertItem(SmallIcon("search_user"),tr2i18n("Search User..."), this, SLOT( slotSearchUserClicked( void ) ) );
	sendPopup->insertItem(SmallIcon("attach"),tr2i18n("Attach file..."), this, SLOT( slotAttachFileClicked( void ) ) );
	sendPopup->insertItem(SmallIcon("attach"),tr2i18n("Attach directory..."), this, SLOT( slotAttachDirectoryClicked( void ) ) );
	sendPopup->insertSeparator();

	sendPopup->insertItem(SmallIcon("fileexport"),tr2i18n("Save Header"), this, SLOT( slotSaveListHeaderClicked( void ) ) );

	fontPopup = new KPopupMenu(this);
	fontPopup->insertItem(tr2i18n("List..."), this, SLOT( slotFontSelectListClicked( void ) ) );
	fontPopup->insertItem(tr2i18n("Edit..."), this, SLOT( slotFontSelectEditClicked( void ) ) );
	fontPopup->insertItem(SmallIcon("undo"),tr2i18n("Restore default"), this, SLOT( slotFontRestoreToDefaultClicked( void ) ) );
	sendPopup->insertItem(SmallIcon("fonts"),tr2i18n("Font Select"), fontPopup );

	sizePopup = new KPopupMenu(this);
	saveSizeMenuId = sizePopup->insertItem(SmallIcon("fileexport"),tr2i18n("Save Size"), this, SLOT( slotSaveSizeClicked( void ) ) );
	sizePopup->insertItem(SmallIcon("undo"),tr2i18n("Restore default(temporary)"), this, SLOT( slotRestoreSizeTempClicked( void ) ) );
	sendPopup->insertItem(SmallIcon("configure"),tr2i18n("Size Configuration"), sizePopup );

	sizePopup->setItemChecked( saveSizeMenuId, KIpMsgSettings::sendDialogSaveSize() );

	fixizePositionMenuId = sendPopup->insertItem(tr2i18n("Fixize Position"), this, SLOT( slotFixizePositionClicked( void ) ) );
	sendPopup->insertItem(SmallIcon("configure"),tr2i18n("View Detail Configuration"), this, SLOT( slotViewDetailConfigurationClicked( void ) ) );

	sendPopup->setItemChecked( fixizePositionMenuId, KIpMsgSettings::sendDialogFixizePosition() );

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

/**
 * デストラクタ
 * ・特にすることなし。
 */
SendDialog::~SendDialog()
{}

/**
 * 返信用のメッセージ設定メソッド（公開）
 * ・返信にメッセージを設定
 */
void SendDialog::setMessageText(QString text){
	m_MessageEditbox->setText( text );
}

/**
 * ソートメニューの移動メニューの有効／無効を設定
 * ・選択中のホストがある場合、移動メニューは有効。無い場合無効に設定
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
	sortPopup->setItemEnabled( moveToPriority1MenuId, sel_count != 0 );
	sortPopup->setItemEnabled( moveToPriority2MenuId, sel_count != 0 );
	sortPopup->setItemEnabled( moveToPriority3MenuId, sel_count != 0 );
	sortPopup->setItemEnabled( moveToPriority4MenuId, sel_count != 0 );
	sortPopup->setItemEnabled( moveToDefaultMenuId, sel_count != 0 );
	sortPopup->setItemEnabled( moveToHiddenMenuId, sel_count != 0 );
}

/**
 * マウス押下時の処理。
 * ・スプリッターフラグオン
 */
void SendDialog::mousePressEvent( QMouseEvent *e )
{
	if(e->button() == RightButton ){
		setMenuStatus();
		sendPopup->popup(QCursor::pos() );
	} else if(e->button() == LeftButton ){
		QRect rectSplitter = m_MainSplitter->geometry();
		if ( rectSplitter.top() <= e->y() && rectSplitter.bottom() >= e->y() &&
			rectSplitter.left() <= e->x() && rectSplitter.right() >= e->x() ) {
			isMainSplitterDragging = true;
		}
	}
}

/**
 * マウス解放時の処理。
 * ・スプリッターを動かす。
 * ・フラグリセット
 */
void SendDialog::mouseReleaseEvent(QMouseEvent *e){
	mouseMoveEvent(e);
	isMainSplitterDragging = false;
	isDownloadSplitterDragging = false;
}

/**
 * マウス移動時の処理。
 * ・スプリッターを動かす。
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

/**
 * ドラッグ開始時の処理。
 * ・受け付けることをシステムに通知する。
 */
void SendDialog::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept( QTextDrag::canDecode(e) );
}

/**
 * URLのデコード。
 * ・%NN+%NN形式に変換する。
 */
const char *decodeUrl( string url )
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

/**
 * ドロップ時の処理。
 * ・テキストエリア外にドロップされた場合はファイルを追加する。
 */
void SendDialog::dropEvent(QDropEvent *e)
{
	QString dropText;

	if ( QTextDrag::decode( e, dropText ) ){
		addDnDFiles( dropText );
	}
	refreshFiles();
}

/**
 * DNDされたファイルを追加する。
 * ・URLの先頭が"file://"以外は受け付けない。CR+LFで複数ファイルが区切られることが有る。
 */
void SendDialog::addDnDFiles(QString fileUrl){
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QStringList dropFileList = QStringList::split("\r\n", fileUrl.data() );
	QString dropFileNames;
	bool isDropObjectNotAFile = false;
	for( QStringList::iterator it = dropFileList.begin(); it != dropFileList.end(); it++ ){
		AttachFile file;
		QString fileUrl = *it;
		fileUrl = decodeUrl( string( fileUrl.data() ) );
		if ( fileUrl.left( 7 ) != "file://" ) {
			isDropObjectNotAFile = true;
			dropFileNames += dropFileNames + "\n[" + fileUrl + "]";
		} else {
			file.setFullPath( fileUrl.mid( 7 ).data() );
			file.GetLocalFileInfo();
			attachFileList.AddFile( file );
		}
	}
	if ( isDropObjectNotAFile ) {
		KMessageBox::sorry( 0, tr2i18n( "Dropped object is not a file or directory." ) + dropFileNames, "KIpMessenger" );
		return;
	}
}

/*$SPECIALIZATION$*/

/**
 * メッセージ送信が押された。
 * ・複数ホストが選択されているなら複数ホストに送信する。
 * ・ファイルも添付することが有る。
 */
void SendDialog::slotMessageSendClicked()
{
	int send_count = 0;
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	QListViewItemIterator it( m_HostListView );
	QStringList encodings = KIpMsgSettings::encodingSettings();
	vector<HostListItem> targets;
	//選択中のホストを送信対象ホストリストに追加する。
	while ( it.current() != NULL ) {
		KIpMsgHostListViewItem *item = dynamic_cast<KIpMsgHostListViewItem *>(it.current());
		if ( item != NULL && item->isSelected() ) {
			//ホストリストはHostListViewItemから取得できる。
			HostListItem host = item->host();
			host.setEncodingName( string( m_EncodingCombobox->currentText().data() ) );
			targets.push_back( host );
		}
		++it;
	}

	//送信対象ホストリストを基にメッセージを送信する。
	agent->SetFileNameConverter( new KIpMsgFileNameConverter() );
	for( vector<HostListItem>::iterator host = targets.begin(); host != targets.end(); host++ ) {
		QTextCodec *codec = QTextCodec::codecForName( host->EncodingName().c_str() );
		string msg = codec->fromUnicode( m_MessageEditbox->text() ).data();
		KIpMessengerLogger::GetInstance()->PutSentMessage( 
			agent->SendMsg( *host, msg, m_SecretCheckbox->isChecked(), attachFileList, m_LockCheckbox->isChecked(), targets.size() ) );
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
	//エンコーディングの設定を更新する。
	if ( send_count > 0 ) {
		KIpMsgSettings::setEncodingSettings( encodings );
		KIpMsgSettings::writeConfig();
		if ( recvDialog != NULL && recvDialog->isShown() && !KIpMsgSettings::noHide() ) {
			recvDialog->close();
		}
		close();
	}
}

/**
 * 封書チェックが押された。
 * ・オフにされたときは錠をクリアして無効にする。
 */
void SendDialog::slotSecretClicked()
{
	m_LockCheckbox->setEnabled( m_SecretCheckbox->isChecked() );
	if ( !m_LockCheckbox->isEnabled() ){
		m_LockCheckbox->setChecked( FALSE );
	}
}

/**
 * 更新ボタンが押された。
 * ・ホストリストを実際にネットワークからリフレッシュ
 */
void SendDialog::slotHostListUpdateClicked()
{
	refreshHostList( true );
}

/**
 * ホストリストをリフレッシュする。
 * ・ホストリストをリフレッシュ
 * @param isUpdate TRUE:ネットワークから。FALSE:エージェントの内部変数から。
 */
void SendDialog::refreshHostList( bool isUpdate )
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
	if ( isUpdate ) {
		hosts = agent->UpdateHostList();
	} else {
		hosts = agent->GetHostList();
	}
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
		if ( ix->Priority() != "X" || sortPopup->isItemChecked( showHiddenMenuId ) ) {
		    KIpMsgHostListViewItem *item = new KIpMsgHostListViewItem( m_HostListView, codec, *ix );
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
	groupPopup->clear();
	groupMenuIdList.clear();
	groupMenuIdList.setAutoDelete( TRUE );
	vector<string> groups = agent->GetGroupList();
	for( vector<string>::iterator ixgr = groups.begin(); ixgr != groups.end();ixgr++ ){
		if ( *ixgr != "" ) {
			QTextCodec *codec;
			codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
			int menu_item = groupPopup->insertItem( codec->toUnicode( QString( ixgr->c_str() ) ) );
			connect( groupPopup, SIGNAL( activated(int) ), this, SLOT( slotGroupSelect( int ) ) );
			groupMenuIdList.insert( menu_item, new QString( codec->toUnicode( QString( ixgr->c_str() ) ) ) );
		}
	}
	//エンコーディング選択メニュー再構築
	encodingPopup->clear();
	encodingMenuIdList.clear();
	encodingMenuIdList.setAutoDelete( TRUE );
	it = QListViewItemIterator( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		QString enc = item->text( ColumnEncoding );
		if ( enc != "" ) {
			bool isFound = false;
			QIntDictIterator<QString> ite( encodingMenuIdList );
			while ( ite.current() != NULL ) {
				if ( *ite == enc ){
					isFound = true;
					break;
				}
				++ite;
			}
			if ( !isFound ) {
				int menu_item = encodingPopup->insertItem( enc );
				connect( encodingPopup, SIGNAL( activated(int) ), this, SLOT( slotEncodingSelect( int ) ) );
				encodingMenuIdList.insert( menu_item, new QString( enc ) );
			}
		}
		it++;
	}
}

/**
 * 添付ファイルリスト表示。
 * ・添付ファイルリストダイアログ表示。
 * ・添付ファイル横一覧を再表示
 */
void SendDialog::slotAttacheFileListButtonClicked()
{
	KIpMessengerAttachedFileDialog *afdialog = new KIpMessengerAttachedFileDialog(this,0,TRUE);
	afdialog->setFiles( attachFileList );
	afdialog->exec();
	attachFileList = afdialog->getFiles();
	refreshFiles();
}

/**
 * グループ選択メニューからグループを選択した。
 * ・グループが一致するホストを一括選択する。
 */
void SendDialog::slotGroupSelect( int menu_item )
{
	static int prev_menu = 0;
	if ( prev_menu == menu_item ) {
		prev_menu = 0;
		return;
	}
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( *groupMenuIdList[menu_item] == item->text( ColumnGroup ) ) {
			m_HostListView->setSelected( item, TRUE );
		}
		it++;
	}
	prev_menu = menu_item;
}

/**
 * エンコーディング選択メニューからエンコーディングを選択した。
 * ・エンコーディングが一致するホストを一括選択する。
 */
void SendDialog::slotEncodingSelect( int menu_item )
{
	static int prev_menu = 0;
	if ( prev_menu == menu_item ) {
		prev_menu = 0;
		return;
	}
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( *encodingMenuIdList[menu_item] == item->text( ColumnEncoding ) ) {
			m_HostListView->setSelected( item, TRUE );
		}
		it++;
	}
	prev_menu = menu_item;
}

/**
 * 優先度から削除する。
 */
void SendDialog::deleteFromPriorityList( QStringList &base, QStringList items )
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

/**
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
	deleteFromPriorityList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel1( tmpList );

	tmpList = KIpMsgSettings::priorityLevel2();
	deleteFromPriorityList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel2( tmpList );

	tmpList = KIpMsgSettings::priorityLevel3();
	deleteFromPriorityList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel3( tmpList );

	tmpList = KIpMsgSettings::priorityLevel4();
	deleteFromPriorityList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel4( tmpList );

	tmpList = KIpMsgSettings::priorityLevel4();
	deleteFromPriorityList( tmpList, priList );
	KIpMsgSettings::setPriorityLevel4( tmpList );

	tmpList = KIpMsgSettings::priorityDefault();
	deleteFromPriorityList( tmpList, priList );
	KIpMsgSettings::setPriorityDefault( tmpList );

	tmpList = KIpMsgSettings::priorityHidden();
	deleteFromPriorityList( tmpList, priList );
	KIpMsgSettings::setPriorityHidden( tmpList );
}

/**
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

/**
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

/**
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

/**
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

/**
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

/**
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

/**
 * 非表示を一時解除。
 */
void SendDialog::slotShowHiddenTempClicked()
{
	sortPopup->setItemChecked( showHiddenMenuId, !sortPopup->isItemChecked( showHiddenMenuId ) );
	refreshHostList();
}

/**
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

/**
 * ユーザ検索ダイアログを表示。
 */
void SendDialog::slotSearchUserClicked()
{
	KIpMsgSearchBox *searchbox = new KIpMsgSearchBox();
	searchbox->m_HostListView = m_HostListView;
	searchbox->show();
}

/**
 * ファイル添付。
 */
void SendDialog::slotAttachFileClicked()
{
	QString attachFileName = KFileDialog::getOpenFileName();
	if ( attachFileName != "" ) {
		AttachFile file;
		file.setFullPath( attachFileName.data() );
		file.GetLocalFileInfo();
		attachFileList.AddFile( file );
	}
	refreshFiles();
}

/**
 * ディレクトリ添付。
 */
void SendDialog::slotAttachDirectoryClicked()
{
	QString attachDirName = KFileDialog::getExistingDirectory();
	if ( attachDirName != "" ) {
		AttachFile file;
		file.setFullPath( attachDirName.data() );
		file.GetLocalFileInfo();
		attachFileList.AddFile( file );
	}
	refreshFiles();
}

/**
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

/**
 * ホストリストのフォントを選択。
 */
void SendDialog::slotFontSelectListClicked()
{
	//現在のフォントを取得
	QFont font = m_HostListView->font();
	//現在のフォントでフォントダイアログを表示
	int result = KFontDialog::getFont(font);
	//フォントが変更された
	if ( result == KFontDialog::Accepted ) {
		//フォントをホストリストのビューに反映
		m_HostListView->setFont(font);
		//フォント設定を保存
		KIpMsgSettings::setListFont( m_HostListView->font() );
		KIpMsgSettings::writeConfig();
	}
}

/**
 * 本文のフォントを選択。
 */
void SendDialog::slotFontSelectEditClicked()
{
	//現在のフォントを取得
	QFont font = m_MessageEditbox->font();
	//現在のフォントでフォントダイアログを表示
	int result = KFontDialog::getFont(font);
	//フォントが変更された
	if ( result == KFontDialog::Accepted ) {
		//フォントをメッセージのビューに反映
		m_MessageEditbox->setFont(font);
		//フォント設定を保存
		KIpMsgSettings::setListFont( m_MessageEditbox->font() );
		KIpMsgSettings::writeConfig();
	}
}

/**
 * フォントをデフォルトに設定。
 */
void SendDialog::slotFontRestoreToDefaultClicked()
{
	//フォント未設定状態にする。
	m_HostListView->unsetFont();
	m_MessageEditbox->unsetFont();
	//フォント設定を保存
	KIpMsgSettings::setListFont( m_HostListView->font() );
	KIpMsgSettings::setEditFont( m_MessageEditbox->font() );
	KIpMsgSettings::writeConfig();
}

/**
 * メニュー同期。
 * ・全ての表示中の送信ダイアログのブーリアン状態をそろえる（位置固定、サイズ保存）
 */
void SendDialog::synchronizeMenu()
{
	KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(IpMessengerAgent::GetInstance()->GetEventObject());
	QPtrListIterator<SendDialog> sendIt(evt->GetSendDialogs());

	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( sendDlg->isShown() && sendDlg->winId() != winId() ) {
			sendDlg->setFixsizePotisionMenu();
			sendDlg->setSaveSizeMenu();
		}
		++sendIt;
	}
}

/**
 * サイズ保存。の状態を保存。
 */
void SendDialog::setSaveSizeMenu()
{
	sizePopup->setItemChecked( saveSizeMenuId, KIpMsgSettings::sendDialogSaveSize() );
}

/**
 * サイズ保存。
 */
void SendDialog::slotSaveSizeClicked()
{
	KIpMsgSettings::setSendDialogSaveSize( !KIpMsgSettings::sendDialogSaveSize() );
	setSaveSizeMenu();
	KIpMsgSettings::setSendDialogHeight( width() );
	KIpMsgSettings::setSendDialogWidth( height() );
	KIpMsgSettings::writeConfig();
	synchronizeMenu();
}

/**
 * サイズを一時的に戻す。
 */
void SendDialog::slotRestoreSizeTempClicked()
{
	resize( defaultWidth, defaultHeight );
}

/**
 * 位置固定。の状態を設定。
 */
void SendDialog::setFixsizePotisionMenu()
{
	sendPopup->setItemChecked( fixizePositionMenuId, KIpMsgSettings::sendDialogFixizePosition() );
}

/**
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

/**
 * 表示詳細設定。
 */
void SendDialog::slotViewDetailConfigurationClicked()
{
	KIPMsgShowConfigDialog *showconfig = new KIPMsgShowConfigDialog();
	showconfig->setSendDialog( this );
	showconfig->show();
}

/**
 * ホストリストでのコンテキストメニュー表示要求。
 */
void SendDialog::slotListContextMenuRequested( QListViewItem *item, const QPoint &pos, int col )
{
	setMenuStatus();
	sendPopup->popup( QCursor::pos() );
}

/**
 * エンコーディング設定表示。
 */
void SendDialog::slotEncodingConfigClicked()
{
	KIPMsgEncodingConfigDialog *encconfig = new KIPMsgEncodingConfigDialog(this,0,TRUE);
	encconfig->exec();
	refreshHostList();
}

/**
 * リサイズイベント。
 * @param e リサイズイベント
 */
void SendDialog::resizeEvent( QResizeEvent *e )
{
	doResize( e->size() );
}

/**
 * リサイズ実行（共通）。
 * @param size サイズ
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
	m_MessageFrame->setGeometry( 2, rectMainSplitter.bottom() + 2, size.width() - 4, size.height() - sizeHostFrame.height() - sizeMainSplitter.height() - rectOperationFrame.height() );
	QRect rectm_MessageFrame = m_MessageFrame->geometry();
	refreshFiles();
}

/**
 * ボタンにファイル名を設定。
 */
void SendDialog::refreshFiles()
{
	QTextCodec *codec;
	codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );

 	m_AttachFileButton->setHidden( TRUE );
	if ( attachFileList.size() > 0 ) {
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
		for( vector<AttachFile>::iterator it = attachFileList.begin(); it != attachFileList.end(); it++ ){
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
