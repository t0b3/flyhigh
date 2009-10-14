/***************************************************************************
 *   Copyright (C) 2004 by Alex Graf                                       *
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

#ifndef Flights_h
#define Flights_h

#include <qdatetime.h>
#include "ContainerDef.h"
#include "DataBaseSub.h"
#include "Flight.h"

class Flights: public DataBaseSub
{
	public:
		Flights(QSqlDatabase *pDB);

		bool add(Flight &flight);
		bool delFlight(Flight &flight);
		int newFlightNr(Pilot &pilot);
		bool flightList(Pilot &pilot, Flight::FlightListType &flightList);
		bool flightsPerYear(Pilot &pilot, FlightsPerYearListType &fpyList);
		bool loadIGCFile(Flight &flight);

		bool setFlightStatistic(Glider &glider);
		
	private:
		enum Elements
		{
			Id, Number, PilotId, Date, Time, GliderId, StartPtId, LandPtId,
			Duration, Distance, Comment, IGCFile
		};

		bool setId(Flight &flight);
};

#endif