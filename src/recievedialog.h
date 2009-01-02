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


#ifndef _RECIEVEDIALOG_H_
#define _RECIEVEDIALOG_H_

#include <qlistview.h>
#include <kfiledialog.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kurl.h>
#include <kpopupmenu.h>
#include "IpMessenger.h"
#include "recievedialogbase.h"

class KIpMsgAttachedFileListViewItem : public QListViewItem {
  private:
    AttachFile m_file;
	static QString ExtName( QString FileName );
	static QString FileType( QString FileName );
	static QString TimeStamp( time_t t );
  public:
    KIpMsgAttachedFileListViewItem( QListView *parent, QTextCodec *codec, AttachFile &file );
    AttachFile file();
};

class RecieveDialog : public RecieveDialogBase
{
    Q_OBJECT

public:
    RecieveDialog(RecievedMessage _msg, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~RecieveDialog();
    /*$PUBLIC_FUNCTIONS$*/
	void setDownloadFiles();
	void setFixsizePotisionMenu();
	void setSaveSizeMenu();
	void synchronizeMenu();
	bool isOpenSaveDialog(){ return _isOpenSaveDialog;}

    KHTMLPart* m_RecievedMessageHTMLPart;

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void slotMessageOpenClicked();
    virtual void slotDialogCloseClicked();
    virtual void slotReplyClicked();
    virtual void slotEncodingChange(int);
    virtual void slotOpenURL(const KURL &);
    virtual void slotSaveSizeClicked();
    virtual void slotRestoreSizeTempClicked();
    virtual void slotFixizePositionClicked();
    virtual void slotAttachFilesMouseButtonClicked(int,QListViewItem *,const QPoint &, int);
    virtual void slotDownloadClicked();
    virtual void slotDownloadAllClicked();
    virtual void slotFontSelectClicked();
    virtual void slotFontRestoreToDefaultClicked();

protected:
    /*$PROTECTED_FUNCTIONS$*/
	virtual void resizeEvent( QResizeEvent *e );
	virtual void closeEvent ( QCloseEvent * e );
	virtual void mousePressEvent ( QMouseEvent *e );
	virtual void mouseReleaseEvent ( QMouseEvent *e );
	virtual void mouseMoveEvent ( QMouseEvent *e );

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
	KPopupMenu *RecvPopup;
	KPopupMenu *SizePopup;
	KPopupMenu *DownloadPopup;
	KPopupMenu *FontPopup;
	int fixize_pos_menu_item;
	int save_size_menu_item;

	int defaultX;
	int defaultY;
	int defaultWidth;
	int defaultHeight;

	bool _isOpenSaveDialog;
	bool isDownloading;
	bool isDownloadSplitterDragging;
	RecievedMessage msg;
	void doResize( QResizeEvent *e = NULL );
	QString getSaveFileName( QString fileName, KFile::Mode mode );
	QString convertMessageToHTML( QString msg );
	void renderMessage( QString msg );
	bool isUrlCharWithoutAmp( QChar c );
	void doDownload( bool isOpenSaveDialog, QString downloadPath="" );
};

#endif

