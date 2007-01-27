/****************************************************************************
** Form interface generated from reading ui file './notifywidget.ui'
**
** Created: 水  1月 24 15:08:13 2007
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef NOTIFYWIDGET_H
#define NOTIFYWIDGET_H

#include "notifywidgetbase.h"
#include "IpMessenger.h"

using namespace ipmsg;

class KIpMsgNotifier : public KIpMsgNotifierBase
{
    Q_OBJECT

public:
    KIpMsgNotifier( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KIpMsgNotifier();
	void setRecievedMessage( RecievedMessage &msg );
	void setLoginMessage( HostListItem &host );
	void setLogoutMessage( HostListItem &host );
	void setAbsenceModeChangeMessage( HostListItem &host );
	time_t createdTime;
	HostListItem Host(){return _host;};

public slots:
    virtual void slotCommandButtonClick();

protected:

protected slots:

private:
	int type;
	RecievedMessage _msg;
	HostListItem _host;
};

#define NOTIFY_ERASE_SEC 5

#endif // NOTIFYWIDGET_H
