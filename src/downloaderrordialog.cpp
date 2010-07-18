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


#include <qlabel.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpushbutton.h>
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

#include "downloaderrordialog.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
DownloadErrorDialog::DownloadErrorDialog(QWidget* parent, const char* name, Qt::WindowFlags fl)
        : DownloadErrorDialogBase()
//        : DownloadErrorDialogBase(parent,name,fl)
{
	kDebug() << "START DownloadErrorDialog::DownloadErrorDialog" << endl;
	setupUi(this);
	setButtons( None );
	kDebug() << "END   DownloadErrorDialog::DownloadErrorDialog" << endl;
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
DownloadErrorDialog::~DownloadErrorDialog()
{}

/*$SPECIALIZATION$*/
/**
 * キャンセルボタンクリック
 * ・リジェクトして画面を閉じる。
 */
void DownloadErrorDialog::slotCancelClicked()
{
	kDebug() << "START DownloadErrorDialog::slotCancelClicked" << endl;
	reject();
	kDebug() << "END   DownloadErrorDialog::slotCancelClicked" << endl;
}

/**
 * 再試行ボタンクリック
 * ・アクセプトして画面を閉じる。
 */
void DownloadErrorDialog::slotRetryClicked()
{
	kDebug() << "START DownloadErrorDialog::slotRetryClicked" << endl;
	accept();
	kDebug() << "END   DownloadErrorDialog::slotRetryClicked" << endl;
}

/**
 * ダウンロード情報設定（外部から）
 * ・ダウンロード情報を画面に表示。
 * @param info ダウンロード情報
 */
void DownloadErrorDialog::setDownloadInfo( DownloadInfo info ){
	kDebug() << "START DownloadErrorDialog::slotRetryClicked" << endl;
	m_DetailLabel1->setText( QString( tr2i18n( "Total %1 (%2)" ) ).arg( info.getSizeString().c_str() ).arg( info.getSpeedString().c_str() ) );
	m_DetailLabel2->setText( QString( tr2i18n( "%1 sec  %2 files" ) ).arg( info.Time() ).arg( info.FileCount() ) );
	kDebug() << "END   DownloadErrorDialog::slotRetryClicked" << endl;
}

#include "downloaderrordialog.moc"

