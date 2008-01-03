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

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <klineedit.h>
#include <klistbox.h>
#include <kpushbutton.h>
#include <kmessagebox.h>
#include <klocale.h>
#include "kipmsgsettings.h"
#include "priorityconfig.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
PriorityConfigForm::PriorityConfigForm(QWidget* parent, const char* name, WFlags fl)
        : PriorityConfigFormBase(parent,name,fl)
{
	m_CustomizeCheckBox->setChecked( KIpMsgSettings::customizePriority() );
	m_PriorityListBox->insertStringList( KIpMsgSettings::prioritySettings() );
	slotCustomizePriorityClicked();
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
PriorityConfigForm::~PriorityConfigForm()
{}

/*$SPECIALIZATION$*/
/**
 * OKクリックイベント
 * ・ウインドウを閉じる
 */
void PriorityConfigForm::slotOKClicked()
{
	KIpMsgSettings::setCustomizePriority( m_CustomizeCheckBox->isChecked() );
	QStringList priorities;
	for( unsigned int i = 0; i < m_PriorityListBox->count(); i++ ) {
		priorities << m_PriorityListBox->text(i);
	}
	KIpMsgSettings::setPrioritySettings( priorities );
	KIpMsgSettings::writeConfig();
	close();
}
/**
 * キャンセルクリックイベント
 * ・ウインドウを閉じる
 */
void PriorityConfigForm::slotCancelClicked()
{
	close();
}

/**
 * カスタマイズするチェックボックスクリックイベント
 * ・
 */
void PriorityConfigForm::slotCustomizePriorityClicked()
{
	m_PrioritySettingGroupBox->setEnabled( m_CustomizeCheckBox->isChecked() );
	m_PriorityEdit->setEnabled( m_CustomizeCheckBox->isChecked() );
	m_PriorityListBox->setEnabled( m_CustomizeCheckBox->isChecked() );
	m_AddPriorityButton->setEnabled( m_CustomizeCheckBox->isChecked() );
	m_DeletePriorityButton->setEnabled( m_CustomizeCheckBox->isChecked() );
}

/**
 * 優先度追加クリックイベント
 * ・
 */
void PriorityConfigForm::slotAddPriorityClicked()
{
	if ( m_PriorityEdit->text() == "" ) {
		return;
	}
	if ( m_PriorityEdit->text() == "-" ) {
		KMessageBox::sorry( 0, tr2i18n( "This character \"-\" was reserved." ), "KIpMessenger" );
		return;
	}
	if ( m_PriorityEdit->text() == "X" ) {
		KMessageBox::sorry( 0, tr2i18n( "This character \"X\" was reserved." ), "KIpMessenger" );
		return;
	}
	QString insertText = m_PriorityEdit->text();
	bool isFound = false;
	for( unsigned int i = 0; i < m_PriorityListBox->count(); i++ ){
		if ( insertText == m_PriorityListBox->item( i )->text() ) {
			m_PriorityListBox->setSelected( i, TRUE );
			isFound = true;
			break;
		}
	}
	if ( !isFound ){
		m_PriorityListBox->insertItem( m_PriorityEdit->text() );
		m_PriorityListBox->setSelected( m_PriorityListBox->count() - 1, TRUE );
	}
	m_PriorityEdit->setText("");
}

/**
 * 優先度削除クリックイベント
 * ・
 */
void PriorityConfigForm::slotDeletePriorityClicked()
{
	m_PriorityEdit->setText( m_PriorityListBox->currentText() );

	unsigned int rmv = m_PriorityListBox->currentItem();
	m_PriorityListBox->removeItem( rmv );

	if ( m_PriorityListBox->count() >= rmv + 1 ) {
		m_PriorityListBox->setSelected( rmv, TRUE );
	} else {
		m_PriorityListBox->setSelected( m_PriorityListBox->count() - 1, TRUE );
	}
}

#include "priorityconfig.moc"
