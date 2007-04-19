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


#ifndef _KIPMSGEVENT_H_
#define _KIPMSGEVENT_H_

#include <qptrlist.h>
#include <vector>
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

#include "kipmsgsettings.h"
#include "recievedialog.h"
#include "senddialog.h"
#include "openconfirm.h"
#include "kipmsgnotify.h"
#include "notifywidget.h"

class KIpMsgEvent: public IpMessengerEvent {
	public:
		virtual void RefreashHostListAfter( HostList& hostList );
		virtual void UpdateHostListAfter( HostList& hostList );
		virtual bool GetHostListRetryError();
		virtual bool RecieveAfter( RecievedMessage& msg );
		virtual void SendAfter( SentMessage& msg );
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
		void RefreshHostListInAllSendDlg();
	private:
		void ShowRecieveMsg( RecievedMessage& msg );
		QPtrList<SendDialog> sendDialogs;
		QPtrList<RecieveDialog> recieveDialogs;
		QPtrList<OpenConfirmDialog> confirmDialogs;
		vector<RecievedMessage> hiddenMessages;
		KIpMsgNotify *createNotifyWindow();
};

#endif
