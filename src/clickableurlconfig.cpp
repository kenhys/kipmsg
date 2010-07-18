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

#include <qmap.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klistwidget.h>
#include <kfiledialog.h>
#include <qstring.h>

#include "clickableurlconfig.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgClickableURLConfigDialog::KIPMsgClickableURLConfigDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
//        : KIPMsgClickableURLConfigDialogBase(parent,name,fl)
        : KIPMsgClickableURLConfigDialogBase()
{
	kDebug() << "START KIPMsgClickableURLConfigDialog::KIPMsgClickableURLConfigDialog" << endl;
	setupUi(this);
	setButtons( None );
	m_DefaultBrowserCheckbox->setChecked( KIpMsgSettings::useDefaultBrowser() );
	QStringList pro = KIpMsgSettings::executePrograms();

	for( QStringList::iterator p = pro.begin(); p != pro.end(); p++ ){
		QString item = *p;
		int pos = item.indexOf(':');
		if ( pos < 0 ) {
			protocols[item] = "";
		} else {
			QString key = item.left( pos );
			QString val = item.mid( pos + 1 );
			protocols[key] = val;
		}
	}
	m_ProtocolsListbox->item( 0 )->setSelected( TRUE );
	m_ProtocolsListbox->setCurrentRow( 0 );
	m_ProgramNameEditbox->setText( protocols[m_ProtocolsListbox->currentItem()->text()] );
	kDebug() << "END   KIPMsgClickableURLConfigDialog::KIPMsgClickableURLConfigDialog" << endl;
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
	kDebug() << "START KIPMsgClickableURLConfigDialog::slotProgramNameChanged" << endl;
	protocols[m_ProtocolsListbox->currentItem()->text()] = text;
	kDebug() << "END   KIPMsgClickableURLConfigDialog::slotProgramNameChanged" << endl;
}

/**
 * プロトコル選択イベント
 * ・プロトコルに設定されているプログラムを表示する。
 * @param item リストボックスアイテム
 */
void KIPMsgClickableURLConfigDialog::slotBrowseClicked()
{
	kDebug() << "START KIPMsgClickableURLConfigDialog::slotBrowseClicked" << endl;
	QString programName = KFileDialog::getOpenFileName();
	if ( programName != "" ) {
		protocols[m_ProtocolsListbox->currentItem()->text()] = programName;
		m_ProgramNameEditbox->setText( protocols[m_ProtocolsListbox->currentItem()->text()] );
	}
	kDebug() << "END   KIPMsgClickableURLConfigDialog::slotBrowseClicked" << endl;
}

/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgClickableURLConfigDialog::slotOkClicked()
{
	kDebug() << "START KIPMsgClickableURLConfigDialog::slotOkClicked" << endl;
	slotApplyClicked();
//	close();
	accept();
	kDebug() << "END   KIPMsgClickableURLConfigDialog::slotOkClicked" << endl;
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgClickableURLConfigDialog::slotCancelClicked()
{
	kDebug() << "START KIPMsgClickableURLConfigDialog::slotCancelClicked" << endl;
//	close();
	reject();
	kDebug() << "END   KIPMsgClickableURLConfigDialog::slotCancelClicked" << endl;
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgClickableURLConfigDialog::slotApplyClicked()
{
	kDebug() << "START KIPMsgClickableURLConfigDialog::slotApplyClicked" << endl;
	KIpMsgSettings::setUseDefaultBrowser( m_DefaultBrowserCheckbox->isChecked() );

	QStringList pro;
	for( QMap<QString,QString>::iterator p = protocols.begin(); p != protocols.end(); p++ ){
//		pro << p.key() + ":" + p.data();
		pro << p.key() + ":" + p.value();
	}

	KIpMsgSettings::setExecutePrograms( pro );

	KIpMsgSettings::self()->writeConfig();
	kDebug() << "END   KIPMsgClickableURLConfigDialog::slotApplyClicked" << endl;
}

/**
 * プロトコル選択イベント
 * ・プロトコルに設定されているプログラムを表示する。
 * @param item リストボックスアイテム
 */
void KIPMsgClickableURLConfigDialog::slotProtocolClicked(QListWidgetItem* item)
{
	kDebug() << "START KIPMsgClickableURLConfigDialog::slotProtocolClicked" << endl;
	m_ProgramNameEditbox->setText( protocols[item->text()] );
	kDebug() << "END   KIPMsgClickableURLConfigDialog::slotProtocolClicked" << endl;
}

#include "clickableurlconfig.moc"

