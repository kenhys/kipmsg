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


#ifndef _SENDDIALOG_H_
#define _SENDDIALOG_H_

#include <kpopupmenu.h>
#include <qintdict.h>
#include <qsize.h>
#include <qdialog.h>
#include <ktextedit.h>
#include "senddialogbase.h"
#include "IpMessenger.h"
#include "kipmsgevent.h"

using namespace std;
using namespace ipmsg;

class KipmsgPriorityHostItem;
class KIpMsgHostListViewItem;

class KIpMsgFileNameConverter : public FileNameConverter {
public:
	KIpMsgFileNameConverter();
	KIpMsgFileNameConverter( QString _encoding );
	virtual ~KIpMsgFileNameConverter(){};
	virtual string ConvertNetworkToLocal( string original_file_name );
	virtual string ConvertLocalToNetwork( string original_file_name );
private:
	QString encoding;
};

class KTextEditNoDnD : public KTextEdit
{
    Q_OBJECT
public:
	KTextEditNoDnD(QWidget *parent, const char *name);
	~KTextEditNoDnD();

public slots:
    /*$PUBLIC_SLOTS$*/
	virtual void slotAddAsText( void );
	virtual void slotAddAsFile( void );

protected:
	virtual void contentsDropEvent(QDropEvent *e);
	virtual void keyPressEvent ( QKeyEvent *e );

private:
	KPopupMenu *DnDPopup;
	QString dropText;
};

class SendDialog : public SendDialogBase
{
    Q_OBJECT

public:
    SendDialog(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~SendDialog();
    /*$PUBLIC_FUNCTIONS$*/
	void refreshHostList( KIpMsgEvent::Event event, bool isUpdate=false);
	void updateHostList( KIpMsgEvent::Event event, HostListItem *ix );
	void deleteHostList( KIpMsgEvent::Event event, HostListItem *ix );
	void setRecieveDialog( QDialog *_recvdialog ) { recvDialog = _recvdialog; };
	void setFixsizePotisionMenu();
	void setSaveSizeMenu();
	void synchronizeMenu();
	void setMessageText(QString text);
	void addDnDFiles(QString fileUrl);
	void refreshFiles();
	void saveColumnSizeGlobalSession();
	void rebuildDynamicMenu();

	enum {
		ColumnUser = 0,
		ColumnGroup,
		ColumnHost,
		ColumnIpAddress,
		ColumnLogin,
		ColumnPriority,
		ColumnEncoding,
		ColumnEncryptionCapacity,
		ColumnRsaMethod,
		ColumnRsaPublicKey,
		ColumnMax=99,
	};
			
public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotHostListUpdateClicked();
    virtual void slotMessageSendClicked();
    virtual void slotSecretClicked();
    virtual void slotShowHiddenTempClicked();
    virtual void slotRestoreAllClicked();
    virtual void slotSearchUserClicked();
    virtual void slotAttachFileClicked();
    virtual void slotAttachDirectoryClicked();
    virtual void slotFontSelectListClicked();
    virtual void slotFontSelectEditClicked();
    virtual void slotFontRestoreToDefaultClicked();
    virtual void slotSaveSizeClicked();
    virtual void slotRestoreSizeTempClicked();
    virtual void slotFixizePositionClicked();
    virtual void slotViewDetailConfigurationClicked();
    virtual void slotSaveListHeaderClicked();
    virtual void slotListContextMenuRequested( QListViewItem *, const QPoint &, int);
    virtual void slotGroupSelect(int);
    virtual void slotEncodingSelect(int);
    virtual void slotEncodingConfigClicked();
    virtual void slotPriorityMove(int);
    virtual void slotPrioritySelect(int);
    virtual void slotPriorityConfigClicked();
    virtual void slotAttacheFileListButtonClicked();
    virtual void slotGetVersionInfoClicked();
    virtual void slotGetAbsenceInfoClicked();
    virtual void slotIdiomButtonClicked();
    virtual void slotInsertSelectedIdiom(int);
    virtual void slotAddIdiomFromSelectedText();
    virtual void slotIdiomConfigClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/
	virtual void mousePressEvent( QMouseEvent *e );
	virtual void mouseReleaseEvent( QMouseEvent *e );
	virtual void mouseMoveEvent( QMouseEvent *e );
	virtual void keyPressEvent( QKeyEvent * e );
	virtual void dropEvent( QDropEvent *e );
	virtual void dragEnterEvent( QDragEnterEvent *e );
	virtual void resizeEvent( QResizeEvent *e );
	virtual void closeEvent( QCloseEvent * e );

protected slots:
    /*$PROTECTED_SLOTS$*/
	virtual void accept();
	virtual void reject();

private:
	bool isAccept;
	bool isShowHiddenTemp;
	KTextEditNoDnD *m_MessageEditbox;
	KPopupMenu *idiomPopup;
	KPopupMenu *idiomTextPopup;
	KPopupMenu *sendPopup;
	KPopupMenu *sortPopup;
	KPopupMenu *priorityPopup;
	KPopupMenu *groupPopup;
	KPopupMenu *encodingPopup;
	KPopupMenu *fontPopup;
	KPopupMenu *sizePopup;
	int showHiddenMenuId;
	int fixizePositionMenuId;
	int saveSizeMenuId;
	int getVersionInfoMenuId;
	int getAbsenceInfoMenuId;
	int addIdiomMenuId;
	QIntDict<QString> priorityHostMenuIdList;
	QIntDict<QString> priorityMenuIdList;
	QIntDict<QString> groupMenuIdList;
	QIntDict<QString> encodingMenuIdList;
	QIntDict<QString> idiomTextMenuIdList;
	AttachFileList attachFileList;
	bool isMainSplitterDragging;
	bool isDownloadSplitterDragging;
	QDialog *recvDialog;

	int defaultX;
	int defaultY;
	int defaultWidth;
	int defaultHeight;

	HostList	hosts;
	void setPriority( QString pri, QStringList &priList );
	void setMenuStatus();
	void doResize( QSize size );
	bool isGarbledMessage( QString beforeText, string afterText );
	void convertPriorityHostListToStringList( QPtrList<KipmsgPriorityHostItem> &list, QStringList &saveList );
	void convertStringListToPriorityHostList( QStringList &loadList, QPtrList<KipmsgPriorityHostItem> &list );
	void changePriorityHostList( QString pri, QPtrList<KipmsgPriorityHostItem> &list, QStringList &priList );

	void rebuildSortFilterMenu();
	void rebuildGroupSelectMenu();
	void rebuildEncodingSelectMenu();
	void rebuildPrioritySelectMenu();
	void setHostListItem(KIpMsgHostListViewItem *item,HostListItem *ix);
	QTextCodec *getHostCodec( HostListItem *ix );

};

#endif

