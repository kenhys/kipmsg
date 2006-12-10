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
#include "kipmsgsettings.h"
#include "recievedialog.h"
#include "openconfirm.h"

class KIpMsgEvent: public IpMessengerEvent {
	public:
		virtual void UpdateHostListAfter( HostList& hostList );
		virtual void GetHostListRetryError();
		virtual bool RecieveAfter( RecievedMessage& msg );
		virtual void SendAfter( SentMessage& msg );
		virtual void SendRetryError( SentMessage& msg );
		virtual void OpenAfter( SentMessage& msg );
		virtual void DownloadStart( SentMessage& msg, AttachFile& file );
		virtual void DownloadProcessing( SentMessage& msg, AttachFile& file );
		virtual void DownloadEnd( SentMessage& msg, AttachFile& file );
		virtual void DownloadError( SentMessage& msg, AttachFile& file );
		virtual void EntryAfter( HostList& hostList );
		virtual void ExitAfter( HostList& hostList );

		void ShowHiddenRecieveMsg();
		void TimerEvent();
		int GetRecievedMessageCount();
		void StayOnTopAllWindows();
		QPtrList<RecieveDialog>& GetRecieveDialogs();
	private:
		void ShowRecieveMsg( RecievedMessage& msg );
		QPtrList<RecieveDialog> recieveDialogs;
		QPtrList<OpenConfirmDialog> confirmDialogs;
		vector<RecievedMessage> hiddenMessages;
};

#endif
