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


#ifndef _DOWNLOAD_COMPLETE_DIALOG_H_
#define _DOWNLOAD_COMPLETE_DIALOG_H_

#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

#include "downloadcompletedialogbase.h"

class DownloadCompleteDialog : public DownloadCompleteDialogBase
{
    Q_OBJECT

public:
    DownloadCompleteDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DownloadCompleteDialog();
    /*$PUBLIC_FUNCTIONS$*/
	void setDownloadInfo( DownloadInfo info );

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotOpenAsClicked();
    virtual void slotCloseClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	DownloadInfo Info;
};

#endif
