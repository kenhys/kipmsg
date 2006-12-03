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


#include <qtextcodec.h>
#include <klocale.h>
#include <klistview.h>
#include <kfiledialog.h>

#include "attachedfiledialog.h"
#include "kipmsgsettings.h"
#include "IpMessenger.h"

KIpMessengerAttachedFileDialog::KIpMessengerAttachedFileDialog(QWidget* parent, const char* name, WFlags fl)
        : KIpMessengerAttachedFileDialogBase(parent,name,fl)
{
	m_AttachFileListView->addColumn( tr2i18n( "File" ) );
	m_AttachFileListView->addColumn( tr2i18n( "Size" ) );
	m_AttachFileListView->addColumn( tr2i18n( "Timestamp" ) );
	m_AttachFileListView->addColumn( tr2i18n( "Full Path" ) );
}

KIpMessengerAttachedFileDialog::~KIpMessengerAttachedFileDialog()
{}

/*$SPECIALIZATION$*/
void KIpMessengerAttachedFileDialog::setFiles( AttachFileList _files )
{
	files = _files;
	setFileNames();
}

AttachFileList KIpMessengerAttachedFileDialog::getFiles()
{
	return files;
}

void KIpMessengerAttachedFileDialog::slotOkClicked()
{
	accept();
}
void KIpMessengerAttachedFileDialog::slotCancelClicked()
{
	reject();
}

void KIpMessengerAttachedFileDialog::slotAddFilesClicked()
{
	QString attachFileName = KFileDialog::getOpenFileName();
	if ( attachFileName != "" ) {
		AttachFile file;
		file.setFullPath( attachFileName.data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	setFileNames();
}

void KIpMessengerAttachedFileDialog::slotAddDirectoriesClicked()
{
	QString attachDirName = KFileDialog::getExistingDirectory();
	if ( attachDirName != "" ) {
		AttachFile file;
		file.setFullPath( attachDirName.data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	setFileNames();
}

void KIpMessengerAttachedFileDialog::slotDeleteClicked()
{
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QListViewItemIterator it( m_AttachFileListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item->isSelected() ) {
//printf("item->text( 3 ).data() = %s\n", codec->fromUnicode( item->text( 3 ) ).data() );
			vector<AttachFile>::iterator i = files.FindByFullPath( codec->fromUnicode( item->text( 3 ) ).data() );
			if ( i != files.end() ) {
				files.erase( i );
			}
		}
		it++;
	}
	setFileNames();
}

void KIpMessengerAttachedFileDialog::setFileNames()
{
	m_AttachFileListView->clear();
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	char timebuf[200];
	for( vector<AttachFile>::iterator it = files.begin(); it != files.end(); it++ ){
		it->GetLocalFileInfo();
		QString size;
		if ( it->IsDirectory() ) {
			size = tr2i18n("(DIR)");
		} else {
			size = QString("%1").arg(it->FileSize());
		}
		time_t mtime = it->MTime();
		ctime_r( &mtime, timebuf );
		new QListViewItem( m_AttachFileListView,
						   codec->toUnicode( it->FileName().c_str() ),
						   size,
						   timebuf,
						   codec->toUnicode( it->FullPath().c_str() ) );
	}
}

#include "attachedfiledialog.moc"
