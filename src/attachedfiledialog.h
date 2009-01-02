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


#ifndef _ATTACHEDFILEDIALOG_H_
#define _ATTACHEDFILEDIALOG_H_

#include "attachedfiledialogbase.h"
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;


class KIpMessengerAttachedFileDialog : public KIpMessengerAttachedFileDialogBase
{
    Q_OBJECT

public:
    KIpMessengerAttachedFileDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KIpMessengerAttachedFileDialog();
    /*$PUBLIC_FUNCTIONS$*/
	void setFiles( AttachFileList _files );
	AttachFileList getFiles();

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotCancelClicked();
    virtual void slotOkClicked();
    virtual void slotDeleteClicked();
    virtual void slotAddFilesClicked();
    virtual void slotAddDirectoriesClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	AttachFileList files;
	void setFileNames();

};

#endif

