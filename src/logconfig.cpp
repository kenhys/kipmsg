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

#include <qpixmap.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <klineedit.h>
#include <knotifyclient.h>
#include <kpushbutton.h>
#include <kprocess.h>
#include <klocale.h>
#include <kfiledialog.h>

#include "logconfig.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgLogConfigDialog::KIPMsgLogConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgLogConfigDialogBase(parent,name,fl)
{
	//ログ
	m_LogCheckbox->setChecked( KIpMsgSettings::log() );
	m_RecordAfterUnlockCheckbox->setChecked( KIpMsgSettings::recordAfterUnlock() );
	m_RecordLoginCheckbox->setChecked( KIpMsgSettings::recordLogin() );
	m_RecordIPAddressCheckbox->setChecked( KIpMsgSettings::recordIPAddress() );
	m_LogFileNameEditbox->setText( KIpMsgSettings::logFileName() );
	m_UseExternalLogViewerCheckbox->setChecked( KIpMsgSettings::useExternalLogViewer() );
	m_LogViewerPathEditbox->setText( KIpMsgSettings::logViewerPath() );
	m_LogViewerOptionEditbox->setText( KIpMsgSettings::logViewerOption() );
	slotUseExternalLogViewerClicked();
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIPMsgLogConfigDialog::~KIPMsgLogConfigDialog()
{}

/*$SPECIALIZATION$*/

/**
 * ログファイル選択クリックイベント
 * ・ログファイルをコモンダイアログ(SaveAs)を表示し選択させる。
 */
void KIPMsgLogConfigDialog::slotLogFileClicked()
{
	QString logFileName = KFileDialog::getSaveFileName();
	if ( logFileName != "" ) {
		m_LogFileNameEditbox->setText( logFileName );
	}
}

void KIPMsgLogConfigDialog::slotUseExternalLogViewerClicked()
{
	if ( m_UseExternalLogViewerCheckbox->isChecked() ) {
		m_LogViewerButton->setEnabled( TRUE );
		m_LogViewerPathEditbox->setEnabled( TRUE );
	} else {
		m_LogViewerButton->setEnabled( FALSE );
		m_LogViewerPathEditbox->setEnabled( FALSE );
	}
}

void KIPMsgLogConfigDialog::slotLogViewerClicked()
{
	QString logViewerPath = KFileDialog::getOpenFileName();
	if ( logViewerPath != "" ) {
		m_LogViewerPathEditbox->setText( logViewerPath );
	}
}

/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgLogConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgLogConfigDialog::slotCancelClicked()
{
	close();
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgLogConfigDialog::slotApplyClicked()
{
	//ログ
	KIpMsgSettings::setLog( m_LogCheckbox->isChecked() );
	KIpMsgSettings::setRecordAfterUnlock( m_RecordAfterUnlockCheckbox->isChecked() );
	KIpMsgSettings::setRecordLogin( m_RecordLoginCheckbox->isChecked() );
	KIpMsgSettings::setRecordIPAddress( m_RecordIPAddressCheckbox->isChecked() );
	KIpMsgSettings::setLogFileName( m_LogFileNameEditbox->text() );

	KIpMsgSettings::setUseExternalLogViewer( m_UseExternalLogViewerCheckbox->isChecked() );
	KIpMsgSettings::setLogViewerPath( m_LogViewerPathEditbox->text() );
	KIpMsgSettings::setLogViewerOption( m_LogViewerOptionEditbox->text() );

	KIpMsgSettings::writeConfig();
}

#include "logconfig.moc"

