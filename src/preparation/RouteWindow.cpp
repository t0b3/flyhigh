/***************************************************************************
 *   Copyright (C) 2004 by Alex Graf                                       *
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
 
#include <qcursor.h>
#include <qheader.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qtable.h>
#include "IGPSDevice.h"
#include "Images.h"
#include "ProgressDlg.h"
#include "IRouteForm.h"
#include "ISql.h"
#include "Route.h"
#include "RouteWindow.h"
#include "WayPoint.h"

RouteWindow::RouteWindow(QWidget* parent, const char* name, int wflags, IDataBase::SourceType src)
	:TableWindow(parent, name, wflags)
{
	QString caption;
	QStringList nameList;
	QTable *pTable = TableWindow::getTable();
	QPopupMenu *pMenu;

	pMenu = new QPopupMenu(this);
	menuBar()->insertItem("&File", pMenu);
	
	switch(src)
	{
		case IDataBase::SqlDB:
			m_pDb = ISql::pInstance();
			caption = "Routes from DB";
			pMenu->insertItem("&New...", this, SLOT(file_new()));
			pMenu->insertItem("&Add to GPS...", this, SLOT(file_AddToGPS()));
		break;
		case IDataBase::GPSdevice:
			m_pDb = IGPSDevice::pInstance();
			caption = "Routes from GPS";
			pMenu->insertItem("&Add to DB...", this, SLOT(file_AddToSqlDB()));
		break;
		default:
			Q_ASSERT(false);
		break;
	}
	
	pMenu->insertItem("&View...", this, SLOT(file_view()));
	pMenu->insertItem("&Delete", this, SLOT(file_delete()));
	pMenu->insertItem("&Update", this, SLOT(file_update()));
	pMenu->insertItem("&Export all...", this, SLOT(exportTable()));
	
	TableWindow::setCaption(caption);
	TableWindow::setIcon(Images::pInstance()->getImage("document.xpm"));
	
	// configure the table
	pTable->setReadOnly(true);
	pTable->setSelectionMode(QTable::SingleRow);

	// header
	nameList += "Name";
	setupHeader(nameList);
	
	pTable->setColumnWidth(Name, 200);
	
	m_lastModified = 0;
}

bool RouteWindow::periodicalUpdate()
{
	int lastModified;
	
	lastModified = m_pDb->routesLastModified();
	
	if(m_lastModified < lastModified)
	{
		file_update();
		m_lastModified = lastModified;
	}
	
	return true;
}

void RouteWindow::file_delete()
{
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));
		m_pDb->delRoute(m_routeList[row]);
		TableWindow::setCursor(QCursor(Qt::WaitCursor));
	}
}

void RouteWindow::file_update()
{
	ProgressDlg progDlg(this);
	QTable *pTable = TableWindow::getTable();
	uint routeNr;
	uint maxRouteNr;

	m_routeList.clear();
	pTable->setNumRows(0);
	progDlg.beginProgress("read routes...", m_pDb);
	m_pDb->routeList(m_routeList);
	progDlg.endProgress();
	maxRouteNr = m_routeList.size();
	pTable->setNumRows(maxRouteNr);
	
	for(routeNr=0; routeNr<maxRouteNr; routeNr++)
	{
		setRouteToRow(routeNr, m_routeList[routeNr]);
	}
	
	TableWindow::unsetCursor();
}

void RouteWindow::file_new()
{
	Route route;
	IRouteForm routeForm(this, tr("Add Route to DB"), &route);
	
	if(routeForm.exec())
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));
		ISql::pInstance()->add(route);
		TableWindow::unsetCursor();
	}
}

void RouteWindow::file_view()
{
	int row;
	Route route;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		route = m_routeList[row];
		IRouteForm routeForm(this, tr("View Route"), &route);
		routeForm.execReadOnly();
	}
}

void RouteWindow::file_AddToGPS()
{
	ProgressDlg progDlg(this);
	QString name;
	int row;
	uint wpNr;
	uint nofWp;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		// make sure all WayPoints exist on GPS
		nofWp = m_routeList[row].wayPointList().size();
		progDlg.beginProgress("writing waypoints...", IGPSDevice::pInstance());
		
		for(wpNr=0; wpNr<nofWp; wpNr++)
		{
			IGPSDevice::pInstance()->add(m_routeList[row].wayPointList().at(wpNr));
		}
		
		progDlg.endProgress();
		
		// Route
		progDlg.beginProgress("writing route...", IGPSDevice::pInstance());
		IGPSDevice::pInstance()->add(m_routeList[row]);
		progDlg.endProgress();
	}
}

void RouteWindow::file_AddToSqlDB()
{
/*	QString str;
	WayPoint wp;
	double lon;
	double lat;
	int alt;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));

		// Coordinates
		str = getTable()->text(row, Name);
		
		for(....)
		{
		lon = str.toDouble();
		str = getTable()->text(row, Latitude);
		lat = str.toDouble();
		str = getTable()->text(row, Altitude);
		alt = str.toInt();
		wp.setCoordinates(lat, lon, alt);
		
		// Name
		str = getTable()->text(row, Name);
		wp.setName(str);
	
		IWayPointForm wayPointForm(this, tr("Add WayPoint to DB"), &wp);
		
		if(wayPointForm.exec())
		{
			ISql::pInstance()->add(wp);
		}
		
		TableWindow::unsetCursor();
	}
	*/
}

void RouteWindow::setRouteToRow(uint row, Route &route)
{
	QTable *pTable = TableWindow::getTable();
	
	pTable->setText(row, Name, route.name());
}