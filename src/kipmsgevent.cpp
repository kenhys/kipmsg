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
#include "kipmsgevent.h"
#include "kipmsgwidget.h"
#include "kipmsgsettings.h"
#include "kipmsglogger.h"

void
KIpMsgEvent::UpdateHostListAfter( HostList& hostList ){
	printf("UpdateHostListAfter KIPMSG\n");
}
void
KIpMsgEvent::GetHostListRetryError(){
	printf("GetHostListRetryError KIPMSG\n");
}
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
void
KIpMsgEvent::ShowHiddenRecieveMsg(){
	for( vector<RecievedMessage>::iterator ix = hiddenMessages.begin(); ix != hiddenMessages.end(); ix++ ){
		ShowRecieveMsg( *ix );
	}
	hiddenMessages.clear();
}
bool
KIpMsgEvent::RecieveAfter( RecievedMessage& msg ){
	if ( !kipmsgWidget::isRecievedOnNonePopup() ) {
		ShowRecieveMsg( msg );
	} else {
		hiddenMessages.push_back( msg );
	}
	return true;
}

QPtrList<RecieveDialog>&
KIpMsgEvent::GetRecieveDialogs(){
	return recieveDialogs;
}

QPtrList<SendDialog>&
KIpMsgEvent::GetSendDialogs(){
	return sendDialogs;
}

void
KIpMsgEvent::ShowSendDlg(){
	SendDialog *sendWin = new SendDialog();
	sendWin->show();
	sendDialogs.append( sendWin );
}

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

int
KIpMsgEvent::GetRecievedMessageCount(){
	return hiddenMessages.size();
}

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

void
KIpMsgEvent::SendAfter( SentMessage& msg ){
	printf("SendAfter KIPMSG\n");
}

void
KIpMsgEvent::SendRetryError( SentMessage& msg ){
	printf("SendRetryError KIPMSG\n");
}
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
void
KIpMsgEvent::DownloadStart( SentMessage& msg, AttachFile& file ){
	printf("DownloadStart KIPMSG\n");
}
void
KIpMsgEvent::DownloadProcessing( SentMessage& msg, AttachFile& file ){
	printf("DownloadProcessing KIPMSG\n");
}
void
KIpMsgEvent::DownloadEnd( SentMessage& msg, AttachFile& file ){
	printf("DownloadEnd KIPMSG\n");
}
void
KIpMsgEvent::DownloadError( SentMessage& msg, AttachFile& file ){
	printf("DownloadError KIPMSG\n");
}
void
KIpMsgEvent::EntryAfter( HostList& hostList ){
	printf("EntryAfter KIPMSG\n");
}
void
KIpMsgEvent::ExitAfter( HostList& hostList ){
	printf("ExitAfter KIPMSG\n");
}

