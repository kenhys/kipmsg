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


#ifndef _LOGCONFIG_H_
#define _LOGCONFIG_H_

#include "logconfigbase.h"

class KIPMsgLogConfigDialog : public KIPMsgLogConfigDialogBase
{
    Q_OBJECT

public:
    KIPMsgLogConfigDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KIPMsgLogConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotLogFileClicked();
    virtual void slotOkClicked();
    virtual void slotCancelClicked();
    virtual void slotApplyClicked();
    virtual void slotUseExternalLogViewerClicked();
    virtual void slotLogViewerClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	void loadNormalIcon(QString iconFileName);
	void loadAbsenceIcon(QString iconFileName);

};

#endif

