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


#ifndef _SENDDIALOG_H_
#define _SENDDIALOG_H_

#include <kpopupmenu.h>
#include <qintdict.h>
#include <qsize.h>
#include <qdialog.h>
#include <ktextedit.h>
#include "senddialogbase.h"
#include "IpMessenger.h"

class KIpMsgFileNameConverter : public FileNameConverter {
public:
	virtual string ConvertNetworkToLocal( string original_file_name );
	virtual string ConvertLocalToNetwork( string original_file_name );
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
	virtual void contentsDragEnterEvent(QDragEnterEvent *e);
	virtual void contentsDropEvent(QDropEvent *e);
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
	void refreshHostList();
	void setRecieveDialog( QDialog *_recvdialog ) { recvdialog = _recvdialog; };
	void setFixsizePotisionMenu();
	void setSaveSizeMenu();
	void synchronizeMenu();
	void setMessageText(QString text);
	void addDnDFiles(QString fileUrl);
	void refreshFiles();

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
	};
			
public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotHostListUpdateClicked();
    virtual void slotMessageSendClicked();
    virtual void slotSecretClicked();
    virtual void slotMoveToPriority1Clicked();
    virtual void slotMoveToPriority2Clicked();
    virtual void slotMoveToPriority3Clicked();
    virtual void slotMoveToPriority4Clicked();
    virtual void slotMoveToDefaultClicked();
    virtual void slotMoveToHiddenClicked();
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
    virtual void slotAttacheFileListButtonClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/
	virtual void mousePressEvent (QMouseEvent *e);
	virtual void mouseReleaseEvent (QMouseEvent *e);
	virtual void mouseMoveEvent (QMouseEvent *e);
	virtual void dropEvent (QDropEvent *e);
	virtual void dragEnterEvent (QDragEnterEvent *e);
	virtual void resizeEvent( QResizeEvent *e );

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	KTextEditNoDnD *m_MessageEditbox;
	KPopupMenu *SendPopup;
	KPopupMenu *SortPopup;
	KPopupMenu *GroupPopup;
	KPopupMenu *EncodingPopup;
	KPopupMenu *FontPopup;
	KPopupMenu *SizePopup;
	int move_priority1_menu_item;
	int move_priority2_menu_item;
	int move_priority3_menu_item;
	int move_priority4_menu_item;
	int move_default_menu_item;
	int move_hidden_menu_item;
	int show_hidden_menu_item;
	int fixize_pos_menu_item;
	int save_size_menu_item;
	QIntDict<QString> group_menu;
	QIntDict<QString> encoding_menu;
	AttachFileList files;
	bool isMainSplitterDragging;
	bool isDownloadSplitterDragging;
	QDialog *recvdialog;

	int defaultX;
	int defaultY;
	int defaultWidth;
	int defaultHeight;

	HostList	hosts;
	void setPriority( string pri, QStringList &priList );
	void deleteFromList( QStringList &base, QStringList items );
	void setMenuStatus();
	void doResize( QSize size );
};

#endif

