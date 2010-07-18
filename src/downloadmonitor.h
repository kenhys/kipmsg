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


#ifndef _DOWNLOADMONITOR_H_
#define _DOWNLOADMONITOR_H_

#include <qabstractitemmodel.h>
#include <qstring.h>
#include <qtimer.h>
#include "ui_downloadmonitorbase.h"

class KIpMessengerDownloadMonitorEntry {
public:
	KIpMessengerDownloadMonitorEntry(QString seqNumber, QString fileName, QString size, QString all, QString done, QString trans, QString userName, QString packetNo);
	~KIpMessengerDownloadMonitorEntry(){};
	QString seqNumber(){ return m_SeqNumber; };
	QString fileName(){ return m_FileName; };
	QString size(){ return m_Size; };
	QString all(){ return m_All; };
	QString done(){ return m_Done; };
	QString trans(){ return m_Trans; };
	QString userName(){ return m_UserName; };
	QString packetNo(){return m_PacketNo;};
	QPixmap icon(){return m_Icon;};
	void setIcon(QPixmap icon){m_Icon = icon;};

private:
	QString m_SeqNumber;
	QString m_FileName;
	QString m_Size;
	QString m_All;
	QString m_Done;
	QString m_Trans;
	QString m_UserName;
	QString m_PacketNo;
	QPixmap m_Icon;
};

class KIpMessengerDownloadMonitorModel : public QAbstractListModel
{
	Q_OBJECT

public:
	KIpMessengerDownloadMonitorModel( QObject *parent ) : QAbstractListModel( parent ){}; 
	~KIpMessengerDownloadMonitorModel(){};
	void clear();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int selection, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addEntry(KIpMessengerDownloadMonitorEntry entry);
	KIpMessengerDownloadMonitorEntry getDownloadEntry(const QModelIndex &index);

private:
	QList<KIpMessengerDownloadMonitorEntry> downloadList;
};

class KIpMsgDownloadMonitor : public KDialog, private Ui::KIpMsgDownloadMonitorBase
{
    Q_OBJECT

public:
    KIpMsgDownloadMonitor(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );
    ~KIpMsgDownloadMonitor();
    /*$PUBLIC_FUNCTIONS$*/
	QTimer *PollingTimer;
	void refreshDownloadFileList();

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotDeleteClicked();
    virtual void slotCloseClicked();
    virtual void slotPollingTimeout();
    virtual void slotNotPermitedIfModifiedCheckboxClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

};

#endif

