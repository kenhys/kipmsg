/****************************************************************************
** Form interface generated from reading ui file './notifywidget.ui'
**
** Created: 水  1月 24 15:08:13 2007
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KIPMSGNOTIFY_H
#define KIPMSGNOTIFY_H

#include "kipmsgnotifybase.h"
#include "IpMessenger.h"
#include "notifywidget.h"
#include <qlayout.h>
#include <vector>

class KIpMsgNotify : public KIpMsgNotifyBase
{
    Q_OBJECT

public:
    KIpMsgNotify( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KIpMsgNotify();

	void addRecievedMessage( RecievedMessage &msg );
	void addLoginMessage( HostListItem &host );
	void addLogoutMessage( HostListItem &host );
	void addAbsenceModeChangeMessage( HostListItem &host );
protected:

protected slots:
	void slotTimeout();

private:
	std::vector<KIpMsgNotifier *> showItems;
	QVBoxLayout *baseLayout;
	QVBoxLayout *frameLayout;
	void adjustPosition();
};

#endif // KIPMSGNOTIFY_H
