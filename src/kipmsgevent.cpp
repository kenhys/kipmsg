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


#include <kmessagebox.h>
#include <klocale.h>
#include <kwin.h>
#include <klistview.h>
#include <qtextcodec.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kuniqueapplication.h>
#include "downloadcompletedialog.h"
#include "downloaderrordialog.h"
#include "kipmsgevent.h"
#include "kipmsgwidget.h"
#include "kipmsgsettings.h"
#include "kipmsglogger.h"
#include "kipmsgutils.h"

static KIpMsgNotify *notity = NULL;

/**
 * ホストリストリフレッシュ後イベント
 * ・全ての送信ウインドウのホストリストを更新する
 * @param hostList ホストリスト
 */
void
KIpMsgEvent::RefreashHostListAfter( HostList& /*hostList*/ ){
	RefreshHostListInAllSendDlg();
}

/**
 * ホストリスト更新後イベント
 * ・全ての送信ウインドウのホストリストを更新する
 * @param hostList ホストリスト
 */
void
KIpMsgEvent::UpdateHostListAfter( HostList& /*hostList*/ ){
	RefreshHostListInAllSendDlg();
}

/**
 * ホストリスト更新リトライエラーイベント
 * ・特にすること無し。
 * @retval true:継続、false:中断
 */
bool
KIpMsgEvent::GetHostListRetryError(){
//	printf("GetHostListRetryError KIPMSG\n");
	//継続しないのでFalseをリターン
	return false;
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
		if ( KIpMsgSettings::notifyOnNoPopupMessageRecieve() ) {
			notity = createNotifyWindow();
			notity->addRecievedMessage( msg );
			notity->show();
		}
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
//	printf("SendAfter KIPMSG\n");
}

/**
 * 送信リトライエラーイベント
 * ・特にすること無し。
 * @param msg メッセージ
 * @retval true:継続、false:中断
 */
bool
KIpMsgEvent::SendRetryError( SentMessage& msg ){
	QString toName = CreateHostInfoString(msg.Host()).c_str();
	if ( msg.Host().EncodingName() != "" ){
		QTextCodec *codec = QTextCodec::codecForName( msg.Host().EncodingName().c_str() );
		if ( codec != NULL ){
			toName = codec->toUnicode( toName );
		}
	}

	if ( KMessageBox::warningContinueCancel( 0, QString( tr2i18n("Can't send to %1.\nDoes it Retry?") ).arg( toName ) ) == KMessageBox::Continue ){
		return true;
	}
	return false;
}

/**
 * 開封後イベント
 * ・開封確認ダイアログを表示し開封確認ダイアログリストに登録する。
 * @param msg メッセージ
 */
void
KIpMsgEvent::OpenAfter( SentMessage& msg ){
	IpMessengerAgent *IpMsgAgent = IpMessengerAgent::GetInstance();
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
		opendlg->m_TimeLabel->setText( "(" + CreateTimeString( time( NULL ) ) + ")" );
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
//	printf("DownloadStart KIPMSG\n");
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
KIpMsgEvent::DownloadProcessing( RecievedMessage& /*msg*/, AttachFile& file, DownloadInfo& info, void * data ){
	if ( kapp != NULL ){
		kapp->processEvents();
	}
	RecieveDialog *recvDlg = static_cast<RecieveDialog *>(data);
	QListViewItemIterator it( recvDlg->m_AttachmentFiles );
	while ( it.current() != NULL ) {
		KIpMsgAttachedFileListViewItem *item = dynamic_cast<KIpMsgAttachedFileListViewItem *>(it.current());
		if ( item != NULL && item->isSelected() ) {
			int percentage = 0;
			if ( file.FileId() == item->file().FileId() ) {
				if ( file.IsRegularFile() ){
					if ( info.File().FileSize() != 0 ) {
						percentage = (int)( (long double)100 * ( (long double)info.Size() / (long double)info.File().FileSize() ) );
					}
				}
				if ( file.IsDirectory() && !info.Processing() ){
					percentage = 100;
				}
				QString percentageStr = QString("%1%").arg(percentage);
				item->setText( 1, percentageStr );
				QString iconName = GetPercentageIconName( percentage );
				item->setPixmap( 0, SmallIcon( iconName ) );
				recvDlg->update();
				return;
			}
		}
		++it;
	}
	recvDlg->update();
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
	if ( recvDlg->isOpenSaveDialog() ){
		DownloadCompleteDialog *dlg = new DownloadCompleteDialog(recvDlg, 0, TRUE);
		dlg->setDownloadInfo( info );
		dlg->exec();
		delete dlg;
	}
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
 * ・通知ウインドウを表示する。
 * @param host ホスト
 */
void
KIpMsgEvent::EntryAfter( HostListItem& host ){
	if ( host.IsLocalHost() || !KIpMsgSettings::notifyOnLoginLogoutAbsence() ) {
		return;
	}
	GetHostEncodingFromConfig( host );
	notity = createNotifyWindow();
	notity->addLoginMessage( host );
	notity->show();
}

/**
 * 脱退通知後イベント
 * ・通知ウインドウを表示する。
 * @param host ホスト
 */
void
KIpMsgEvent::ExitAfter( HostListItem& host ){
	if ( host.IsLocalHost() || !KIpMsgSettings::notifyOnLoginLogoutAbsence() ) {
		return;
	}
	GetHostEncodingFromConfig( host );
	notity = createNotifyWindow();
	notity->addLogoutMessage( host );
	notity->show();
}

/**
 * 不在モード変更後イベント
 * ・通知ウインドウを表示する。
 * @param host ホスト
 */
void
KIpMsgEvent::AbsenceModeChangeAfter( HostListItem& host )
{
	if ( host.IsLocalHost() || !KIpMsgSettings::notifyOnLoginLogoutAbsence() ) {
		return;
	}
	GetHostEncodingFromConfig( host );
	notity = createNotifyWindow();
	notity->addAbsenceModeChangeMessage( host );
	notity->show();
}

/**
 * 通知ウインドウ生成
 * @retval 通知ウインドウ
 */
KIpMsgNotify*
KIpMsgEvent::createNotifyWindow()
{
	if ( notity == NULL ) {
		notity = new KIpMsgNotify( 0,0, Qt::WStyle_StaysOnTop | Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WX11BypassWM);
	}
	return notity;
}

/**
 * バージョン情報受信後イベント
 * ・メッセージボックスにバージョン情報を表示する。
 * @param version バージョン情報
 */
void KIpMsgEvent::VersionInfoRecieveAfter( HostListItem &host, string version )
{
	GetHostEncodingFromConfig( host );
	string hostInfo = CreateHostInfoString(host);
	QString msg = ( hostInfo + "\n" + version ).c_str();
	if ( host.EncodingName() != "" ){
		QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
		msg = codec->toUnicode( hostInfo.c_str() ) + "\n" + codec->toUnicode( version.c_str() );
	}
	KMessageBox::information( 0, msg );
}

/**
 * 不在モード受信後イベント
 * ・メッセージボックスに不在詳細情報を表示する。
 * @param absenceDetail 不在詳細情報
 */
void KIpMsgEvent::AbsenceDetailRecieveAfter( HostListItem &host, string absenceDetail )
{
	GetHostEncodingFromConfig( host );
	string hostInfo = CreateHostInfoString(host);
	QString msg = ( hostInfo + "\n" + absenceDetail ).c_str();
	if ( host.EncodingName() != "" ){
		QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
		msg = codec->toUnicode( hostInfo.c_str() ) + "\n" + codec->toUnicode( absenceDetail.c_str() );
	}
	KMessageBox::information( 0, msg );
}

//Original Methods

void 
KIpMsgEvent::GetHostEncodingFromConfig( HostListItem &host )
{
	host.setEncodingName( "" );
	QStringList encodings = KIpMsgSettings::encodingSettings();
	for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
		QStringList fields = QStringList::split( ":", *ite );
		if ( QString( host.IpAddress().c_str() ) == fields[0] && 
			QString( host.UserName().c_str() ) == fields[1] ) {
			host.setEncodingName( string( fields[2].data() ) );
			break;
		}
	}
}

/**
 * 全ての送信ダイアログのホストリストを更新する。
 * ・全ての送信ウインドウのホストリストをネットワークから再取得しないで、
 *   エージェント内のホストリストでリフレッシュする
 */
void 
KIpMsgEvent::RefreshHostListInAllSendDlg()
{
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
	RecieveDialog *recv = new RecieveDialog(msg,0,0,0);
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

/**
 * 指定された受信済のメッセージを検索して表示する。
 * ・受信済みの未表示メッセージリストから指定された受信済みの未表示メッセージを受信ダイアログに表示。
 * ・受信済みの未表示メッセージリストから該当メッセージを削除。
 */
void
KIpMsgEvent::FindAndShowHiddenRecieveMsg( RecievedMessage& msg ){
	for( vector<RecievedMessage>::iterator ix = hiddenMessages.begin(); ix != hiddenMessages.end(); ix++ ){
		if ( msg.MessagePacket().PacketNo() == ix->MessagePacket().PacketNo() ) {
			ShowRecieveMsg( *ix );
			hiddenMessages.erase( ix );
			break;
		}
	}
}

/**
 * ホスト情報を生成する。
 * @retval Nickname(GroupName/Hostname)形式の文字列
 */
string
KIpMsgEvent::CreateHostInfoString(HostListItem host){
	string hostInfoName = "";
	if ( host.GroupName() != "" ) {
		hostInfoName = host.Nickname() + "(" + host.GroupName() + "/" + host.HostName() + ")";
	} else {
		hostInfoName = host.Nickname() + "(" + host.HostName() + ")";
	}
	return hostInfoName;
}
