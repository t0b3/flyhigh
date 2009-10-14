/***************************************************************************
 *   Copyright (C) 2005 by Alex Graf                                       *
 *   grafal@sourceforge.net                                                         *
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
 
#ifndef IDataBase_h
#define IDataBase_h

#include <qdatetime.h>
#include <qobject.h>
#include "ContainerDef.h"
#include "AirSpace.h"
#include "Flight.h"
#include "Pilot.h"
#include "Route.h"
#include "WayPoint.h"

class IDataBase: public QObject
{
	Q_OBJECT
	public:
		typedef enum SourceType{GPSdevice, SqlDB, File}SourceType;
		
		IDataBase();
		virtual ~IDataBase();

		// flights
		virtual bool add(Flight &flight);
		virtual bool delFlight(Flight &flight);
		virtual int flightsLastModified();
		virtual bool flightList(Pilot &pilot, Flight::FlightListType &flightList);
		virtual bool loadIGCFile(Flight &flight);
		
		// waypoints
		virtual bool add(WayPoint &wp);
		virtual bool delWayPoint(WayPoint &wp);
		virtual bool delAllWayPoints();
		virtual int wayPointsLastModified();
		virtual bool wayPointList(WayPoint::WayPointListType &wpList);
		
		// gliders
		virtual bool add(Glider &glider);
		virtual bool delGlider(Glider &glider);
		virtual bool gliderList(Glider::GliderListType &gliderList);
		virtual int glidersLastModified();

		// routes
		virtual bool add(Route &route);
		virtual bool delRoute(Route &route);
		virtual int routesLastModified();
		virtual bool routeList(Route::RouteListType &routeList);
		
		// airspaces
		virtual bool add(AirSpace &airspace);
		virtual bool delAirSpace(const QString &name);
		virtual bool airspace(const QString &name, AirSpace &airspace);
		virtual int airspacesLastModified();
		virtual bool airspaceList(AirSpace::AirSpaceListType &airspaceList);

		// pilots
		virtual bool add(Pilot &pilot);
		virtual bool update(Pilot &pilot);
		virtual bool pilot(int id, Pilot &pilot);
		virtual bool setId(Pilot &pilot);
		virtual int pilotsLastModified();
		
	signals:
		void progress(int percent);
		
	public slots:
		virtual void cancel();
};

#endif