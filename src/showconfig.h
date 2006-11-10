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


#ifndef _SHOWCONFIG_H_
#define _SHOWCONFIG_H_

#include "showconfigbase.h"

class SendDialog;

class KIPMsgShowConfigDialog : public KIPMsgShowConfigDialogBase
{
    Q_OBJECT

public:
    KIPMsgShowConfigDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KIPMsgShowConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/
	void setSendDialog( SendDialog *_senddialog ){ senddialog = _senddialog; };

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotOkClicked();
    virtual void slotCancelClicked();
    virtual void slotApplyClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	SendDialog *senddialog;

};

#endif

