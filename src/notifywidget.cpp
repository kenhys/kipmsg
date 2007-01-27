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
#include <klistview.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include "kipmsgutils.h"
#include "kipmsgevent.h"
#include "notifywidget.h"
#include "senddialog.h"

enum TYPE{
	LOGIN,
	LOGOUT,
	ABSENCE_CHANGE,
	RECV_MSG
};

/**
 * コンストラクタ
 * ・生成された時間を記憶する。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMsgNotifier::KIpMsgNotifier( QWidget* parent, const char* name, WFlags fl )
        : KIpMsgNotifierBase(parent,name,fl)
{
	createdTime = time(NULL);
}

/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMsgNotifier::~KIpMsgNotifier()
{}

/**
 * 受信メッセージ設定
 * @param msg メッセージ
 */
void KIpMsgNotifier::setRecievedMessage( RecievedMessage &msg )
{
	printf( "KIpMsgNotifier::setRecievedMessage=[%s]\n", msg.Host().EncodingName().c_str() );
	QString text = tr2i18n( "Message from %1 at %2.").arg( msg.Host().Nickname().c_str() ).arg( CreateTimeString( msg.Recieved() ) );
	m_MessageLabel->setText(text);
	m_ActionButton->setPixmap( SmallIcon("fileopen") );
	resize( sizeHint().expandedTo( minimumSizeHint() ) );
	_msg = msg;
	type = RECV_MSG;
}

/**
 * ログインメッセージ設定
 * @param host ホスト
 */
void KIpMsgNotifier::setLoginMessage( HostListItem &host )
{
	QString nickname = host.Nickname().c_str();
	if ( host.EncodingName() != "" ){
		QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
		if ( codec != NULL ){
			nickname = codec->toUnicode( nickname );
		}
	}

	QString text = tr2i18n( "%1 was logged in at %2.").arg( nickname ).arg( CreateTimeString( time( NULL ) ) );
	m_MessageLabel->setText(text);
	m_ActionButton->setPixmap( SmallIcon("filenew") );
	resize( sizeHint().expandedTo( minimumSizeHint() ) );
	_host = host;
	type = LOGIN;
}

/**
 * ログインメッセージ設定
 * @param host ホスト
 */
void KIpMsgNotifier::setLogoutMessage( HostListItem &host )
{
	QString nickname = host.Nickname().c_str();
	if ( host.EncodingName() != "" ){
		QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
		if ( codec != NULL ){
			nickname = codec->toUnicode( nickname );
		}
	}
	QString text = tr2i18n( "%1 was logged out at %2.").arg( nickname ).arg( CreateTimeString( time( NULL ) ) );
	m_MessageLabel->setText(text);
	resize( sizeHint().expandedTo( minimumSizeHint() ) );
	m_ActionButton->setEnabled(FALSE);
	_host = host;
	type = LOGOUT;
}

/**
 * 不在モード変更メッセージ設定
 * @param host ホスト
 */
void KIpMsgNotifier::setAbsenceModeChangeMessage( HostListItem &host )
{
	QString nickname = host.Nickname().c_str();
	if ( host.EncodingName() != "" ){
		QTextCodec *codec = QTextCodec::codecForName( host.EncodingName().c_str() );
		if ( codec != NULL ){
			nickname = codec->toUnicode( nickname );
		}
	}
	QString text = tr2i18n( "%1 was absence mode in at %2.").arg( nickname ).arg( CreateTimeString( time( NULL ) ) );
	m_MessageLabel->setText(text);
	resize( sizeHint().expandedTo( minimumSizeHint() ) );
	m_ActionButton->setEnabled(FALSE);
	_host = host;
	type = ABSENCE_CHANGE;
}

/**
 * ボタン押下イベント
 */
void KIpMsgNotifier::slotCommandButtonClick()
{
	if ( type == LOGIN ) {
		SendDialog *send = new SendDialog();
		//ホストリストを選択。
		QString IpAddr = _host.IpAddress().c_str();
		QListViewItemIterator its( send->m_HostListView );
		while ( its.current() != NULL ) {
			QListViewItem *item = its.current();
			item->setSelected( FALSE );
			if ( item->text( SendDialog::ColumnIpAddress ) == IpAddr ) {
				send->m_HostListView->setSelected( item, TRUE );
				break;
			}
			++its;
		}
	} else 	if ( type == RECV_MSG ) {
		IpMessengerAgent *agent = IpMessengerAgent::GetInstance();
		KIpMsgEvent *evt = dynamic_cast<KIpMsgEvent *>(agent->GetEventObject());
		if ( evt != NULL ) {
			evt->FindAndShowHiddenRecieveMsg( _msg );
		}
		//次のタイムアウトで消させる。
		createdTime -= NOTIFY_ERASE_SEC;
	}
}

#include "notifywidget.moc"
