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


#include <qdialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kpassdlg.h>
#include "passworddialog.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMessengerPasswordDialog::KIpMessengerPasswordDialog(QWidget* parent, const char* name, WFlags fl)
        : KIpMessengerPasswordDialogBase(parent,name,fl)
{}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMessengerPasswordDialog::~KIpMessengerPasswordDialog()
{}

/*$SPECIALIZATION$*/
/**
 * 開錠クリックイベント
 * ・パスワード一致判定を行い、
 *   不一致ならメッセージ表示。
 *   一致ならダイアログを閉じる。
 */
void KIpMessengerPasswordDialog::slotUnlockClicked()
{
	if ( m_PasswordInputbox->password() != KIpMsgSettings::unlockPassword() ) {
		KMessageBox::sorry( 0, tr2i18n( "This Password cannot confirmed." ), "KIpMessenger" );
		return;
	}
	accept();
}

#include "passworddialog.moc"

