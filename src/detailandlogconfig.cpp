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

KIPMsgDetailConfigDialog::~KIPMsgDetailConfigDialog()
{}

/*$SPECIALIZATION$*/
void KIPMsgDetailConfigDialog::slotRecievedSoundClicked()
{
	QString soundFileName = KFileDialog::getOpenFileName();
	if ( soundFileName != "" ) {
		m_RecieveSoundFileNameEditbox->setText( soundFileName );
	}
}

void KIPMsgDetailConfigDialog::slotPlaySoundClicked()
{
	QString soundFile = m_RecieveSoundFileNameEditbox->text();
	if ( soundFile == "" ) {
		KNotifyClient::beep();
	} else {
		KAudioPlayer::play( soundFile );
	}
}

void KIPMsgDetailConfigDialog::slotIconClicked()
{
	QString iconFileName = KFileDialog::getOpenFileName();
	if ( iconFileName != "" ) {
		m_IconFileNameEditbox->setText( iconFileName );
	}
	loadNormalIcon( iconFileName );
}

void KIPMsgDetailConfigDialog::slotIconNameChanged(const QString &text){
	loadNormalIcon( m_IconFileNameEditbox->text() );
}

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

void KIPMsgDetailConfigDialog::slotAbsenceIconClicked()
{
	QString iconFileName = KFileDialog::getOpenFileName();
	if ( iconFileName != "" ) {
		m_AbsenceIconFileNameEditbox->setText( iconFileName );
	}
	loadAbsenceIcon(iconFileName);
}

void KIPMsgDetailConfigDialog::slotAbsenceIconNameChanged(const QString &text){
	loadAbsenceIcon( m_AbsenceIconFileNameEditbox->text() );
}

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

void KIPMsgDetailConfigDialog::slotLogFileClicked()
{
	QString logFileName = KFileDialog::getSaveFileName();
	if ( logFileName != "" ) {
		m_LogFileNameEditbox->setText( logFileName );
	}
}
void KIPMsgDetailConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}
void KIPMsgDetailConfigDialog::slotCancelClicked()
{
	close();
}
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

