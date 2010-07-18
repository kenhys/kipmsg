/***************************************************************************
 *   Copyright (C) 2006-2010 by nikikuni                                   *
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


#include <qtextcodec.h>
#include <kdebug.h>
#include <klocale.h>
//#include <klistview.h>
#include <kfiledialog.h>

#include "attachedfiledialog.h"
#include "kipmsgsettings.h"
#include "kipmsgutils.h"
#include "IpMessenger.h"


using namespace std;
using namespace ipmsg;

/**
 * コンストラクタ
 * ・リストビューの初期設定。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMessengerAttachedFileDialog::KIpMessengerAttachedFileDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
        : KIpMessengerAttachedFileDialogBase()
//        : KIpMessengerAttachedFileDialogBase(parent,name,fl)
{
	kDebug() << "START KIpMessengerAttachedFileDialog::KIpMessengerAttachedFileDialog" << endl;
	setupUi(this);
	setButtons( None );
	KIpMessengerAttachedFileModel *model = new KIpMessengerAttachedFileModel(m_AttachFileListView);
	m_AttachFileListView->setHeaderHidden( false );
	m_AttachFileListView->setRootIsDecorated( false );
	m_AttachFileListView->setAllColumnsShowFocus( true );
	m_AttachFileListView->setModel( model );
	m_AttachFileListView->setColumnWidth( KIpMessengerAttachedFileModel::fileNameCol, 200 );
	m_AttachFileListView->setColumnWidth( KIpMessengerAttachedFileModel::sizeCol, 60 );
	m_AttachFileListView->setColumnWidth( KIpMessengerAttachedFileModel::timeStampCol, 240 );
	m_AttachFileListView->setColumnWidth( KIpMessengerAttachedFileModel::fullPathCol, 800 );
	kDebug() << "END   KIpMessengerAttachedFileDialog::KIpMessengerAttachedFileDialog" << endl;
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMessengerAttachedFileDialog::~KIpMessengerAttachedFileDialog()
{}

/*$SPECIALIZATION$*/

/**
 * 添付ファイルリスト設定
 * ・添付ファイルリストを保存。
 * ・添付ファイルリストからリストビューに表示。
 */
void KIpMessengerAttachedFileDialog::setFiles( AttachFileList _files )
{
	kDebug() << "START KIpMessengerAttachedFileDialog::setFiles" << endl;
	files = _files;
	setFileNames();
	kDebug() << "END   KIpMessengerAttachedFileDialog::setFiles" << endl;
}

/**
 * 添付ファイルリスト取得
 * ・添付ファイルリストを返す。
 */
AttachFileList KIpMessengerAttachedFileDialog::getFiles()
{
	kDebug() << "START KIpMessengerAttachedFileDialog::getFiles" << endl;
	kDebug() << "END   KIpMessengerAttachedFileDialog::getFiles" << endl;
	return files;
}

/**
 * OKクリックイベント
 * ・アクセプトによりダイアログを閉じる。
 */
void KIpMessengerAttachedFileDialog::slotOkClicked()
{
	kDebug() << "START KIpMessengerAttachedFileDialog::slotOkClicked" << endl;
	accept();
	kDebug() << "END   KIpMessengerAttachedFileDialog::slotOkClicked" << endl;
}

/**
 * キャンセルクリックイベント
 * ・リジェクトによりダイアログを閉じる。
 */
void KIpMessengerAttachedFileDialog::slotCancelClicked()
{
	kDebug() << "START KIpMessengerAttachedFileDialog::slotCancelClicked" << endl;
	reject();
	kDebug() << "END   KIpMessengerAttachedFileDialog::slotCancelClicked" << endl;
}

/**
 * ファイル追加クリックイベント
 * ・コモンダイアログによりファイルを選択しリストビューに追加。
 */
void KIpMessengerAttachedFileDialog::slotAddFilesClicked()
{
	kDebug() << "START KIpMessengerAttachedFileDialog::slotAddFilesClicked" << endl;
	QString attachFileName = KFileDialog::getOpenFileName();
	if ( attachFileName != "" ) {
		AttachFile file;
		QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding().toAscii() );
		file.setFullPath( codec->fromUnicode( attachFileName ).data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	setFileNames();
	kDebug() << "END   KIpMessengerAttachedFileDialog::slotAddFilesClicked" << endl;
}

/**
 * ディレクトリ追加クリックイベント
 * ・コモンダイアログによりディレクトリを選択しリストビューに追加。
 */
void KIpMessengerAttachedFileDialog::slotAddDirectoriesClicked()
{
	kDebug() << "START KIpMessengerAttachedFileDialog::slotAddDirectoriesClicked" << endl;
	QString attachDirName = KFileDialog::getExistingDirectory();
	if ( attachDirName != "" ) {
		AttachFile file;
		QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding().toAscii() );
		file.setFullPath( codec->fromUnicode( attachDirName ).data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	setFileNames();
	kDebug() << "END   KIpMessengerAttachedFileDialog::slotAddDirectoriesClicked" << endl;
}

/**
 * 削除クリックイベント
 * ・リストビューの選択中のアイテムを削除。
 */
void KIpMessengerAttachedFileDialog::slotDeleteClicked()
{
	kDebug() << "START KIpMessengerAttachedFileDialog::slotDeleteClicked" << endl;
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding().toAscii() );
	QAbstractItemModel *dataModel = m_AttachFileListView->model();
	QListIterator<QModelIndex> it = m_AttachFileListView->selectionModel()->selectedRows();
	QVariant data;
	while ( it.hasNext() ) {
		QModelIndex index = dataModel->index( it.next().row(), KIpMessengerAttachedFileModel::fullPathCol );
		data = dataModel->data( index, Qt::DisplayRole );
		string filename = string( codec->fromUnicode( data.toString() ).data() );
		vector<AttachFile>::iterator i = files.FindByFullPath( filename );
		if ( i != files.end() ) {
			files.erase( i );
		}
	}
	setFileNames();
	kDebug() << "END   KIpMessengerAttachedFileDialog::slotDeleteClicked" << endl;
}

/**
 * 添付ファイルリスト表示
 * ・添付ファイルリストからリストビューに表示。
 */
void KIpMessengerAttachedFileDialog::setFileNames()
{
	kDebug() << "START KIpMessengerAttachedFileDialog::setFileNames" << endl;
	KIpMessengerAttachedFileModel *dataModel = (KIpMessengerAttachedFileModel *)m_AttachFileListView->model();
	dataModel->clear();
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding().toAscii() );
	for( vector<AttachFile>::iterator it = files.begin(); it != files.end(); it++ ){
		it->GetLocalFileInfo();
		QString size("");
		if ( it->IsDirectory() ) {
			size = tr2i18n("(DIR)");
		} else {
			size = QString("%1").arg(it->FileSize());
		}
		dataModel->addEntry( KIpMessengerAttachedFileEntry( 
									codec->toUnicode( it->FileName().c_str() ),
									size,
									CreateTimeString( it->MTime() ),
									codec->toUnicode( it->FullPath().c_str() ) ) );
	}
	m_AttachFileListView->reset();
	kDebug() << "END   KIpMessengerAttachedFileDialog::setFileNames" << endl;
}

/**
 * 添付ファイルコンストラクタ。
 */
KIpMessengerAttachedFileEntry::KIpMessengerAttachedFileEntry(QString fileName, QString size, QString mTime, QString fullPath){
	this->m_FileName = fileName;
	this->m_Size = size;
	this->m_MTime = mTime;
	this->m_FullPath = fullPath;
}

/**
 * 添付ファイルモデルのリストをクリアします。
 */
void KIpMessengerAttachedFileModel::clear(){
	removeRows(0,rowCount()-1);
	fileList.clear();
}

/**
 * 添付ファイルモデルのリストの行数を返します。
 */
int KIpMessengerAttachedFileModel::rowCount(const QModelIndex &parent) const{
	return fileList.count();
}
/**
 * 添付ファイルモデルのリストの列数を返します。
 */
int KIpMessengerAttachedFileModel::columnCount(const QModelIndex &parent) const{
	return ColumnMax;
}
/**
 * 添付ファイルモデルのデータを返します。
 */
QVariant KIpMessengerAttachedFileModel::data(const QModelIndex &index, int role) const{
	if (!index.isValid()){
		return QVariant();
	}

	if (index.row() >= fileList.size()){
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		KIpMessengerAttachedFileEntry entry = fileList.at(index.row());
		switch(index.column()){
			case fileNameCol:	return entry.fileName();
			case sizeCol:		return entry.size();
			case timeStampCol:	return entry.mTime();
			case fullPathCol:	return entry.fullPath();
			default:			return QVariant();
		}
	} else {
		return QVariant();
	}
}

/**
 * 添付ファイルモデルのヘッダーを返します。
 */
QVariant KIpMessengerAttachedFileModel::headerData(int selection, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole){
		return QVariant();
	}

	if (orientation == Qt::Horizontal){
		switch(selection){
			case fileNameCol:	return tr2i18n( "File" );
			case sizeCol: 		return tr2i18n( "Size" );
			case timeStampCol: 	return tr2i18n( "Timestamp" );
			case fullPathCol: 	return tr2i18n( "Full Path" );
			default:			return QVariant();
		}
	}
	return QVariant();
}

/**
 * 添付ファイルモデルに添付ファイルを追加します。
 */
bool KIpMessengerAttachedFileModel::addEntry(const KIpMessengerAttachedFileEntry entry){
	fileList.append(entry);
	return true;
}
#include "attachedfiledialog.moc"
