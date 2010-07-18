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


#ifndef _ATTACHEDFILEDIALOG_H_
#define _ATTACHEDFILEDIALOG_H_

#include <qstring.h>
#include <qabstractitemmodel.h>
#include "ui_attachedfiledialogbase.h"
#include "IpMessenger.h"

using namespace std;
using namespace ipmsg;

class KIpMessengerAttachedFileEntry {
public:
	KIpMessengerAttachedFileEntry(QString fileName, QString size, QString mTime, QString fullPath);
	QString fileName(){ return m_FileName;};
	QString size(){ return m_Size;};
	QString mTime(){ return m_MTime;};
	QString fullPath(){ return m_FullPath;};

private:
	QString m_FileName;
	QString m_Size;
	QString m_MTime;
	QString m_FullPath;
};
class KIpMessengerAttachedFileModel : public QAbstractListModel
{
	Q_OBJECT

public:
	KIpMessengerAttachedFileModel( QObject *parent ) : QAbstractListModel( parent ){}; 
	~KIpMessengerAttachedFileModel(){};
	void clear();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int selection, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addEntry(KIpMessengerAttachedFileEntry entry);

	enum Column {
		fileNameCol=0,
		sizeCol,
		timeStampCol,
		fullPathCol,
		ColumnMax
	};

private:
	QList<KIpMessengerAttachedFileEntry> fileList;
};

class KIpMessengerAttachedFileDialog : public KDialog, private Ui::KIpMessengerAttachedFileDialogBase
{
    Q_OBJECT

public:
    KIpMessengerAttachedFileDialog(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );
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

