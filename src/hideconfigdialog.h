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


#ifndef _HIDECONFIGDIALOG_H_
#define _HIDECONFIGDIALOG_H_

#include "hideconfigdialogbase.h"

class HideConfigDialog : public HideConfigDialogBase
{
    Q_OBJECT

public:
    HideConfigDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~HideConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/
	static void SetupHideHosts();

public slots:
    /*$PUBLIC_SLOTS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/
    virtual void slotAddHideAddressClicked();
    virtual void slotDeleteHideAddressClicked();
    virtual void slotOKClicked();
    virtual void slotCancelClicked();
    virtual void slotApplyClicked();
    virtual void slotEnableSpecifiedHostClicked();
    virtual void slotCopyIPAddressClicked();
    virtual void slotIPAddressListViewSelectionChanged();

protected slots:
    /*$PROTECTED_SLOTS$*/

};

#endif

