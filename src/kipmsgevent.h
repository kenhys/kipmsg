/***************************************************************************
 *   Copyright (C) 2006-2009 by nikikuni                                        *
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


#ifndef _KIPMSGEVENT_H_
#define _KIPMSGEVENT_H_

#include <qptrlist.h>
#include <vector>
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

#include "kipmsgsettings.h"
#include "recievedialog.h"
#include "openconfirm.h"
#include "kipmsgnotify.h"
#include "notifywidget.h"
class SendDialog;

class KIpMsgEvent: public IpMessengerEvent {
	public:
		enum Event{
			Event_TriggerIsNotEvent=-1,
			Event_BrEntryAfter=1,
			Event_AnsEntryAfter,
			Event_BrExitAfter,
			Event_BrAbsenceAfter,
			Event_AnsPubKeyAfter,
			Event_RefreshHostListAfter,
			Event_UpdateHostListAfter,
			Event_Max=99
		};
		virtual int EventStartProcessThread();
		virtual void EventBefore();
		virtual void EventAfter();
		virtual void RefreshHostListAfter( HostList& hostList );
		virtual void UpdateHostListAfter( HostList& hostList );
		virtual bool GetHostListRetryError();
		virtual bool RecieveAfter( RecievedMessage& msg );
		virtual void SendAfter( SentMessage& msg );
		virtual void NotifySendEncryptionFail( HostListItem& host );
		virtual bool IsSendContinueOnEncryptionFail( HostListItem& host );
		virtual bool SendRetryError( SentMessage& msg );
		virtual void OpenAfter( SentMessage& msg );
		virtual void DownloadStart( RecievedMessage& msg, AttachFile& file, DownloadInfo& info, void *data );
		virtual void DownloadProcessing( RecievedMessage& msg, AttachFile& file, DownloadInfo& info, void *data );
		virtual void DownloadEnd( RecievedMessage& msg, AttachFile& file, DownloadInfo& info, void *data );
		virtual bool DownloadError( RecievedMessage& msg, AttachFile& file, DownloadInfo& info, void *data );
		virtual void EntryAfter( HostListItem& host );
		virtual void ExitAfter( HostListItem& host );
		virtual void AbsenceModeChangeAfter( HostListItem& host );
		virtual void VersionInfoRecieveAfter( HostListItem &host, string version );
		virtual void AbsenceDetailRecieveAfter( HostListItem &host, string absenceDetail );
		virtual void EventBrEntryAfter( HostListItem& host );
		virtual void EventAnsEntryAfter( HostListItem& host );
		virtual void EventBrExitAfter( HostListItem& host );
		virtual void EventBrAbsenceAfter( HostListItem& host );
		virtual void EventAnsPubKeyAfter( HostListItem& host );

		SendDialog * ShowSendDlg();
		void ShowHiddenRecieveMsg();
		void FindAndShowHiddenRecieveMsg( RecievedMessage& msg );

		void HideAllOpenConfirm();
		void TimerEvent();
		int GetRecievedMessageCount();
		void StayOnTopAllWindows();
		QPtrList<RecieveDialog>& GetRecieveDialogs();
		QPtrList<SendDialog>& GetSendDialogs();
		static string CreateHostInfoString(HostListItem host);
		static void GetHostEncodingFromConfig( HostListItem &host );
		void RefreshHostListInAllSendDlg(KIpMsgEvent::Event event);
		void UpdateHostInAllSendDlg(KIpMsgEvent::Event event, HostListItem &host );
		void DeleteHostInAllSendDlg(KIpMsgEvent::Event event, HostListItem &host );
	private:
		void ShowRecieveMsg( RecievedMessage& msg );
		QPtrList<SendDialog> sendDialogs;
		QPtrList<RecieveDialog> recieveDialogs;
		QPtrList<OpenConfirmDialog> confirmDialogs;
		vector<RecievedMessage> hiddenMessages;
		KIpMsgNotify *createNotifyWindow();
};

#endif
