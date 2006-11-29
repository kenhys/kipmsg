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
#include <qlabel.h>

#include <kpushbutton.h>
#include <klocale.h>
#include <kcombobox.h>
#include <klistview.h>

#include "IpMessenger.h"
#include "kipmsgsettings.h"
#include "encodingconfig.h"

#define COL_USER 0
#define COL_GROUP 1
#define COL_IPADDR 3
#define COL_LOGIN 4
#define COL_ENCODING 5

KIPMsgEncodingConfigDialog::KIPMsgEncodingConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgEncodingConfigDialogBase(parent,name,fl)
{
	m_HostListView->addColumn(tr2i18n("User"));
	m_HostListView->addColumn(tr2i18n("Group"));
	m_HostListView->addColumn(tr2i18n("Host"));
	m_HostListView->addColumn(tr2i18n("IP address"));
	m_HostListView->addColumn(tr2i18n("Login"));
	m_HostListView->addColumn(tr2i18n("Encoding"));
    refreshEncoding();
	QListViewItemIterator it( m_HostListView );
	if ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		slotHostSelected( item );
	}
}

void
KIPMsgEncodingConfigDialog::refreshEncoding()
{
	m_HostListView->clear();
	m_HostListView->setRootIsDecorated(FALSE);

	//ホストリストを再取得
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	HostList hosts = agent->GetHostList();
	QStringList encodings = KIpMsgSettings::encodingSettings();
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
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
		QListViewItem *item = new QListViewItem( m_HostListView,
												codec->toUnicode( ix->Nickname().c_str() ),
												codec->toUnicode( ix->GroupName().c_str() ),
												codec->toUnicode( ix->HostName().c_str() ),
												codec->toUnicode( ix->IpAddress().c_str() ),
												codec->toUnicode( ix->UserName().c_str() ),
												codec->toUnicode( ix->EncodingName().c_str() ) );
	}
	
	QListViewItemIterator it( m_HostListView );
	QListViewItem *topItem = it.current();
	if ( topItem != NULL ) {
		m_HostListView->setSelected( topItem, TRUE );
	}
}

KIPMsgEncodingConfigDialog::~KIPMsgEncodingConfigDialog()
{}

/*$SPECIALIZATION$*/
void KIPMsgEncodingConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}

void KIPMsgEncodingConfigDialog::slotCancelClicked()
{
	close();
}

void KIPMsgEncodingConfigDialog::slotHostSelected( QListViewItem *item )
{
	if ( item == NULL ) {
		return;
	}
	m_GroupNameLabel->setText( item->text( COL_GROUP ) );
	m_UserNameLabel->setText( item->text( COL_USER ) );
	SeletedIpAddress = item->text( COL_IPADDR );
	SeletedLoginUser = item->text( COL_LOGIN );
	QString enc = item->text( COL_ENCODING );
	if ( enc != "" ) {
		m_EncodingCombobox->setCurrentText( enc );
	} else {
		item->setText( COL_ENCODING, KIpMsgSettings::messageEncoding() );
		m_EncodingCombobox->setCurrentText( KIpMsgSettings::messageEncoding() );
	}
}

void KIPMsgEncodingConfigDialog::slotEncodingComboChanged(int index)
{
	string userName;
	string groupName;
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	HostList hosts = agent->GetHostList();
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
		if ( QString( ix->IpAddress().c_str() ) == SeletedIpAddress && 
			QString( ix->UserName().c_str() ) == SeletedLoginUser ) {
			userName = ix->Nickname();
			groupName = ix->GroupName();
			break;
		}
		
	}

	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item != NULL ) {
			if ( item->text( COL_IPADDR ) == SeletedIpAddress &&
				 item->text( COL_LOGIN ) == SeletedLoginUser ) {
				QTextCodec *codec = QTextCodec::codecForName( m_EncodingCombobox->currentText() );
				item->setText( COL_GROUP, codec->toUnicode( groupName.c_str() ) );
				item->setText( COL_USER, codec->toUnicode( userName.c_str() ) );
				item->setText( COL_ENCODING, m_EncodingCombobox->currentText() );
				m_UserNameLabel->setText( item->text( COL_USER ) );
				m_GroupNameLabel->setText( item->text( COL_GROUP ) );
				break;
			}
		}
		++it;
	}
}

void KIPMsgEncodingConfigDialog::slotApplyClicked()
{
	vector<HostListItem> targets;
	QListViewItemIterator it( m_HostListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item != NULL && item->text( COL_ENCODING ).data() != NULL ) {
			printf("item != NULL\n");
			fflush(stdout);
			HostListItem host;
			printf("item != NULL\n");
			fflush(stdout);
			host.setEncodingName( item->text( COL_ENCODING ).data() );
			if ( host.EncodingName() != "" ){
				printf("ENCNAME != NULL B\n");
				fflush(stdout);
				QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
				host.setIpAddress( codec->fromUnicode( item->text( COL_IPADDR ) ).data() );
				host.setUserName( codec->fromUnicode( item->text( COL_LOGIN ) ).data() );
				targets.push_back( host );
				printf("ENCNAME != NULL A\n");
				fflush(stdout);
			}
		}
		++it;
	}
	printf("LOOP END\n");
	fflush(stdout);
	QStringList encodings = KIpMsgSettings::encodingSettings();
	for( vector<HostListItem>::iterator host = targets.begin(); host != targets.end(); host++ ) {
		if ( host->EncodingName() != "" ){
			printf("ENC ADD B\n");
			fflush(stdout);
			QTextCodec *codec = QTextCodec::codecForName( host->EncodingName().c_str() );
			QString ip = codec->toUnicode( host->IpAddress().c_str() );
			QString login = codec->toUnicode( host->UserName().c_str() );
			QString enc = codec->toUnicode( host->EncodingName().c_str() );
			for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
				QStringList fields = QStringList::split( ":", *ite );
				if ( ip == fields[0] &&
					login == fields[1] ) {
					encodings.remove( ite );
					break;
				}
			}
			printf("ENC ADD A\n");
			fflush(stdout);
			encodings << ip + ":" + login + ":" + enc;
		}
		printf("LOOP INNER END\n");
		fflush(stdout);
	}
	KIpMsgSettings::setEncodingSettings( encodings );
	KIpMsgSettings::writeConfig();
}

#include "encodingconfig.moc"
