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


#ifndef _KIPMSGWIDGET_H_
#define _KIPMSGWIDGET_H_

#include "IpMessenger.h"
#include <vector>
#include <qintdict.h>
#include <qptrlist.h>
#include "kipmsgwidgetbase.h"
#include "openconfirm.h"
#include "downloadmonitor.h"

class SendDialog;
class RecieveDialog;

class kipmsgWidget : public kipmsgWidgetBase
{
    Q_OBJECT

public:
    kipmsgWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~kipmsgWidget();
	static bool isRecievedOnNonePopup();
	static bool popupRecieve();
	static void playSound();
    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotConfigureClicked();
    virtual void slotDownloadMonitorClicked();
    virtual void slotHideAllOpenConfirmClicked();
    virtual void slotStayOnTopAllWindowsClick();
    virtual void slotAboutClicked();
    virtual void slotViewLogClicked();
    virtual void slotExitClicked();
    virtual void slotAbsenceModeConfigClicked();
    virtual void slotPollingTimeout();
    virtual void slotResetAbsenceModeClicked();
    virtual void slotAbsenceModeSelect( int );

protected:
    /*$PROTECTED_FUNCTIONS$*/
	virtual void mouseDoubleClickEvent (QMouseEvent *e);
	virtual void mousePressEvent (QMouseEvent *e);

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	KPopupMenu *MainPopup;
	KPopupMenu *AbsencePopup;
	QTimer *PollingTimer;
	QPixmap _OrigEnableIconImage;
	QPixmap _OrigDisableIconImage;
	IpMessengerAgent *IpMsgAgent;
	KIpMsgDownloadMonitor *downMonitor;
	QIntDict<QString> absence_mode_menu;
	QString currentAbsenceMode;
	int absence_mode_config_menu_item;
	int prevUserSize;
	QPtrList<OpenConfirmDialog> confirmDialogs;

	void rebuildMenu();
	void loadIcon();

};

#endif

