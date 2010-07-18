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

#include <qlabel.h>
#include <qcheckbox.h>
#include <qtreeview.h>
#include <klocale.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klistwidget.h>
#include <kpushbutton.h>
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
HideConfigDialog::HideConfigDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
//        : HideConfigDialogBase(parent,name,fl)
        : HideConfigDialogBase()
{
	kDebug() << "START HideConfigDialog::HideConfigDialog" << endl;
	setupUi(this);
	setButtons( None );
	m_HiddenHostsCheckBox->setChecked( KIpMsgSettings::hideFromHiddenHosts() );
	m_EncriptionNotSupporedHostsCheckBox->setChecked( KIpMsgSettings::hideFromEncryptionNotSupportedHosts() );
	m_EnableHideFromSpecifiedHostsCheckBox->setChecked( KIpMsgSettings::hideFromSpecifiedHosts() );
//TESTME
	m_AddressListBox->addItems( KIpMsgSettings::hideFromSpecifiedHostList() );
	slotEnableSpecifiedHostClicked();

	//ホスト名一覧
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
/*
	m_HostsListView->addColumn( tr2i18n("IP Address"), 0 );
	m_HostsListView->addColumn( tr2i18n("Host"), 1 );
*/
	KIpMessengerHiddenConfigModel *model = new KIpMessengerHiddenConfigModel(m_HostsListView);
	m_HostsListView->setHeaderHidden( false );
	m_HostsListView->setRootIsDecorated( false );
	m_HostsListView->setAllColumnsShowFocus( true );
	m_HostsListView->setModel( model );

	HostList hosts = agent->GetHostList();
//TESTME
	for( vector<HostListItem>::iterator ix = hosts.begin(); ix != hosts.end(); ix++ ){
		KIpMessengerHiddenConfigEntry entry = KIpMessengerHiddenConfigEntry(
													ix->IpAddress().c_str(),
													ix->HostName().c_str() );
		model->addEntry(entry);
	}
	kDebug() << "END   HideConfigDialog::HideConfigDialog" << endl;
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
HideConfigDialog::~HideConfigDialog()
{}

void
HideConfigDialog::slotAddHideAddressClicked(){
	kDebug() << "START HideConfigDialog::slotAddHideAddressClicked" << endl;
	QString insertText = m_AddressEditbox->text();
	if ( insertText == "" ) {
		kDebug() << "END   HideConfigDialog::slotAddHideAddressClicked" << endl;
		return;
	}
	if(m_AddressListBox->findItems( insertText, Qt::MatchCaseSensitive ).count() > 0){
		kDebug() << "END   HideConfigDialog::slotAddHideAddressClicked" << endl;
		return;
	}
	m_AddressListBox->addItem( m_AddressEditbox->text() );
	m_AddressListBox->item( m_AddressListBox->count() - 1)->setSelected( TRUE );
	m_AddressListBox->setCurrentRow( m_AddressListBox->count() - 1);
	m_AddressEditbox->setText("");
	kDebug() << "END   HideConfigDialog::slotAddHideAddressClicked" << endl;
}

void
HideConfigDialog::slotDeleteHideAddressClicked(){
	kDebug() << "START HideConfigDialog::slotDeleteHideAddressClicked" << endl;
	int rmv = m_AddressListBox->currentRow();
	if( rmv < 0 ){
		return;
	}

	QListWidgetItem *currentItem = m_AddressListBox->takeItem( rmv );
	m_AddressEditbox->setText( currentItem->text() );
	delete currentItem;
	if ( m_AddressListBox->count() > 0 ) {
		int currentRow = 0;
		if ( m_AddressListBox->count() > rmv + 1 ) {
			currentRow = rmv;
		} else {
			currentRow = m_AddressListBox->count() - 1;
		}
		m_AddressListBox->item( currentRow )->setSelected( TRUE );
		m_AddressListBox->setCurrentRow( currentRow );
	}
	kDebug() << "END   HideConfigDialog::slotDeleteHideAddressClicked" << endl;
}

void
HideConfigDialog::slotOKClicked(){
	kDebug() << "START HideConfigDialog::slotOKClicked" << endl;
	slotApplyClicked();
//	close();
	accept();
	kDebug() << "END   HideConfigDialog::slotOKClicked" << endl;
}

void
HideConfigDialog::slotCancelClicked(){
	kDebug() << "START HideConfigDialog::slotCancelClicked" << endl;
//	close();
	reject();
	kDebug() << "END   HideConfigDialog::slotCancelClicked" << endl;
}

void
HideConfigDialog::slotApplyClicked(){
	kDebug() << "START HideConfigDialog::slotApplyClicked" << endl;
	KIpMsgSettings::setHideFromHiddenHosts( m_HiddenHostsCheckBox->isChecked() );
	KIpMsgSettings::setHideFromEncryptionNotSupportedHosts( m_EnableHideFromSpecifiedHostsCheckBox->isChecked() );
	KIpMsgSettings::setHideFromSpecifiedHosts( m_EncriptionNotSupporedHostsCheckBox->isChecked() );

	QStringList addresses;
	for( int i = 0; i < m_AddressListBox->count(); i++ ) {
		addresses << m_AddressListBox->item(i)->text();
	}
	KIpMsgSettings::setHideFromSpecifiedHostList( addresses );

	KIpMsgSettings::self()->writeConfig();

	HideConfigDialog::SetupHideHosts();
	kDebug() << "END   HideConfigDialog::slotApplyClicked" << endl;
}

void
HideConfigDialog::slotEnableSpecifiedHostClicked(){
	kDebug() << "START HideConfigDialog::slotEnableSpecifiedHostClicked" << endl;
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
	QModelIndex index = QModelIndex();
	slotIPAddressListViewSelectionChanged(index);
	kDebug() << "END   HideConfigDialog::slotEnableSpecifiedHostClicked" << endl;
}

void
HideConfigDialog::slotCopyIPAddressClicked()
{
	kDebug() << "START HideConfigDialog::slotCopyIPAddressClicked" << endl;
	//選択中のホストのアドレスをコピー
//TESTME
	KIpMessengerHiddenConfigModel *dataModel = (KIpMessengerHiddenConfigModel*)m_HostsListView->model();
	QListIterator<QModelIndex> it = m_HostsListView->selectionModel()->selectedRows();

	//一個目のみが対象
	while( it.hasNext() ){
		KIpMessengerHiddenConfigEntry entry = dataModel->getHiddenConfigEntry( it.next() );
		m_AddressEditbox->setText( entry.ipAddress() );
		break;
	}
	kDebug() << "END   HideConfigDialog::slotCopyIPAddressClicked" << endl;
}

void
HideConfigDialog::slotIPAddressListViewSelectionChanged(QModelIndex &current)
{
	kDebug() << "START HideConfigDialog::slotIPAddressListViewSelectionChanged" << endl;
	//コピーボタンの有効／無効を設定する
	if(!current.isValid()){
		m_CopyIPAddressButton->setEnabled( FALSE );
	}else{
		m_CopyIPAddressButton->setEnabled( TRUE );
	}
	kDebug() << "END   HideConfigDialog::slotIPAddressListViewSelectionChanged" << endl;
}

void
HideConfigDialog::SetupHideHosts(){
	kDebug() << "START HideConfigDialog::SetupHideHosts" << endl;
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	QStringList hiddenHostList;
	QStringList visibleHostList;

	//非表示ホストから隠す
	QStringList priorityHiddenList = KIpMsgSettings::priorityHidden();
	if ( KIpMsgSettings::hideFromHiddenHosts() ) {
		//隠すホストの一覧（優先）
		for( QStringList::iterator ith = priorityHiddenList.begin(); ith != priorityHiddenList.end(); ith++ ) {
//			QStringList hListItem = QStringList::split( "|", *ith );
			QStringList hListItem = ith->split( "|" );
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
//			QStringList hListItem = QStringList::split( "|", *ith );
			QStringList hListItem = ith->split( "|" );
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
	for( int i = 0; i < hiddenHostList.count(); i++ ){
		for( QStringList::iterator itv = visibleHostList.begin(); itv != visibleHostList.end(); itv++ ){
			if ( hiddenHostList[i] == *itv ) {
				visibleHostList.removeAll( *itv );
				break;
			}
		}
	}
	
	//隠す対象から外れたらしいホストはリストから削除
	HostList hostlist = agent->GetSkulkHost();
	for( std::vector<HostListItem>::iterator item = hostlist.begin(); item != hostlist.end(); item++ ) {
		bool isFound = false;
		//エージェントの隠すホストに追加
		for( int i = 0; i < hiddenHostList.count(); i++ ){
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
	for( int i = 0; i < visibleHostList.count(); i++ ){
		agent->DeleteSkulkHostAddress( (const char*)visibleHostList[i].toAscii().data() );
	}
	//エージェントの隠すホストに追加
	for( int i = 0; i < hiddenHostList.count(); i++ ){
		agent->AddSkulkHostAddress( (const char*)hiddenHostList[i].toAscii().data() );
	}
	kDebug() << "END   HideConfigDialog::SetupHideHosts" << endl;
}

KIpMessengerHiddenConfigEntry::KIpMessengerHiddenConfigEntry(QString ipAddress, QString hostName){
	kDebug() << "START KIpMessengerHiddenConfigEntry::KIpMessengerHiddenConfigEntry" << endl;
	this->m_IpAddress = ipAddress;
	this->m_HostName = hostName;
	kDebug() << "END   KIpMessengerHiddenConfigEntry::KIpMessengerHiddenConfigEntry" << endl;
}

void
KIpMessengerHiddenConfigModel::clear(){
	kDebug() << "START KIpMessengerHiddenConfigModel::clear" << endl;
	removeRows(0,rowCount()-1);
	kDebug() << "END   KIpMessengerHiddenConfigModel::clear" << endl;
}
void
KIpMessengerHiddenConfigModel::setHiddenConfigEntry(const QModelIndex &index, const KIpMessengerHiddenConfigEntry entry) {
	kDebug() << "START KIpMessengerHiddenConfigModel::setHiddenConfigEntry" << endl;
	hiddenConfigList.replace(index.row(), entry);
	kDebug() << "END   KIpMessengerHiddenConfigModel::setHiddenConfigEntry" << endl;
}
KIpMessengerHiddenConfigEntry KIpMessengerHiddenConfigModel::getHiddenConfigEntry(const QModelIndex &index) {
	kDebug() << "START KIpMessengerHiddenConfigModel::getHiddenConfigEntry" << endl;
	KIpMessengerHiddenConfigEntry entry = hiddenConfigList.at(index.row());
	kDebug() << "END   KIpMessengerHiddenConfigModel::getHiddenConfigEntry" << endl;
	return entry;
}
int
KIpMessengerHiddenConfigModel::rowCount(const QModelIndex &parent) const{
	kDebug() << "START KIpMessengerHiddenConfigModel::rowCount" << endl;
	kDebug() << "END   KIpMessengerHiddenConfigModel::rowCount" << endl;
	return hiddenConfigList.count();
}
QVariant
KIpMessengerHiddenConfigModel::data(const QModelIndex &index, int role) const{
	kDebug() << "START KIpMessengerHiddenConfigModel::data" << endl;
	if (!index.isValid()){
		kDebug() << "END   KIpMessengerHiddenConfigModel::data" << endl;
		return QVariant();
	}

	if (index.row() >= hiddenConfigList.size()){
		kDebug() << "END   KIpMessengerHiddenConfigModel::data" << endl;
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		KIpMessengerHiddenConfigEntry entry = hiddenConfigList.at(index.row());
		switch(index.column()){
			case 0:		return entry.ipAddress();
			case 1:		return entry.hostName();
			default:	return QVariant();
		}
	} else {
		kDebug() << "END   KIpMessengerHiddenConfigModel::data" << endl;
		return QVariant();
	}

}
QVariant
KIpMessengerHiddenConfigModel::headerData(int selection, Qt::Orientation orientation, int role) const{
	kDebug() << "START KIpMessengerHiddenConfigModel::headerData" << endl;
	if (role != Qt::DisplayRole){
		kDebug() << "END   KIpMessengerHiddenConfigModel::headerData" << endl;
		return QVariant();
	}

	if (orientation == Qt::Horizontal){
		switch(selection){
			case 0: 	return tr2i18n( "IP address" );
			case 1: 	return tr2i18n( "Host" );
			default:	return QVariant();
		}
	}
	kDebug() << "END   KIpMessengerHiddenConfigModel::headerData" << endl;
	return QVariant();
}

bool
KIpMessengerHiddenConfigModel::addEntry(const KIpMessengerHiddenConfigEntry entry){
	kDebug() << "START KIpMessengerHiddenConfigModel::addEntry" << endl;
	hiddenConfigList.append(entry);
	kDebug() << "END   KIpMessengerHiddenConfigModel::addEntry" << endl;
	return true;
}

#include "hideconfigdialog.moc"

