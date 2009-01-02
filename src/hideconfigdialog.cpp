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

#include <qlabel.h>
#include <qcheckbox.h>
#include <klocale.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klistbox.h>
#include <kpushbutton.h>
#include <klistview.h>
#include "kipmsgsettings.h"
#include "IpMessenger.h"

#include "hideconfigdialog.h"

using namespace std;
using namespace ipmsg;

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
HideConfigDialog::HideConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : HideConfigDialogBase(parent,name,fl)
{
	m_HiddenHostsCheckBox->setChecked( KIpMsgSettings::hideFromHiddenHosts() );
	m_EncriptionNotSupporedHostsCheckBox->setChecked( KIpMsgSettings::hideFromEncryptionNotSupportedHosts() );
	m_EnableHideFromSpecifiedHostsCheckBox->setChecked( KIpMsgSettings::hideFromSpecifiedHosts() );
	m_AddressListBox->insertStringList( KIpMsgSettings::hideFromSpecifiedHostList() );

	slotEnableSpecifiedHostClicked();

	//ホスト名一覧
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	m_HostsListView->addColumn( tr2i18n("IP Address"), 0 );
	m_HostsListView->addColumn( tr2i18n("Host"), 1 );
	HostList hosts = agent->GetHostList();
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
		new QListViewItem( m_HostsListView, ix->IpAddress().c_str(), ix->HostName().c_str() );
	}
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
HideConfigDialog::~HideConfigDialog()
{}

void HideConfigDialog::slotAddHideAddressClicked(){
	if ( m_AddressEditbox->text() == "" ) {
		return;
	}
	QString insertText = m_AddressEditbox->text();
	bool isFound = false;
	for( unsigned int i = 0; i < m_AddressListBox->count(); i++ ){
		if ( insertText == m_AddressListBox->item( i )->text() ) {
			m_AddressListBox->setSelected( i, TRUE );
			isFound = true;
			break;
		}
	}
	if ( !isFound ){
		m_AddressListBox->insertItem( m_AddressEditbox->text() );
		m_AddressListBox->setSelected( m_AddressListBox->count() - 1, TRUE );
	}
	m_AddressEditbox->setText("");
}

void HideConfigDialog::slotDeleteHideAddressClicked(){
	m_AddressEditbox->setText( m_AddressListBox->currentText() );


	unsigned int rmv = m_AddressListBox->currentItem();
	m_AddressListBox->removeItem( rmv );

	if ( m_AddressListBox->count() >= rmv + 1 ) {
		m_AddressListBox->setSelected( rmv, TRUE );
	} else {
		m_AddressListBox->setSelected( m_AddressListBox->count() - 1, TRUE );
	}
}

void HideConfigDialog::slotOKClicked(){
	slotApplyClicked();
	close();
}

void HideConfigDialog::slotCancelClicked(){
	close();
}

void HideConfigDialog::slotApplyClicked(){
	KIpMsgSettings::setHideFromHiddenHosts( m_HiddenHostsCheckBox->isChecked() );
	KIpMsgSettings::setHideFromEncryptionNotSupportedHosts( m_EnableHideFromSpecifiedHostsCheckBox->isChecked() );
	KIpMsgSettings::setHideFromSpecifiedHosts( m_EncriptionNotSupporedHostsCheckBox->isChecked() );

	QStringList addresses;
	for( unsigned int i = 0; i < m_AddressListBox->count(); i++ ) {
		addresses << m_AddressListBox->text(i);
	}
	KIpMsgSettings::setHideFromSpecifiedHostList( addresses );

	KIpMsgSettings::writeConfig();

	HideConfigDialog::SetupHideHosts();
}

void HideConfigDialog::slotEnableSpecifiedHostClicked(){
	if ( m_EnableHideFromSpecifiedHostsCheckBox->isChecked() ) {
		m_DeleteAddressButton->setEnabled( TRUE );
		m_AddAddressButton->setEnabled( TRUE );
		m_DeleteAddressButton->setEnabled( TRUE );
		m_AddressLabel->setEnabled( TRUE );
		m_AddressListBox->setEnabled( TRUE );
		m_AddressEditbox->setEnabled( TRUE );
		m_LoginHostLabel->setEnabled( TRUE );
		m_HostsListView->setEnabled( TRUE );
		m_CopyIPAddressButton->setEnabled( FALSE );
	} else {
		m_DeleteAddressButton->setEnabled( FALSE );
		m_AddAddressButton->setEnabled( FALSE );
		m_DeleteAddressButton->setEnabled( FALSE );
		m_AddressLabel->setEnabled( FALSE );
		m_AddressListBox->setEnabled( FALSE );
		m_AddressEditbox->setEnabled( FALSE );
		m_LoginHostLabel->setEnabled( FALSE );
		m_HostsListView->setEnabled( FALSE );
		m_CopyIPAddressButton->setEnabled( FALSE );
	}
	slotIPAddressListViewSelectionChanged();
}

void HideConfigDialog::slotCopyIPAddressClicked()
{
	//選択中のホストのアドレスをコピー
	QListViewItem *item = m_HostsListView->selectedItem();
	if ( item != NULL ) {
		m_AddressEditbox->setText( item->text( 0 ) );
	}
}

void HideConfigDialog::slotIPAddressListViewSelectionChanged()
{
	//コピーボタンの有効／無効を設定する
	if ( m_HostsListView->selectedItem() != NULL ) {
		m_CopyIPAddressButton->setEnabled( TRUE );
	} else {
		m_CopyIPAddressButton->setEnabled( FALSE );
	}
}

void HideConfigDialog::SetupHideHosts(){
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	QStringList hiddenHostList;
	QStringList visibleHostList;

	//非表示ホストから隠す
	QStringList priorityHiddenList = KIpMsgSettings::priorityHidden();
	if ( KIpMsgSettings::hideFromHiddenHosts() ) {
		//隠すホストの一覧（優先）
		for( QStringList::iterator ith = priorityHiddenList.begin(); ith != priorityHiddenList.end(); ith++ ) {
			QStringList hListItem = QStringList::split( "|", *ith );
			if ( hListItem.count() == 2 ){
				if ( hListItem[0] == "" ){
					hiddenHostList.append( hListItem[0] );
				}
				break;
			}
		}
	} else {
		//見せるホストの一覧
		for( QStringList::iterator ith = priorityHiddenList.begin(); ith != priorityHiddenList.end(); ith++ ) {
			QStringList hListItem = QStringList::split( "|", *ith );
			if ( hListItem.count() == 2 ){
				if ( hListItem[0] == "" ){
					visibleHostList.append( hListItem[0] );
				}
				break;
			}
		}
	}

	//暗号化未サポートホストから隠す
	HostList hosts = agent->GetHostList();
	if ( KIpMsgSettings::hideFromEncryptionNotSupportedHosts() ) {
		//隠すホストの一覧（優先）
		for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
			if ( !ix->IsEncryptSupport() ) {
				hiddenHostList.append( ix->IpAddress().c_str() );
			}
		}
	} else {
		//見せるホストの一覧
		for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
			if ( !ix->IsEncryptSupport() ) {
				visibleHostList.append( ix->IpAddress().c_str() );
			}
		}
	}

	//指定したホストから隠す
	QStringList specifiedList = KIpMsgSettings::hideFromSpecifiedHostList();
	if ( KIpMsgSettings::hideFromSpecifiedHosts() ) {
		//隠すホストの一覧（優先）
		hiddenHostList += specifiedList;
	} else {
		//見せるホストの一覧
		visibleHostList += specifiedList;
	}

	//見せるホストと隠すホストが重複したら見せる方のホストリストから重複ホストを削除
	for( unsigned int i = 0; i < hiddenHostList.count(); i++ ){
		for( QStringList::iterator itv = visibleHostList.begin(); itv != visibleHostList.end(); itv++ ){
			if ( hiddenHostList[i] == *itv ) {
				visibleHostList.remove( itv );
				break;
			}
		}
	}
	
	//隠す対象から外れたらしいホストはリストから削除
	HostList hostlist = agent->GetSkulkHost();
	for( std::vector<HostListItem>::iterator item = hostlist.begin(); item != hostlist.end(); item++ ) {
		bool isFound = false;
		//エージェントの隠すホストに追加
		for( unsigned int i = 0; i < hiddenHostList.count(); i++ ){
			if ( hiddenHostList[i] == QString( item->IpAddress().c_str() ) ) {
				isFound = true;
				break;
			}
		}
		if ( !isFound ) {
			agent->DeleteSkulkHost( *item );
		}
	}
	//エージェントの隠すホストに追加
	for( unsigned int i = 0; i < visibleHostList.count(); i++ ){
		agent->DeleteSkulkHostAddress( visibleHostList[i].data() );
	}
	//エージェントの隠すホストに追加
	for( unsigned int i = 0; i < hiddenHostList.count(); i++ ){
		agent->AddSkulkHostAddress( hiddenHostList[i].data() );
	}
}
#include "hideconfigdialog.moc"

