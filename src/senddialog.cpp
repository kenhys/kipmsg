/***************************************************************************
 *   Copyright (C) 2006-2009 by nikikuni                                        *
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
#include <kdebug.h>
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
#include "priorityconfig.h"
#include "idiomconfig.h"

static QTextCodec *utf8codec = QTextCodec::codecForName( "UTF-8" );


static int globalUserNameColumnWidth = -1;
static int globalGroupNameColumnWidth = -1;
static int globalHostNameColumnWidth = -1;
static int globalIpAddressColumnWidth = -1;
static int globalLoginNameColumnWidth = -1;
static int globalPriorityColumnWidth = -1;
static int globalEncodingColumnWidth = -1;


class KIpmsgColumnWidthTemporarySaveFilter : public QObject {
	protected:
		bool eventFilter( QObject *object, QEvent *event ){
			if(event->type() == QEvent::MouseMove || event->type() == QEvent::FocusOut ) {
				QWidget *widget = dynamic_cast<QWidget*>(object);
				SendDialog *top = dynamic_cast<SendDialog*>(widget->topLevelWidget());
				if ( top != NULL ){
					top->saveColumnSizeGlobalSession();
				}else{
				}
			}
			return QObject::eventFilter( object, event );
		}
};

class KipmsgPriorityHostItem {
	public:
		QString ipAddress(){ return m_IpAddress;};
		QString loginName(){ return m_LoginName;};
		QString priority() { return m_Priority; };
		void setIpAddress(QString value){ m_IpAddress = value; };
		void setLoginName(QString value){ m_LoginName = value; };
		void setPriority(QString value) { m_Priority  = value; };
	private:
		QString m_IpAddress;
		QString m_LoginName;
		QString m_Priority;
};

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
					utf8codec->toUnicode( host.Priority().c_str() ),
					codec->toUnicode( host.EncodingName().c_str() ) )
	{
		m_host = host;
		
	};
    HostListItem host(){ return m_host; }
};

KIpMsgFileNameConverter::KIpMsgFileNameConverter(){
	encoding = KIpMsgSettings::messageEncoding();
}

KIpMsgFileNameConverter::KIpMsgFileNameConverter( QString _encoding ){
	encoding = _encoding;
}

/**
 * ファイル名コンバータのネットワークエンコーディングからファイルシステムエンコーディングへの変換メソッド。
 * @param ネットワークエンコーディングのファイル名
 * @retval ファイルシステムエンコーディングに変換されたファイル名
 */
string
KIpMsgFileNameConverter::ConvertNetworkToLocal( string original_file_name ){
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QTextCodec *msgCodec = QTextCodec::codecForName( encoding );
	return fsCodec->fromUnicode( msgCodec->toUnicode( original_file_name.c_str() ) ).data();
}
/**
 * ファイル名コンバータのファイルシステムエンコーディングからネットワークエンコーディングへの変換メソッド。
 * @param ファイルシステムエンコーディングのファイル名
 * @retval ネットワークエンコーディングに変換されたファイル名
 */
string
KIpMsgFileNameConverter::ConvertLocalToNetwork( string original_file_name ){
	QTextCodec *msgCodec = QTextCodec::codecForName( encoding );
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
 * キー押下時の処理。
 * ・CTRL+ENTER時は無視して上位ウィジットに通知する。
 */
void KTextEditNoDnD::keyPressEvent( QKeyEvent * e )
{
	bool isCtrl = e->state() & ControlButton;
	bool isShift = e->state() & ShiftButton;
	bool isMeta = e->state() & MetaButton;
	bool isAlt = e->state() & AltButton;

	if ( isCtrl && !isShift && !isAlt && !isMeta ) {
		if ( e->key() == Key_Return ) {
			e->ignore();
			return;
		}
	}
	KTextEdit::keyPressEvent( e );
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

	if(globalUserNameColumnWidth < 0){
		globalUserNameColumnWidth = KIpMsgSettings::userNameColumnWidth();
		globalGroupNameColumnWidth = KIpMsgSettings::groupNameColumnWidth();
		globalHostNameColumnWidth = KIpMsgSettings::hostNameColumnWidth();
		globalIpAddressColumnWidth = KIpMsgSettings::ipAddressColumnWidth();
		globalLoginNameColumnWidth = KIpMsgSettings::loginNameColumnWidth();
		globalPriorityColumnWidth = KIpMsgSettings::priorityColumnWidth();
		globalEncodingColumnWidth = KIpMsgSettings::encodingColumnWidth();
kdDebug() << "Column width variables initialized." << endl;
	}

	defaultX = x();
	defaultY = y();
	defaultWidth = width();
	defaultHeight = height();

	isShowHiddenTemp = false;
	recvDialog = NULL;
	setAcceptDrops( TRUE );

    m_MessageEditbox = new KTextEditNoDnD( m_MessageFrame, "m_MessageEditbox" );
    m_MessageEditbox->setGeometry( QRect( 0, 40, 526, 190 ) );
    m_MessageEditbox->setMinimumSize( QSize( 0, 140 ) );
    m_MessageEditbox->setCursor( QCursor( 4 ) );
    m_MessageEditbox->setMouseTracking( FALSE );
    m_MessageEditbox->setAcceptDrops( FALSE );

	m_MessageEditbox->setAcceptDrops( TRUE );

	m_HostListView->setSortColumn( ColumnMax );
	m_HostListView->setFont( KIpMsgSettings::listFont() );
	m_MessageEditbox->setFont( KIpMsgSettings::editFont() );

	QObject *saveFilter = new KIpmsgColumnWidthTemporarySaveFilter();
	m_HostListView->installEventFilter(saveFilter);

//	connect( this, SIGNAL( reject() ), this, SLOT( slotReject() ) );
//	connect( this, SIGNAL( accept() ), this, SLOT( slotAccept() ) );

    sendPopup = new KPopupMenu(this);

	sortPopup = new KPopupMenu(this);

	//設定を移行する。
	QPtrList<KipmsgPriorityHostItem> list;
	QStringList loadList = KIpMsgSettings::priorityHostList();
	convertStringListToPriorityHostList( loadList, list );

	QStringList tmpList = KIpMsgSettings::priorityLevel1();
	changePriorityHostList( QString( "1" ), list, tmpList );
	tmpList = KIpMsgSettings::priorityLevel2();
	changePriorityHostList( QString( "2" ), list, tmpList );
	tmpList = KIpMsgSettings::priorityLevel3();
	changePriorityHostList( QString( "3" ), list, tmpList );
	tmpList = KIpMsgSettings::priorityLevel4();
	changePriorityHostList( QString( "4" ), list, tmpList );
	tmpList = KIpMsgSettings::priorityDefault();
	changePriorityHostList( QString( "-" ), list, tmpList );
	tmpList = KIpMsgSettings::priorityHidden();
	changePriorityHostList( QString( "X" ), list, tmpList );

	QStringList saveList;
	convertPriorityHostListToStringList( list, saveList );
	KIpMsgSettings::setPriorityHostList( saveList );

	QStringList nullList;
	KIpMsgSettings::setPriorityLevel1( nullList );
	KIpMsgSettings::setPriorityLevel2( nullList );
	KIpMsgSettings::setPriorityLevel3( nullList );
	KIpMsgSettings::setPriorityLevel4( nullList );
	KIpMsgSettings::setPriorityDefault( nullList );
	KIpMsgSettings::setPriorityHidden( nullList );
	KIpMsgSettings::writeConfig();

	sendPopup->insertItem(SmallIcon("filter"),tr2i18n("Sort Filter"), sortPopup );

	sendPopup->insertItem(SmallIcon("configure"),tr2i18n("Priority Configuration"), this, SLOT( slotPriorityConfigClicked( void ) ) );

	priorityPopup = new KPopupMenu(this);
	sendPopup->insertItem(tr2i18n("Priority Select"), priorityPopup );

	groupPopup = new KPopupMenu(this);
	sendPopup->insertItem(tr2i18n("Group Select"), groupPopup );

	encodingPopup = new KPopupMenu(this);
	sendPopup->insertItem(SmallIcon("charset"), tr2i18n("Encoding Select"), encodingPopup );

	sendPopup->insertItem(SmallIcon("configure"), tr2i18n("Configure Encoding..."), this, SLOT( slotEncodingConfigClicked( void ) ) );

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

	getVersionInfoMenuId = sendPopup->insertItem(SmallIcon("kipmsg_about"),tr2i18n("Get version information"), this, SLOT( slotGetVersionInfoClicked( void ) ) );
	getAbsenceInfoMenuId = sendPopup->insertItem(SmallIcon("kipmsg_rev"),tr2i18n("Get absence information"), this, SLOT( slotGetAbsenceInfoClicked( void ) ) );

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
	}else{
		resize( defaultWidth, defaultHeight );
	}
	m_EncodingCombobox->setCurrentText( KIpMsgSettings::messageEncoding() );

    idiomPopup = new KPopupMenu(this);

	idiomTextPopup = new KPopupMenu(this);
	idiomPopup->insertItem(SmallIcon("text"),tr2i18n("Insert idiom to textbox."), idiomTextPopup );
	idiomPopup->insertSeparator();
	addIdiomMenuId = idiomPopup->insertItem(SmallIcon("text"),tr2i18n("Add idiom from selected text."), this, SLOT( slotAddIdiomFromSelectedText( void ) ) );
	idiomPopup->insertItem(SmallIcon("configure"), tr2i18n("Configure Idiom..."), this, SLOT( slotIdiomConfigClicked( void ) ) );

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

	QIntDictIterator<QString> itp( priorityHostMenuIdList );
	while ( itp.current() != NULL ) {
		sortPopup->setItemEnabled( itp.currentKey(), sel_count != 0 );
		++itp;
	}
	sendPopup->setItemEnabled( getVersionInfoMenuId, sel_count != 0 );
	sendPopup->setItemEnabled( getAbsenceInfoMenuId, sel_count != 0 );
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
	} else 
	if(e->button() == LeftButton ){
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
void SendDialog::mouseMoveEvent(QMouseEvent */*e*/){
	if ( isMainSplitterDragging ){
		QRect rectDlg = geometry();
		QRect rectSplitter = m_MainSplitter->geometry();
		QSize sizeSplitter = m_MainSplitter->size();
		QSize sizeFrameOperation = m_OperationFrame->size();
		QSize sizeHostFrame = m_HostFrame->size();

		QSize minsizeHostFrame = m_HostFrame->minimumSize();
		QSize minsizeMessageFrame = m_MessageFrame->minimumSize();

		QSize sizeDlg = size();
		int msgHeight = sizeDlg.height() - sizeHostFrame.height() - sizeSplitter.height() - sizeFrameOperation.height();
		QPoint pos = QCursor::pos();
		int localY = pos.y() - rectDlg.top();
		int msgAfterHeight = sizeDlg.height() - localY - sizeSplitter.height() - sizeFrameOperation.height();
		int prevTop = rectSplitter.top();
		if ( prevTop < minsizeHostFrame.height() || localY < minsizeHostFrame.height() ) {
			;
		} else if ( msgAfterHeight < minsizeMessageFrame.height() || msgHeight < minsizeMessageFrame.height() ) {
			;
		} else {
			rectSplitter.setTop( localY );
		}
		m_MainSplitter->setGeometry( rectSplitter );
	}
	doResize( size() );
}

/**
 * キー押下時の処理。
 * ・CTRL+ENTER時は送信する。
 */
void SendDialog::keyPressEvent( QKeyEvent * e )
{
	bool isCtrl = e->state() & ControlButton;
	bool isShift = e->state() & ShiftButton;
	bool isMeta = e->state() & MetaButton;
	bool isAlt = e->state() & AltButton;

	if ( isCtrl && !isShift && !isAlt && !isMeta ) {
		if ( e->key() == Key_Return ) {
			slotMessageSendClicked();
			return;
		}
	}
	SendDialogBase::keyPressEvent( e );
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
	for( uint i = 0 ; i < url.size(); ){
		int c = url.at( i );
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
	QString dropText("");

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
	QStringList dropFileList = QStringList::split("\r\n", fileUrl.data() );
	QString dropFileNames("");
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

/**
 * クローズ時の処理。
 * ・下書き保存。
 */
void SendDialog::closeEvent( QCloseEvent * e )
{
	if ( m_MessageEditbox->text() != "" && !isAccept ) {
		int answer = KMessageBox::warningYesNoCancel( 0, tr2i18n( "Do you want to save the message for later or discard it?" ),
														 "",
														 KGuiItem( tr2i18n( "&Save as drafts" ), "filesave" ),
														 tr2i18n( "&Discard message" ) );
		if ( answer == KMessageBox::Yes ){
//kdDebug() << "closeEvent()3 保存" << endl;
			QStringList draftList = KIpMsgSettings::drafts();
			draftList << m_MessageEditbox->text();
			KIpMsgSettings::setDrafts( draftList );
			KIpMsgSettings::writeConfig();
		} else if ( answer == KMessageBox::Cancel ){
			return;
		}
	}
	e->accept();
}

void SendDialog::accept()
{
	isAccept = TRUE;
	close();
}

void SendDialog::reject()
{
	isAccept = FALSE;
	close();
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

	//メッセージの文字化けチェック
	QTextCodec *codec = QTextCodec::codecForName( m_EncodingCombobox->currentText() );
	string msg = codec->fromUnicode( m_MessageEditbox->text() ).data();
	if ( isGarbledMessage( m_EncodingCombobox->currentText(), msg ) ) {
		if ( KMessageBox::warningContinueCancel( 0, QString( tr2i18n("This message is garbled to read.\nAre you sure?") ) ) != KMessageBox::Continue ){
			return;
		}
	}

	//ファイル名の文字化けチェック
	KIpMsgFileNameConverter *fconv = new KIpMsgFileNameConverter( m_EncodingCombobox->currentText() );
	for( vector<AttachFile>::iterator file = attachFileList.begin(); file != attachFileList.end(); file++ ) {
		string networkFileName = fconv->ConvertLocalToNetwork( file->FileName() );
		if ( isGarbledMessage( file->FileName().c_str(), networkFileName ) ) {
			QString fname = QString::fromLocal8Bit( file->FileName().c_str() );
			if ( KMessageBox::warningContinueCancel( 0, tr2i18n("This file name is garbled to read.\n(%1)\nAre you sure?").arg( fname ) ) != KMessageBox::Continue ){
				return;
			}
		}
	}

	//送信対象ホストリストを基にメッセージを送信する。
	agent->SetFileNameConverter( fconv );
	for( vector<HostListItem>::iterator host = targets.begin(); host != targets.end(); host++ ) {
		agent->SendMsg( *host, msg,
						ipmsg::Secret( m_SecretCheckbox->isChecked() ),
						attachFileList,
						ipmsg::LockPassword( m_LockCheckbox->isChecked() ),
						targets.size() );
		QString ip = codec->toUnicode( host->IpAddress().c_str() );
		QString login = codec->toUnicode( host->UserName().c_str() );
		for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
			QStringList fields = QStringList::split( "|", *ite );
			if ( ip == fields[0] && 
				login == fields[1] ) {
				encodings.remove( ite );
				break;
			}
		}
		encodings << ip + "|" + login + "|" + m_EncodingCombobox->currentText();
		send_count++;
	}
	//エンコーディングの設定を更新する。
	if ( send_count > 0 ) {
		KIpMsgSettings::setEncodingSettings( encodings );
		KIpMsgSettings::writeConfig();
		if ( recvDialog != NULL && recvDialog->isShown() && !KIpMsgSettings::noHide() ) {
			recvDialog->close();
		}
		accept();
	}
}

/**
 * メッセージ文字化けするかどうかを判定する。
 * @retval true:文字化けしている。false:文字化けしていない。
 */
bool SendDialog::isGarbledMessage( QString beforeText, string afterText )
{
	int bqpos = beforeText.find( '?' );
	int bqcnt = 0;
	while( bqpos >= 0 ){
		bqcnt++;
		bqpos = beforeText.find( '?', bqpos + 1 );
	}
	int aqpos = afterText.find( '?' );
	int aqcnt = 0;
	while( aqpos >= 0 ){
		aqcnt++;
		aqpos = afterText.find( '?', aqpos + 1 );
	}
	if ( bqcnt != aqcnt ) {
		return true;
	}
	return false;
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
 * 定型文ボタンが押された。
 * ・定型文メニューを表示
 */
void SendDialog::slotIdiomButtonClicked()
{
	QSize size = m_IdiomButton->size();
	QPoint pos = m_IdiomButton->mapToGlobal( QPoint( 0, size.height() ) );

	idiomTextPopup->clear();
	idiomTextMenuIdList.clear();
	idiomTextMenuIdList.setAutoDelete( TRUE );
	QStringList idiomList = KIpMsgSettings::idiomSettings();
	QStringList::Iterator itIdiom = idiomList.begin();
	while ( itIdiom != idiomList.end() ) {
		QStringList lines = QStringList::split( "\n", *itIdiom );
		int menu_item;
		if ( lines.size() == 1 ) {
			menu_item = idiomTextPopup->insertItem( *itIdiom );
		} else {
			menu_item = idiomTextPopup->insertItem( lines[0] + " ..." );
		}
		connect( idiomTextPopup, SIGNAL( activated(int) ), this, SLOT( slotInsertSelectedIdiom( int ) ) );
		idiomTextMenuIdList.insert( menu_item, new QString( *itIdiom ) );
		itIdiom++;
	}
	idiomPopup->setItemEnabled( addIdiomMenuId, m_MessageEditbox->hasSelectedText() );
	idiomPopup->popup( pos );
}

/**
 * 定型文をメッセージのカーソル位置に挿入する。
 * @param menu_item 選択されたメニューID
 */
void SendDialog::slotInsertSelectedIdiom( int menu_item )
{
	static int prev_menu = 0;
	//再入禁止
	if ( prev_menu == menu_item ) {
//		prev_menu = 0;
		return;
	}
	if ( !idiomTextMenuIdList.find( menu_item ) ) {
		return;
	}

	m_MessageEditbox->insert( *idiomTextMenuIdList[menu_item] );
	prev_menu = menu_item;
}

/**
 * メッセージ選択範囲を定型文に追加する。
 */
void SendDialog::slotAddIdiomFromSelectedText()
{
	QString selStr = m_MessageEditbox->selectedText();
	if ( selStr == "" ) {
		return;
	}
	QStringList idiomList = KIpMsgSettings::idiomSettings();
	QStringList::iterator itIdiom = idiomList.begin();
	while ( itIdiom != idiomList.end() ) {
		if ( *itIdiom == selStr ) {
			return;
		}
		itIdiom++;
	}
	idiomList << selStr;
	KIpMsgSettings::setIdiomSettings( idiomList );
	KIpMsgSettings::writeConfig();
}

/**
 * 定型文を設定する。
 * ・定型文設定ダイアログを表示する。
 */
void SendDialog::slotIdiomConfigClicked()
{
	IdiomConfigDialog *dlg = new IdiomConfigDialog(this,0,TRUE);
	dlg->exec();
}

/**
 * ホストリストをリフレッシュする。
 * ・ホストリストをリフレッシュ
 * @param isUpdate TRUE:ネットワークから。FALSE:エージェントの内部変数から。
 */
void SendDialog::refreshHostList( bool isUpdate )
{
kdDebug() << "refreshHostList(" << isUpdate << ") called." << endl;
	//選択状態を保存して
	QListViewItemIterator it( m_HostListView );
	QStringList saveSelectedValues;
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item->isSelected() ) {
			saveSelectedValues << item->text( ColumnIpAddress ) + "|" + item->text( ColumnLogin );
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
	m_HostListView->addColumn( tr2i18n( "User" ), globalUserNameColumnWidth );
kdDebug() << "globalUserNameColumnWidth=" << globalUserNameColumnWidth << endl;
	if ( KIpMsgSettings::showGroupName() ) {
		m_HostListView->addColumn( tr2i18n("Group"), globalGroupNameColumnWidth );
kdDebug() << "globalGroupNameColumnWidth=" << globalGroupNameColumnWidth << endl;
	} else {
		int col = m_HostListView->addColumn( tr2i18n("Group"), 0 );
		m_HostListView->hideColumn( col );
kdDebug() << "hideColumn" << col << endl;
	}
	if ( KIpMsgSettings::showHostName() ) {
		m_HostListView->addColumn( tr2i18n("Host"), globalHostNameColumnWidth );
kdDebug() << "globalHostNameColumnWidth=" << globalHostNameColumnWidth << endl;
	} else {
		int col = m_HostListView->addColumn( tr2i18n("Host"), 0 );
		m_HostListView->hideColumn( col );
kdDebug() << "hideColumn" << col << endl;
	}
	if ( KIpMsgSettings::showIpAddress() ) {
		m_HostListView->addColumn( tr2i18n("IP address"), globalIpAddressColumnWidth );
kdDebug() << "globalIpAddressColumnWidth=" << globalIpAddressColumnWidth << endl;
	} else {
		int col = m_HostListView->addColumn( tr2i18n("IP address"), 0 );
		m_HostListView->hideColumn( col );
kdDebug() << "hideColumn" << col << endl;
	}
	if ( KIpMsgSettings::showLoginName() ) {
		m_HostListView->addColumn( tr2i18n("Login"), globalLoginNameColumnWidth );
kdDebug() << "globalLoginNameColumnWidth=" << globalLoginNameColumnWidth << endl;
	} else {
		int col = m_HostListView->addColumn( tr2i18n("Login"), 0 );
		m_HostListView->hideColumn( col );
kdDebug() << "hideColumn" << col << endl;
	}
	if ( KIpMsgSettings::showPriority() ) {
		m_HostListView->addColumn( tr2i18n("Priority"), globalPriorityColumnWidth );
kdDebug() << "globalPriorityColumnWidth=" << globalPriorityColumnWidth << endl;
	} else {
		int col = m_HostListView->addColumn( tr2i18n("Priority"), 0 );
		m_HostListView->hideColumn( col );
kdDebug() << "hideColumn" << col << endl;
	}
	if ( KIpMsgSettings::showEncoding() ) {
		m_HostListView->addColumn(tr2i18n("Encoding"), globalEncodingColumnWidth );
kdDebug() << "globalEncodingColumnWidth=" << globalEncodingColumnWidth << endl;
	} else {
		int col = m_HostListView->addColumn(tr2i18n("Encoding"), 0 );
		m_HostListView->hideColumn( col );
kdDebug() << "hideColumn" << col << endl;
	}
//	m_HostListView->addColumn(tr2i18n("Encryption Flags"), 0 );
//	m_HostListView->addColumn(tr2i18n("RSA Method"), 0 );
//	m_HostListView->addColumn(tr2i18n("RSA Public Key"), 0 );
//	m_HostListView->hideColumn( ColumnEncryptionCapacity );
//	m_HostListView->hideColumn( ColumnRsaMethod );
//	m_HostListView->hideColumn( ColumnRsaPublicKey );

	//ヘッダー順序を設定
	m_HostListView->setSortColumn( ColumnMax );
	QHeader *header = m_HostListView->header();
	header->moveSection( ColumnUser, KIpMsgSettings::userNameColumnIndex() );
	header->moveSection( ColumnGroup, KIpMsgSettings::groupNameColumnIndex() );
	header->moveSection( ColumnHost, KIpMsgSettings::hostNameColumnIndex() );
	header->moveSection( ColumnIpAddress, KIpMsgSettings::ipAddressColumnIndex() );
	header->moveSection( ColumnLogin, KIpMsgSettings::loginNameColumnIndex() );
	header->moveSection( ColumnPriority, KIpMsgSettings::priorityColumnIndex() );
	header->moveSection( ColumnEncoding, KIpMsgSettings::encodingColumnIndex() );

	bool hideEncryptionUnsupported = KIpMsgSettings::hideEncryptionNotSupportedHost();
	QStringList encodings = KIpMsgSettings::encodingSettings();
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
		if ( hideEncryptionUnsupported && !ix->IsEncryptSupport() ) {
			continue;
		}

		//設定によって表示内容を変更する
		QStringList values;
		QTextCodec *codec;
		for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
			QStringList fields = QStringList::split( "|", *ite );
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
		QString pitem = string(ix->IpAddress() + "|" + ix->UserName()).c_str();
		//TODO
		QPtrList<KipmsgPriorityHostItem> list;
		QStringList loadList;
		if ( KIpMsgSettings::customizePriority() ) {
			loadList = KIpMsgSettings::customizedPriorityHostList();
		} else {
			loadList = KIpMsgSettings::priorityHostList();
		}
		convertStringListToPriorityHostList( loadList, list );
		QPtrListIterator<KipmsgPriorityHostItem> it( list );
		KipmsgPriorityHostItem *item;
		while( ( item = it.current() ) != 0 ) {
			if ( pitem == item->ipAddress() +"|" + item->loginName() ) {
				ix->setPriority( utf8codec->fromUnicode( item->priority() ).data() );
				break;
			}
			++it;
		}
		if ( ix->Priority() == "" ) {
			ix->setPriority( "-" );
		}
		//アイコンの設定
		if ( ix->Priority() != "X" || isShowHiddenTemp ) {
			KIpMsgHostListViewItem *item = new KIpMsgHostListViewItem( m_HostListView, codec, *ix );
			if ( ix->IsAbsence() ) {
				if ( ix->IsFileAttachSupport() && !ix->IsEncryptSupport() ){
					item->setPixmap( 0 ,SmallIcon("kipmsg_fileabs") );
				} else if ( !ix->IsFileAttachSupport() && !ix->IsEncryptSupport() ){
					item->setPixmap( 0 ,SmallIcon("kipmsg_v1abs") );
				} else {
					item->setPixmap( 0 ,SmallIcon("kipmsg_absence") );
				}
			} else {
				if ( ix->IsFileAttachSupport() && !ix->IsEncryptSupport() ){
					item->setPixmap( 0 ,SmallIcon("kipmsg_file") );
				} else if ( !ix->IsFileAttachSupport() && !ix->IsEncryptSupport() ){
					item->setPixmap( 0 ,SmallIcon("kipmsg_v1") );
				} else {
					item->setPixmap( 0 ,SmallIcon("kipmsg_normal") );
				}
			}
		}
	}
	//件数を表示
	m_UserCountLCD->display( (int)hosts.size() );
	//選択状態をリストア
	QListViewItemIterator its( m_HostListView );
	while ( its.current() != NULL ) {
		QListViewItem *item = its.current();
		for( uint i = 0; i < saveSelectedValues.count(); i++ ) {
			if ( item->text( ColumnIpAddress ) + "|" + item->text( ColumnLogin ) == saveSelectedValues[i] ) {
				item->setSelected( TRUE );
				break;
			}
		}
		++its;
	}
	//優先度選択メニュー再構築
	sortPopup->clear();
	priorityHostMenuIdList.clear();
	priorityHostMenuIdList.setAutoDelete( TRUE );
	QStringList priList;
	if ( KIpMsgSettings::customizePriority() ) {
		priList = KIpMsgSettings::prioritySettings();
	} else {
		priList << "1";
		priList << "2";
		priList << "3";
		priList << "4";
	}

	QStringList::Iterator itPri = priList.begin();
	while ( itPri != priList.end() ) {
		int menu_item = sortPopup->insertItem( tr2i18n("Move to Priority %1").arg( *itPri ) );
		connect( sortPopup, SIGNAL( activated(int) ), this, SLOT( slotPriorityMove( int ) ) );
		priorityHostMenuIdList.insert( menu_item, new QString( *itPri ) );
		itPri++;
	}
	int moveToDefaultMenuId = sortPopup->insertItem( SmallIcon("move"),tr2i18n("Move to default") );
	connect( sortPopup, SIGNAL( activated(int) ), this, SLOT( slotPriorityMove( int ) ) );
	priorityHostMenuIdList.insert( moveToDefaultMenuId, new QString( "-" ) );
	int moveToHiddenMenuId = sortPopup->insertItem( SmallIcon("move"),tr2i18n("Move to hidden") );
	connect( sortPopup, SIGNAL( activated(int) ), this, SLOT( slotPriorityMove( int ) ) );
	priorityHostMenuIdList.insert( moveToHiddenMenuId, new QString( "X" ) );
	sortPopup->insertSeparator();
	showHiddenMenuId = sortPopup->insertItem(tr2i18n("Show Hidden(temporary)"), this, SLOT( slotShowHiddenTempClicked( void ) ) );
	sortPopup->setItemChecked( showHiddenMenuId, FALSE );
	sortPopup->insertItem(SmallIcon("undo"),tr2i18n("Restore All to default"), this, SLOT( slotRestoreAllClicked( void ) ) );

	//グループ選択メニュー再構築
	groupPopup->clear();
	groupMenuIdList.clear();
	groupMenuIdList.setAutoDelete( TRUE );
	vector<GroupItem> groups = agent->GetGroupList();
	for( vector<GroupItem>::iterator ixgr = groups.begin(); ixgr != groups.end();ixgr++ ){
		if ( ixgr->GroupName() != "" ) {
			QTextCodec *codec = QTextCodec::codecForName( ixgr->EncodingName().c_str() );
			if ( codec == NULL ) {
				codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
			}
			int menu_item = groupPopup->insertItem( codec->toUnicode( QString( ixgr->GroupName().c_str() ) ) );
			connect( groupPopup, SIGNAL( activated(int) ), this, SLOT( slotGroupSelect( int ) ) );
			groupMenuIdList.insert( menu_item, new QString( codec->toUnicode( QString( ixgr->GroupName().c_str() ) ) ) );
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
	//優先度選択メニュー再構築
	priorityPopup->clear();
	priorityMenuIdList.clear();
	priorityMenuIdList.setAutoDelete( TRUE );
	it = QListViewItemIterator( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		QString pri = item->text( ColumnPriority );
		if ( pri != "" ) {
			bool isFound = false;
			QIntDictIterator<QString> ite( priorityMenuIdList );
			while ( ite.current() != NULL ) {
				if ( *ite == pri ){
					isFound = true;
					break;
				}
				++ite;
			}
			if ( !isFound ) {
				int menu_item;
				if ( isShowHiddenTemp && pri == "X") {
					menu_item = priorityPopup->insertItem( tr2i18n( "Hidden" ) );
				} else if ( pri == "-") {
					menu_item = priorityPopup->insertItem( tr2i18n( "Default" ) );
				} else {
					menu_item = priorityPopup->insertItem( pri );
				}
				connect( priorityPopup, SIGNAL( activated(int) ), this, SLOT( slotPrioritySelect( int ) ) );
				priorityMenuIdList.insert( menu_item, new QString( pri ) );
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
	//再入禁止
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
	//再入禁止
	if ( prev_menu == menu_item ) {
		prev_menu = 0;
		return;
	}
	QListViewItemIterator it( m_HostListView );
	int count=0;
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( *encodingMenuIdList[menu_item] == item->text( ColumnEncoding ) ) {
			m_HostListView->setSelected( item, TRUE );
		}
		it++;
		count++;
	}
	if ( count > 0 ) {
		m_EncodingCombobox->setCurrentText( *encodingMenuIdList[menu_item] );
	}
	prev_menu = menu_item;
}

/**
 * 優先度選択メニューから優先度を選択した。
 * ・優先度が一致するホストを一括選択する。
 */
void SendDialog::slotPrioritySelect( int menu_item )
{
	static int prev_menu = 0;
	//再入禁止
	if ( prev_menu == menu_item ) {
		prev_menu = 0;
		return;
	}
	QListViewItemIterator it( m_HostListView );
	int count=0;
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( *priorityMenuIdList[menu_item] == item->text( ColumnPriority ) ) {
			m_HostListView->setSelected( item, TRUE );
		}
		it++;
		count++;
	}
	prev_menu = menu_item;
}

/**
 * 優先度移動メニューを選択した。
 * ・選択中のホストを指定された優先度に移動する。
 */
void SendDialog::slotPriorityMove(int menu_item )
{
	static int prev_menu = 0;
	//再入禁止
	if ( prev_menu == menu_item ) {
		return;
	}
	if ( !priorityHostMenuIdList.find( menu_item ) ) {
		return;
	}
	QStringList priList;
	setPriority( *priorityHostMenuIdList[menu_item], priList );
	KIpMsgSettings::writeConfig();
	refreshHostList();
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(agent->GetEventObject());
	evt->RefreshHostListInAllSendDlg();
	prev_menu = menu_item;
}

/**
 * 優先度設定メニューを選択した。
 * ・優先度設定画面を表示する。
 */
void SendDialog::slotPriorityConfigClicked()
{
	PriorityConfigForm *priconfig = new PriorityConfigForm(this,0,TRUE);
	priconfig->exec();

	if ( KIpMsgSettings::customizePriority() ) {
		QPtrList<KipmsgPriorityHostItem> list;
		QStringList loadList = KIpMsgSettings::customizedPriorityHostList();
		convertStringListToPriorityHostList( loadList, list );
		QPtrListIterator<KipmsgPriorityHostItem> it( list );
		KipmsgPriorityHostItem *item;
		QStringList priorities = KIpMsgSettings::prioritySettings();

		while( ( item = it.current() ) != 0 ) {
//kdDebug() << "item->priority()" << item->priority() << endl;
			bool isFound = false;
			for( uint i = 0; i < priorities.count(); i++ ){
//kdDebug() << "priorities[" << i << "]=" << priorities[i] << endl;
				if ( item->priority() == priorities[i] ) {
//kdDebug() << "found" << endl;
					isFound = true;
					break;
				}
			}
			if ( !isFound ) {
//kdDebug() << "set before item->priority()" << item->priority() << endl;
				item->setPriority( "-" );
//kdDebug() << "set after  item->priority()" << item->priority() << endl;
			}
			++it;
		}
		QStringList saveList;
		convertPriorityHostListToStringList( list, saveList );
		KIpMsgSettings::setCustomizedPriorityHostList( saveList );
	}

	refreshHostList();
}

/**
 * ホストリストをQStringList(設定ファイルの形式)に変換する。
 * ・保存形式のQStringListに変換。
 * @param list ホストリスト
 * @param priList 変換後の文字列リスト
 */
void SendDialog::convertPriorityHostListToStringList( QPtrList<KipmsgPriorityHostItem> &list, QStringList &priList )
{
	QPtrListIterator<KipmsgPriorityHostItem> it( list );
	KipmsgPriorityHostItem *item;
	while( ( item = it.current() ) != 0 ) {
		priList << item->priority() + "|" + item->ipAddress() + "|" + item->loginName();
		++it;
	}
}

/**
 * 指定優先度にQStringListの内容をホストリストに設定する。
 * ・存在しない場合追加存在する場合変更。
 * @param pri 設定する優先度文字列
 * @param list 変更されるホストリスト
 * @param priList 画面から選択されたホストの情報（IPアドレス:ログイン名）の文字列リスト
 */
void SendDialog::changePriorityHostList( QString pri, QPtrList<KipmsgPriorityHostItem> &list, QStringList &priList )
{
	for( QStringList::iterator its = priList.begin(); its!= priList.end(); its++ ) {
		QPtrListIterator<KipmsgPriorityHostItem> it( list );
		KipmsgPriorityHostItem *item;
		bool isFound = false;
		while( ( item = it.current() ) != 0 ) {
			if ( item->ipAddress() + "|" + item->loginName() == *its ) {
				item->setPriority( pri );
				isFound = true;
				break;
			}
			++it;
		}
		if ( !isFound ) {
			KipmsgPriorityHostItem tmpItem;
			tmpItem.setPriority( pri );
			QStringList hList = QStringList::split( "|", *its );
			if ( hList.count() == 2 ){
				if ( hList[0].isEmpty() ) {
					continue;
				}
				if ( hList[1].isEmpty() ) {
					continue;
				}
				tmpItem.setIpAddress( hList[0] );
				tmpItem.setLoginName( hList[1] );
				KipmsgPriorityHostItem *item = new KipmsgPriorityHostItem( tmpItem );
				list.append( item );
			}
		}
	}
}

/**
 * QStringList(設定ファイルの形式)をホストリストに変換する。
 * ・QPtrListに変換。
 * @param loadList 設定ファイルから取得した文字列リスト
 * @param list 変換後のホストリスト
 */
void SendDialog::convertStringListToPriorityHostList( QStringList &loadList, QPtrList<KipmsgPriorityHostItem> &list )
{
	for( QStringList::iterator it = loadList.begin(); it != loadList.end(); it++ ) {
		KipmsgPriorityHostItem tmpItem;
		QStringList pList = QStringList::split( "|", *it );
		if ( pList.count() >= 3 ){
			QString p = "";
			for( uint i = 0; i < pList.count() - 2; i++ ){
				if ( i != 0 ) {
					p += "|";
				}
				p += pList[i];
			}
			if ( p.isEmpty() ) {
				continue;
			}
			tmpItem.setPriority( p );

			if ( pList[pList.count() - 2].isEmpty() ) {
				continue;
			}
			tmpItem.setIpAddress( pList[pList.count() - 2] );
			if ( pList[pList.count() - 1].isEmpty() ) {
				continue;
			}
			tmpItem.setLoginName( pList[pList.count() - 1] );
			KipmsgPriorityHostItem *item = new KipmsgPriorityHostItem( tmpItem );
			list.append( item );
		}
	}
}

/**
 * 優先度を設定する。
 */
void SendDialog::setPriority( QString pri, QStringList &priList )
{
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item->isSelected() ) {
			item->setText( ColumnPriority, pri.data() );
			priList << item->text( ColumnIpAddress ) + "|" + item->text( ColumnLogin );
		}
		++it;
	}
	QStringList tmpList;
	QPtrList<KipmsgPriorityHostItem> list;
	QStringList loadList;
	
	if ( KIpMsgSettings::customizePriority() ) {
		loadList = KIpMsgSettings::customizedPriorityHostList();
	} else {
		loadList = KIpMsgSettings::priorityHostList();
	}
	convertStringListToPriorityHostList( loadList, list );
	changePriorityHostList( pri, list, priList );
	QStringList saveList;
	convertPriorityHostListToStringList( list, saveList );
	if ( KIpMsgSettings::customizePriority() ) {
		KIpMsgSettings::setCustomizedPriorityHostList( saveList );
	} else {
		KIpMsgSettings::setPriorityHostList( saveList );
	}
}


/**
 * 非表示を一時解除。
 */
void SendDialog::slotShowHiddenTempClicked()
{
	sortPopup->setItemChecked( showHiddenMenuId, !sortPopup->isItemChecked( showHiddenMenuId ) );
	isShowHiddenTemp = sortPopup->isItemChecked( showHiddenMenuId );
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
	if ( KIpMsgSettings::customizePriority() ) {
		KIpMsgSettings::setCustomizedPriorityHostList( emptyList );
	} else {
		KIpMsgSettings::setPriorityHostList( emptyList );
	}
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
		QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
		file.setFullPath( fsCodec->fromUnicode( attachFileName ).data() );
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
		QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
		file.setFullPath( fsCodec->fromUnicode( attachDirName ).data() );
		file.GetLocalFileInfo();
		attachFileList.AddFile( file );
	}
	refreshFiles();
}

/**
 * ヘッダ保存。
 */
void SendDialog::saveColumnSizeGlobalSession()
{
kdDebug() << "saveColumnSizeGlobalSession called." << endl;
	QHeader *header = m_HostListView->header();

	KIpMsgSettings::setUserNameColumnIndex( 0 );
	if ( m_HostListView->columnWidth( 0 ) > 0 ) {
		globalUserNameColumnWidth =  m_HostListView->columnWidth( 0 );
	}
	KIpMsgSettings::setGroupNameColumnIndex( header->mapToIndex( 1 ) );
	if ( m_HostListView->columnWidth( 1 ) > 0 ) {
		globalGroupNameColumnWidth = m_HostListView->columnWidth( 1 );
	}
	KIpMsgSettings::setHostNameColumnIndex( header->mapToIndex( 2 ) );
	if ( m_HostListView->columnWidth( 2 ) > 0 ) {
		globalHostNameColumnWidth = m_HostListView->columnWidth( 2 );
	}
	KIpMsgSettings::setIpAddressColumnIndex( header->mapToIndex( 3 ) );
	if ( m_HostListView->columnWidth( 3 ) > 0 ) {
		globalIpAddressColumnWidth = m_HostListView->columnWidth( 3 );
	}
	KIpMsgSettings::setLoginNameColumnIndex( header->mapToIndex( 4 ) );
	if ( m_HostListView->columnWidth( 4 ) > 0 ) {
		globalLoginNameColumnWidth = m_HostListView->columnWidth( 4 );
	}
	KIpMsgSettings::setPriorityColumnIndex( header->mapToIndex( 5 ) );
	if ( m_HostListView->columnWidth( 5 ) > 0 ) {
		globalPriorityColumnWidth = m_HostListView->columnWidth( 5 );
	}
	KIpMsgSettings::setEncodingColumnIndex( header->mapToIndex( 6 ) );
	if ( m_HostListView->columnWidth( 6 ) > 0 ) {
		globalEncodingColumnWidth = m_HostListView->columnWidth( 6 );
	}
}

/**
 * ヘッダ保存。
 */
void SendDialog::slotSaveListHeaderClicked()
{
kdDebug() << "slotSaveListHeaderClicked called." << endl;
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
	//設定ファイルに保存した内容で上書く
	globalUserNameColumnWidth = KIpMsgSettings::userNameColumnWidth();
	globalGroupNameColumnWidth = KIpMsgSettings::groupNameColumnWidth();
	globalHostNameColumnWidth = KIpMsgSettings::hostNameColumnWidth();
	globalIpAddressColumnWidth = KIpMsgSettings::ipAddressColumnWidth();
	globalLoginNameColumnWidth = KIpMsgSettings::loginNameColumnWidth();
	globalPriorityColumnWidth = KIpMsgSettings::priorityColumnWidth();
	globalEncodingColumnWidth = KIpMsgSettings::encodingColumnWidth();
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
		KIpMsgSettings::setEditFont( m_MessageEditbox->font() );
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
void SendDialog::slotListContextMenuRequested( QListViewItem * /*item*/, const QPoint & /*pos*/, int /* col */ )
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
 * バージョン情報取得。
 */
void SendDialog::slotGetVersionInfoClicked()
{
	//選択中のホストを送信対象ホストリストに追加する。
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		KIpMsgHostListViewItem *item = dynamic_cast<KIpMsgHostListViewItem *>(it.current());
		if ( item != NULL && item->isSelected() ) {
			//ホストリストはHostListViewItemから取得できる。
			HostListItem host = item->host();
			host.QueryVersionInfo();
		}
		++it;
	}
}

/**
 * 不在情報取得。
 */
void SendDialog::slotGetAbsenceInfoClicked()
{
	//選択中のホストを送信対象ホストリストに追加する。
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		KIpMsgHostListViewItem *item = dynamic_cast<KIpMsgHostListViewItem *>(it.current());
		if ( item != NULL && item->isSelected() ) {
			//ホストリストはHostListViewItemから取得できる。
			HostListItem host = item->host();
			host.QueryAbsenceInfo();
		}
		++it;
	}
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
		QString AttachedFileListCaption( "" );
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
