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


#ifndef _ABSENCEMODE_H_
#define _ABSENCEMODE_H_

#include <qmap.h>
#include "absenceconfigbase.h"

class AbsenceSetting
{
public:
	QString title;
	QString detail;
};


class KIpMsgAbsenceModeConfigDialog : public KIpMsgAbsenceModeConfigDialogBase
{
    Q_OBJECT

public:
    KIpMsgAbsenceModeConfigDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KIpMsgAbsenceModeConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotConfigAndAbsenceModeClicked();
    virtual void slotConfigOnlyClicked();
    virtual void slotCancelClicked();
    virtual void slotSetClicked();
    virtual void slotAbsenceModeClicked(QListBoxItem*);
    virtual void slotAddClicked();
    virtual void slotDeleteClicked();
    virtual void slotEncodingChanged(int);
    virtual void slotAutoAbsenceClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	void setStatus();
	//第一キーがリストのアイテムの先頭の":"の前（通番）第二キーがエンコーディング。
	QMap< QString, QMap< QString, AbsenceSetting > > settings;
	QString currentKey;
	QString currentEncoding;
	void load();
	void save();
};

#endif

