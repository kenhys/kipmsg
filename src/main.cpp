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

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>


#include <kpopupmenu.h>
#include "kipmsgwidget.h"
#include <kwin.h>
#include <kaboutdata.h>
#include <kuniqueapplication.h>

static const char description[] =
    I18N_NOOP("A KDE KPart Application");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

/**
 * メイン
 * ・アバウトデータの設定
 * ・ユニークインスタンスアプリの生成
 * ・システムトレイの設定
 * ・Widget表示
 * ・実行。
 */
int main(int argc, char **argv)
{
    KAboutData about("kipmsg", I18N_NOOP("kipmsg"), version, description,
                     KAboutData::License_GPL, "(C) 2006-2009 nikikuni", 0, 0, "nikikuni@yahoo.co.jp");
    about.addAuthor( "nikikuni", 0, "nikikuni@yahoo.co.jp" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
	if ( !KUniqueApplication::start() ) {
		fprintf(stderr, "KIpMessenger is already running!\n");
		exit(0);
	}
    KUniqueApplication app;
//	app.disableSessionManagement();
    kipmsgWidget *toplevel = new kipmsgWidget();

	KWin::setSystemTrayWindowFor( toplevel->winId(), 0 );
	toplevel->setGeometry(-100, -100, 42, 42 );
	toplevel->show();

    int ret = app.exec();

	delete toplevel;
	return ret;
}

