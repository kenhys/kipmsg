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


#include <qcheckbox.h>
#include <kcombobox.h>
#include "searchbox.h"

KIpMsgSearchBox::KIpMsgSearchBox(QWidget* parent, const char* name, WFlags fl)
        : KIpMsgSearchBoxBase(parent,name,fl)
{}

KIpMsgSearchBox::~KIpMsgSearchBox()
{}

/*$SPECIALIZATION$*/
void KIpMsgSearchBox::slotOkClicked()
{
	close();
}
void KIpMsgSearchBox::slotSearchClicked()
{
printf("search_clicked()\n");
	if ( m_SearchAllCheckbox->isChecked() ) {
		QListViewItemIterator its( m_HostListView );
		while ( its.current() != NULL ) {
			QListViewItem *item = its.current();
			item->setSelected( FALSE );
			if ( item->text( SendDialog::ColumnUser ).find( m_SearchWordCombobox->currentText() ) >= 0 ||
				 item->text( SendDialog::ColumnGroup).find( m_SearchWordCombobox->currentText() ) >= 0 ||
				 item->text( SendDialog::ColumnHost ).find( m_SearchWordCombobox->currentText() ) >= 0 ||
				 item->text( SendDialog::ColumnIpAddress ).find( m_SearchWordCombobox->currentText() ) >= 0 ||
				 item->text( SendDialog::ColumnLogin ).find( m_SearchWordCombobox->currentText() ) >= 0 ||
				 item->text( SendDialog::ColumnPriority ).find( m_SearchWordCombobox->currentText() ) >= 0 ||
				 item->text( SendDialog::ColumnEncoding ).find( m_SearchWordCombobox->currentText() ) >= 0 ) {
				m_HostListView->setSelected( item, TRUE );
printf("found all\n");
			}
			++its;
		}
	} else {
		QListViewItemIterator its( m_HostListView );
		while ( its.current() != NULL ) {
			QListViewItem *item = its.current();
			item->setSelected( FALSE );
			if ( item->text( SendDialog::ColumnUser ).find( m_SearchWordCombobox->currentText() ) >= 0 ) {
				m_HostListView->setSelected( item, TRUE );
printf("found\n");
			}
			++its;
		}
	}
//	emit ListViewHost->repaint();
}

#include "searchbox.moc"

