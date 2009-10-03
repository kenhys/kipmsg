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


#include <ktextedit.h>
#include <klistbox.h>
#include "idiomconfig.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
IdiomConfigDialog::IdiomConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : IdiomConfigDialogBase(parent,name,fl)
{
	idioms = KIpMsgSettings::idiomSettings();
	QStringList::Iterator itIdiom = idioms.begin();
	while ( itIdiom != idioms.end() ) {
		m_IdiomListBox->insertItem( getListItem( *itIdiom ) );
		itIdiom++;
	}
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
void IdiomConfigDialog::slotOKClicked()
{
	KIpMsgSettings::setIdiomSettings( idioms );
	KIpMsgSettings::writeConfig();
    close();
}

/**
 * キャンセルクリックイベント
 * ・ウインドウを閉じる
 */
void IdiomConfigDialog::slotCancelClicked()
{
    close();
}

/**
 * ＞＞クリックイベント
 * ・定型文をアクティブなリストに追加
 */
void IdiomConfigDialog::slotAddIdiomClicked()
{
	if ( m_IdiomTextEdit->text() == "" ) {
		return;
	}
	m_IdiomListBox->insertItem( getListItem( m_IdiomTextEdit->text() ) );
	idioms << m_IdiomTextEdit->text();
	m_IdiomTextEdit->setText("");
}

/**
 * ＜＜クリックイベント
 * ・定型文をアクティブなリストにから削除しテキストボックスに移動。
 */
void IdiomConfigDialog::slotDeleteIdiomClicked()
{
	int selectedIndex = m_IdiomListBox->index( m_IdiomListBox->selectedItem() );
	if ( selectedIndex < 0 ) {
		return;
	}
	m_IdiomTextEdit->setText( idioms[selectedIndex] );
	m_IdiomListBox->removeItem( selectedIndex );
	QStringList::iterator it = idioms.begin();
	for( int i = 0; i <= selectedIndex - 1; ++it, i++ );
	idioms.remove( it );
}

/**
 * リストボックスに表示する名称を取得する。
 * ・改行コードまでを取り出し、改行が含まれている場合は"..."を追加して返す。
 * @param テキストボックスの内容
 * @retval リストボックスに表示する内容
 */
QString IdiomConfigDialog::getListItem( QString idiom )
{
	QStringList lines = QStringList::split( "\n", idiom );
	if ( lines.size() == 1 ) {
		return idiom;
	} else {
		return lines[0] + "..." ;
	}
}

#include "idiomconfig.moc"
