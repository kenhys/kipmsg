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
#include "kipmsglogger.h"
#include "kipmsgsettings.h"

static KIpMessengerLogger *_instance = NULL;

/**
 * ロガーインスタンス取得
 * ・ロガーのインスタンスがまだ生成されていなければ生成して取得
 * @retval ロガーのインスタンス
 */
KIpMessengerLogger *KIpMessengerLogger::GetInstance()
{
	if ( _instance == NULL ){
		_instance = new KIpMessengerLogger();
	}
	return _instance;
}

/**
 * 受信メッセージをログに書き込む
 * ・設定でログ記録が有効ならUTF-8でログを出力
 * @param msg 受信メッセージ
 * @param encodingChanged エンコーディングが変更された
 */
void KIpMessengerLogger::PutRecivedMessage( RecievedMessage msg, bool encodingChanged )
{
	if ( !KIpMsgSettings::log() ){
		return;
	}
	if ( msg.IsNoLogging() ){
		return;
	}
	/* ローカルファイルシステムのエンコーディング */
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	/* メッセージのエンコーディング */
	QStringList encodings = KIpMsgSettings::encodingSettings();
	QString IpAddr = msg.Host().IpAddress().c_str();
	QString UserName = msg.Host().UserName().c_str();
	QString encoding = KIpMsgSettings::messageEncoding();
	for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
		QStringList fields = QStringList::split( ":", *ite );
		if ( IpAddr == fields[0] && UserName == fields[1] ) {
			encoding = fields[2];
			break;
		}
	}
	QTextCodec *msgCodec = QTextCodec::codecForName( encoding );
	/* ログのエンコーディング */
	QTextCodec *logCodec = QTextCodec::codecForName( "UTF-8" );
	FILE *fp = fopen( fsCodec->fromUnicode( KIpMsgSettings::logFileName() ).data(), "a" );
	if ( fp == NULL ){
		return;
	}
	fprintf( fp, "=====================================\n" );
	QString title = " From: " + logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().Nickname().c_str() ) );
	if ( KIpMsgSettings::recordLogin() ){
		if ( msg.Host().UserName() != "" ){
			title += QString( "[%1]" ).arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().UserName().c_str() ) ) );
		} else {
			title += QString( "[]" );
		}
	}
	QString optionalTitle = "";
	if ( msg.Host().GroupName() == "" && msg.Host().HostName() == "" ){
		optionalTitle = "";
	} else if ( msg.Host().GroupName() != "" && msg.Host().HostName() == "" ){
		optionalTitle = QString( "(%1" ).arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().GroupName().c_str() ) ) );
	} else if ( msg.Host().GroupName() == "" && msg.Host().HostName() != "" ){
		optionalTitle = QString( "(%1" ).arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().HostName().c_str() ) ) );
	} else if ( msg.Host().GroupName() != "" && msg.Host().HostName() != "" ){
		optionalTitle = QString( "(%1/%2" )
				.arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().GroupName().c_str() ) ) )
				.arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().HostName().c_str() ) ) ) ;
	}
	if ( KIpMsgSettings::recordIPAddress() ){
		if ( optionalTitle == "" ){
			optionalTitle = "(";
		} else {
			optionalTitle += "/";
		}
		optionalTitle = optionalTitle + QString( msg.Host().IpAddress().c_str() ) + ")";
	}
	title = title + optionalTitle;
	fprintf( fp, "%s\n", title.data() );

	char tmpbuf[100];
	time_t recieved = msg.Recieved();
	ctime_r( &recieved, tmpbuf );
	tmpbuf[ strlen( tmpbuf ) - 1] = '\0';

	fprintf( fp, "  at %s ", tmpbuf );
	if ( msg.IsCrypted() ){
		fprintf( fp, "(RSA)" );
	}
	if ( msg.IsPasswordLock() ){
		fprintf( fp, "%s", logCodec->fromUnicode( tr2i18n( "(Lock)" ) ).data() );
	} else {
		if ( msg.IsSecret() ){
			fprintf( fp, "%s", logCodec->fromUnicode( tr2i18n( "(Secret)" ) ).data() );
		}
	}
	if ( encodingChanged ){
		fprintf( fp, "%s", logCodec->fromUnicode( tr2i18n( "(Encoding Changed)" ) ).data() );
	}
	fprintf( fp, "\n" );

	string files = "";
	for( vector<AttachFile>::iterator ix = msg.Files().begin(); ix != msg.Files().end(); ix++ ) {
		if ( !files.empty() ){
			files += ", ";
		}
		files += logCodec->fromUnicode( msgCodec->toUnicode( ix->FileName().c_str() ) ).data();
	}
	if ( !files.empty() ){
		fprintf( fp, "  %s %s\n", logCodec->fromUnicode( tr2i18n( "(Attach File)" ) ).data(), files.c_str() );
	}
	fprintf( fp, "-------------------------------------\n" );
	fprintf( fp, "%s", logCodec->fromUnicode( msgCodec->toUnicode( msg.Message().c_str() ) ).data() );

	fprintf( fp, "\n\n" );
	fclose( fp );
}

/**
 * 送信メッセージをログに書き込む
 * ・設定でログ記録が有効ならUTF-8でログを出力
 * @param msg 送信メッセージ
 */
void KIpMessengerLogger::PutSentMessage( SentMessage msg )
{
	if ( !KIpMsgSettings::log() ){
		return;
	}
	if ( msg.IsNoLogging() ){
		return;
	}
	/* ローカルファイルシステムのエンコーディング */
	QTextCodec *fsCodec = QTextCodec::codecForName( KIpMsgSettings::localFilesystemEncoding() );
	/* メッセージのエンコーディング */
	QStringList encodings = KIpMsgSettings::encodingSettings();
	QString IpAddr = msg.Host().IpAddress().c_str();
	QString UserName = msg.Host().UserName().c_str();
	QString encoding = KIpMsgSettings::messageEncoding();
	for( QStringList::iterator ite = encodings.begin(); ite != encodings.end(); ite++ ){
		QStringList fields = QStringList::split( ":", *ite );
		if ( IpAddr == fields[0] && UserName == fields[1] ) {
			encoding = fields[2];
			break;
		}
	}
	QTextCodec *msgCodec = QTextCodec::codecForName( encoding );
	/* ログのエンコーディング */
	QTextCodec *logCodec = QTextCodec::codecForName( "UTF-8" );
	FILE *fp = fopen( fsCodec->fromUnicode( KIpMsgSettings::logFileName() ).data(), "a" );
	if ( fp == NULL ){
		return;
	}
	fprintf( fp, "=====================================\n" );
	QString title = " To: " + logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().Nickname().c_str() ) );
	if ( KIpMsgSettings::recordLogin() ){
		if ( msg.Host().UserName() != "" ){
			title += QString( "[%1]" ).arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().UserName().c_str() ) ) );
		} else {
			title += QString( "[]" );
		}
	}
	QString optionalTitle = "";
	if ( msg.Host().GroupName() == "" && msg.Host().HostName() == "" ){
		optionalTitle = "";
	} else if ( msg.Host().GroupName() != "" && msg.Host().HostName() == "" ){
		optionalTitle = QString( "(%1" ).arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().GroupName().c_str() ) ) );
	} else if ( msg.Host().GroupName() == "" && msg.Host().HostName() != "" ){
		optionalTitle = QString( "(%1" ).arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().HostName().c_str() ) ) );
	} else if ( msg.Host().GroupName() != "" && msg.Host().HostName() != "" ){
		optionalTitle = QString( "(%1/%2" )
				.arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().GroupName().c_str() ) ) )
				.arg( logCodec->fromUnicode( msgCodec->toUnicode( msg.Host().HostName().c_str() ) ) ) ;
	}
	if ( KIpMsgSettings::recordIPAddress() ){
		if ( optionalTitle == "" ){
			optionalTitle = "(";
		} else {
			optionalTitle += "/";
		}
		optionalTitle = optionalTitle + QString( msg.Host().IpAddress().c_str() ) + ")";
	}
	title = title + optionalTitle;
	fprintf( fp, "%s\n", title.data() );

	char tmpbuf[100];
	time_t sent = msg.Sent();
	ctime_r( &sent, tmpbuf );
	tmpbuf[ strlen( tmpbuf ) - 1] = '\0';

	fprintf( fp, "  at %s ", tmpbuf );
	if ( msg.IsCrypted() ){
		fprintf( fp, "(RSA)" );
	}
	if ( msg.IsPasswordLock() ){
		fprintf( fp, "%s", logCodec->fromUnicode( tr2i18n( "(Lock)" ) ).data() );
	} else {
		if ( msg.IsSecret() ){
			fprintf( fp, "%s", logCodec->fromUnicode( tr2i18n( "(Secret)" ) ).data() );
		}
	}
	fprintf( fp, "\n" );

	string files = "";
	for( vector<AttachFile>::iterator ix = msg.Files().begin(); ix != msg.Files().end(); ix++ ) {
		if ( !files.empty() ){
			files += ", ";
		}
		files += logCodec->fromUnicode( fsCodec->toUnicode( ix->FileName().c_str() ) ).data();
	}
	if ( !files.empty() ){
		fprintf( fp, "  %s %s\n", logCodec->fromUnicode( tr2i18n( "(Attach File)" ) ).data(), files.c_str() );
	}
	fprintf( fp, "-------------------------------------\n" );
	fprintf( fp, "%s", logCodec->fromUnicode( msgCodec->toUnicode( msg.Message().c_str() ) ).data() );

	fprintf( fp, "\n\n" );
	fclose( fp );
}

/**
 * コンストラクタ
 * ・特にすること無し。
 */
KIpMessengerLogger::KIpMessengerLogger()
{
}
/**
 * デストラクタ
 * ・特にすること無し。
 */
KIpMessengerLogger::~KIpMessengerLogger()
{
}

