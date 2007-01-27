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


#include "kipmsgnotify.h"
#include <kuniqueapplication.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kpanelextension.h>
#include <qtimer.h>

QTimer *MessageTimer;

#define TIMER_INTERVAL 500

#define KICKER_HEIGHT_TINY 24
#define KICKER_HEIGHT_SMALL 30
#define KICKER_HEIGHT_NORMAL 46
#define KICKER_HEIGHT_LARGE 58

/**
 * コンストラクタ
 * ・タイマーを生成。
 * ・レイアウトを生成。
 * @param parent 親ウィジェット
 * @param name 名前
 * @param fl フラグ
 */
KIpMsgNotify::KIpMsgNotify( QWidget* parent, const char* name, WFlags fl )
        : KIpMsgNotifyBase(parent,name,fl)
{
	MessageTimer = new QTimer( this );
	connect( MessageTimer, SIGNAL( timeout() ), this, SLOT( slotTimeout() ) );
	frameLayout = new QVBoxLayout( m_NotifyFrame, 2, 0 );
}

/**
 * デストラクタ
 * ・タイマーを止める。
 */
KIpMsgNotify::~KIpMsgNotify()
{
	MessageTimer->stop();
}

/**
 * 受信メッセージ追加
 * ・ウィジェットを生成。
 * ・レイアウトに追加。
 * @param msg 受信メッセージ
 */
void KIpMsgNotify::addRecievedMessage( RecievedMessage &msg )
{
	KIpMsgNotifier *item = new KIpMsgNotifier( (QWidget*)m_NotifyFrame, 0, 0 );
	item->setRecievedMessage( msg );
	item->setMinimumSize( item->sizeHint() );
	showItems.push_back( item );
	frameLayout->addWidget( item );
	item->show();
	adjustPosition();
}

/**
 * ログインメッセージ追加
 * ・ウィジェットを生成。
 * ・レイアウトに追加。
 * @param host ホスト情報
 */
void KIpMsgNotify::addLoginMessage( HostListItem &host )
{
	std::vector<KIpMsgNotifier *>::iterator it = showItems.begin();
	while( it != showItems.end() ){
		KIpMsgNotifier *notifier = *it;
		if ( notifier->Host().IpAddress() == host.IpAddress() ) {
			return;
		}
		++it;
	}
	KIpMsgNotifier *item = new KIpMsgNotifier( (QWidget*)m_NotifyFrame, 0, 0 );
	item->setLoginMessage( host );
	item->setMinimumSize( item->sizeHint() );
	showItems.push_back( item );
	frameLayout->addWidget( item );
	item->show();
	adjustPosition();
}

/**
 * ログアウトメッセージ追加
 * ・ウィジェットを生成。
 * ・レイアウトに追加。
 * @param host ホスト情報
 */
void KIpMsgNotify::addLogoutMessage( HostListItem &host )
{
	std::vector<KIpMsgNotifier *>::iterator it = showItems.begin();
	while( it != showItems.end() ){
		KIpMsgNotifier *notifier = *it;
		if ( notifier->Host().IpAddress() == host.IpAddress() ) {
			return;
		}
		++it;
	}
	KIpMsgNotifier *item = new KIpMsgNotifier( (QWidget*)m_NotifyFrame, 0, 0 );
	item->setLogoutMessage( host );
	item->setMinimumSize( item->sizeHint() );
	showItems.push_back( item );
	frameLayout->addWidget( item );
	item->show();
	adjustPosition();
}

/**
 * 不在変更メッセージ追加
 * ・ウィジェットを生成。
 * ・レイアウトに追加。
 * @param host ホスト情報
 */
void KIpMsgNotify::addAbsenceModeChangeMessage( HostListItem &host )
{
	std::vector<KIpMsgNotifier *>::iterator it = showItems.begin();
	while( it != showItems.end() ){
		KIpMsgNotifier *notifier = *it;
		if ( notifier->Host().IpAddress() == host.IpAddress() &&
			 notifier->Host().Nickname() == host.Nickname() ) {
			return;
		}
		++it;
	}
	KIpMsgNotifier *item = new KIpMsgNotifier( (QWidget*)m_NotifyFrame, 0, 0 );
	item->setAbsenceModeChangeMessage( host );
	item->setMinimumSize( item->sizeHint() );
	showItems.push_back( item );
	frameLayout->addWidget( item );
	item->show();
	adjustPosition();
}

/**
 * 位置調整
 */
void KIpMsgNotify::adjustPosition()
{
	///////////////////////////////////////
	//ウィジェットのサイズを決めて、
	///////////////////////////////////////

	static unsigned int prev_count = 0;

	if ( showItems.size() != prev_count ) {
		//サイズを確定。
		setMinimumSize( QSize(10, 10) );
		resize( QSize(10, 10) );

		kapp->processEvents();

		setMinimumSize( sizeHint() );
		resize( QSize(10, 10).expandedTo( sizeHint() ) );
	}
	prev_count = showItems.size();

	///////////////////////////////////////
	//位置を移動
	///////////////////////////////////////
	//デスクトップの大きさ
	QRect r = KGlobalSettings::splashScreenDesktopGeometry();

	//Kickerの設定ファイルからPanelの位置／高さ
	KStandardDirs *dir = KGlobal::dirs();
	QString kickerrc = dir->findResource( "config", "kickerrc" );

	KConfig *kicker = new KConfig(kickerrc, true );
	kicker->setGroup("General");

	int panelSize = kicker->readNumEntry("CustomSize");
	int size = kicker->readNumEntry("Size");
	int pos = kicker->readNumEntry("Position");
	delete kicker;

	//パネルの高さを確定。
	switch( size ) {
		case KPanelExtension::SizeTiny:
			panelSize = KICKER_HEIGHT_TINY; break;
		case KPanelExtension::SizeSmall:
			panelSize = KICKER_HEIGHT_SMALL; break;
		case KPanelExtension::SizeNormal:
			panelSize = KICKER_HEIGHT_NORMAL; break;
		case KPanelExtension::SizeLarge:
			panelSize = KICKER_HEIGHT_LARGE; break;
	}

	//そして適切な位置に移動。
	if ( pos == KPanelExtension::Left ){
		move( panelSize, r.height() - height() );
	} else if ( pos == KPanelExtension::Right ) {
		move( r.width() - panelSize - width(), r.height() - height() );
	} else if ( pos == KPanelExtension::Top ) {
		move( r.width() - width(), panelSize );
	} else if ( pos == KPanelExtension::Bottom ) {
		move( r.width() - width(), r.height() - panelSize - height() );
	}

	if ( !MessageTimer->isActive() ){
		MessageTimer->start( TIMER_INTERVAL, FALSE );
	} else {
		if ( showItems.empty() ){
			MessageTimer->stop();
			hide();
		}
	}
}

/**
 * タイムアウトイベント
 */
void KIpMsgNotify::slotTimeout()
{
	time_t now = time( NULL );
	std::vector<KIpMsgNotifier *>::iterator it = showItems.begin();
	while( it != showItems.end() ){
		KIpMsgNotifier *item = *it;
		if ( item->createdTime + NOTIFY_ERASE_SEC < now ) {
			item->hide();
			frameLayout->remove( item );
			item->close();
			it = showItems.erase( it );
		} else {
			++it;
		}
	}
	adjustPosition();
}

#include "kipmsgnotify.moc"
