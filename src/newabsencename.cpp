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


#include <klineedit.h>
#include "newabsencename.h"
#include "absenceconfig.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
NamingNewAbsenceMode::NamingNewAbsenceMode(QWidget* parent, const char* name, WFlags fl)
        : NamingNewAbsenceModeBase(parent,name,fl)
{}

/**
 * デストラクタ
 * ・特にすること無し。
 */
NamingNewAbsenceMode::~NamingNewAbsenceMode()
{}

/*$SPECIALIZATION$*/

/**
 * OKクリックイベント
 * ・ウインドウを閉じる
 */
void NamingNewAbsenceMode::slotOkClicked()
{
	if ( m_UniqueIdEditbox->text() == "" ) {
		return;
	}
	AbsenceConfigDlg->setNewId( m_UniqueIdEditbox->text() );
	accept();
}
/**
 * キャンセルクリックイベント
 * ・ウインドウを閉じる
 */
void NamingNewAbsenceMode::slotCancelClicked()
{
	reject();
}

#include "newabsencename.moc"

