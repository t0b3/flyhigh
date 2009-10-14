/***************************************************************************
 *   Copyright (C) 2006 by Alex Graf                                     *
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
#include <qsqldatabase.h>
#include "AirSpaces.h"
#include "Error.h"
#include "ISql.h" 
#include "WayPoints.h"
#include "Gliders.h"
#include "Flights.h"
#include "Pilots.h"
#include "Routes.h"
#include "Servicings.h"
#include "Upgrade.h"
#include "ISql.h"

ISql* ISql::m_pInst = NULL;

ISql::ISql()
{
	m_pDefaultDB = QSqlDatabase::addDatabase("QMYSQL3");
	
	setName("flyhigh_v2");
	setUserName("flyhigh");
	setPassword("flyhigh");
	setHostName("localhost");
	setPort(3306);
	
//	m_pAirSpaces = new AirSpaces(m_pDefaultDB);
	m_pWayPoints = new WayPoints(m_pDefaultDB);
	m_pGliders = new Gliders(m_pDefaultDB);
	m_pFlights = new Flights(m_pDefaultDB);
	m_pRoutes = new Routes(m_pDefaultDB);
	m_pServicings = new Servicings(m_pDefaultDB);
	m_pPilots = new Pilots(m_pDefaultDB);
}

ISql::~ISql()
{
	m_pDefaultDB->close();
	
//	delete m_pAirSpaces;
	delete m_pWayPoints;
	delete m_pGliders;
	delete m_pFlights;
	delete m_pRoutes;
	delete m_pServicings;
	delete m_pPilots;
}

void ISql::setName(const QString &name)
{
	m_pDefaultDB->setDatabaseName(name);
}

void ISql::setUserName(const QString &userName)
{
	m_pDefaultDB->setUserName(userName);
}

void ISql::setPassword(const QString &passwd)
{
	m_pDefaultDB->setPassword(passwd);
}

void ISql::setHostName(const QString &hostName)
{
	m_pDefaultDB->setHostName(hostName);
}

void ISql::setPort(int port)
{
	m_pDefaultDB->setPort(port);
}

bool ISql::open()
{
	bool success;
	
	success = m_pDefaultDB->open();
	Error::verify(success, Error::SQL_OPEN);
	setupTables();
	
	return success;
}

ISql* ISql::pInstance()
{
	if(m_pInst == NULL)
	{
		m_pInst = new ISql();
	}
	
	return m_pInst;
}

bool ISql::add(WayPoint &wp)
{
	return m_pWayPoints->add(wp);
}

bool ISql::delWayPoint(WayPoint &wp)
{
	return m_pWayPoints->delWayPoint(wp);
}

bool ISql::findWayPoint(WayPoint &wp, uint radius)
{
	return m_pWayPoints->findWayPoint(wp, radius);
}

int ISql::wayPointsLastModified()
{
	return m_pWayPoints->lastModified("WayPoints");
}

bool ISql::wayPointList(WayPoint::WayPointListType &wpList)
{
	return m_pWayPoints->wayPointList(wpList);
}

bool ISql::add(Glider &glider)
{
	return m_pGliders->add(glider);
}

bool ISql::delGlider(Glider &glider)
{
	return m_pGliders->delGlider(glider);
}

bool ISql::glider(const QString &model, Glider &glider)
{
	return m_pGliders->glider(model, glider);
}

bool ISql::gliderList(Glider::GliderListType &gliderList)
{
	return m_pGliders->gliderList(gliderList);
}

int ISql::glidersLastModified()
{
	return m_pGliders->lastModified("Gliders");
}

bool ISql::add(Flight &flight)
{
	return m_pFlights->add(flight);
}

bool ISql::delFlight(Flight &flight)
{
	return m_pFlights->delFlight(flight);
}

int ISql::newFlightNr(Pilot &pilot)
{
	return m_pFlights->newFlightNr(pilot);
}

bool ISql::flightList(Pilot &pilot, Flight::FlightListType &flightList)
{
	return m_pFlights->flightList(pilot, flightList);
}

bool ISql::flightsPerYear(Pilot &pilot, FlightsPerYearListType &fpyList)
{
	return m_pFlights->flightsPerYear(pilot, fpyList);
}

bool ISql::loadIGCFile(Flight &flight)
{
	return m_pFlights->loadIGCFile(flight);
}

int ISql::flightsLastModified()
{
	return m_pFlights->lastModified("Flights");
}

bool ISql::add(Route &route)
{
	return m_pRoutes->add(route);
}

bool ISql::delRoute(Route &route)
{
	return m_pRoutes->delRoute(route);
}

int ISql::routesLastModified()
{
	return m_pRoutes->lastModified("Routes");
}

bool ISql::routeList(Route::RouteListType &routeList)
{
	return m_pRoutes->routeList(routeList);
}
/*
bool ISql::add(AirSpace &airspace)
{
	return m_pAirSpaces->add(airspace);
}

bool ISql::delAirSpace(AirSpace &airspace)
{
	return m_pAirSpaces->delAirSpace(airspace);
}

bool ISql::airspace(const QString &name, AirSpace &airspace)
{
	return m_pAirSpaces->airspace(name, airspace);
}

int ISql::airspacesLastModified()
{
	return m_pAirSpaces->lastModified("AirSpaces");
}

bool ISql::airspaceList(AirSpace::AirSpaceListType &airspaceList)
{
	return m_pAirSpaces->airspaceList(airspaceList);
}
*/
bool ISql::add(Servicing &serv)
{
	return m_pServicings->add(serv);
}

bool ISql::delServicing(Servicing &servicing)
{
	return m_pServicings->delServicing(servicing);
}

bool ISql::servicingList(Servicing::ServicingListType &servicingList)
{
	return m_pServicings->servicingList(servicingList);
}

int ISql::servicingsLastModified()
{
	return m_pServicings->lastModified("Servicings");
}

bool ISql::add(Pilot &pilot)
{
	return m_pPilots->add(pilot);
}

bool ISql::setId(Pilot &pilot)
{
	return m_pPilots->setId(pilot);
}

bool ISql::update(Pilot &pilot)
{
	return m_pPilots->update(pilot);
}

bool ISql::pilot(int id, Pilot &pilot)
{
	return m_pPilots->pilot(id, pilot);
}

int ISql::pilotsLastModified()
{
	return m_pPilots->lastModified("Pilots");
}

/*
AirSpaces* ISql::pAirSpaceTable()
{
	return m_pAirSpaces;
}*/

WayPoints* ISql::pWayPointTable()
{
	return m_pWayPoints;
}

Gliders* ISql::pGliderTable()
{
	return m_pGliders;
}

Flights* ISql::pFlightTable()
{
	return m_pFlights;
}

Servicings* ISql::pServicingTable()
{
	return m_pServicings;
}

Pilots* ISql::pPilotTable()
{
	return m_pPilots;
}

void ISql::setupTables()
{
	Upgrade upgrade(m_pDefaultDB);
	
	upgrade.upgrade();
}