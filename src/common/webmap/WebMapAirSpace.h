/***************************************************************************
 *   Copyright (C) 2011 by Alex Graf                                       *
 *   grafal@sourceforge.net                                                *
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

#ifndef WebMapAirSpace_h
#define WebMapAirSpace_h

#include <QObject>
#include "WayPoint.h"

class WebMap;
class AirSpace;

class WebMapAirSpace: public QObject
{
	Q_OBJECT

	public:
		WebMapAirSpace(WebMap *pWebMap);

		~WebMapAirSpace();

		void init();

		void pushAirSpace(AirSpace *pAirSpace);

		void selectAirSpace(int id);

	private:
		WebMap *m_pWebMap;
        QWebEnginePage *m_pPage;
};

#endif
