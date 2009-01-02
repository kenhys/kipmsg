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


#include <klineedit.h>
#include <klistbox.h>
#include <kcombobox.h>
#include <qcheckbox.h>
#include <qtextcodec.h>
#include <qstringlist.h>

#include "kipmsgconfig.h"
#include "networkconfig.h"
#include "clickableurlconfig.h"
#include "detailconfig.h"
#include "logconfig.h"
#include "unlockpasswordconfig.h"
#include "hideconfigdialog.h"
#include "kipmsgsettings.h"
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

/**
 * コンストラクタ
 * ・設定をロードし画面に表示する。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIPMsgConfigDialog::KIPMsgConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgConfigDialogBase(parent,name,fl)
{
	m_UserNameEditbox->setText(KIpMsgSettings::userName() );

	IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
	vector<GroupItem> groups = agent->GetGroupList();
	for( vector<GroupItem>::iterator ixgr = groups.begin(); ixgr != groups.end();ixgr++ ){
		QTextCodec *codec = QTextCodec::codecForName( ixgr->EncodingName().c_str() );
		if ( codec == NULL ){
			codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
		}
		m_GroupNameCombobox->insertItem( codec->toUnicode( ixgr->GroupName().c_str() ) );
	}
	m_GroupNameCombobox->setCurrentText( KIpMsgSettings::groupName() );
	m_ConfirmOpenCheckbox->setChecked(KIpMsgSettings::confirmOpen() );
	m_NoPopupCheckbox->setChecked( KIpMsgSettings::noPopup() );
	m_NotifyNoPopupRecieveCheckbox->setChecked( KIpMsgSettings::notifyOnNoPopupMessageRecieve() );
	m_NoSoundCheckbox->setChecked( KIpMsgSettings::noSound() );
	m_SecretDefaultCheckbox->setChecked( KIpMsgSettings::secretDefault() );
	m_QuoteDefaultCheckbox->setChecked( KIpMsgSettings::quoteDefault() );
	m_NotifyLoginLogoutAbsenceCheckbox->setChecked( KIpMsgSettings::notifyOnLoginLogoutAbsence() );

	m_MessageEncodingCombobox->setCurrentText( KIpMsgSettings::messageEncoding() );
	m_LocalFilesystemEncodingCombobox->setCurrentText( KIpMsgSettings::localFilesystemEncoding() );
	if ( m_NoPopupCheckbox->isChecked() ){
		m_NotifyNoPopupRecieveCheckbox->setEnabled( TRUE );
	} else {
		m_NotifyNoPopupRecieveCheckbox->setEnabled( FALSE );
	}
	m_HideEncryptionNotSupportedHostCheckbox->setChecked( KIpMsgSettings::hideEncryptionNotSupportedHost() );
	m_NoSendOnEncryptionFaildCheckbox->setChecked( KIpMsgSettings::noSendOnEncryptionFaild() );
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIPMsgConfigDialog::~KIPMsgConfigDialog()
{}

/*$SPECIALIZATION$*/
/**
 * ノンポップアップ受信クリックイベント
 * ・通知するチェックボックスの有効／無効を設定する。
 */
void KIPMsgConfigDialog::slotNoPopupClicked()
{
	if ( m_NoPopupCheckbox->isChecked() ){
		m_NotifyNoPopupRecieveCheckbox->setEnabled( TRUE );
	} else {
		m_NotifyNoPopupRecieveCheckbox->setEnabled( FALSE );
	}
}

/**
 * ネットワーク設定クリックイベント
 * ・ネットワーク設定ダイアログを開く
 */
void KIPMsgConfigDialog::slotNetworkSetupClicked()
{
	NetworkConfig *netConfig = new NetworkConfig(this,0,TRUE);
	netConfig->exec();
}

/**
 * 詳細設定クリックイベント
 * ・詳細設定ダイアログを開く
 */
void KIPMsgConfigDialog::slotDetailSetupClicked()
{
	KIPMsgDetailConfigDialog *detailConfig = new KIPMsgDetailConfigDialog(this,0,TRUE);
	detailConfig->exec();
}

/**
 * ログ設定クリックイベント
 * ・ログ設定ダイアログを開く
 */
void KIPMsgConfigDialog::slotLogSetupClicked()
{
	KIPMsgLogConfigDialog *logConfig = new KIPMsgLogConfigDialog(this,0,TRUE);
	logConfig->exec();
}

/**
 * クリッカブルURLクリックイベント
 * ・クリッカブルURL設定ダイアログを開く
 */
void KIPMsgConfigDialog::slotClickableUrlClicked()
{
	KIPMsgClickableURLConfigDialog *clickableUrl = new KIPMsgClickableURLConfigDialog(this,0,TRUE);
	clickableUrl->exec();
}
/**
 * 鍵開封パスワードクリックイベント
 * ・鍵開封パスワード設定ダイアログを開く
 */
void KIPMsgConfigDialog::slotUnlockPasswordSetup()
{
	KIPMsgUnlockPasswordConfigDialog *passwordConfig = new KIPMsgUnlockPasswordConfigDialog(this,0,TRUE);
	passwordConfig->exec();
}

/**
 * 隠れる設定クリックイベント
 * ・隠れる設定ダイアログを開く
 */
void KIPMsgConfigDialog::slotHideSetupClicked()
{
	HideConfigDialog *hideConfig = new HideConfigDialog(this,0,TRUE);
	hideConfig->exec();
}

/**
 * OKクリックイベント
 * ・設定を保存
 * ・ダイアログを閉じる
 */
void KIPMsgConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}
/**
 * キャンセルクリックイベント
 * ・ダイアログを閉じる
 */
void KIPMsgConfigDialog::slotCancelClicked()
{
	close();
}
/**
 * 適用クリックイベント
 * ・設定を保存
 */
void KIPMsgConfigDialog::slotApplyClicked()
{
	KIpMsgSettings::setUserName( m_UserNameEditbox->text() );
	KIpMsgSettings::setGroupName( m_GroupNameCombobox->currentText() );
	KIpMsgSettings::setConfirmOpen( m_ConfirmOpenCheckbox->isChecked() );
	KIpMsgSettings::setNoPopup( m_NoPopupCheckbox->isChecked() );
	if ( m_NoPopupCheckbox->isChecked() ){
		m_NotifyNoPopupRecieveCheckbox->setEnabled( TRUE );
	} else {
		m_NotifyNoPopupRecieveCheckbox->setEnabled( FALSE );
	}
	KIpMsgSettings::setNotifyOnNoPopupMessageRecieve( m_NotifyNoPopupRecieveCheckbox->isChecked() );
	KIpMsgSettings::setNoSound( m_NoSoundCheckbox->isChecked() );
	KIpMsgSettings::setSecretDefault( m_SecretDefaultCheckbox->isChecked() );
	KIpMsgSettings::setQuoteDefault( m_QuoteDefaultCheckbox->isChecked() );
	KIpMsgSettings::setNotifyOnLoginLogoutAbsence( m_NotifyLoginLogoutAbsenceCheckbox->isChecked() );

	KIpMsgSettings::setMessageEncoding( m_MessageEncodingCombobox->currentText() );
	KIpMsgSettings::setLocalFilesystemEncoding( m_LocalFilesystemEncodingCombobox->currentText() );
	KIpMsgSettings::setHideEncryptionNotSupportedHost( m_HideEncryptionNotSupportedHostCheckbox->isChecked() );
	KIpMsgSettings::setNoSendOnEncryptionFaild( m_NoSendOnEncryptionFaildCheckbox->isChecked() );
	KIpMsgSettings::writeConfig();

	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::messageEncoding() );
	IpMessengerAgent *IpMsgAgent = IpMessengerAgent::GetInstance();
	IpMsgAgent->setNoSendMessageOnEncryptionFailed( KIpMsgSettings::noSendOnEncryptionFaild() );
	IpMsgAgent->Login( codec->fromUnicode( KIpMsgSettings::userName() ).data(), codec->fromUnicode( KIpMsgSettings::groupName() ).data() );
}

#include "kipmsgconfig.moc"

