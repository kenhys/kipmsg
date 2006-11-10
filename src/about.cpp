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


#include "about.h"
#include "kurl.h"
#include "krun.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMessengerAboutDialog::KIpMessengerAboutDialog(QWidget* parent, const char* name, WFlags fl)
        : KIpMessengerAboutDialogBase(parent,name,fl)
{}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMessengerAboutDialog::~KIpMessengerAboutDialog()
{}

/*$SPECIALIZATION$*/
/**
 * OKクリックイベント
 * ・ウインドウを閉じる
 */
void KIpMessengerAboutDialog::slotOkClicked()
{
	close();
}
/**
 * Webサイトを表示
 * ・デフォルトブラウザでWebサイトを表示します。
 */
void KIpMessengerAboutDialog::slotJumpClicked()
{
	KURL url = KURL( QString( "http://www.ipmsg.org" ) );
	(new KRun( url ))->setAutoDelete( true );
}

#include "about.moc"

