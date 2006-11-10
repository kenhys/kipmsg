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
#include <klistview.h>
#include <klocale.h>
#include <IpMessenger.h>
#include "downloadmonitor.h"
#include "kipmsgsettings.h"

#define COLUMN_PACKET 7

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
}

KIpMsgDownloadMonitor::~KIpMsgDownloadMonitor()
{}

/*$SPECIALIZATION$*/
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
void KIpMsgDownloadMonitor::slotCloseClicked()
{
	close();
}
void KIpMsgDownloadMonitor::slotPollingTimeout()
{
	refreshDownloadFileList();
}


void KIpMsgDownloadMonitor::refreshDownloadFileList()
{
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	vector<SentMessage> msgs = agent->CloneSentMessages();
	int i = 0;
	char size[200];
	char i_s[200];
	char all[200];
	char done[200];
	char trans[200];
	char p_no[200];
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
			long long llsize = 0LL;
			int iAll = 0;
			int iDone = 0;
			int iTrans = 0;
			for( vector<AttachFile>::iterator f = m->Files().begin(); f != m->Files().end(); f++ ){
				fileNames += codec->toUnicode( f->FileName().c_str() );
				if ( f->IsDirectory() ) {
					fileNames += "(DIR)";
				}
				fileNames += " ";
				llsize += f->FileSize();
				if ( f->IsDownloaded() ) {
					iDone++;
				} else if ( f->IsDownloading() ) {
					iTrans++;
				}
				iAll++;
			}
			if ( iAll > iDone ){
				snprintf( size, sizeof(size),"%lld", llsize );
				snprintf( i_s, sizeof(i_s),"%d", i++ );
				snprintf( all, sizeof(all),"%d", iAll );
				snprintf( done, sizeof(done),"%d", iDone );
				snprintf( trans, sizeof(trans),"%d", iTrans );
				snprintf( p_no, sizeof(p_no),"%ld", m->PacketNo() );
				printf( "%ld", m->PacketNo() );
				QListViewItem *item = new QListViewItem( m_FileListView,
														QString(i_s),
														fileNames,
														size,
														QString(all),
														QString(done),
														QString(trans),
														codec->toUnicode( m->Host().UserName().c_str() ),
														QString(p_no) );
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

