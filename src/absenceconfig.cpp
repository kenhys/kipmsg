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


#include <klistbox.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <knuminput.h>
#include <ktextedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include "kipmsgsettings.h"
#include "absenceconfig.h"

/**
 * コンストラクタ
 * ・設定をロード
 * ・有効／無効の状態を画面に反映
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMsgAbsenceModeConfigDialog::KIpMsgAbsenceModeConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIpMsgAbsenceModeConfigDialogBase(parent,name,fl)
{
	load();
	setStatus();
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMsgAbsenceModeConfigDialog::~KIpMsgAbsenceModeConfigDialog()
{}

/*$SPECIALIZATION$*/
/**
 * 設定＋不在設定クリックイベント
 * ・設定を保存
 * ・不在モードに移行
 * ・画面を閉じる
 */
void KIpMsgAbsenceModeConfigDialog::slotConfigAndAbsenceModeClicked()
{
	save();
	//Goto AbsenceMode
	close();
}
/**
 * 設定クリックイベント
 * ・設定を保存
 * ・画面を閉じる
 */
void KIpMsgAbsenceModeConfigDialog::slotConfigOnlyClicked()
{
	save();
	close();
}
/**
 * 設定をロード
 */
void KIpMsgAbsenceModeConfigDialog::load()
{
	QStringList keys = KIpMsgSettings::absenceKeys();
	QStringList encodings = KIpMsgSettings::absenceEncodings();
	QStringList titles = KIpMsgSettings::absenceTitles();
	QStringList details = KIpMsgSettings::absenceDetails();

	for( unsigned int i = 0; i < keys.count(); i++ ){
		QMap<QString, AbsenceSetting>encode;
		AbsenceSetting config;
		config.title = titles[i];
		config.detail = details[i];
		settings[keys[i]][encodings[i]] = config;
	}

	QMap<QString, QMap<QString, AbsenceSetting> >::Iterator it;
	for( it = settings.begin(); it != settings.end(); ++it ){
		AbsenceSetting config = it.data()[KIpMsgSettings::messageEncoding()];
		m_AbsenceModeListbox->insertItem( it.key() + ":" + config.title );
	}
	it = settings.begin();
	currentKey = it.key();
	currentEncoding = KIpMsgSettings::messageEncoding();
	m_AutoAbsenceModeCheckbox->setChecked( KIpMsgSettings::autoAbsenceEnabled() );
	m_AutoAbsenceMinutesInputbox->setValue( KIpMsgSettings::autoAbsenceMinutes() );
	slotAutoAbsenceClicked();
}
/**
 * 設定を保存
 */
void KIpMsgAbsenceModeConfigDialog::save()
{
	QStringList keys;
	QStringList encodings;
	QStringList titles;
	QStringList details;
	QString lastKey;
	int i = 0;
	QMap<QString, QMap<QString, AbsenceSetting> >::Iterator it;
	for( it = settings.begin(); it != settings.end(); ++it ){
		QMap<QString, AbsenceSetting>::Iterator enc;
		i++;
		for( enc = it.data().begin(); enc != it.data().end(); ++enc ){
			KIpMsgSettings::setAbsenceLastKey( i );
			keys << QString( "%1").arg(i);
			encodings << enc.key();
			titles << enc.data().title;
			details << enc.data().detail;
		}
	}
	KIpMsgSettings::setAbsenceKeys( keys );
	KIpMsgSettings::setAbsenceEncodings( encodings );
	KIpMsgSettings::setAbsenceTitles( titles );
	KIpMsgSettings::setAbsenceDetails( details );
	KIpMsgSettings::setAutoAbsenceEnabled( m_AutoAbsenceModeCheckbox->isChecked() );
	KIpMsgSettings::setAutoAbsenceMinutes( m_AutoAbsenceMinutesInputbox->value() );
	KIpMsgSettings::writeConfig();
}

/**
 * キャンセルクリックイベント
 * ・画面を閉じる
 */
void KIpMsgAbsenceModeConfigDialog::slotCancelClicked()
{
	close();
}

/**
 * 自動不在モードクリックイベント
 * ・チェックオンなら設定項目を有効にする。
 */
void KIpMsgAbsenceModeConfigDialog::slotAutoAbsenceClicked()
{
	m_AutoAbsenceMinutesLabel->setEnabled( m_AutoAbsenceModeCheckbox->isChecked() );
	m_MinutesLabel->setEnabled( m_AutoAbsenceModeCheckbox->isChecked() );
	m_AutoAbsenceMinutesInputbox->setEnabled( m_AutoAbsenceModeCheckbox->isChecked() );
}

/**
 * セットクリックイベント
 * ・指定された不在モード、エンコーディングに設定する。
 */
void KIpMsgAbsenceModeConfigDialog::slotSetClicked()
{
	AbsenceSetting conf = settings[currentKey][m_EncodingCombobox->currentText()];
	conf.title = m_TitleEditbox->text();
	conf.detail = m_DescriptionTextbox->text();
	settings[currentKey][m_EncodingCombobox->currentText()] = conf;
}

/**
 * 不在モードクリックイベント
 * ・指定された不在モードを設定項目に表示する。
 * @param item リストボックスで選択された項目
 */
void KIpMsgAbsenceModeConfigDialog::slotAbsenceModeClicked(QListBoxItem *item)
{
	if ( item == NULL ){
		return;
	}
	QStringList strs = QStringList::split( ":", item->text() );
	currentKey = strs[0];
	AbsenceSetting conf = settings[currentKey][currentEncoding];
	m_TitleEditbox->setText( conf.title );
	m_DescriptionTextbox->setText( conf.detail );
}

/**
 * エンコーディング変更イベント
 * ・表示中のエンコーディングを切替える。
 * @param index 選択されたエンコーディングのインデックス
 */
void KIpMsgAbsenceModeConfigDialog::slotEncodingChanged(int index)
{
	currentEncoding = m_EncodingCombobox->text(index);
	AbsenceSetting conf = settings[currentKey][currentEncoding];
	m_TitleEditbox->setText( conf.title );
	m_DescriptionTextbox->setText( conf.detail );
}

/**
 * 追加ボタンクリックイベント
 * ・表示中の不在モードを追加する。
 */
void KIpMsgAbsenceModeConfigDialog::slotAddClicked()
{
	int i = KIpMsgSettings::absenceLastKey();
	KIpMsgSettings::setAbsenceLastKey(++i);
	QString NoName = QString( "%1:No Name").arg(i);
	QString AbsenceKey = QString( "%1").arg(i);
	m_AbsenceModeListbox->insertItem( NoName );
	m_AbsenceModeListbox->setCurrentItem( i );
	QMap<QString, AbsenceSetting>AbsenceMap;
	AbsenceSetting conf;
	conf.title = NoName;
	conf.detail = "";
	AbsenceMap[KIpMsgSettings::messageEncoding()] = conf;
	settings[AbsenceKey] = AbsenceMap;
	currentKey = AbsenceKey;
	currentEncoding = KIpMsgSettings::messageEncoding();
	m_EncodingCombobox->setCurrentText( currentEncoding );
	m_TitleEditbox->setText( conf.title );
	m_DescriptionTextbox->setText( conf.detail );
	setStatus();
}

/**
 * 削除ボタンクリックイベント
 * ・表示中の不在モードを削除する。
 */
void KIpMsgAbsenceModeConfigDialog::slotDeleteClicked()
{
	if ( m_AbsenceModeListbox->currentItem() < 0 ) {
		return;
	}
	QStringList strs = QStringList::split( ":", m_AbsenceModeListbox->currentText() );
	settings.remove(strs[0]);
	m_AbsenceModeListbox->removeItem( m_AbsenceModeListbox->currentItem() );
	m_TitleEditbox->setText( "" );
	m_DescriptionTextbox->setText( "" );
	setStatus();
}

/**
 * 入力有効／無効のステータスを設定する。
 */
void KIpMsgAbsenceModeConfigDialog::setStatus()
{
	if ( m_AbsenceModeListbox->count() > 0 ) {
		m_EncodingCombobox->setEnabled( TRUE );
		m_TitleEditbox->setEnabled( TRUE );
		m_DescriptionTextbox->setEnabled( TRUE );
	} else {
		m_EncodingCombobox->setEnabled( FALSE );
		m_TitleEditbox->setEnabled( FALSE );
		m_DescriptionTextbox->setEnabled( FALSE );
	}
}

#include "absenceconfig.moc"
