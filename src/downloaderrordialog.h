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


#ifndef _DOWNLOAD_ERROR_DIALOG_H_
#define _DOWNLOAD_ERROR_DIALOG_H_

#include "IpMessenger.h"
#include "ui_downloaderrordialogbase.h"

class DownloadErrorDialog : public KDialog, private Ui::DownloadErrorDialogBase
{
    Q_OBJECT

public:
    DownloadErrorDialog(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );
    ~DownloadErrorDialog();
    /*$PUBLIC_FUNCTIONS$*/
	void setDownloadInfo( DownloadInfo info );

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotRetryClicked();
    virtual void slotCancelClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	DownloadInfo Info;
};

#endif
