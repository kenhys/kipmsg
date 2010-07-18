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

#include <qpixmap.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <knotification.h>
#include <kpushbutton.h>
#include <kprocess.h>
#include <klocale.h>
#include <kfiledialog.h>

#include "detailconfig.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・設定をロードし画面に表示
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgDetailConfigDialog::KIPMsgDetailConfigDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
//        : KIPMsgDetailConfigDialogBase(parent,name,fl)
        : KIPMsgDetailConfigDialogBase()
{
	kDebug() << "START KIPMsgDetailConfigDialog::KIPMsgDetailConfigDialog" << endl;
	setupUi(this);
	setButtons( None );
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
	kDebug() << "END   KIPMsgDetailConfigDialog::KIPMsgDetailConfigDialog" << endl;
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
	kDebug() << "START KIPMsgDetailConfigDialog::slotRecievedSoundClicked" << endl;
	QString soundFileName = KFileDialog::getOpenFileName();
	if ( soundFileName != "" ) {
		m_RecieveSoundFileNameEditbox->setText( soundFileName );
	}
	kDebug() << "END   KIPMsgDetailConfigDialog::slotRecievedSoundClicked" << endl;
}

/**
 * 受信音再生クリックイベント
 * ・受信音を再生する。
 */
void KIPMsgDetailConfigDialog::slotPlaySoundClicked()
{
	kDebug() << "START KIPMsgDetailConfigDialog::slotPlaySoundClicked" << endl;
	QString soundFile = m_RecieveSoundFileNameEditbox->text();
	if ( soundFile == "" ) {
		KNotification::beep();
	} else {
//FIXME using phonon
/*
		KProcess test;
		test << "artsplay";
		test << soundFile;
		test.start(KProcess::DontCare);
*/
	}
	kDebug() << "END   KIPMsgDetailConfigDialog::slotPlaySoundClicked" << endl;
}

/**
 * アイコン選択クリックイベント
 * ・画像ファイルをコモンダイアログ(Load)を表示し選択させ、アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotIconClicked()
{
	kDebug() << "START KIPMsgDetailConfigDialog::slotIconClicked" << endl;
	QString iconFileName = KFileDialog::getOpenFileName();
	if ( iconFileName != "" ) {
		m_IconFileNameEditbox->setText( iconFileName );
	}
	loadNormalIcon( iconFileName );
	kDebug() << "END   KIPMsgDetailConfigDialog::slotIconClicked" << endl;
}

/**
 * アイコン変更イベント
 * ・アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotIconNameChanged(const QString & /*text*/){
	kDebug() << "START KIPMsgDetailConfigDialog::slotIconNameChanged" << endl;
	loadNormalIcon( m_IconFileNameEditbox->text() );
	kDebug() << "END   KIPMsgDetailConfigDialog::slotIconNameChanged" << endl;
}

/**
 * アイコン表示
 * ・アイコンをロードしラベルに設定する。
 */
void KIPMsgDetailConfigDialog::loadNormalIcon(QString iconFileName){
	kDebug() << "START KIPMsgDetailConfigDialog::loadNormalIcon" << endl;
	if ( iconFileName != "" ) {
		QPixmap icon;
		icon.load( iconFileName );
		m_PreviewIconLabel->setPixmap( icon );
	} else {
		if ( iconFileName == "" ) {
			m_PreviewIconLabel->setText( tr2i18n("default\nicon") );
		}
	}
	kDebug() << "END   KIPMsgDetailConfigDialog::loadNormalIcon" << endl;
}

/**
 * 不在アイコン選択クリックイベント
 * ・画像ファイルをコモンダイアログ(Load)を表示し選択させ、アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotAbsenceIconClicked()
{
	kDebug() << "START KIPMsgDetailConfigDialog::slotAbsenceIconClicked" << endl;
	QString iconFileName = KFileDialog::getOpenFileName();
	if ( iconFileName != "" ) {
		m_AbsenceIconFileNameEditbox->setText( iconFileName );
	}
	loadAbsenceIcon(iconFileName);
	kDebug() << "END   KIPMsgDetailConfigDialog::slotAbsenceIconClicked" << endl;
}

/**
 * 不在アイコン変更イベント
 * ・不在アイコンを表示する。
 */
void KIPMsgDetailConfigDialog::slotAbsenceIconNameChanged(const QString & /*text*/){
	kDebug() << "START KIPMsgDetailConfigDialog::slotAbsenceIconClicked" << endl;
	loadAbsenceIcon( m_AbsenceIconFileNameEditbox->text() );
	kDebug() << "END   KIPMsgDetailConfigDialog::slotAbsenceIconClicked" << endl;
}

/**
 * 不在アイコン表示
 * ・不在アイコンをロードしラベルに設定する。
 */
void KIPMsgDetailConfigDialog::loadAbsenceIcon(QString iconFileName){
	kDebug() << "START KIPMsgDetailConfigDialog::loadAbsenceIcon" << endl;
	if ( iconFileName != "" ) {
		QPixmap icon;
		icon.load( iconFileName );
		m_PreviewAbsenceIconLabel->setPixmap( icon );
	} else {
		if ( iconFileName == "" ) {
			m_PreviewAbsenceIconLabel->setText( tr2i18n("default\nicon") );
		}
	}
	kDebug() << "END   KIPMsgDetailConfigDialog::loadAbsenceIcon" << endl;
}

/**
 * OKクリックイベント
 * ・設定を保存してダイアログを閉じる。
 */
void KIPMsgDetailConfigDialog::slotOkClicked()
{
	kDebug() << "START KIPMsgDetailConfigDialog::slotOkClicked" << endl;
	slotApplyClicked();
//	close();
	accept();
	kDebug() << "END   KIPMsgDetailConfigDialog::slotOkClicked" << endl;
}

/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる。
 */
void KIPMsgDetailConfigDialog::slotCancelClicked()
{
	kDebug() << "START KIPMsgDetailConfigDialog::slotCancelClicked" << endl;
//	close();
	reject();
	kDebug() << "END   KIPMsgDetailConfigDialog::slotCancelClicked" << endl;
}

/**
 * 適用クリックイベント
 * ・設定を保存する。
 */
void KIPMsgDetailConfigDialog::slotApplyClicked()
{
	kDebug() << "START KIPMsgDetailConfigDialog::slotApplyClicked" << endl;
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

	KIpMsgSettings::self()->writeConfig();
	kDebug() << "END   KIPMsgDetailConfigDialog::slotApplyClicked" << endl;
}

#include "detailconfig.moc"

