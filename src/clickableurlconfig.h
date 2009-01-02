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


#ifndef _CLICKABLEURLCONFIG_H_
#define _CLICKABLEURLCONFIG_H_

#include "clickableurlconfigbase.h"

class KIPMsgClickableURLConfigDialog : public KIPMsgClickableURLConfigDialogBase
{
    Q_OBJECT

public:
    KIPMsgClickableURLConfigDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KIPMsgClickableURLConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/
	QMap<QString,QString> protocols;

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotBrowseClicked();
    virtual void slotOkClicked();
    virtual void slotCancelClicked();
    virtual void slotApplyClicked();
    virtual void slotProtocolClicked(QListBoxItem*);
    virtual void slotProgramNameChanged(const QString&);

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

};

#endif

