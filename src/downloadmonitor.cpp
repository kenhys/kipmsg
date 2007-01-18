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


#include <qstringlist.h>
#include <qtextcodec.h>
#include <qcheckbox.h>
#include <klistview.h>
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
KIpMsgDownloadMonitor::KIpMsgDownloadMonitor(QWidget* parent, const char* name, WFlags fl)
        : KIpMsgDownloadMonitorBase(parent,name,fl)
{
	m_FileListView->addColumn( tr2i18n("No") );
	m_FileListView->addColumn( tr2i18n("Files") );
	m_FileListView->addColumn( tr2i18n("Size") );
	m_FileListView->addColumn( tr2i18n("All") );
	m_FileListView->addColumn( tr2i18n("Done") );
	m_FileListView->addColumn( tr2i18n("Trans") );
	m_FileListView->addColumn( tr2i18n("Users") );
	m_FileListView->addColumn( tr2i18n("PacketNo"), 0 );
	refreshDownloadFileList();
	PollingTimer = new QTimer( this );
	connect( PollingTimer, SIGNAL( timeout() ), this, SLOT( slotPollingTimeout() ) );
	PollingTimer->start(1400, FALSE);
	m_NotPermitedIfModifiedCheckbox->setChecked(
							IpMessengerAgent::GetInstance()->GetAbortDownloadAtFileChanged() );
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
	QListViewItemIterator it( m_FileListView, QListViewItemIterator::Selected );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item != NULL ) {
			printf( "削除の仕方を考え中。PacketNo(%s)\n", item->text( COLUMN_PACKET ).data() );
		}
		++it;
	}
}

/**
 * 閉じるクリックイベント
 * ・ウインドウを閉じる
 */
void KIpMsgDownloadMonitor::slotCloseClicked()
{
	close();
}

/**
 * タイマイベント
 * ・ファイルリストを更新する。
 */
void KIpMsgDownloadMonitor::slotPollingTimeout()
{
	refreshDownloadFileList();
}

/**
 * 「ファイルが更新されたらダウンロードを許可しない。」クリックイベント
 * ・エージェントに設定。
 * ・設定を保存。
 */
void KIpMsgDownloadMonitor::slotNotPermitedIfModifiedCheckboxClicked()
{
	IpMessengerAgent::GetInstance()->SetAbortDownloadAtFileChanged(
											m_NotPermitedIfModifiedCheckbox->isChecked() );
	KIpMsgSettings::setNotPermitedIfModified( m_NotPermitedIfModifiedCheckbox->isChecked() );
	KIpMsgSettings::writeConfig();
}

/**
 * ファイルリストを更新
 * ・選択状態を保存して。
 * ・再表示。
 * ・選択状態を再設定。
 */
void KIpMsgDownloadMonitor::refreshDownloadFileList()
{
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	SentMessageList msgs = agent->CloneSentMessages();
	int i = 0;
	char sizeBuffer[200];
	char seqBuffer[200];
	char allBuffer[200];
	char doneBuffer[200];
	char transBuffer[200];
	char packetNoBuffer[200];

	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	if ( codec == NULL ) {
		return;
	}

	QListViewItemIterator it( m_FileListView );
	QStringList selPackets;
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item != NULL && item->isSelected() ) {
			selPackets << item->text( COLUMN_PACKET );
		}
		++it;
	}

	m_FileListView->clear();
	m_FileListView->setRootIsDecorated(FALSE);
	m_FileListView->setMultiSelection(TRUE);
	m_FileListView->setSelectionMode(QListView::Extended);

	for( vector<SentMessage>::iterator m = msgs.begin(); m != msgs.end(); m++ ) {
		if ( m->Files().size() > 0 ) {
			QString fileNames = "";
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
				QListViewItem *item = new QListViewItem( m_FileListView,
														 QString(seqBuffer),
														 fileNames,
														 sizeBuffer,
														 QString(allBuffer),
														 QString(doneBuffer),
														 QString(transBuffer),
														 codec->toUnicode( m->Host().UserName().c_str() ),
														 QString(packetNoBuffer) );
				QString iconName = GetPercentageIconName( percentage );
				item->setPixmap( 0, SmallIcon( iconName ) );
			}
		}
	}
	QListViewItemIterator its( m_FileListView );
	while ( its.current() != NULL ) {
		QListViewItem *item = its.current();
		if ( item != NULL ) {
			for( QStringList::iterator itp = selPackets.begin(); itp != selPackets.end(); ++itp ){
				QString currPacketNo = *itp;
				if ( currPacketNo == item->text( COLUMN_PACKET ) ){
					m_FileListView->setSelected( item, TRUE ) ;
					break;
				}
			}
		}
		++its;
	}
}

#include "downloadmonitor.moc"

