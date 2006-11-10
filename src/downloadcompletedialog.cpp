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


#include <qlabel.h>
#include <krun.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kuserprofile.h>
#include <kservice.h>
#include <kpushbutton.h>
#include "IpMessenger.h"
#include "downloadcompletedialog.h"

DownloadCompleteDialog::DownloadCompleteDialog(QWidget* parent, const char* name, WFlags fl)
        : DownloadCompleteDialogBase(parent,name,fl)
{}

DownloadCompleteDialog::~DownloadCompleteDialog()
{}

/*$SPECIALIZATION$*/
void DownloadCompleteDialog::slotCloseClicked()
{
	reject();
}

void DownloadCompleteDialog::slotOpenAsClicked()
{
	KURL url( string( "file://" + LocalFileName ).c_str() );
	
	if ( File.IsDirectory() ) {
		( new KRun( url ) )->setAutoDelete( true );
	} else if ( File.IsRegularFile() ) {
		KMimeType::Ptr mimetype = KMimeType::findByURL( url );
		if ( mimetype == NULL ) {
			return;
		}
		KService::Ptr offer = KServiceTypeProfile::preferredService( mimetype->name(), "Application" );
		if ( offer == NULL ) {
			KRun::displayOpenWithDialog( url );
		} else {
			KRun::run( *offer, url );
		}
	}
	accept();
}

void DownloadCompleteDialog::setDownloadInfo( DownloadInfo info, string localFileName, AttachFile file ){
	LocalFileName = localFileName;
	File = file;
	if ( file.IsDirectory() ) {
		m_OpenAsButton->setText( tr2i18n( "Open folder as Konqueror" ) );
	} else if ( file.IsRegularFile() ) {
		KURL url( string( "file://" + LocalFileName ).c_str() );
		KMimeType::Ptr mimetype = KMimeType::findByURL( url );
		if ( mimetype == NULL ) {
			m_OpenAsButton->setHidden( TRUE );
			return;
		} else {
			KService::Ptr offer = KServiceTypeProfile::preferredService( mimetype->name(), "Application" );
			if ( offer == NULL ) {
				m_OpenAsButton->setText( tr2i18n( "Open as Application" ) );
			} else {
				m_OpenAsButton->setText( tr2i18n( "Open as '%1'" ).arg( offer->name() ) );
			}
		}
	} else {
		m_OpenAsButton->setHidden( TRUE );
	}
    m_DetailLabel1->setText( QString( tr2i18n( "Total %1 (%2)" ) ).arg( info.getSizeString().c_str() ).arg( info.getSpeedString().c_str() ) );
    m_DetailLabel2->setText( QString( tr2i18n( "%1 sec  %2 files" ) ).arg( info.Time() ).arg( info.FileCount() ) );
}

#include "downloadcompletedialog.moc"

