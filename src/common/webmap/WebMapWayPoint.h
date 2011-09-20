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

#ifndef WebMapWayPoint_h
#define WebMapWayPoint_h

#include <QObject>
#include "WayPoint.h"

class WebMap;

class WebMapWayPoint: public QObject
{
	Q_OBJECT

	public:
		WebMapWayPoint(WebMap *pWebMap);

		~WebMapWayPoint();

		void init();

		void pushWayPoint(const WayPoint &wp);

    void selectWayPoint(uint id);

    void populateObject();

	signals:
		void changeWayPoint(const WayPoint &wp);

	protected slots:
		void saveWayPoint(int id, const QString &name, const QString &spot, const QString &country,
                      double lat, double lon, int alt);

	private:
		WebMap *m_pWebMap;
};

#endif