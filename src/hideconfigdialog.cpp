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
#include <qcheckbox.h>
#include <klineedit.h>
#include <klistbox.h>
#include <kpushbutton.h>
#include "kipmsgsettings.h"

#include "hideconfigdialog.h"

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
	m_EnableSpecifiedHostsCheckBox->setChecked( KIpMsgSettings::hideFromSpecifiedHosts() );
	m_AddressListBox->insertStringList( KIpMsgSettings::hideFromSpecifiedHostList() );

	slotEnableSpecifiedHostClicked();
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
	m_AddressListBox->insertItem( m_AddressEditbox->text() );
	m_AddressListBox->setSelected( m_AddressListBox->count() - 1, TRUE );
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
	KIpMsgSettings::setHideFromEncryptionNotSupportedHosts( m_EnableSpecifiedHostsCheckBox->isChecked() );
	KIpMsgSettings::setHideFromSpecifiedHosts( m_EncriptionNotSupporedHostsCheckBox->isChecked() );

	QStringList addresses;
	for( unsigned int i = 0; i < m_AddressListBox->count(); i++ ) {
		addresses << m_AddressListBox->text(i);
	}
	KIpMsgSettings::setHideFromSpecifiedHostList( addresses );

	KIpMsgSettings::writeConfig();
}

void HideConfigDialog::slotEnableSpecifiedHostClicked(){
	if ( m_EnableSpecifiedHostsCheckBox->isChecked() ) {
		m_DeleteAddressButton->setEnabled( TRUE );
		m_AddAddressButton->setEnabled( TRUE );
		m_DeleteAddressButton->setEnabled( TRUE );
		m_AddressLabel->setEnabled( TRUE );
		m_AddressListBox->setEnabled( TRUE );
		m_AddressEditbox->setEnabled( TRUE );
	} else {
		m_DeleteAddressButton->setEnabled( FALSE );
		m_AddAddressButton->setEnabled( FALSE );
		m_DeleteAddressButton->setEnabled( FALSE );
		m_AddressLabel->setEnabled( FALSE );
		m_AddressListBox->setEnabled( FALSE );
		m_AddressEditbox->setEnabled( FALSE );
	}
}

#include "hideconfigdialog.moc"

