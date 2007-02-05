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


#ifndef _NETWORK_CONFIG_H_
#define _NETWORK_CONFIG_H_

#include "IpMessenger.h"
#include "networkconfigbase.h"

class NetworkConfig : public NetworkConfigBase
{
    Q_OBJECT

public:
    NetworkConfig(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~NetworkConfig();
    /*$PUBLIC_FUNCTIONS$*/
	static std::vector<ipmsg::NetworkInterface> getSpecifyNics();

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotAddBroadcastAddressClicked();
    virtual void slotDeleteBroadcastAddressClicked();
    virtual void slotOkClicked();
    virtual void slotCancelClicked();
    virtual void slotApplyClicked();
    virtual void slotAddInterfaceClicked();
    virtual void slotDeleteInterfaceClicked();
    virtual void slotSetPortNoClicked();
    virtual void slotSpecifyNetworkInterfaceClicked();
    virtual void slotUseInterfaceClicked(QListBoxItem*);
    virtual void slotSetDefaultPortNoClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	void setNetworkInterfaceStatus();
};

#endif

