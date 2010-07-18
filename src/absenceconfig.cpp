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


#include <ctype.h>
#include <kdebug.h>
#include <klistwidget.h>
//#include <klistbox.h>
#include <kcombobox.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <knuminput.h>
#include <ktextedit.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include "kipmsgsettings.h"
#include "absenceconfig.h"
#include "newabsencename.h"

/**
 * コンストラクタ
 * ・設定をロード
 * ・有効／無効の状態を画面に反映
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMsgAbsenceModeConfigDialog::KIpMsgAbsenceModeConfigDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
        : KIpMsgAbsenceModeConfigDialogBase()
//        : KIpMsgAbsenceModeConfigDialogBase(parent,name,fl)
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::KIpMsgAbsenceModeConfigDialog" << endl;
	setupUi(this);
	setButtons( None );
	load();
	setStatus();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::KIpMsgAbsenceModeConfigDialog" << endl;
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
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotConfigAndAbsenceModeClicked" << endl;
	save();
	// TODO Goto AbsenceMode
//	close();
	done(QDialog::Accepted);
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotConfigAndAbsenceModeClicked" << endl;
}
/**
 * 設定クリックイベント
 * ・設定を保存
 * ・画面を閉じる
 */
void KIpMsgAbsenceModeConfigDialog::slotConfigOnlyClicked()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotConfigOnlyClicked" << endl;
	save();
//	close();
//	done(QDialog::Accepted);
	accept();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotConfigOnlyClicked" << endl;
}
/**
 * 設定をロード
 * ・不在キー、不在エンコーディング、不在タイトル、不在詳細は
 *    それぞれQStringListとして保存されている。
 * ・キーをエンコーディング、タイトル、詳細をまとめたクラスを
 *    「設定[不在キー][エンコーディング]」のマップに格納
 * ・リストボックスに展開
 * ・自動不在に関する設定を画面に初期設定
 */
void KIpMsgAbsenceModeConfigDialog::load()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::load" << endl;
	//ID=ID1,ID2,ID3
	//ENCODING=ID1\aShift_JIS,ID2\aShift_JIS,ID3\aShift_JIS,ID1\autf8,ID2\autf8,ID3\autf8
	//TITLE=ID1\aShift_JIS\aTITLE1,ID2\aShift_JIS\aTITLE2,ID3\aShift_JIS\aTITLE2
	//DETAIL=ID1\aShift_JIS\aDETAIL1,ID2\aShift_JIS\aDETAIL2,ID3\aShift_JIS\aDETAIL
	QStringList keys = KIpMsgSettings::absenceKeys();
	// エンコーディングのリスト(ID毎にある)
	QStringList encodings = KIpMsgSettings::absenceEncodings();
	// ID,エンコーディングのリストの要素(タイトル)
	QStringList titles = KIpMsgSettings::absenceTitles();
	// ID,エンコーディングのリストの要素(詳細)
	QStringList details = KIpMsgSettings::absenceDetails();

	//QMap<Encoding,Key, AbsenceSetting>
	for( int i = 0; i < titles.count(); i++ ){
		QStringList titleFields = titles[i].split( "\a" );
		QStringList detailFields = details[i].split( "\a" );
//		QStringList titleFields = QStringList::split( "\a", titles[i] );
//		QStringList detailFields = QStringList::split( "\a", details[i] );
		AbsenceSetting config;
		config.title = titleFields[2];
		config.detail = detailFields[2];
		settings[titleFields[0]/* ID */][titleFields[1]/* ENCODING */] = config;
	}

	for( int i = 0; i < keys.count(); i++ ){
		AbsenceSetting config = settings[keys[i]][KIpMsgSettings::messageEncoding()];
		m_AbsenceModeListbox->insertItem( m_AbsenceModeListbox->count(), keys[i] + ":" + config.title );
	}

	currentKey = "";
	if ( keys.count() > 0 ){
		currentKey = keys[0];
//		m_AbsenceModeListbox->setSelected( 0, TRUE );
		m_AbsenceModeListbox->item( 0 )->setSelected( TRUE );
	}

	currentEncoding = KIpMsgSettings::messageEncoding();
	m_AutoAbsenceModeCheckbox->setChecked( KIpMsgSettings::autoAbsenceEnabled() );
	setSelectedItem();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::load" << endl;
}
/**
 * 設定を保存
 * ・キーをエンコーディング、タイトル、詳細をまとめたクラスを
 *   「設定[不在キー][エンコーディング]」のマップから取り出し、
 * ・不在キー、不在エンコーディング、不在タイトル、不在詳細を
 *   それぞれ別々のQStringListに保存する。
 * ・自動不在に関する設定を保存
 */
void KIpMsgAbsenceModeConfigDialog::save()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::save" << endl;
	QStringList keys;
	QStringList encodings;
	QStringList titles;
	QStringList details;

	for( int i = 0; i < m_AbsenceModeListbox->count(); i++ ){
		keys << getSelectedAbsenceModeKey( m_AbsenceModeListbox->item(i)->text() );
	}

	for( int i = 0; i < keys.count(); i++ ){
		QString saveKey = keys[i];
		for( int j = 0; j < m_EncodingCombobox->count(); j++ ){
//			QString saveEncoding = m_EncodingCombobox->item(j)->text();
			QString saveEncoding = m_EncodingCombobox->itemText(j);
			titles << saveKey + "\a" + saveEncoding + "\a" + settings[saveKey][saveEncoding].title;
			details << saveKey + "\a" + saveEncoding + "\a" + settings[saveKey][saveEncoding].detail;
		}
	}
	KIpMsgSettings::setAbsenceKeys( keys );
	KIpMsgSettings::setAbsenceEncodings( encodings );
	KIpMsgSettings::setAbsenceTitles( titles );
	KIpMsgSettings::setAbsenceDetails( details );
	KIpMsgSettings::setAutoAbsenceEnabled( m_AutoAbsenceModeCheckbox->isChecked() );
	KIpMsgSettings::self()->writeConfig();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::save" << endl;
}

/**
 * キャンセルクリックイベント
 * ・画面を閉じる
 */
void KIpMsgAbsenceModeConfigDialog::slotCancelClicked()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotCancelClicked" << endl;
//	close();
	done(QDialog::Rejected);
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotCancelClicked" << endl;
}

/**
 * セットクリックイベント
 * ・指定された不在モード、エンコーディングに設定する。
 */
void KIpMsgAbsenceModeConfigDialog::slotSetClicked()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotSetClicked" << endl;
	AbsenceSetting conf = settings[currentKey][m_EncodingCombobox->currentText()];
	conf.title = m_TitleEditbox->text();
//	conf.detail = m_DescriptionTextbox->text();
	conf.detail = m_DescriptionTextbox->toPlainText();
	settings[currentKey][m_EncodingCombobox->currentText()] = conf;

	for( int i = 0; i < m_AbsenceModeListbox->count(); i++ ){
		if ( getSelectedAbsenceModeKey( m_AbsenceModeListbox->item(i)->text() ) == currentKey ){
//			m_AbsenceModeListbox->removeItem(i);
			m_AbsenceModeListbox->removeItemWidget(m_AbsenceModeListbox->item(i));
//			m_AbsenceModeListbox->insertItem(m_AbsenceModeListbox->count(), currentKey + ":" + conf.title, i);
			m_AbsenceModeListbox->insertItem(i, currentKey + ":" + conf.title);
//			m_AbsenceModeListbox->setSelected( i, TRUE );
			m_AbsenceModeListbox->item(i)->setSelected( TRUE );
			break;
		}
	}
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotSetClicked" << endl;
}

/**
 * 不在モードクリックイベント
 * ・指定された不在モードを設定項目に表示する。
 * @param item リストボックスで選択された項目
 */
void KIpMsgAbsenceModeConfigDialog::slotAbsenceModeChanged(QListWidgetItem *item)
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotAbsenceModeChanged" << endl;
	if ( item == NULL ){
		return;
	}
	currentKey = getSelectedAbsenceModeKey( item->text() );
	setSelectedItem();
	setStatus();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotAbsenceModeChanged" << endl;
}

/**
 * エンコーディング変更イベント
 * ・表示中のエンコーディングを切替える。
 * @param index 選択されたエンコーディングのインデックス
 */
void KIpMsgAbsenceModeConfigDialog::slotEncodingChanged(int index)
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotEncodingChanged" << endl;
	currentEncoding = m_EncodingCombobox->itemText(index);

	m_AbsenceModeListbox->clear();
	QMap<QString, QMap<QString, AbsenceSetting> >::Iterator it;
	for( it = settings.begin(); it != settings.end(); ++it ){
//		AbsenceSetting config = it.data()[currentEncoding];
		AbsenceSetting config = it.value()[currentEncoding];
		m_AbsenceModeListbox->insertItem( m_AbsenceModeListbox->count(), it.key() + ":" + config.title );
	}

	setSelectedItem();
	setStatus();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotEncodingChanged" << endl;
}

/**
 * 追加ボタンクリックイベント
 * ・表示中の不在モードを追加する。
 */
void KIpMsgAbsenceModeConfigDialog::slotAddClicked()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotAddClicked" << endl;
	while( true ) {
//		NamingNewAbsenceMode *dlg = new NamingNewAbsenceMode(this,0,TRUE);
		NamingNewAbsenceMode *dlg = new NamingNewAbsenceMode(this);
		dlg->setAbsenceConfigDlg( this );
		if ( dlg->exec() != QDialog::Accepted ){
			delete dlg;
			return;
		}
		delete dlg;
		if ( canAcceptNewId() ){
			//重複ならやりなおし
			if ( settings.contains( newId ) ) {
				continue;
			}
			m_AbsenceModeListbox->insertItem( m_AbsenceModeListbox->count(), newId + ":" + newId );
			//全てのエンコーディングにnewIdを追加。
			for( int i = 0; i < m_EncodingCombobox->count(); i++ ){
				AbsenceSetting config;
				config.title = newId;
				config.detail = "";
				settings[newId][m_EncodingCombobox->itemText(i)] = config;
			}
			//追加されたアイテムを選択
//			m_AbsenceModeListbox->setSelected( m_AbsenceModeListbox->count() - 1, TRUE );
			m_AbsenceModeListbox->item( m_AbsenceModeListbox->count() - 1)->setSelected( TRUE );
			break;
		}
	}
	setStatus();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotAddClicked" << endl;
}

bool KIpMsgAbsenceModeConfigDialog::canAcceptNewId()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::canAcceptNewId" << endl;
	for( int i = 0; i < newId.length(); i++ ) {
//		char c = newId.at(i).latin1();
		char c = newId.at(i).toLatin1();
		if ( !isdigit( c ) && !isalpha( c ) && c != ' ' ){
			kDebug() << "END   KIpMsgAbsenceModeConfigDialog::canAcceptNewId" << endl;
			return false;
		}
	}
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::canAcceptNewId" << endl;
	return true;
}

/**
 * 削除ボタンクリックイベント
 * ・表示中の不在モードを削除する。
 */
void KIpMsgAbsenceModeConfigDialog::slotDeleteClicked()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotDeleteClicked" << endl;
//	if ( m_AbsenceModeListbox->currentItem() < 0 ) {
	if ( m_AbsenceModeListbox->currentRow() < 0 ) {
		return;
	}
//	settings.remove( getSelectedAbsenceModeKey( m_AbsenceModeListbox->currentText() ) );
	settings.remove( getSelectedAbsenceModeKey( m_AbsenceModeListbox->currentItem()->text() ) );
//	int deleteIndex = m_AbsenceModeListbox->currentItem();
//	m_AbsenceModeListbox->removeItem( deleteIndex );
	int deleteIndex = m_AbsenceModeListbox->currentRow();
	m_AbsenceModeListbox->removeItemWidget( m_AbsenceModeListbox->item( deleteIndex ) );
	if ( m_AbsenceModeListbox->count() > 0 && m_AbsenceModeListbox->count() > deleteIndex ) {
		QListWidgetItem *currentItem = m_AbsenceModeListbox->item(deleteIndex);
		if(currentItem != NULL) currentItem->setSelected( TRUE );
//		m_AbsenceModeListbox->setSelected( deleteIndex, TRUE );
	} else if ( m_AbsenceModeListbox->count() > 0 && m_AbsenceModeListbox->count() == deleteIndex ) {
		QListWidgetItem *currentItem = m_AbsenceModeListbox->item( deleteIndex - 1 );
		if(currentItem != NULL) currentItem->setSelected( TRUE );
//		m_AbsenceModeListbox->setSelected( deleteIndex - 1, TRUE );
	} else if ( m_AbsenceModeListbox->count() > 0 && m_AbsenceModeListbox->count() < deleteIndex ) {
		QListWidgetItem *currentItem = m_AbsenceModeListbox->item(m_AbsenceModeListbox->count() - 1);
		if(currentItem != NULL) currentItem->setSelected( TRUE );
//		m_AbsenceModeListbox->setSelected( m_AbsenceModeListbox->count() - 1, TRUE );
	}
	setSelectedItem();
	setStatus();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotDeleteClicked" << endl;
}


void KIpMsgAbsenceModeConfigDialog::slotUpClicked()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotUpClicked" << endl;
	int deleteIndex = m_AbsenceModeListbox->currentRow();
//	unsigned int deleteIndex = m_AbsenceModeListbox->currentItem();
	QListWidgetItem *currentItem = m_AbsenceModeListbox->item(deleteIndex);
	if(currentItem == NULL){
		setStatus();
		kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotUpClicked" << endl;
		return;
	}
//	QString currentText = m_AbsenceModeListbox->currentText();
	QString currentText = currentItem->text();
//	m_AbsenceModeListbox->removeItem( deleteIndex );
	m_AbsenceModeListbox->removeItemWidget( currentItem );
//	m_AbsenceModeListbox->insertItem( currentText, deleteIndex - 1 );
//	m_AbsenceModeListbox->setSelected( deleteIndex - 1, TRUE );
	QListWidgetItem *item = new QListWidgetItem( currentText );
	m_AbsenceModeListbox->insertItem( deleteIndex - 1, item );
	item->setSelected( TRUE );
	setStatus();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotUpClicked" << endl;
}
void KIpMsgAbsenceModeConfigDialog::slotDownClicked()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::slotDownClicked" << endl;
//	unsigned int deleteIndex = m_AbsenceModeListbox->currentItem();
	int deleteIndex = m_AbsenceModeListbox->currentRow();
//	QString currentText = m_AbsenceModeListbox->currentText();
	QListWidgetItem *currentItem = m_AbsenceModeListbox->item(deleteIndex);
	if(currentItem == NULL){
		setStatus();
		return;
	}
//	QString currentText = m_AbsenceModeListbox->currentText();
	QString currentText = currentItem->text();
//	m_AbsenceModeListbox->removeItem( deleteIndex );
	m_AbsenceModeListbox->removeItemWidget( currentItem );
//	m_AbsenceModeListbox->insertItem( currentText, deleteIndex + 1 );
//	m_AbsenceModeListbox->setSelected( deleteIndex + 1, TRUE );
	QListWidgetItem *item = new QListWidgetItem( currentText );
	m_AbsenceModeListbox->insertItem( deleteIndex + 1, item );
	item->setSelected( TRUE );
	setStatus();
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::slotDownClicked" << endl;
}

QString KIpMsgAbsenceModeConfigDialog::getSelectedAbsenceModeKey( QString text )
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::getSelectedAbsenceModeKey" << endl;
	QStringList strs = text.split( ":" );
//	QStringList strs = QStringList::split( ":", text );
	return strs[0];
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::getSelectedAbsenceModeKey" << endl;
}

/**
 * 選択された項目をテキストに設定する。
 */
void KIpMsgAbsenceModeConfigDialog::setSelectedItem()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::setSelectedItem" << endl;
	AbsenceSetting conf = settings[currentKey][currentEncoding];
	m_TitleEditbox->setText( conf.title );
	m_DescriptionTextbox->setText( conf.detail );
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::setSelectedItem" << endl;
}

/**
 * 入力有効／無効のステータスを設定する。
 */
void KIpMsgAbsenceModeConfigDialog::setStatus()
{
	kDebug() << "START KIpMsgAbsenceModeConfigDialog::setStatus" << endl;
	if ( m_AbsenceModeListbox->count() > 0 ) {
		m_EncodingCombobox->setEnabled( TRUE );
		m_TitleEditbox->setEnabled( TRUE );
		m_DescriptionTextbox->setEnabled( TRUE );
		m_DeleteButton->setEnabled( TRUE );
//		if ( m_AbsenceModeListbox->currentItem() == 0 ){
		if ( m_AbsenceModeListbox->currentRow() == 0 ){
//			m_UpButton->setPixmap( SmallIcon("kipmsg_up_disabled") );
			m_UpButton->setIcon( SmallIcon("kipmsg_up_disabled") );
			m_UpButton->setEnabled( FALSE );
		} else {
//			m_UpButton->setPixmap( SmallIcon("kipmsg_up_enabled") );
			m_UpButton->setIcon( SmallIcon("kipmsg_up_enabled") );
			m_UpButton->setEnabled( TRUE );
		}
//		if ( m_AbsenceModeListbox->currentItem() >= (int)m_AbsenceModeListbox->count() - 1 ){
		if ( m_AbsenceModeListbox->currentRow() >= (int)m_AbsenceModeListbox->count() - 1 ){
//			m_DownButton->setPixmap( SmallIcon("kipmsg_down_disabled") );
			m_DownButton->setIcon( SmallIcon("kipmsg_down_disabled") );
			m_DownButton->setEnabled( FALSE );
		} else {
//			m_DownButton->setPixmap( SmallIcon("kipmsg_down_enabled") );
			m_DownButton->setIcon( SmallIcon("kipmsg_down_enabled") );
			m_DownButton->setEnabled( TRUE );
		}
		m_SetButton->setEnabled( TRUE );
	} else {
		m_EncodingCombobox->setEnabled( FALSE );
		m_TitleEditbox->setEnabled( FALSE );
		m_DescriptionTextbox->setEnabled( FALSE );
		m_DeleteButton->setEnabled( FALSE );
		m_UpButton->setIcon( SmallIcon("kipmsg_up_disabled") );
//		m_UpButton->setPixmap( SmallIcon("kipmsg_up_disabled") );
		m_UpButton->setEnabled( FALSE );
		m_DownButton->setIcon( SmallIcon("kipmsg_down_disabled") );
//		m_DownButton->setPixmap( SmallIcon("kipmsg_down_disabled") );
		m_DownButton->setEnabled( FALSE );
		m_SetButton->setEnabled( FALSE );
	}
	kDebug() << "END   KIpMsgAbsenceModeConfigDialog::setStatus" << endl;
}

#include "absenceconfig.moc"
