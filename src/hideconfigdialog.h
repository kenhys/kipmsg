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


#ifndef _HIDECONFIGDIALOG_H_
#define _HIDECONFIGDIALOG_H_

#include "ui_hideconfigdialogbase.h"

class KIpMessengerHiddenConfigEntry {
public:
	KIpMessengerHiddenConfigEntry(QString ipAddress, QString hostName); 
	~KIpMessengerHiddenConfigEntry(){};
	QString ipAddress(){ return m_IpAddress; };
	QString hostName(){ return m_HostName; };

private:
	QString m_IpAddress;
	QString m_HostName;
};

class KIpMessengerHiddenConfigModel : public QAbstractListModel
{
	Q_OBJECT

public:
	KIpMessengerHiddenConfigModel( QObject *parent ) : QAbstractListModel( parent ){}; 
	~KIpMessengerHiddenConfigModel(){};
	void clear();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int selection, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addEntry(KIpMessengerHiddenConfigEntry entry);
	KIpMessengerHiddenConfigEntry getHiddenConfigEntry(const QModelIndex &index);
	void setHiddenConfigEntry(const QModelIndex &index, const KIpMessengerHiddenConfigEntry entry);

private:
	QList<KIpMessengerHiddenConfigEntry> hiddenConfigList;
};

class HideConfigDialog : public KDialog, private Ui::HideConfigDialogBase
{
    Q_OBJECT

public:
    HideConfigDialog(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );
    ~HideConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/
	static void SetupHideHosts();

public slots:
    /*$PUBLIC_SLOTS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    virtual void slotAddHideAddressClicked();
    virtual void slotDeleteHideAddressClicked();
    virtual void slotOKClicked();
    virtual void slotCancelClicked();
    virtual void slotApplyClicked();
    virtual void slotEnableSpecifiedHostClicked();
    virtual void slotCopyIPAddressClicked();
    virtual void slotIPAddressListViewSelectionChanged(QModelIndex &);
    /*$PROTECTED_SLOTS$*/

};

#endif

