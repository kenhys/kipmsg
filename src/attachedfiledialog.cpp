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


#include <qtextcodec.h>
#include <klocale.h>
#include <klistview.h>
#include <kfiledialog.h>

#include "attachedfiledialog.h"
#include "kipmsgsettings.h"
#include "kipmsgutils.h"
#include "IpMessenger.h"


using namespace std;
using namespace ipmsg;

/**
 * コンストラクタ
 * ・リストビューの初期設定。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMessengerAttachedFileDialog::KIpMessengerAttachedFileDialog(QWidget* parent, const char* name, WFlags fl)
        : KIpMessengerAttachedFileDialogBase(parent,name,fl)
{
	m_AttachFileListView->addColumn( tr2i18n( "File" ) );
	m_AttachFileListView->addColumn( tr2i18n( "Size" ) );
	m_AttachFileListView->addColumn( tr2i18n( "Timestamp" ) );
	m_AttachFileListView->addColumn( tr2i18n( "Full Path" ) );
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMessengerAttachedFileDialog::~KIpMessengerAttachedFileDialog()
{}

/*$SPECIALIZATION$*/

/**
 * 添付ファイルリスト設定
 * ・添付ファイルリストを保存。
 * ・添付ファイルリストからリストビューに表示。
 */
void KIpMessengerAttachedFileDialog::setFiles( AttachFileList _files )
{
	files = _files;
	setFileNames();
}

/**
 * 添付ファイルリスト取得
 * ・添付ファイルリストを返す。
 */
AttachFileList KIpMessengerAttachedFileDialog::getFiles()
{
	return files;
}

/**
 * OKクリックイベント
 * ・アクセプトによりダイアログを閉じる。
 */
void KIpMessengerAttachedFileDialog::slotOkClicked()
{
	accept();
}

/**
 * キャンセルクリックイベント
 * ・リジェクトによりダイアログを閉じる。
 */
void KIpMessengerAttachedFileDialog::slotCancelClicked()
{
	reject();
}

/**
 * ファイル追加クリックイベント
 * ・コモンダイアログによりファイルを選択しリストビューに追加。
 */
void KIpMessengerAttachedFileDialog::slotAddFilesClicked()
{
	QString attachFileName = KFileDialog::getOpenFileName();
	if ( attachFileName != "" ) {
		AttachFile file;
		file.setFullPath( attachFileName.data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	setFileNames();
}

/**
 * ディレクトリ追加クリックイベント
 * ・コモンダイアログによりディレクトリを選択しリストビューに追加。
 */
void KIpMessengerAttachedFileDialog::slotAddDirectoriesClicked()
{
	QString attachDirName = KFileDialog::getExistingDirectory();
	if ( attachDirName != "" ) {
		AttachFile file;
		file.setFullPath( attachDirName.data() );
		file.GetLocalFileInfo();
		files.AddFile( file );
	}
	setFileNames();
}

/**
 * 削除クリックイベント
 * ・リストビューの選択中のアイテムを削除。
 */
void KIpMessengerAttachedFileDialog::slotDeleteClicked()
{
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	QListViewItemIterator it( m_AttachFileListView );
	while ( it.current() != NULL ) {
		QListViewItem *item = it.current();
		if ( item->isSelected() ) {
			vector<AttachFile>::iterator i = files.FindByFullPath( codec->fromUnicode( item->text( 3 ) ).data() );
			if ( i != files.end() ) {
				files.erase( i );
			}
		}
		it++;
	}
	setFileNames();
}

/**
 * ・添付ファイルリスト表示
 * ・添付ファイルリストからリストビューに表示。
 */
void KIpMessengerAttachedFileDialog::setFileNames()
{
	m_AttachFileListView->clear();
	QTextCodec *codec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	for( vector<AttachFile>::iterator it = files.begin(); it != files.end(); it++ ){
		it->GetLocalFileInfo();
		QString size("");
		if ( it->IsDirectory() ) {
			size = tr2i18n("(DIR)");
		} else {
			size = QString("%1").arg(it->FileSize());
		}
		new QListViewItem( m_AttachFileListView,
						   codec->toUnicode( it->FileName().c_str() ),
						   size,
						   CreateTimeString( it->MTime() ),
						   codec->toUnicode( it->FullPath().c_str() ) );
	}
}

#include "attachedfiledialog.moc"
