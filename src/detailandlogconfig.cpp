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
#include <kaudioplayer.h>
#include <klocale.h>
#include <kfiledialog.h>

#include "detailandlogconfig.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgDetailConfigDialog::KIPMsgDetailConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgDetailConfigDialogBase(parent,name,fl)
{
	//詳細
	m_IMEAutomaticalyCheckbox->setChecked( KIpMsgSettings::iMEAutomaticaly() );
	m_ConfirmIconCheckbox->setChecked( KIpMsgSettings::confirmIcon() );
	m_NoHideCheckbox->setChecked( KIpMsgSettings::noHide() );
	m_HotKeyCheckbox->setChecked( KIpMsgSettings::hotKey() );
	m_FireInterceptCheckbox->setChecked( KIpMsgSettings::fireIntercept() );
	m_OpenBySingleClickCheckbox->setChecked( KIpMsgSettings::openBySingleClick() );
	m_NonePopupOnAbsenceCheckbox->setChecked( KIpMsgSettings::nonePopupOnAbsence() );
	m_QuoteCharEditbox->setText( KIpMsgSettings::quoteChar() );
	m_RecieveSoundFileNameEditbox->setText( KIpMsgSettings::recieveSoundFileName() );
	m_IconFileNameEditbox->setText( KIpMsgSettings::iconFileName() );
	m_AbsenceIconFileNameEditbox->setText( KIpMsgSettings::absenceIconFileName() );
	//ログ
	m_LogCheckbox->setChecked( KIpMsgSettings::log() );
	m_RecordAfterUnlockCheckbox->setChecked( KIpMsgSettings::recordAfterUnlock() );
	m_RecordLoginCheckbox->setChecked( KIpMsgSettings::recordLogin() );
	m_RecordIPAddressCheckbox->setChecked( KIpMsgSettings::recordIPAddress() );
	m_LogFileNameEditbox->setText( KIpMsgSettings::logFileName() );
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIPMsgDetailConfigDialog::~KIPMsgDetailConfigDialog()
{}

/*$SPECIALIZATION$*/

/**
 * 受信音選択クリックイベント
 * ・サウンドファイルをコモンダイアログ(Load)を表示し選択させる。
 */
void KIPMsgDetailConfigDialog::slotRecievedSoundClicked()
{
	QString soundFileName = KFileDialog::getOpenFileName();
	if ( soundFileName != "" ) {
		m_RecieveSoundFileNameEditbox->setText( soundFileName );
	}
}

/**
 * 受信音再生クリックイベント
 * ・受信音を再生する。
 */
void KIPMsgDetailConfigDialog::slotPlaySoundClicked()
{
	QString soundFile = m_RecieveSoundFileNameEditbox->text();
	if ( soundFile == "" ) {
		KNotifyClient::beep();
	} else {
		KAudioPlayer::play( soundFile );
	}
}

/**
 * アイコン選択クリックイベント
 * ・画像ファイルをコモンダイアログ(Load)を表示し選択させ、アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotIconClicked()
{
	QString iconFileName = KFileDialog::getOpenFileName();
	if ( iconFileName != "" ) {
		m_IconFileNameEditbox->setText( iconFileName );
	}
	loadNormalIcon( iconFileName );
}

/**
 * アイコン変更イベント
 * ・アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotIconNameChanged(const QString & /*text*/){
	loadNormalIcon( m_IconFileNameEditbox->text() );
}

/**
 * アイコン表示
 * ・アイコンをロードしラベルに設定する。
 */
void KIPMsgDetailConfigDialog::loadNormalIcon(QString iconFileName){
	if ( iconFileName != "" ) {
		QPixmap icon;
		icon.load( iconFileName );
		m_PreviewIconLabel->setPixmap( icon );
	} else {
		if ( iconFileName == "" ) {
			m_PreviewIconLabel->setText( tr2i18n("default\nicon") );
		}
	}
}

/**
 * 不在アイコン選択クリックイベント
 * ・画像ファイルをコモンダイアログ(Load)を表示し選択させ、アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotAbsenceIconClicked()
{
	QString iconFileName = KFileDialog::getOpenFileName();
	if ( iconFileName != "" ) {
		m_AbsenceIconFileNameEditbox->setText( iconFileName );
	}
	loadAbsenceIcon(iconFileName);
}

/**
 * 不在アイコン変更イベント
 * ・不在アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotAbsenceIconNameChanged(const QString & /*text*/){
	loadAbsenceIcon( m_AbsenceIconFileNameEditbox->text() );
}

/**
 * 不在アイコン表示
 * ・不在アイコンをロードしラベルに設定する。
 */
void KIPMsgDetailConfigDialog::loadAbsenceIcon(QString iconFileName){
	if ( iconFileName != "" ) {
		QPixmap icon;
		icon.load( iconFileName );
		m_PreviewAbsenceIconLabel->setPixmap( icon );
	} else {
		if ( iconFileName == "" ) {
			m_PreviewAbsenceIconLabel->setText( tr2i18n("default\nicon") );
		}
	}
}

/**
 * ログファイル選択クリックイベント
 * ・ログファイルをコモンダイアログ(SaveAs)を表示し選択させる。
 */
void KIPMsgDetailConfigDialog::slotLogFileClicked()
{
	QString logFileName = KFileDialog::getSaveFileName();
	if ( logFileName != "" ) {
		m_LogFileNameEditbox->setText( logFileName );
	}
}

/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgDetailConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgDetailConfigDialog::slotCancelClicked()
{
	close();
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgDetailConfigDialog::slotApplyClicked()
{
	//詳細
	KIpMsgSettings::setIMEAutomaticaly( m_IMEAutomaticalyCheckbox->isChecked() );
	KIpMsgSettings::setConfirmIcon( m_ConfirmIconCheckbox->isChecked() );
	KIpMsgSettings::setNoHide( m_NoHideCheckbox->isChecked() );
	KIpMsgSettings::setHotKey( m_HotKeyCheckbox->isChecked() );
	KIpMsgSettings::setFireIntercept( m_FireInterceptCheckbox->isChecked() );
	KIpMsgSettings::setOpenBySingleClick( m_OpenBySingleClickCheckbox->isChecked() );
	KIpMsgSettings::setNonePopupOnAbsence( m_NonePopupOnAbsenceCheckbox->isChecked() );
	KIpMsgSettings::setQuoteChar( m_QuoteCharEditbox->text() );
	KIpMsgSettings::setRecieveSoundFileName( m_RecieveSoundFileNameEditbox->text() );
	KIpMsgSettings::setIconFileName( m_IconFileNameEditbox->text() );
	KIpMsgSettings::setAbsenceIconFileName( m_AbsenceIconFileNameEditbox->text() );
	//ログ
	KIpMsgSettings::setLog( m_LogCheckbox->isChecked() );
	KIpMsgSettings::setRecordAfterUnlock( m_RecordAfterUnlockCheckbox->isChecked() );
	KIpMsgSettings::setRecordLogin( m_RecordLoginCheckbox->isChecked() );
	KIpMsgSettings::setRecordIPAddress( m_RecordIPAddressCheckbox->isChecked() );
	KIpMsgSettings::setLogFileName( m_LogFileNameEditbox->text() );

	KIpMsgSettings::writeConfig();
}

#include "detailandlogconfig.moc"

