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

#include <qmap.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <klineedit.h>
#include <klistbox.h>
#include <kfiledialog.h>

#include "clickableurlconfig.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgClickableURLConfigDialog::KIPMsgClickableURLConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgClickableURLConfigDialogBase(parent,name,fl)
{
	m_DefaultBrowserCheckbox->setChecked( KIpMsgSettings::useDefaultBrowser() );
	QStringList pro = KIpMsgSettings::executePrograms();

	for( QStringList::iterator p = pro.begin(); p != pro.end(); p++ ){
		QString item = *p;
		int pos = item.find(':');
		if ( pos < 0 ) {
			protocols[item] = "";
		} else {
			QString key = item.left( pos );
			QString val = item.mid( pos + 1 );
			protocols[key] = val;
		}
	}
	if ( m_ProtocolsListbox->count() == 0 ) {
		m_ProtocolsListbox->setCurrentItem( 0 );
		m_ProgramNameEditbox->setText( protocols[m_ProtocolsListbox->currentText()] );
	}
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIPMsgClickableURLConfigDialog::~KIPMsgClickableURLConfigDialog()
{}

/*$SPECIALIZATION$*/

/**
 * プログラム変更イベント
 * ・変更されたプログラムをプロトコルに対応づける。
 * @param text 変更されたテキスト
 */
void KIPMsgClickableURLConfigDialog::slotProgramNameChanged(const QString& text)
{
	protocols[m_ProtocolsListbox->currentText()] = text;
}

/**
 * プロトコル選択イベント
 * ・プロトコルに設定されているプログラムを表示する。
 * @param item リストボックスアイテム
 */
void KIPMsgClickableURLConfigDialog::slotBrowseClicked()
{
	QString programName = KFileDialog::getOpenFileName();
	if ( programName != "" ) {
		protocols[m_ProtocolsListbox->currentText()] = programName;
		m_ProgramNameEditbox->setText( protocols[m_ProtocolsListbox->currentText()] );
	}
}

/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgClickableURLConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgClickableURLConfigDialog::slotCancelClicked()
{
	close();
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgClickableURLConfigDialog::slotApplyClicked()
{
	KIpMsgSettings::setUseDefaultBrowser( m_DefaultBrowserCheckbox->isChecked() );

	QStringList pro;
	for( QMap<QString,QString>::iterator p = protocols.begin(); p != protocols.end(); p++ ){
		pro << p.key() + ":" + p.data();
	}

	KIpMsgSettings::setExecutePrograms( pro );

	KIpMsgSettings::writeConfig();
}

/**
 * プロトコル選択イベント
 * ・プロトコルに設定されているプログラムを表示する。
 * @param item リストボックスアイテム
 */
void KIPMsgClickableURLConfigDialog::slotProtocolClicked(QListBoxItem* item)
{
	m_ProgramNameEditbox->setText( protocols[item->text()] );
}

#include "clickableurlconfig.moc"

