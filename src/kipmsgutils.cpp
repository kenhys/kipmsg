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

#define _KIPMSGUTILS_H_
#include <qstring.h>
#include "kipmsgutils.h"

QString GetPercentageIconName( int percent ){
	if ( percent >= 100 ) {
		return "kipmsg_progress_100";
	} else if ( percent < 100 && percent >= 93 ) {
		return "kipmsg_progress_093";
	} else if ( percent < 93  && percent >= 85 ) {
		return "kipmsg_progress_085";
	} else if ( percent < 85  && percent >= 78 ) {
		return "kipmsg_progress_078";
	} else if ( percent < 78  && percent >= 71 ) {
		return "kipmsg_progress_071";
	} else if ( percent < 71  && percent >= 64 ) {
		return "kipmsg_progress_064";
	} else if ( percent < 64  && percent >= 57 ) {
		return "kipmsg_progress_057";
	} else if ( percent < 57  && percent >= 50 ) {
		return "kipmsg_progress_050";
	} else if ( percent < 50  && percent >= 42 ) {
		return "kipmsg_progress_042";
	} else if ( percent < 42  && percent >= 35 ) {
		return "kipmsg_progress_035";
	} else if ( percent < 35  && percent >= 28 ) {
		return "kipmsg_progress_028";
	} else if ( percent < 28  && percent >= 21 ) {
		return "kipmsg_progress_021";
	} else if ( percent < 21  && percent >= 14 ) {
		return "kipmsg_progress_014";
	} else if ( percent < 14  && percent >= 7  ) {
		return "kipmsg_progress_007";
	}
	return "kipmsg_progress_000";
}

QString CreateTimeStringAsCTimeStyle( time_t t )
{
	char buf[100];

	ctime_r( &t, buf );
	buf[strlen(buf)-1] = 0;
	return QString(buf);
}

QString CreateTimeString( time_t t )
{
	return CreateTimeStringAsCTimeStyle( t );
}

