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


#ifndef _ENCODINGCONFIG_H_
#define _ENCODINGCONFIG_H_

#include <qabstractitemmodel.h>
#include <qstring.h>
#include "ui_encodingconfigbase.h"

class KIpMessengerEncodingConfigEntry {
public:
	KIpMessengerEncodingConfigEntry(QString nickName, QString groupName, QString hostName, QString ipAddress, QString userName, QString encodingName);
	~KIpMessengerEncodingConfigEntry(){};
	QString nickName(){ return m_NickName; };
	QString groupName(){ return m_GroupName; };
	QString hostName(){ return m_HostName; };
	QString ipAddress(){ return m_IpAddress; };
	QString userName(){ return m_UserName; };
	QString encodingName(){ return m_EncodingName; };
	void setNickName( QString value ){ m_NickName = value; };
	void setGroupName( QString value ){ m_GroupName = value; };
	void setEncodingName( QString value ){ m_EncodingName = value; };

private:
	QString m_NickName;
	QString m_GroupName;
	QString m_HostName;
	QString m_IpAddress;
	QString m_UserName;
	QString m_EncodingName;
};

class KIpMessengerEncodingConfigModel : public QAbstractListModel
{
	Q_OBJECT

public:
	KIpMessengerEncodingConfigModel( QObject *parent ) : QAbstractListModel( parent ){}; 
	~KIpMessengerEncodingConfigModel(){};
	void clear();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int selection, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addEntry(KIpMessengerEncodingConfigEntry entry);
	KIpMessengerEncodingConfigEntry getEncodingConfigEntry(const QModelIndex &index);
	void setEncodingConfigEntry(const QModelIndex &index, const KIpMessengerEncodingConfigEntry entry);

private:
	QList<KIpMessengerEncodingConfigEntry> encodingConfigList;
};

class KIPMsgEncodingConfigDialog : public KDialog, private Ui::KIPMsgEncodingConfigDialogBase
{
    Q_OBJECT

public:
    KIPMsgEncodingConfigDialog(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );
    ~KIPMsgEncodingConfigDialog();
    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotOkClicked();
    virtual void slotApplyClicked();
    virtual void slotCancelClicked();
//    virtual void slotHostSelected(QListViewItem*);
	virtual void slotCurrentHostChanged( QModelIndex &current, QModelIndex &previous );
    virtual void slotEncodingComboChanged(int);

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/
private:
	QString SeletedIpAddress;
	QString SeletedLoginUser;
    void refreshEncoding();

};

#endif

