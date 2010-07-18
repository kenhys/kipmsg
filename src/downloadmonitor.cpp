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

#include <qstringlist.h>
#include <qtextcodec.h>
#include <qcheckbox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

#include "downloadmonitor.h"
#include "kipmsgsettings.h"
#include "kipmsgutils.h"

#define COLUMN_PACKET 7

/**
 * コンストラクタ
 * ・ファイルリストの表示。
 * ・タイマ開始。
 * ・設定のロード。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMsgDownloadMonitor::KIpMsgDownloadMonitor(QWidget* parent, const char* name, Qt::WindowFlags fl)
//        : KIpMsgDownloadMonitorBase(parent,name,fl)
        : KIpMsgDownloadMonitorBase()
{
	kDebug() << "START KIpMsgDownloadMonitor::KIpMsgDownloadMonitor" << endl;
	setupUi(this);
	setButtons( None );
	KIpMessengerDownloadMonitorModel *model = new KIpMessengerDownloadMonitorModel(m_FileListView);
/*
	model->setHorizontalHeaderItem( 0, new QStandardItem( tr2i18n( "No" ) ) );
	model->setHorizontalHeaderItem( 1, new QStandardItem( tr2i18n( "Files" ) ) );
	model->setHorizontalHeaderItem( 2, new QStandardItem( tr2i18n( "Size" ) ) );
	model->setHorizontalHeaderItem( 3, new QStandardItem( tr2i18n( "All" ) ) );
	model->setHorizontalHeaderItem( 4, new QStandardItem( tr2i18n( "Done" ) ) );
	model->setHorizontalHeaderItem( 5, new QStandardItem( tr2i18n( "Trans" ) ) );
	model->setHorizontalHeaderItem( 6, new QStandardItem( tr2i18n( "UsersPath" ) ) );
	model->setHorizontalHeaderItem( 7, new QStandardItem( tr2i18n( "PackerNo" ) ) );
*/
	m_FileListView->setHeaderHidden( false );
	m_FileListView->setRootIsDecorated( false );
	m_FileListView->setAllColumnsShowFocus( true );
	m_FileListView->setModel( model );

//	m_FileListView->addColumn( tr2i18n("No") );
//	m_FileListView->addColumn( tr2i18n("Files") );
//	m_FileListView->addColumn( tr2i18n("Size") );
//	m_FileListView->addColumn( tr2i18n("All") );
//	m_FileListView->addColumn( tr2i18n("Done") );
//	m_FileListView->addColumn( tr2i18n("Trans") );
//	m_FileListView->addColumn( tr2i18n("Users") );
//	m_FileListView->addColumn( tr2i18n("PacketNo"), 0 );
	refreshDownloadFileList();
	PollingTimer = new QTimer( this );
	connect( PollingTimer, SIGNAL( timeout() ), this, SLOT( slotPollingTimeout() ) );
	PollingTimer->start(1400);
//	PollingTimer->start(1400, FALSE);
	m_NotPermitedIfModifiedCheckbox->setChecked(
							IpMessengerAgent::GetInstance()->AbortDownloadAtFileChanged() );
	kDebug() << "END   KIpMsgDownloadMonitor::KIpMsgDownloadMonitor" << endl;
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMsgDownloadMonitor::~KIpMsgDownloadMonitor()
{}

/*$SPECIALIZATION$*/

/**
 * 削除クリックイベント
 * ・ファイルリストから削除する。
 */
void KIpMsgDownloadMonitor::slotDeleteClicked()
{
	kDebug() << "START KIpMsgDownloadMonitor::slotDeleteClicked" << endl;
	KIpMessengerDownloadMonitorModel *dataModel = (KIpMessengerDownloadMonitorModel*)m_FileListView->model();
	QListIterator<QModelIndex> it = m_FileListView->selectionModel()->selectedRows();
	while ( it.hasNext() ) {
		QModelIndex index = it.next();
		KIpMessengerDownloadMonitorEntry entry = dataModel->getDownloadEntry( index );
		printf( "削除の仕方を考え中。PacketNo(%s)\n", (const char*)entry.packetNo().data() );
	}
	kDebug() << "END   KIpMsgDownloadMonitor::slotDeleteClicked" << endl;
}

/**
 * 閉じるクリックイベント
 * ・ウインドウを閉じる
 */
void KIpMsgDownloadMonitor::slotCloseClicked()
{
	kDebug() << "START KIpMsgDownloadMonitor::slotCloseClicked" << endl;
//	close();
	accept();
	kDebug() << "END   KIpMsgDownloadMonitor::slotCloseClicked" << endl;
}

/**
 * タイマイベント
 * ・ファイルリストを更新する。
 */
void KIpMsgDownloadMonitor::slotPollingTimeout()
{
	kDebug() << "START KIpMsgDownloadMonitor::slotPollingTimeout" << endl;
	refreshDownloadFileList();
	kDebug() << "END   KIpMsgDownloadMonitor::slotPollingTimeout" << endl;
}

/**
 * 「ファイルが更新されたらダウンロードを許可しない。」クリックイベント
 * ・エージェントに設定。
 * ・設定を保存。
 */
void KIpMsgDownloadMonitor::slotNotPermitedIfModifiedCheckboxClicked()
{
	kDebug() << "START KIpMsgDownloadMonitor::slotNotPermitedIfModifiedCheckboxClicked" << endl;
	IpMessengerAgent::GetInstance()->setAbortDownloadAtFileChanged(
											m_NotPermitedIfModifiedCheckbox->isChecked() );
	KIpMsgSettings::setNotPermitedIfModified( m_NotPermitedIfModifiedCheckbox->isChecked() );
	KIpMsgSettings::self()->writeConfig();
	kDebug() << "END   KIpMsgDownloadMonitor::slotNotPermitedIfModifiedCheckboxClicked" << endl;
}

/**
 * ファイルリストを更新
 * ・選択状態を保存して。
 * ・再表示。
 * ・選択状態を再設定。
 */
void KIpMsgDownloadMonitor::refreshDownloadFileList()
{
	kDebug() << "START KIpMsgDownloadMonitor::refreshDownloadFileList" << endl;
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	SentMessageList msgs = agent->CloneSentMessages();
	int i = 0;
	char sizeBuffer[200];
	char seqBuffer[200];
	char allBuffer[200];
	char doneBuffer[200];
	char transBuffer[200];
	char packetNoBuffer[200];

//TESTME
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding().toAscii() );
	if ( codec == NULL ) {
		kDebug() << "END   KIpMsgDownloadMonitor::refreshDownloadFileList" << endl;
		return;
	}

	KIpMessengerDownloadMonitorModel *dataModel = (KIpMessengerDownloadMonitorModel*)m_FileListView->model();
	QListIterator<QModelIndex> it = m_FileListView->selectionModel()->selectedRows();
	QStringList selPackets;
	while ( it.hasNext() ) {
		KIpMessengerDownloadMonitorEntry entry = dataModel->getDownloadEntry( it.next() );
		selPackets << entry.packetNo();
	}

	dataModel->clear();
//	m_FileListView->clear();
//FIXME m_FileListView->setRootIsDecorated(FALSE);
//FIXME m_FileListView->setMultiSelection(TRUE);
//FIXME m_FileListView->setSelectionMode(QListView::Extended);
//
//TESTME
	for( vector<SentMessage>::iterator m = msgs.begin(); m != msgs.end(); m++ ) {
		if ( m->Files().size() > 0 ) {
			QString fileNames("");
			long long fileSizeTotal = 0LL;
			long long transFileSizeTotal = 0LL;
			int allFileCount = 0;
			int doneFileCount = 0;
			int transFileCount = 0;
			bool hasDir = false;
			for( vector<AttachFile>::iterator f = m->Files().begin(); f != m->Files().end(); f++ ){
				fileNames += codec->toUnicode( f->FileName().c_str() );
				if ( f->IsDirectory() ) {
					fileNames += "(DIR)";
					hasDir = true;
				}
				fileNames += " ";
				fileSizeTotal += f->FileSize();
				transFileSizeTotal += f->TransSize();
				if ( f->IsDownloaded() ) {
					doneFileCount++;
				} else if ( f->IsDownloading() ) {
					transFileCount++;
				}
				allFileCount++;
			}
			if ( allFileCount > doneFileCount ){
				int percentage = 0;
				if ( hasDir ){
					percentage = allFileCount == 0 ? 0 : (int)(100 * ( (double)doneFileCount / (double)allFileCount ) );
				} else {
					percentage = fileSizeTotal == 0 ? 0 : (int)(100 * ( (double)transFileSizeTotal / (double)fileSizeTotal ) );
				}
				snprintf( sizeBuffer, sizeof(sizeBuffer),"%lld", fileSizeTotal );
				snprintf( seqBuffer, sizeof(seqBuffer),"%d", i++ );
				snprintf( allBuffer, sizeof(allBuffer),"%d", allFileCount );
				snprintf( doneBuffer, sizeof(doneBuffer),"%d", doneFileCount );
				snprintf( transBuffer, sizeof(transBuffer),"%d", transFileCount );
				snprintf( packetNoBuffer, sizeof(packetNoBuffer),"%ld", m->PacketNo() );
//				QListViewItem *item = new QListViewItem( m_FileListView,
				KIpMessengerDownloadMonitorEntry entry = KIpMessengerDownloadMonitorEntry(
														 QString(seqBuffer),
														 fileNames,
														 sizeBuffer,
														 QString(allBuffer),
														 QString(doneBuffer),
														 QString(transBuffer),
														 codec->toUnicode( m->Host().UserName().c_str() ),
														 QString(packetNoBuffer) );
				QString iconName = GetPercentageIconName( percentage );
				entry.setIcon( SmallIcon( iconName ) );
				dataModel->addEntry(entry);
			}
		}
	}

//	QListViewItemIterator its( m_FileListView );
	int maxRows = m_FileListView->model()->rowCount();
	QItemSelectionModel *selectionModel = m_FileListView->selectionModel();
//	while ( its.current() != NULL ) {
//		QListViewItem *item = its.current();
//		if ( item != NULL ) {
	for ( int i = 0; i < maxRows; i++ ){
		QModelIndex rowIndexTop = dataModel->index( i, 0 );
		KIpMessengerDownloadMonitorEntry entry = dataModel->getDownloadEntry( rowIndexTop );
		for( QStringList::iterator itp = selPackets.begin(); itp != selPackets.end(); ++itp ){
			QString currPacketNo = *itp;
			//if ( currPacketNo == item->text( COLUMN_PACKET ) ){
			if ( currPacketNo == entry.packetNo() ){
				//m_FileListView->setSelected( item, TRUE ) ;
				QModelIndex rowIndexLast = dataModel->index( i, dataModel->columnCount()-1 );
				QItemSelection selection = QItemSelection( rowIndexTop, rowIndexLast );
				selectionModel->select(selection, QItemSelectionModel::Select);
//				m_FileListView->setSelected( item, TRUE ) ;
				break;
			}
		}
	}
	kDebug() << "END   KIpMsgDownloadMonitor::refreshDownloadFileList" << endl;
}

KIpMessengerDownloadMonitorEntry::KIpMessengerDownloadMonitorEntry(QString seqNumber, QString fileName, QString size, QString all, QString done, QString trans, QString userName, QString packetNo){
	kDebug() << "START KIpMessengerDownloadMonitorEntry::KIpMessengerDownloadMonitorEntry" << endl;
	this->m_SeqNumber = seqNumber;
	this->m_FileName = fileName;
	this->m_Size = size;
	this->m_All = all;
	this->m_Done = done;
	this->m_Trans = trans;
	this->m_UserName = userName;
	this->m_PacketNo = packetNo;
	kDebug() << "END   KIpMessengerDownloadMonitorEntry::KIpMessengerDownloadMonitorEntry" << endl;
}

void KIpMessengerDownloadMonitorModel::clear(){
	kDebug() << "START KIpMessengerDownloadMonitorModel::clear" << endl;
	removeRows(0,rowCount()-1);
	kDebug() << "END   KIpMessengerDownloadMonitorModel::clear" << endl;
}
KIpMessengerDownloadMonitorEntry KIpMessengerDownloadMonitorModel::getDownloadEntry(const QModelIndex &index) {
	kDebug() << "START KIpMessengerDownloadMonitorModel::getDownloadEntry" << endl;
	KIpMessengerDownloadMonitorEntry entry = downloadList.at(index.row());
	kDebug() << "END   KIpMessengerDownloadMonitorModel::getDownloadEntry" << endl;
	return entry;
}
int KIpMessengerDownloadMonitorModel::rowCount(const QModelIndex &parent) const{
	kDebug() << "START KIpMessengerDownloadMonitorModel::rowCount" << endl;
	kDebug() << "END   KIpMessengerDownloadMonitorModel::rowCount" << endl;
	return downloadList.count();
}

int KIpMessengerDownloadMonitorModel::columnCount(const QModelIndex &parent) const{
	kDebug() << "START KIpMessengerDownloadMonitorModel::columnCount" << endl;
	kDebug() << "END   KIpMessengerDownloadMonitorModel::columnCount" << endl;
	return 8;
}

QVariant KIpMessengerDownloadMonitorModel::data(const QModelIndex &index, int role) const{
	kDebug() << "START KIpMessengerDownloadMonitorModel::data" << endl;
	if (!index.isValid()){
		kDebug() << "END   KIpMessengerDownloadMonitorModel::data" << endl;
		return QVariant();
	}

	if (index.row() >= downloadList.size()){
		kDebug() << "END   KIpMessengerDownloadMonitorModel::data" << endl;
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		KIpMessengerDownloadMonitorEntry entry = downloadList.at(index.row());
		switch(index.column()){
			case 0:		return entry.seqNumber();
			case 1:		return entry.fileName();
			case 2:		return entry.size();
			case 3:		return entry.all();
			case 4:		return entry.done();
			case 5:		return entry.trans();
			case 6:		return entry.userName();
			case 7:		return entry.packetNo();
			default:	return QVariant();
		}
	} else {
		kDebug() << "END   KIpMessengerDownloadMonitorModel::data" << endl;
		return QVariant();
	}

}
QVariant
KIpMessengerDownloadMonitorModel::headerData(int selection, Qt::Orientation orientation, int role) const{
	kDebug() << "START KIpMessengerDownloadMonitorModel::headerData" << endl;
	if (role != Qt::DisplayRole){
		kDebug() << "END   KIpMessengerDownloadMonitorModel::headerData" << endl;
		return QVariant();
	}

	if (orientation == Qt::Horizontal){
		switch(selection){
			case 0:		return tr2i18n( "No" );
			case 1:		return tr2i18n( "Files" );
			case 2: 	return tr2i18n( "Size" );
			case 3: 	return tr2i18n( "All" );
			case 4: 	return tr2i18n( "Done" );
			case 5: 	return tr2i18n( "Trans" );
			case 6: 	return tr2i18n( "UsersPath" );
			case 7: 	return tr2i18n( "PacketNo" );
			default:	return QVariant();
		}
	}
	kDebug() << "END   KIpMessengerDownloadMonitorModel::headerData" << endl;
	return QVariant();
}

bool
KIpMessengerDownloadMonitorModel::addEntry(const KIpMessengerDownloadMonitorEntry entry){
	kDebug() << "START KIpMessengerDownloadMonitorModel::addEntry" << endl;
	downloadList.append(entry);
	kDebug() << "END   KIpMessengerDownloadMonitorModel::addEntry" << endl;
	return true;
}

#include "downloadmonitor.moc"

