/***************************************************************************
 *   Copyright (C) 2006-2010 by nikikuni                                   *
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


#ifndef _IDIOM_CONFIG_H_
#define _IDIOM_CONFIG_H_

#include "ui_idiomconfigbase.h"
#include "qstringlist.h"

class IdiomConfigDialog : public KDialog, private Ui::IdiomConfigDialogBase
{
    Q_OBJECT

public:
    IdiomConfigDialog(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );
    ~IdiomConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotOKClicked();
    virtual void slotCancelClicked();
    virtual void slotAddIdiomClicked();
    virtual void slotDeleteIdiomClicked();
protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	QStringList idioms;
	QString getListItem( QString idiom );
};

#endif

