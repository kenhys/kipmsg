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


#include <kwin.h>
#include <qtextcodec.h>
#include <qlabel.h>
#include "downloadcompletedialog.h"
#include "downloaderrordialog.h"
#include "kipmsgevent.h"
#include "kipmsgwidget.h"
#include "kipmsgsettings.h"
#include "kipmsglogger.h"

/**
 * ホストリスト更新後イベント
 * ・全ての送信ウインドウのホストリストをネットワークから再取得しないで、
 *   エージェント内のホストリストでリフレッシュする
 * @param hostList ホストリスト
 */
void
KIpMsgEvent::UpdateHostListAfter( HostList& /*hostList*/ ){
	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( sendDlg->isShown() ) {
			sendDlg->refreshHostList( false );
		}
		++sendIt;
	}
}

/**
 * ホストリスト更新リトライエラーイベント
 * ・特にすること無し。
 */
void
KIpMsgEvent::GetHostListRetryError(){
	printf("GetHostListRetryError KIPMSG\n");
}

/**
 * 受信後イベント
 * ・ノンポップアップ受信条件なら受信ダイアログを表示。
 * ・ノンポップアップ受信条件ではないなら受信済みの未表示メッセージリストに登録。
 * @param msg メッセージ
 */
bool
KIpMsgEvent::RecieveAfter( RecievedMessage& msg ){
	if ( !kipmsgWidget::isRecievedOnNonePopup() ) {
		ShowRecieveMsg( msg );
	} else {
		hiddenMessages.push_back( msg );
	}
	return true;
}

/**
 * 送信後イベント
 * ・特にすること無し。
 * @param msg メッセージ
 */
void
KIpMsgEvent::SendAfter( SentMessage& /*msg*/ ){
	printf("SendAfter KIPMSG\n");
}

/**
 * 送信リトライエラーイベント
 * ・特にすること無し。
 * @param msg メッセージ
 */
void
KIpMsgEvent::SendRetryError( SentMessage& /*msg*/ ){
	printf("SendRetryError KIPMSG\n");
}

/**
 * 開封後イベント
 * ・開封確認ダイアログを表示し開封確認ダイアログリストに登録する。
 * @param msg メッセージ
 */
void
KIpMsgEvent::OpenAfter( SentMessage& msg ){
	IpMessengerAgent *IpMsgAgent = IpMessengerAgent::GetInstance();
	time_t t = time( NULL );
	char timebuf[30];
	ctime_r( &t, timebuf );
	timebuf[strlen(timebuf) - 1] = 0;
	if ( msg.IsSecret() && msg.IsConfirmed() && !msg.IsConfirmAnswered() ) {
		QString encode = "";
		if ( msg.Host().EncodingName() == "" ) {
			encode = msg.Host().EncodingName().c_str();
		}else {
			encode = KIpMsgSettings::messageEncoding();
		}
		QStringList encodings = KIpMsgSettings::encodingSettings();
		QString IpAddr = msg.Host().IpAddress().c_str();
		QString UserName = msg.Host().UserName().c_str();
		for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
			QStringList fields = QStringList::split( ":", *ite );
			if ( IpAddr == fields[0] && UserName == fields[1] ) {
				encode = fields[2];
				break;
			}
		}
		QTextCodec *codec = QTextCodec::codecForName( encode );
		OpenConfirmDialog *opendlg = new OpenConfirmDialog();
		IpMsgAgent->AcceptConfirmNotify( msg );
		opendlg->m_NicknameLabel->setText( codec->toUnicode( msg.Host().Nickname().c_str() ) );
		opendlg->m_TimeLabel->setText( "(" + QString(timebuf) + ")" );
		opendlg->show();
		if ( KIpMsgSettings::confirmIcon() ) {
			KWin::iconifyWindow( opendlg->winId() );
		} else {
			KWin::activateWindow( opendlg->winId() );
		}
		confirmDialogs.append(opendlg);
	}
}

/**
 * ダウンロード開始イベント
 * ・特にすること無し。
 * @param msg メッセージ
 * @param file 添付ファイル
 * @param info ダウンロード情報
 * @param data 付加データ
 */
void
KIpMsgEvent::DownloadStart( RecievedMessage& /*msg*/, AttachFile& /*file*/, DownloadInfo& /*info*/, void * /*data*/ ){
	printf("DownloadStart KIPMSG\n");
}

/**
 * ダウンロード処理中イベント
 * ・特にすること無し。
 * @param msg メッセージ
 * @param file 添付ファイル
 * @param info ダウンロード情報
 * @param data 付加データ
 */
void
KIpMsgEvent::DownloadProcessing( RecievedMessage& /*msg*/, AttachFile& /*file*/, DownloadInfo& /*info*/, void * /*data*/ ){
	printf("DownloadProcessing KIPMSG\n");
}

/**
 * ダウンロード終了イベント
 * ・終了ダイアログを受信ダイアログの子ウインドウとして表示する。
 *   ダイアログを閉じたら添付ファイルリストから該当のファイルを削除。
 * @param msg メッセージ
 * @param file 添付ファイル
 * @param info ダウンロード情報
 * @param data 付加データ
 */
void
KIpMsgEvent::DownloadEnd( RecievedMessage& msg, AttachFile& file, DownloadInfo& info, void *data ){
	RecieveDialog *recvDlg = static_cast<RecieveDialog *>(data);
	DownloadCompleteDialog *dlg = new DownloadCompleteDialog(recvDlg, 0, TRUE);
	dlg->setDownloadInfo( info );
	dlg->exec();
	delete dlg;
	msg.Files().erase( file );
}

/**
 * ダウンロードエラーイベント
 * ・エラーダイアログを受信ダイアログの子ウインドウとして表示する。
 *   ダイアログがTRUEを返したらリトライする。
 * @param msg メッセージ
 * @param file 添付ファイル
 * @param info ダウンロード情報
 * @param data 付加データ
 */
bool
KIpMsgEvent::DownloadError( RecievedMessage& /*msg*/, AttachFile& /*file*/, DownloadInfo& info, void *data ){
	RecieveDialog *recvDlg = static_cast<RecieveDialog *>(data);
	DownloadErrorDialog *dlg = new DownloadErrorDialog(recvDlg, 0, TRUE);
	dlg->setDownloadInfo( info );
	if ( dlg->exec() == QDialog::Accepted ) {
		return true;
	}
	return false;
}

/**
 * 参加通知後イベント
 * ・全ての送信ウインドウのホストリストをネットワークから再取得しないで、
 *   エージェント内のホストリストでリフレッシュする
 * @param hostList ホストリスト
 */
void
KIpMsgEvent::EntryAfter( HostList& /*hostList*/ ){
	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( sendDlg->isShown() ) {
			sendDlg->refreshHostList( false );
		}
		++sendIt;
	}
}

/**
 * 脱退通知後イベント
 * ・全ての送信ウインドウのホストリストをネットワークから再取得しないで、
 *   エージェント内のホストリストでリフレッシュする
 * @param hostList ホストリスト
 */
void
KIpMsgEvent::ExitAfter( HostList& /*hostList*/ ){
	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( sendDlg->isShown() ) {
			sendDlg->refreshHostList( false );
		}
		++sendIt;
	}
}

//Original Methods

/**
 * 受信ダイアログ一覧取得
 * ・受信ダイアログリストを返す。
 */
QPtrList<RecieveDialog>&
KIpMsgEvent::GetRecieveDialogs(){
	return recieveDialogs;
}

/**
 * 送信ダイアログ一覧取得
 * ・送信ダイアログリストを返す。
 */
QPtrList<SendDialog>&
KIpMsgEvent::GetSendDialogs(){
	return sendDialogs;
}

/**
 * 送信ダイアログ表示
 * ・送信ウインドウを表示し、送信ダイアログリストに登録する。
 */
void
KIpMsgEvent::ShowSendDlg(){
	SendDialog *sendWin = new SendDialog();
	sendWin->show();
	sendDialogs.append( sendWin );
}

/**
 * 全ての開封確認を閉じる。
 * ・開封確認ダイアログリストを基に全ての開封確認ダイアログを閉じ、開封確認ダイアログリストをクリアする。
 */
void
KIpMsgEvent::HideAllOpenConfirm(){
	QPtrListIterator<OpenConfirmDialog> itp(confirmDialogs);
	OpenConfirmDialog *dlg;
	while( ( dlg = itp.current() ) != 0 ) {
		if ( dlg->isShown() ) {
			dlg->setHidden(TRUE);
		}
		++itp;
	}
	while( confirmDialogs.count() > 0 ){
		confirmDialogs.remove( 0U );
	}
}

/**
 * 全てのウインドウを前面に
 * ・開封確認ダイアログリストを基に全ての開封確認ダイアログを前面に表示する。
 * ・送信ダイアログリストを基に全ての送信ダイアログを前面に表示する。
 * ・受信ダイアログリストを基に全ての受信ダイアログを前面に表示する。
 */
void
KIpMsgEvent::StayOnTopAllWindows(){
	QPtrListIterator<OpenConfirmDialog> confirmIt(confirmDialogs);
	OpenConfirmDialog *confirmDlg;
	while( ( confirmDlg = confirmIt.current() ) != 0 ) {
		if ( confirmDlg->isShown() ) {
			KWin::activateWindow( confirmDlg->winId() );
		}
		++confirmIt;
	}
	QPtrListIterator<RecieveDialog> recieveIt(recieveDialogs);
	RecieveDialog *recieveDlg;
	while( ( recieveDlg = recieveIt.current() ) != 0 ) {
		if ( recieveDlg->isShown() ) {
			KWin::activateWindow( recieveDlg->winId() );
		}
		++recieveIt;
	}
	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( sendDlg->isShown() ) {
			KWin::activateWindow( sendDlg->winId() );
		}
		++sendIt;
	}
}

/**
 * 受信済みのメッセージの個数を取得
 * 受信済みの未表示メッセージの個数を返す。
 */
int
KIpMsgEvent::GetRecievedMessageCount(){
	return hiddenMessages.size();
}

/**
 * タイマごとの処理。クローズしたウインドウの後始末
 * ・開封確認ダイアログリストを基に閉じられた全ての開封確認ダイアログをを削除する。
 * ・送信ダイアログリストを基に閉じられた全ての送信ダイアログをを削除する。
 * ・受信ダイアログリストを基に閉じられた全ての受信ダイアログを削除する。
 */
void
KIpMsgEvent::TimerEvent(){
	QPtrListIterator<OpenConfirmDialog> confirmIt(confirmDialogs);
	OpenConfirmDialog *confirmDlg;
	while( ( confirmDlg = confirmIt.current() ) != 0 ) {
		if ( !confirmDlg->isShown() ) {
			confirmDialogs.remove(confirmDlg);
			continue;
		}
		++confirmIt;
	}

	QPtrListIterator<SendDialog> sendIt(sendDialogs);
	SendDialog *sendDlg;
	while( ( sendDlg = sendIt.current() ) != 0 ) {
		if ( !sendDlg->isShown() ) {
			sendDialogs.remove(sendDlg);
			continue;
		}
		++sendIt;
	}

	QPtrListIterator<RecieveDialog> recieveIt(recieveDialogs);
	RecieveDialog *recieveDlg;
	while( ( recieveDlg = recieveIt.current() ) != 0 ) {
		if ( !recieveDlg->isShown() ) {
			recieveDialogs.remove(recieveDlg);
			continue;
		}
		++recieveIt;
	}
}

/**
 * 受信メッセージの表示
 * ・受信ダイアログを表示。
 * ・受信ダイアログリストに受信ダイアログを登録。
 * ・受信音再生。
 * @param msg メッセージ
 */
void
KIpMsgEvent::ShowRecieveMsg( RecievedMessage& msg ){
	RecieveDialog *recv = new RecieveDialog();
	recv->setMessage( msg );
	recv->setDownloadFiles();
	recv->show();
	if ( !KIpMsgSettings::recordAfterUnlock() ){
		KIpMessengerLogger::GetInstance()->PutRecivedMessage( msg );
	}
	KWin::activateWindow( recv->winId() );
	recieveDialogs.append( recv );
	kipmsgWidget::playSound();
}

/**
 * 受信済のメッセージを全て表示する。
 * ・受信済みの未表示メッセージリストから全ての受信済みの未表示メッセージを受信ダイアログに表示。
 * ・受信済みの未表示メッセージリストを全てクリア。
 */
void
KIpMsgEvent::ShowHiddenRecieveMsg(){
	for( vector<RecievedMessage>::iterator ix = hiddenMessages.begin(); ix != hiddenMessages.end(); ix++ ){
		ShowRecieveMsg( *ix );
	}
	hiddenMessages.clear();
}
