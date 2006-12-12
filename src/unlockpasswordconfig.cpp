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


#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpassdlg.h>
#include "kipmsgsettings.h"
#include "unlockpasswordconfig.h"

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgUnlockPasswordConfigDialog::KIPMsgUnlockPasswordConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgUnlockPasswordConfigDialogBase(parent,name,fl)
{
	m_OldPasswordInputbox->setEnabled( !KIpMsgSettings::unlockPassword().isEmpty() );
	if ( KIpMsgSettings::unlockPassword().isEmpty() ) {
		m_OldPasswordInputbox->setText("");
	}
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIPMsgUnlockPasswordConfigDialog::~KIPMsgUnlockPasswordConfigDialog()
{}

/*$SPECIALIZATION$*/
/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgUnlockPasswordConfigDialog::slotOkClicked()
{
	if ( save() ) {
		close();
	}
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgUnlockPasswordConfigDialog::slotCancelClicked()
{
	close();
}

/**
 * 設定保存
 * ・設定を保存する。
 * @retval TRUE:パスワード問題無し。保存された。FALSE:パスワードに問題がある。保存されなかった。
 */
bool KIPMsgUnlockPasswordConfigDialog::save()
{
	if ( m_OldPasswordInputbox->isEnabled() ) {
		if ( m_OldPasswordInputbox->text() != KIpMsgSettings::unlockPassword() ) {
			KMessageBox::error( this, tr2i18n("Input Old Password and configured password not match."));
			return false;
		}
	}
	if ( m_NewPasswordInputbox->text() != m_ReNewPasswordInputbox->text() ) {
		KMessageBox::error( this, tr2i18n("Input New Password and ReEnter password not match."));
		return false;
	}
	KIpMsgSettings::setUnlockPassword( m_NewPasswordInputbox->text() );
	m_OldPasswordInputbox->setEnabled( !KIpMsgSettings::unlockPassword().isEmpty() );
	KIpMsgSettings::writeConfig();
	return true;
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgUnlockPasswordConfigDialog::slotApplyClicked()
{
	save();
}

#include "unlockpasswordconfig.moc"

