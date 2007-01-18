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


#include <qlabel.h>
#include <qtextcodec.h>
#include <krun.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kuserprofile.h>
#include <kservice.h>
#include <kpushbutton.h>
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

#include "downloadcompletedialog.h"
#include "kipmsgsettings.h"

/**
 * コンストラクタ
 * ・特にすること無し。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
DownloadCompleteDialog::DownloadCompleteDialog(QWidget* parent, const char* name, WFlags fl)
        : DownloadCompleteDialogBase(parent,name,fl)
{}

/**
 * デストラクタ
 * ・特にすること無し。
 */
DownloadCompleteDialog::~DownloadCompleteDialog()
{}

/*$SPECIALIZATION$*/
/**
 * 閉じるボタンクリック
 * ・リジェクトして画面を閉じる。
 */
void DownloadCompleteDialog::slotCloseClicked()
{
	reject();
}

/**
 * 「なんとか」で開くボタンクリック
 * ・ローカルファイル名からURLを生成。
 * ・KDEにMIMETYPEを問い合わせ、サービスを得る。
 * ・サービスが取得出来ない場合、ユーザに対応アプリケーションを選択させアプリケーションで開く。
 * ・サービスが取得出来た場合、KDEのMIMEの関連付けに基づきアプリケーションで開く。
 * ・アクセプトして画面を閉じる。
 */
void DownloadCompleteDialog::slotOpenAsClicked()
{
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	KURL url = KURL::fromPathOrURL( QString( fsCodec->toUnicode( Info.LocalFileName().c_str() ) ) );
	
	if ( Info.File().IsDirectory() ) {
		( new KRun( url ) )->setAutoDelete( true );
	} else if ( Info.File().IsRegularFile() ) {
		KMimeType::Ptr mimetype = KMimeType::findByURL( url );
		if ( mimetype == NULL ) {
			return;
		}
		KService::Ptr offer = KServiceTypeProfile::preferredService( mimetype->name(), "Application" );
		if ( offer == NULL ) {
			KRun::displayOpenWithDialog( url );
		} else {
			KRun::run( *offer, url );
		}
	}
	accept();
}

/**
 * ダウンロード情報設定（外部から）
 * ・ローカルファイル名からURLを生成。
 * ・KDEにMIMETYPEを問い合わせ、サービスを得る。
 * ・サービスが取得出来ない場合、ボタンフェイスを「アプリケーションで開く」に設定。
 * ・サービスが取得出来た場合、KDEのMIMEの関連付けに基づき「”なんとかかんとか”で開く」に設定。
 * ・ダウンロード情報を基に各種情報を表示。
 * @param info ダウンロード情報
 */
void DownloadCompleteDialog::setDownloadInfo( DownloadInfo info ){
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	Info = info;
	if ( Info.File().IsDirectory() ) {
		m_OpenAsButton->setText( tr2i18n( "Open folder as Konqueror" ) );
	} else if ( Info.File().IsRegularFile() ) {
		KURL url = KURL::fromPathOrURL( QString( fsCodec->toUnicode( Info.LocalFileName().c_str() ) ) );
		KMimeType::Ptr mimetype = KMimeType::findByURL( url );
		if ( mimetype == NULL ) {
			m_OpenAsButton->setHidden( TRUE );
			return;
		} else {
			KService::Ptr offer = KServiceTypeProfile::preferredService( mimetype->name(), "Application" );
			if ( offer == NULL ) {
				m_OpenAsButton->setText( tr2i18n( "Open as Application" ) );
			} else {
				m_OpenAsButton->setText( tr2i18n( "Open as '%1'" ).arg( offer->name() ) );
			}
		}
	} else {
		m_OpenAsButton->setHidden( TRUE );
	}
    m_DetailLabel1->setText( QString( tr2i18n( "Total %1 (%2)" ) ).arg( info.getSizeString().c_str() ).arg( info.getSpeedString().c_str() ) );
    m_DetailLabel2->setText( QString( tr2i18n( "%1 sec  %2 files" ) ).arg( info.Time() ).arg( info.FileCount() ) );
}

#include "downloadcompletedialog.moc"
