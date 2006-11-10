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

#include <qmap.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <klineedit.h>
#include <klistbox.h>
#include <kfiledialog.h>

#include "clickableurlconfig.h"
#include "kipmsgsettings.h"

KIPMsgClickableURLConfigDialog::KIPMsgClickableURLConfigDialog(QWidget* parent, const char* name, WFlags fl)
        : KIPMsgClickableURLConfigDialogBase(parent,name,fl)
{
	m_DefaultBrowserCheckbox->setChecked( KIpMsgSettings::useDefaultBrowser() );
	QStringList pro = KIpMsgSettings::executePrograms();

	for( QStringList::iterator p = pro.begin(); p != pro.end(); p++ ){
		QString item = *p;
		int pos = item.find(':');
		if ( pos < 0 ) {
			protocols[item] = "";
		} else {
			QString key = item.left( pos );
			QString val = item.mid( pos + 1 );
			protocols[key] = val;
		}
	}
	if ( m_ProtocolsListbox->count() == 0 ) {
		m_ProtocolsListbox->setCurrentItem( 0 );
		m_ProgramNameEditbox->setText( protocols[m_ProtocolsListbox->currentText()] );
	}
}

KIPMsgClickableURLConfigDialog::~KIPMsgClickableURLConfigDialog()
{}

/*$SPECIALIZATION$*/
void KIPMsgClickableURLConfigDialog::slotProgramNameChanged(const QString& text)
{
	protocols[m_ProtocolsListbox->currentText()] = text;
}
void KIPMsgClickableURLConfigDialog::slotBrowseClicked()
{
	QString programName = KFileDialog::getOpenFileName();
	if ( programName != "" ) {
		protocols[m_ProtocolsListbox->currentText()] = programName;
		m_ProgramNameEditbox->setText( protocols[m_ProtocolsListbox->currentText()] );
	}
}
void KIPMsgClickableURLConfigDialog::slotOkClicked()
{
	slotApplyClicked();
	close();
}
void KIPMsgClickableURLConfigDialog::slotCancelClicked()
{
	close();
}
void KIPMsgClickableURLConfigDialog::slotApplyClicked()
{
	KIpMsgSettings::setUseDefaultBrowser( m_DefaultBrowserCheckbox->isChecked() );

	QStringList pro;
	for( QMap<QString,QString>::iterator p = protocols.begin(); p != protocols.end(); p++ ){
		pro << p.key() + ":" + p.data();
	}

	KIpMsgSettings::setExecutePrograms( pro );

	KIpMsgSettings::writeConfig();
}
void KIPMsgClickableURLConfigDialog::slotProtocolClicked(QListBoxItem* item)
{
	m_ProgramNameEditbox->setText( protocols[item->text()] );
}

#include "clickableurlconfig.moc"

