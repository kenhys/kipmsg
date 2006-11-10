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


#include <klineedit.h>
#include <klistbox.h>
#include <kcombobox.h>
#include <qcheckbox.h>
#include <qcheckbox.h>
#include <qstringlist.h>

#include "kipmsgconfig.h"
#include "clickableurlconfig.h"
#include "detailandlogconfig.h"
#include "unlockpasswordconfig.h"
#include "kipmsgsettings.h"
#include "IpMessenger.h"

KIPMsgConfigDialog::KIPMsgConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgConfigDialogBase(parent,name,fl)
{
	m_UserNameEditbox->setText(KIpMsgSettings::userName() );

	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	vector<string> groups = agent->GetGroupList();
	for( vector<string>::iterator ixgr = groups.begin(); ixgr != groups.end();ixgr++ ){
		m_GroupNameCombobox->insertItem( ixgr->c_str() );
	}
	m_GroupNameCombobox->setCurrentText( KIpMsgSettings::groupName() );
	m_ConfirmOpenCheckbox->setChecked(KIpMsgSettings::confirmOpen() );
	m_NoPopupCheckbox->setChecked(KIpMsgSettings::noPopup() );
	m_NoSoundCheckbox->setChecked(KIpMsgSettings::noSound() );
	m_SecretDefaultCheckbox->setChecked(KIpMsgSettings::secretDefault() );
	m_QuoteDefaultCheckbox->setChecked(KIpMsgSettings::quoteDefault() );
	m_MessageEncodingCombobox->setCurrentText( KIpMsgSettings::messageEncoding() );
	m_LocalFilesystemEncodingCombobox->setCurrentText( KIpMsgSettings::localFilesystemEncoding() );
	m_BroadcastListbox->insertStringList( KIpMsgSettings::broadcastNetworkAddress() );
	m_DialupCheckbox->setChecked( KIpMsgSettings::connectDialup() );
}

KIPMsgConfigDialog::~KIPMsgConfigDialog()
{}

/*$SPECIALIZATION$*/
void KIPMsgConfigDialog::slotDetailAndLogSetupClicked()
{
	KIPMsgDetailConfigDialog *detailConfig = new KIPMsgDetailConfigDialog(this,0,TRUE);
	detailConfig->exec();
}
void KIPMsgConfigDialog::slotClickableUrlClicked()
{
	KIPMsgClickableURLConfigDialog *clickableUrl = new KIPMsgClickableURLConfigDialog(this,0,TRUE);
	clickableUrl->exec();
}
void KIPMsgConfigDialog::slotUnlockPasswordSetup()
{
	KIPMsgUnlockPasswordConfigDialog *passwordConfig = new KIPMsgUnlockPasswordConfigDialog(this,0,TRUE);
	passwordConfig->exec();
}
void KIPMsgConfigDialog::slotAddBroadcastAddressClicked()
{
	m_BroadcastListbox->insertItem( m_IpAddressOrFQDNEditbox->text() );
	m_IpAddressOrFQDNEditbox->setText("");
}
void KIPMsgConfigDialog::slotDeleteBroadcastAddressClicked()
{
	m_IpAddressOrFQDNEditbox->setText( m_BroadcastListbox->currentText() );
	m_BroadcastListbox->removeItem( m_BroadcastListbox->currentItem() );
}
void KIPMsgConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}
void KIPMsgConfigDialog::slotCancelClicked()
{
	close();
}
void KIPMsgConfigDialog::slotApplyClicked()
{
	KIpMsgSettings::setUserName( m_UserNameEditbox->text() );
	KIpMsgSettings::setGroupName( m_GroupNameCombobox->currentText() );
	KIpMsgSettings::setConfirmOpen( m_ConfirmOpenCheckbox->isChecked() );
	KIpMsgSettings::setNoPopup( m_NoPopupCheckbox->isChecked() );
	KIpMsgSettings::setNoSound( m_NoSoundCheckbox->isChecked() );
	KIpMsgSettings::setSecretDefault( m_SecretDefaultCheckbox->isChecked() );
	KIpMsgSettings::setQuoteDefault( m_QuoteDefaultCheckbox->isChecked() );
	KIpMsgSettings::setMessageEncoding( m_MessageEncodingCombobox->currentText() );
	KIpMsgSettings::setLocalFilesystemEncoding( m_LocalFilesystemEncodingCombobox->currentText() );
	QStringList networks;
	
	for( unsigned int i = 0; i < m_BroadcastListbox->count(); i++ ) {
		networks << m_BroadcastListbox->text(i);
	}
	KIpMsgSettings::setBroadcastNetworkAddress( networks );
	KIpMsgSettings::setConnectDialup( m_DialupCheckbox->isChecked() );

	KIpMsgSettings::writeConfig();
}

#include "kipmsgconfig.moc"

