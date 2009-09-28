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

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <kpushbutton.h>

#include "kipmsgsettings.h"
#include "senddialog.h"
#include "showconfig.h"

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgShowConfigDialog::KIPMsgShowConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgShowConfigDialogBase(parent,name,fl)
{
	m_GroupCheckbox->setChecked( KIpMsgSettings::showGroupName() );
	m_HostCheckbox->setChecked( KIpMsgSettings::showHostName() );
	m_IpAddrCheckbox->setChecked( KIpMsgSettings::showIpAddress() );
	m_LoginCheckbox->setChecked( KIpMsgSettings::showLoginName() );
	m_PriorityCheckbox->setChecked( KIpMsgSettings::showPriority() );
	m_EncodingCheckbox->setChecked( KIpMsgSettings::showEncoding() );
	m_GroupOrderCheckbox->setChecked( KIpMsgSettings::sortGroupName() );
	m_1stOrderDescButton->setOn( KIpMsgSettings::sort1stGroupingDesc() );
	m_UserOrderRadio->setChecked( KIpMsgSettings::sortUserName() );
	m_IpAddrOrderRadio->setChecked( KIpMsgSettings::sortIpAddress() );
	m_HostRadio->setChecked( KIpMsgSettings::sortHostName() );
	m_2ndOrderDescButton->setOn( KIpMsgSettings::sort2ndGroupingDesc() );
	m_IgnoreCaseSingleByteCheckbox->setChecked( KIpMsgSettings::ignoreCaseAsSingleByte() );
	m_PriorityMultiBytesCheckbox->setChecked( KIpMsgSettings::priorityMultiBytes() );
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIPMsgShowConfigDialog::~KIPMsgShowConfigDialog()
{}

/*$SPECIALIZATION$*/
/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgShowConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgShowConfigDialog::slotCancelClicked()
{
	close();
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgShowConfigDialog::slotApplyClicked()
{
	KIpMsgSettings::setShowGroupName( m_GroupCheckbox->isChecked() );
	KIpMsgSettings::setShowHostName( m_HostCheckbox->isChecked() );
	KIpMsgSettings::setShowIpAddress( m_IpAddrCheckbox->isChecked() );
	KIpMsgSettings::setShowLoginName( m_LoginCheckbox->isChecked() );
	KIpMsgSettings::setShowPriority( m_PriorityCheckbox->isChecked() );
	KIpMsgSettings::setShowEncoding( m_EncodingCheckbox->isChecked() );
	KIpMsgSettings::setSortGroupName( m_GroupOrderCheckbox->isChecked() );
	KIpMsgSettings::setSort1stGroupingDesc( m_1stOrderDescButton->isOn() );
	KIpMsgSettings::setSortUserName( m_UserOrderRadio->isChecked() );
	KIpMsgSettings::setSortIpAddress( m_IpAddrOrderRadio->isChecked() );
	KIpMsgSettings::setSortHostName( m_HostRadio->isChecked() );
	KIpMsgSettings::setSort2ndGroupingDesc( m_2ndOrderDescButton->isOn() );
	KIpMsgSettings::setIgnoreCaseAsSingleByte( m_IgnoreCaseSingleByteCheckbox->isChecked() );
	KIpMsgSettings::setPriorityMultiBytes( m_PriorityMultiBytesCheckbox->isChecked() );
	KIpMsgSettings::writeConfig();
	if ( senddialog != NULL ) senddialog->refreshHostList(KIpMsgEvent::Event_TriggerIsNotEvent);
}

#include "showconfig.moc"
