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


#include <kdebug.h>
#include <ktextedit.h>
#include <klistwidget.h>
#include "idiomconfig.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
IdiomConfigDialog::IdiomConfigDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
        : IdiomConfigDialogBase()
//        : IdiomConfigDialogBase(parent,name,fl)
{
	kDebug() << "START IdiomConfigDialog::IdiomConfigDialog" << endl;
	setupUi(this);
	setButtons( None );
	idioms = KIpMsgSettings::idiomSettings();
	QStringList::Iterator itIdiom = idioms.begin();
	while ( itIdiom != idioms.end() ) {
//FIXME m_IdiomListBox->insertItem( getListItem( *itIdiom ) );
		m_IdiomListBox->addItem( getListItem( *itIdiom ) );
		itIdiom++;
	}
	kDebug() << "END   IdiomConfigDialog::IdiomConfigDialog" << endl;
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
IdiomConfigDialog::~IdiomConfigDialog()
{}

/*$SPECIALIZATION$*/
/**
 * OKクリックイベント
 * ・ウインドウを閉じる
 */
void
IdiomConfigDialog::slotOKClicked()
{
	kDebug() << "START IdiomConfigDialog::slotOKClicked" << endl;
	KIpMsgSettings::setIdiomSettings( idioms );
	KIpMsgSettings::self()->writeConfig();
//	close();
	accept();
	kDebug() << "END   IdiomConfigDialog::slotOKClicked" << endl;
}

/**
 * キャンセルクリックイベント
 * ・ウインドウを閉じる
 */
void
IdiomConfigDialog::slotCancelClicked()
{
	kDebug() << "START IdiomConfigDialog::slotCancelClicked" << endl;
//	close();
	reject();
	kDebug() << "END   IdiomConfigDialog::slotCancelClicked" << endl;
}

/**
 * ＞＞クリックイベント
 * ・定型文をアクティブなリストに追加
 */
void
IdiomConfigDialog::slotAddIdiomClicked()
{
	kDebug() << "START IdiomConfigDialog::slotAddIdiomClicked" << endl;
	if ( m_IdiomTextEdit->toPlainText() == "" ) {
		kDebug() << "END   IdiomConfigDialog::slotAddIdiomClicked" << endl;
		return;
	}
//FIXME m_IdiomListBox->insertItem( getListItem( m_IdiomTextEdit->toPlainText() ) );
	m_IdiomListBox->addItem( getListItem( m_IdiomTextEdit->toPlainText() ) );
	idioms << m_IdiomTextEdit->toPlainText();
	m_IdiomTextEdit->setPlainText("");
	kDebug() << "END   IdiomConfigDialog::slotAddIdiomClicked" << endl;
}

/**
 * ＜＜クリックイベント
 * ・定型文をアクティブなリストにから削除しテキストボックスに移動。
 */
void
IdiomConfigDialog::slotDeleteIdiomClicked()
{
	kDebug() << "START IdiomConfigDialog::slotDeleteIdiomClicked" << endl;
// FIXME int selectedIndex = m_IdiomListBox->index( m_IdiomListBox->selectedItem() );
	int selectedIndex = m_IdiomListBox->currentRow();
	if ( selectedIndex < 0 ) {
		kDebug() << "END   IdiomConfigDialog::slotDeleteIdiomClicked" << endl;
		return;
	}
	m_IdiomTextEdit->setText( idioms[selectedIndex] );
	m_IdiomListBox->removeItemWidget( m_IdiomListBox->item( selectedIndex ) );
	QStringList::iterator it = idioms.begin();
	for( int i = 0; i <= selectedIndex - 1; ++it, i++ );
	idioms.removeAll( *it );
	kDebug() << "END   IdiomConfigDialog::slotDeleteIdiomClicked" << endl;
}

/**
 * リストボックスに表示する名称を取得する。
 * ・改行コードまでを取り出し、改行が含まれている場合は"..."を追加して返す。
 * @param テキストボックスの内容
 * @retval リストボックスに表示する内容
 */
QString
IdiomConfigDialog::getListItem( QString idiom )
{
	kDebug() << "START IdiomConfigDialog::getListItem" << endl;
//	QStringList lines = QStringList::split( "\n", idiom );
	QStringList lines = idiom.split( "\n" );
	if ( lines.size() == 1 ) {
		kDebug() << "END   IdiomConfigDialog::getListItem" << endl;
		return idiom;
	} else {
		kDebug() << "END   IdiomConfigDialog::getListItem" << endl;
		return lines[0] + "..." ;
	}
}

#include "idiomconfig.moc"
