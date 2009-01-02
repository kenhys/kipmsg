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
#include <qstringlist.h>
#include <qcheckbox.h>
#include <qtextcodec.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <klistbox.h>
#include <knuminput.h>

#include "networkconfig.h"
#include "kipmsgsettings.h"
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

/**
 * コンストラクタ
 * ・設定をロード
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
NetworkConfig::NetworkConfig(QWidget* parent, const char* name, WFlags fl)
        : NetworkConfigBase(parent,name,fl)
{
	m_BroadcastListbox->insertStringList( KIpMsgSettings::broadcastNetworkAddress() );
	m_DialupCheckbox->setChecked( KIpMsgSettings::connectDialup() );

	m_SpecifyNicCheckbox->setChecked( KIpMsgSettings::specifyNetworkInterfaceCard() );

	//使うリスト
	QStringList portSettings = KIpMsgSettings::networkInterfacePortNumbers();
	QStringList nicNames;
	QStringList nicAddrs;

	//移行前のチェック
	bool isShift5to6After=false;
	for( unsigned int i = 0; i < portSettings.count(); i++ ){
		if ( portSettings[i].find( "|" ) >= 0 ){
			isShift5to6After = true;
			break;
		}
	}
	if ( isShift5to6After ) {
		for( unsigned int i = 0; i < portSettings.count(); i++ ){
			QStringList items = QStringList::split( "|", portSettings[i] );
			nicNames << items[0];
			nicAddrs << items[1];
		}
	} else {
		for( unsigned int i = 0; i < portSettings.count(); i++ ){
			QStringList items = QStringList::split( ":", portSettings[i] );
			nicNames << items[0];
			nicAddrs << items[1];
			portSettings[i] = QString( "%1|%2|%3" ).arg( items[0] ).arg( items[1] ).arg( items[2] );
		}
	}
	m_UseInterfaceListbox->insertStringList( portSettings );

	//使わないリスト
	vector<NetworkInterface> nics;
	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	IpMessengerAgent::GetNetworkInterfaceInfo( nics, agent->UseIPv6() );
	QStringList list;
	for( vector<NetworkInterface>::iterator n = nics.begin(); n != nics.end(); ++n ) {
		bool isFound = false;
		for( unsigned int i = 0; i < nicNames.count(); i++ ){
			if ( nicNames[i] == QString( n->DeviceName().c_str() ) && nicAddrs[i] == QString( n->IpAddress().c_str() ) ) {
				isFound = true;
				break;
			}
		}
		if ( !isFound ) {
			list << QString( "%1|%2|%3" ).arg( n->DeviceName().c_str() ).arg( n->IpAddress().c_str() ).arg( n->PortNo() );
		}
	}
	m_InterfaceListbox->insertStringList( list );

	m_UseInterfaceListbox->setSelected( 0, TRUE );
	slotUseInterfaceClicked( m_UseInterfaceListbox->selectedItem() );
	setNetworkInterfaceStatus();
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
NetworkConfig::~NetworkConfig()
{}

/*$SPECIALIZATION$*/

/**
 * ブロードキャスト設定の「>>」クリックイベント
 * ・左のネットワークアドレス入力ボックスから右のリストボックスに移動し、
 * ・左のネットワークアドレス入力ボックスをクリア
 */
void NetworkConfig::slotAddBroadcastAddressClicked()
{
	if ( m_IpAddressOrFQDNEditbox->text() == "" ) {
		return;
	}
	QString insertText = m_IpAddressOrFQDNEditbox->text();
	bool isFound = false;
	for( unsigned int i = 0; i < m_BroadcastListbox->count(); i++ ){
		if ( insertText == m_BroadcastListbox->item( i )->text() ) {
			m_BroadcastListbox->setSelected( i, TRUE );
			isFound = true;
			break;
		}
	}
	if ( !isFound ){
		m_BroadcastListbox->insertItem( m_IpAddressOrFQDNEditbox->text() );
		m_BroadcastListbox->setSelected( m_BroadcastListbox->count() - 1, TRUE );
	}
	m_IpAddressOrFQDNEditbox->setText("");
}
/**
 * ブロードキャスト設定の「<<」クリックイベント
 * ・右のリストボックスの選択中のアドレスのデータを左のネットワークアドレス入力ボックスにコピーし、
 * ・右のリストボックスの選択中のアドレスを削除
 */
void NetworkConfig::slotDeleteBroadcastAddressClicked()
{
	m_IpAddressOrFQDNEditbox->setText( m_BroadcastListbox->currentText() );


	unsigned int rmv = m_BroadcastListbox->currentItem();
	m_BroadcastListbox->removeItem( rmv );

	if ( m_BroadcastListbox->count() >= rmv + 1 ) {
		m_BroadcastListbox->setSelected( rmv, TRUE );
	} else {
		m_BroadcastListbox->setSelected( m_BroadcastListbox->count() - 1, TRUE );
	}
//	m_BroadcastListbox->removeItem( m_BroadcastListbox->currentItem() );
}
/**
 * OKクリックイベント
 * ・設定を保存
 * ・ダイアログを閉じる
 */
void NetworkConfig::slotOkClicked()
{
	slotApplyClicked();
	close();
}
/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる
 */
void NetworkConfig::slotCancelClicked()
{
	close();
}
/**
 * 適用クリックイベント
 * ・設定を保存
 */
void NetworkConfig::slotApplyClicked()
{
	KIpMsgSettings::setSpecifyNetworkInterfaceCard( m_SpecifyNicCheckbox->isChecked() );
	if ( m_SpecifyNicCheckbox->isChecked() && m_UseInterfaceListbox->count() == 0 ){
		//TODO Messagebox
		printf("TODO Messagebox\n");
	}
	QStringList iface;
	QStringList nicNames;
	for( unsigned int i = 0; i < m_UseInterfaceListbox->count(); i++ ) {
		iface << m_UseInterfaceListbox->text(i);
	}
	KIpMsgSettings::setNetworkInterfacePortNumbers( iface );

	QStringList networks;
	for( unsigned int i = 0; i < m_BroadcastListbox->count(); i++ ) {
		networks << m_BroadcastListbox->text(i);
	}
	KIpMsgSettings::setBroadcastNetworkAddress( networks );
	KIpMsgSettings::setConnectDialup( m_DialupCheckbox->isChecked() );

	KIpMsgSettings::writeConfig();

	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
//TODO IPv6
	agent->setUseIPv6( true );
	agent->RestartNetwork( getSpecifyNics() );
	agent->setIsDialup( KIpMsgSettings::connectDialup() );
	agent->ClearBroadcastAddress();
	QStringList broadcastNetworkAddress = KIpMsgSettings::broadcastNetworkAddress();
	for( QStringList::iterator it = broadcastNetworkAddress.begin(); it != broadcastNetworkAddress.end(); it++){
		QString broadcastAddress = *it;
		agent->AddBroadcastAddress( broadcastAddress.data() );
	}
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
	agent->Login( codec->fromUnicode( KIpMsgSettings::userName() ).data(), codec->fromUnicode( KIpMsgSettings::groupName() ).data() );
}

/**
 * NIC設定の「>>」クリックイベント
 * ・左のNICリストボックスから右のリストボックスに移動し、
 * ・左のNICリストボックスをクリア
 */
void NetworkConfig::slotAddInterfaceClicked()
{
	if ( m_InterfaceListbox->currentText() == "" ) {
		return;
	}
	m_UseInterfaceListbox->insertItem( m_InterfaceListbox->currentText() );
	unsigned int rmv = m_InterfaceListbox->currentItem();
	m_InterfaceListbox->removeItem( rmv );

	if ( m_InterfaceListbox->count() >= rmv + 1 ) {
		m_InterfaceListbox->setSelected( rmv, TRUE );
	} else {
		m_InterfaceListbox->setSelected( m_InterfaceListbox->count() - 1, TRUE );
	}
	m_UseInterfaceListbox->setSelected( m_UseInterfaceListbox->count() - 1, TRUE );
	slotUseInterfaceClicked( m_UseInterfaceListbox->selectedItem() );
}

/**
 * NIC設定の「<<」クリックイベント
 * ・右のリストボックスの選択中のNICのデータを左のNICリストボックスにコピーし、
 * ・右のリストボックスの選択中のNICを削除
 */
void NetworkConfig::slotDeleteInterfaceClicked()
{
	if ( m_UseInterfaceListbox->currentText() == "" ) {
		return;
	}
	m_InterfaceListbox->insertItem( m_UseInterfaceListbox->currentText() );
	unsigned int rmv = m_UseInterfaceListbox->currentItem();
	m_UseInterfaceListbox->removeItem( rmv );
	if ( m_UseInterfaceListbox->count() >= rmv + 1 ) {
		m_UseInterfaceListbox->setSelected( rmv, TRUE );
	} else {
		m_UseInterfaceListbox->setSelected( m_UseInterfaceListbox->count() - 1, TRUE );
	}
	m_InterfaceListbox->setSelected( m_InterfaceListbox->count() - 1, TRUE );
	slotUseInterfaceClicked( m_UseInterfaceListbox->selectedItem() );
}

/**
 * 右のNICリストボックスクリックイベント
 * ・リストボックスから入力領域にデータを設定
 */
void NetworkConfig::slotUseInterfaceClicked(QListBoxItem* item)
{
	if ( item == NULL ) {
		m_InterfaceNameLabel->setText( "" );
		m_IpAddressLabel->setText( "" );
		m_PortNoNumInput->setValue( 2425 );
		setNetworkInterfaceStatus();
		return;
	}
	QStringList items = QStringList::split("|", item->text() );
	m_InterfaceNameLabel->setText( items[0] );
	m_IpAddressLabel->setText( items[1] );
	m_PortNoNumInput->setValue( items[2].toInt() );
	
	setNetworkInterfaceStatus();
}

/**
 * 設定ボタンクリックイベント
 * ・入力領域からリストボックスにデータを設定
 */
void NetworkConfig::slotSetPortNoClicked()
{
	int index = m_UseInterfaceListbox->currentItem();
	if ( index < 0 ) {
		return;
	}
	QString set = QString( "%1|%2|%3" ).arg( m_InterfaceNameLabel->text() ).arg( m_IpAddressLabel->text() ).arg( m_PortNoNumInput->value() );
	m_UseInterfaceListbox->removeItem( index );
	m_UseInterfaceListbox->insertItem( set, index );
	m_UseInterfaceListbox->setSelected( index, TRUE );
}

/**
 * NICを指定チェックボックスクリックイベント
 * ・入力領域からリストボックスにデータを設定
 */
void NetworkConfig::slotSpecifyNetworkInterfaceClicked()
{
	setNetworkInterfaceStatus();
}

/**
 * デフォルトボタンクリックイベント
 * ・入力領域のポート番号をデフォルトに戻す
 */
void NetworkConfig::slotSetDefaultPortNoClicked()
{
	m_PortNoNumInput->setValue( 2425 );
}

/**
 * 入力領域の有効／無効を設定
 */
void NetworkConfig::setNetworkInterfaceStatus()
{
	m_InterfaceListbox->setEnabled( m_SpecifyNicCheckbox->isChecked() );
	m_UseInterfaceListbox->setEnabled( m_SpecifyNicCheckbox->isChecked() );
	m_AddInterfaceButton->setEnabled( m_SpecifyNicCheckbox->isChecked() );
	m_DeleteInterfaceButton->setEnabled( m_SpecifyNicCheckbox->isChecked() );
	if ( m_UseInterfaceListbox->count() == 0 ) {
		m_InterfaceNameLabel->setHidden( TRUE );
		m_IpAddressLabel->setHidden( TRUE );
		m_PortNoNumInput->setHidden( TRUE );
		m_PortNoSetButton->setHidden( TRUE );
		m_SetDefaultPortNoButton->setHidden( TRUE );
	} else {
		m_InterfaceNameLabel->setHidden( FALSE );
		m_IpAddressLabel->setHidden( FALSE );
		m_PortNoNumInput->setHidden( FALSE );
		m_PortNoSetButton->setHidden( FALSE );
		m_SetDefaultPortNoButton->setHidden( FALSE );
		m_InterfaceNameLabel->setEnabled( m_SpecifyNicCheckbox->isChecked() );
		m_IpAddressLabel->setEnabled( m_SpecifyNicCheckbox->isChecked() );
		m_PortNoNumInput->setEnabled( m_SpecifyNicCheckbox->isChecked() );
		m_PortNoSetButton->setEnabled( m_SpecifyNicCheckbox->isChecked() );
		m_SetDefaultPortNoButton->setEnabled( m_SpecifyNicCheckbox->isChecked() );
	}
}


/**
 * 設定されている使用するNICを返す。
 * @retval NICのベクタ
 */
std::vector<ipmsg::NetworkInterface> NetworkConfig::getSpecifyNics()
{
	QStringList portNumbers = KIpMsgSettings::networkInterfacePortNumbers();
	std::vector<ipmsg::NetworkInterface> nics;
	if ( !KIpMsgSettings::specifyNetworkInterfaceCard() ) {
		return nics;
	}

	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	IpMessengerAgent::GetNetworkInterfaceInfo( nics, agent->UseIPv6() );
	//移行前のチェック
	bool isShift5to6After=false;
	for( unsigned int i = 0; i < portNumbers.count(); i++ ){
		if ( portNumbers[i].find( "|" ) >= 0 ){
			isShift5to6After = true;
			break;
		}
	}
	if ( isShift5to6After ) {
		for(vector<NetworkInterface>::iterator n = nics.begin(); n != nics.end(); n++ ){
			bool isFound = false;
			for(unsigned int i = 0; i < portNumbers.size(); i++ ){
				QStringList items = QStringList::split( "|", portNumbers[i] );
				if ( QString( n->DeviceName().c_str() ) == items[0] && QString( n->IpAddress().c_str() ) == items[1] ){
					n->setPortNo( items[2].toInt() );
					isFound = true;
					break;
				}
			}
			if ( !isFound ) {
				n = nics.erase( n ) - 1;
			}
		}
	} else {
		for(vector<NetworkInterface>::iterator n = nics.begin(); n != nics.end(); n++ ){
			bool isFound = false;
			for(unsigned int i = 0; i < portNumbers.size(); i++ ){
				QStringList items = QStringList::split( ":", portNumbers[i] );
				if ( QString( n->DeviceName().c_str() ) == items[0] && QString( n->IpAddress().c_str() ) == items[1] ){
					n->setPortNo( items[2].toInt() );
					isFound = true;
					break;
				}
			}
			if ( !isFound ) {
				n = nics.erase( n ) - 1;
			}
		}
	}
	return nics;
}

#include "networkconfig.moc"

