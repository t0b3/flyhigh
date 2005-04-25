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
 
#include <qcursor.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtable.h>
#include <qdatetime.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qwidget.h>
#include "Error.h"
#include "FlightWindow.h"
#include "Glider.h"
#include "WayPoint.h"
#include "IGPSDevice.h"
#include "IGliderForm.h"
#include "IFlightForm.h"
#include "IWayPointForm.h"
#include "ISql.h"
#include "Flight.h"
#include "Images.h"
#include "IGCFileParser.h"
#include "OLCOptimizer.h"

FlightWindow::FlightWindow(QWidget* parent, const char* name, int wflags, IDataBase::SourceType src)
	:TableWindow(parent, name, wflags)
{
	QString caption;
	QStringList nameList;
	QTable *pTable = TableWindow::getTable();
	QPopupMenu *pMenu;

	pMenu = new QPopupMenu(this);
	menuBar()->insertItem("&File", pMenu);
	pMenu->insertItem("&Update", this, SLOT(file_update()));

	switch(src)
	{
		case IDataBase::SqlDB:
			m_pDb = ISql::pInstance();
			caption = "Flights from DB";
			pMenu->insertItem("&New...", this, SLOT(file_new()));
			pMenu->insertItem("&Delete", this, SLOT(file_delete()));
			pMenu->insertItem("&Import...", this, SLOT(file_import()));
		break;
		case IDataBase::GPSdevice:
			m_pDb = IGPSDevice::pInstance();
			caption = "Flights from GPS";
			pMenu->insertItem("&Add to DB...", this, SLOT(file_AddToSqlDB()));
		break;
		default:
			Q_ASSERT(false);
		break;
	}
	
	pMenu->insertItem("&Export...", this, SLOT(file_export()));
	pMenu->insertItem("&Print...", this, SLOT(print()));
	TableWindow::printer().setOrientation(QPrinter::Landscape);
	
/*	- ground speed / time
- vario / time
- altitude / time
- 3D plot of flight*/
	
	pMenu = new QPopupMenu(this);
	menuBar()->insertItem("&Plot", pMenu);
	pMenu->insertItem("&Speed vs Time", this, SLOT(plot_speedVsTime()));
	pMenu->insertItem("&Alt vs Time", this, SLOT(plot_altVsTime()));
	pMenu->insertItem("&Vario vs Time", this, SLOT(plot_varioVsTime()));
	pMenu->insertItem("&OLC", this, SLOT(plot_OLC()));
	pMenu->insertItem("&3D View", this, SLOT(plot_3d()));
	
	TableWindow::setCaption(caption);
	TableWindow::setIcon(Images::pInstance()->getImage("document.xpm"));
	
	// configure the table
	pTable->setReadOnly(true);
	pTable->setSelectionMode(QTable::SingleRow);
	
	// header
	nameList += "Nr";
	nameList += "Date\n[DD.MM.YYYY]";
	nameList += "Time\n[hh:mm:ss]";
	nameList += "Duration\n[hh:mm:ss]";
	nameList += "Glider";
	nameList += "Starting Point";
	nameList += "Landing Strip";
	nameList += "Distance\n[km]";
	nameList += "Comment";
	setupHeader(nameList);
	
	pTable->setColumnWidth(Nr, 50);
	pTable->setColumnWidth(Date, 95);
	pTable->setColumnWidth(Time, 80);
	pTable->setColumnWidth(Duration, 80);
	pTable->setColumnWidth(Model, 50);
	pTable->setColumnWidth(StartPt, 120);
	pTable->setColumnWidth(LandPt, 120);
	pTable->setColumnWidth(Distance, 65);
	pTable->setColumnWidth(Comment, 500);
	
	// menu
/*
	pMenu = new QPopupMenu(this);
	menuBar()->insertItem("&Flights", pMenu);
	pMenu->insertItem("Update", this, SLOT(tracks_update()));
	pMenu->insertItem("Read...", this, SLOT(tracks_read()));
*/

	m_fileName = "";
	m_lastModified = 0;
	startTimer(1);
}

void FlightWindow::timerEvent(QTimerEvent *pEvent)
{
	int lastModified;
	
	if(pEvent != NULL)
	{
		lastModified = m_pDb->flightsLastModified();
		
		if(m_lastModified < lastModified)
		{
			file_update();
			m_lastModified = lastModified;
		}
	}
}

void FlightWindow::file_update()
{
	Flight::FlightListType flightList; 
	Flight flight;
	QTable *pTable = TableWindow::getTable();
	uint flightNr;
	uint maxFlightNr;

	pTable->setNumRows(0);
	m_pDb->flightList(flightList);
	maxFlightNr = flightList.size();
	
	for(flightNr=0; flightNr<maxFlightNr; flightNr++)
	{
		pTable->insertRows(flightNr);
		setFlightToRow(flightNr, flightList[flightNr]);
	}
}

void FlightWindow::setFlightToRow(uint row, Flight &flight)
{
	Glider glider;
	WayPoint wp;
	QString str;
	QTime duration;
	QTable *pTable = TableWindow::getTable();
	
	str.sprintf("%i",flight.number());
	pTable->setText(row, Nr, str);
	pTable->setText(row, Date, flight.date().toString("dd.MM.yyyy"));
	pTable->setText(row, Time, flight.time().toString(Qt::ISODate));
	
	duration.setHMS(0, 0, 0);
	pTable->setText(row, Duration, duration.addSecs(flight.duration()).toString(Qt::ISODate));

	pTable->setText(row, Model, flight.glider());
	pTable->setText(row, StartPt, flight.startPt());
	pTable->setText(row, LandPt, flight.landPt());
	
	str.sprintf("%.3f", flight.distance()/1000.0);
	pTable->setText(row, Distance, str);
	
	pTable->setText(row, Comment, flight.comment());
}

void FlightWindow::file_AddToSqlDB()
{
	IFlightForm newFlightForm(this, "New Flight");
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	QTime time;
	QDate date;
	Flight flight;
	Glider glider;
	WayPoint wp;
	FlightPointList fpList;
	OLCOptimizer::FlightPointIndexListType fpIndexList;
	int hh;
	int mm;
	int ss;
	int DD;
	int MM;
	int YY;
	QString str ;
	int row;
	int id;

	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));
		m_pDb->igcFile(row, flight.igcData());
	
		// parse and optimize
		igcParser.parse(flight.igcData());
		olcOptimizer.setFlightPoints(igcParser.flightPointList(), 5); // ignore deltaSpeeds under 5 m/s
		
		if(olcOptimizer.optimize())
		{
			// distance
			flight.setDistance(olcOptimizer.freeDistance(fpIndexList));
		}
		
		TableWindow::unsetCursor();
		
		// nr
		id = ISql::pInstance()->newFlightNr();
		flight.setNumber(id);
		
		// date
		str = getTable()->text(row, Date);
		sscanf(str.ascii(), "%2d%*c%2d%*c%4d", &DD, &MM, &YY);
		date.setYMD(YY, MM, DD);
		flight.setDate(date);

		// start
		id = igcParser.flightPointList().firstValidFlightData();
		
		if(id >= 0)
		{
			// start time
			flight.setTime(igcParser.flightPointList().at(id).time);
		
			// start place
			wp = igcParser.flightPointList().at(id).wp;
			
			if(!ISql::pInstance()->findWayPoint(wp, WayPoint::startLandRadius))
			{
				IWayPointForm newWayPoint(this, tr("New WayPoint Starting Place"), &wp);
	
				if(newWayPoint.exec())
				{
					ISql::pInstance()->add(wp);
				}
			}
			flight.setStartPt(wp.name());
		}
		
		// landing strip
		id = igcParser.flightPointList().lastValidFlightData();
		
		if(id >= 0)
		{
			wp = igcParser.flightPointList().at(id).wp;
			
			if(!ISql::pInstance()->findWayPoint(wp, WayPoint::startLandRadius))
			{
				IWayPointForm newWayPoint(this, tr("New WayPoint Landing Strip"), &wp);
	
				if(newWayPoint.exec())
				{
					ISql::pInstance()->add(wp);
				}
			}
			flight.setLandPt(wp.name());
		}
		
		// duration
		str = getTable()->text(row, Duration);
		sscanf(str.ascii(), "%2d%*c%2d%*c%2d", &hh, &mm, &ss);
		time.setHMS(hh, mm, ss);
		flight.setDuration(time);

		// glider
		if(!ISql::pInstance()->glider(igcParser.model(), glider))
		{
			IGliderForm newGlider(this, "New Glider", &glider);
			
			if(newGlider.exec())
			{
				ISql::pInstance()->add(glider);
			}
		}
		flight.setGlider(glider.model());

		// a new flight
		newFlightForm.setFlight(&flight);
		
		if(newFlightForm.exec())
		{
			ISql::pInstance()->add(flight);
			flight.setNumber(row); // restore original track nr or gps will be confused
			setFlightToRow(row, flight);
		}
	}
}

void FlightWindow::file_import()
{
	IFlightForm newFlightForm(this, "New Flight");
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	QTime time;
	QDate date;
	Flight flight;
	Glider glider;
	WayPoint wp;
	FlightPointList fpList;
	OLCOptimizer::FlightPointIndexListType fpIndexList;
	QString fileName;
	QFile file;
	int nr;
	int startPtId;
	int landPtId;
	int duration;

	fileName = QFileDialog::getOpenFileName(m_fileName, "IGC Files (*.igc)", this);
	file.setName(fileName);

	if(file.open(IO_ReadOnly))
	{
		flight.setIgcData(file.readAll());
		file.close();
	
		// parse and optimize
		igcParser.parse(flight.igcData());
		olcOptimizer.setFlightPoints(igcParser.flightPointList(), 5); // ignore deltaSpeeds under 5 m/s
		
		if(olcOptimizer.optimize())
		{
			// distance
			flight.setDistance(olcOptimizer.freeDistance(fpIndexList));
		}

		// nr
		nr = ISql::pInstance()->newFlightNr();
		flight.setNumber(nr);
		
		// date
		flight.setDate(igcParser.date());
	
		// start
		startPtId = igcParser.flightPointList().firstValidFlightData();
		
		if(startPtId >= 0)
		{
			// start time
			flight.setTime(igcParser.flightPointList().at(startPtId).time);
		
			// start place
			wp = igcParser.flightPointList().at(startPtId).wp;
			
			if(!ISql::pInstance()->findWayPoint(wp, WayPoint::startLandRadius))
			{
				IWayPointForm newWayPoint(this, tr("New WayPoint Starting Place"), &wp);
	
				if(newWayPoint.exec())
				{
					ISql::pInstance()->add(wp);
				}
			}
			flight.setStartPt(wp.name());
		}
		
		// landing strip
		landPtId = igcParser.flightPointList().lastValidFlightData();
		
		if(landPtId >= 0)
		{
			wp = igcParser.flightPointList().at(landPtId).wp;
			
			if(!ISql::pInstance()->findWayPoint(wp, WayPoint::startLandRadius))
			{
				IWayPointForm newWayPoint(this, tr("New WayPoint Landing Strip"), &wp);
	
				if(newWayPoint.exec())
				{
					ISql::pInstance()->add(wp);
				}
			}
			flight.setLandPt(wp.name());
		}
		
		// duration
		time = igcParser.flightPointList().at(startPtId).time;
		duration = time.secsTo(igcParser.flightPointList().at(landPtId).time);
		flight.setDuration(duration);
	
		// glider
		if(!ISql::pInstance()->glider(igcParser.model(), glider))
		{
			IGliderForm newGlider(this, "New Glider", &glider);
			
			if(newGlider.exec())
			{
				ISql::pInstance()->add(glider);
			}
		}
		flight.setGlider(glider.model());
	
		// a new flight
		newFlightForm.setFlight(&flight);
		
		if(newFlightForm.exec())
		{
			ISql::pInstance()->add(flight);
		}
	}
}

void FlightWindow::file_export()
{
	QByteArray igcData;
	QFile file;
	QString fileName;
	QString line;
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	OLCOptimizer::FlightPointIndexListType fpIndexListFree;
	OLCOptimizer::FlightPointIndexListType fpIndexListFAI;
	OLCOptimizer::FlightPointIndexListType fpIndexListFlat;
	FlightPointList fpList;
	double distFree;
	double distFAI;
	double distFlat;
	double dist;
	double ptsFree;
	double ptsFAI;
	double ptsFlat;
	double pts;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		TableWindow::setCursor(QCursor(Qt::WaitCursor));
			
		if(m_pDb->igcFile(getTable()->text(row, Nr).toInt(), igcData))
		{
			// IGC file
			fileName = QFileDialog::getSaveFileName(m_fileName, "Track Files (*.igc; *.olc)", this);
			file.setName(fileName + ".igc");
			
			if(file.open(IO_WriteOnly))
			{
				file.writeBlock(igcData);
				file.close();
			}
		
			// OLC file
			igcParser.parse(igcData);
			olcOptimizer.setFlightPoints(igcParser.flightPointList(), 5); // ignore deltaSpeeds under 5 m/s
			file.setName(fileName + ".olc");
			
			if(file.open(IO_WriteOnly))
			{
				QTextStream textStream(&file);
				
				textStream << "Pilot: " << igcParser.pilot() << "\n";
				textStream << "Take-off location: " << getTable()->text(row, StartPt) << "\n";
				textStream << "Callsign: " << igcParser.gliderId() << "\n";
				textStream << "IGC-filename: " << fileName << ".olc\n";
				textStream << "Date of flight: " << igcParser.date().toString("dd.MM.yyyy") << "\n";
				textStream << "Model of glider: " << igcParser.model() << "\n";
				
				if(olcOptimizer.optimize())
				{
					distFree = olcOptimizer.freeDistance(fpIndexListFree) / 1000.0;
					ptsFree = distFree * 1.5;
					distFAI = olcOptimizer.FAITriangle(fpIndexListFAI) / 1000.0;
					ptsFAI = distFAI  * 1.75;
					distFlat = olcOptimizer.flatTriangle(fpIndexListFlat) / 1000.0;
					ptsFlat = distFlat * 2.0;
					
					if((ptsFree > ptsFAI) && (ptsFree > ptsFlat))
					{
						textStream << "Departure: " << olcOptimizer.flyPointList().at(fpIndexListFree[0]) << "\n";
						textStream << "1st waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFree[1]) << "\n";
						textStream << "2nd waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFree[2]) << "\n";
						textStream << "3rd waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFree[3]) << "\n";
						textStream << "Finish: " << olcOptimizer.flyPointList().at(fpIndexListFree[4]) << "\n";
						pts = ptsFree;
						dist = distFree;
					}
					else if(ptsFlat > ptsFAI)
					{
						textStream << "Departure: " << olcOptimizer.flyPointList().at(fpIndexListFlat[0]) << "\n";
						textStream << "1st waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFlat[1]) << "\n";
						textStream << "2nd waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFlat[2]) << "\n";
						textStream << "3rd waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFlat[3]) << "\n";
						textStream << "Finish: " << olcOptimizer.flyPointList().at(fpIndexListFlat[4]) << "\n";
						pts = ptsFlat;
						dist = distFlat;
					}
					else
					{
						textStream << "Departure: " << olcOptimizer.flyPointList().at(fpIndexListFAI[0]) << "\n";
						textStream << "1st waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFAI[1]) << "\n";
						textStream << "2nd waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFAI[2]) << "\n";
						textStream << "3rd waypoint: " << olcOptimizer.flyPointList().at(fpIndexListFAI[3]) << "\n";
						textStream << "Finish: " << olcOptimizer.flyPointList().at(fpIndexListFAI[4]) << "\n";
						pts = ptsFAI;
						dist = distFAI;
					}
					
					textStream << "Flight distance: " << dist << " km\n";
					textStream << "Points for the flight: " << pts << "\n";
					textStream << "Comment Pilot: " << getTable()->text(row, Comment) << "\n";
				}
				
				file.close();
			}
		}
		
		TableWindow::unsetCursor();
	}
}

void FlightWindow::file_new()
{
	IFlightForm newFlightForm(this, "New Flight");
	Flight flight;
	int nr;
	
	// nr
	nr = ISql::pInstance()->newFlightNr();
	flight.setNumber(nr);
	
	// a new flight
	newFlightForm.setFlight(&flight);
	
	if(newFlightForm.exec())
	{
		ISql::pInstance()->add(flight);
	}
}

void FlightWindow::file_delete()
{
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		m_pDb->delFlight(getTable()->text(row, Nr).toInt());
	}
}

void FlightWindow::plot_speedVsTime()
{
	QByteArray igcData;
	QTime time;
	IGCFileParser igcParser;
	FlightPointList fpList;
	GnuPlot::TimeVectorType x;
	GnuPlot::PlotVectorType y;
	uint fpNr;
	uint fpListSize;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		if(m_pDb->igcFile(getTable()->text(row, Nr).toInt(), igcData))
		{
			igcParser.parse(igcData);
			fpList = igcParser.flightPointList();
			fpListSize = fpList.size() - 1;
			
			for(fpNr=0; fpNr<fpListSize; fpNr++)
			{
				x.push_back(fpList[fpNr].time);
				y.push_back(fpList.speedH(fpNr, fpNr+1)*3.6); // in km/h
//				x.push_back(fpList[fpNr].time.hour()*3600 + fpList[fpNr].time.minute()*60 + fpList[fpNr].time.second());
			}
		
			m_plotter.clear();
			m_plotter.setLabelX("time [s]");
			m_plotter.setLabelY("speed [km/h]");
			m_plotter.plotXY(x, y, "Speed vs Time");
		}
	}
}

void FlightWindow::plot_altVsTime()
{
	QByteArray igcData;
	QTime time;
	IGCFileParser igcParser;
	FlightPointList fpList;
	GnuPlot::TimeVectorType x;
	GnuPlot::PlotVectorType y;
	uint fpNr;
	uint fpListSize;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		if(m_pDb->igcFile(getTable()->text(row, Nr).toInt(), igcData))
		{
			igcParser.parse(igcData);
			fpList = igcParser.flightPointList();
			fpListSize = fpList.size() - 1;
			
			for(fpNr=0; fpNr<fpListSize; fpNr++)
			{
				x.push_back(fpList[fpNr].time);
				y.push_back(fpList[fpNr].wp.altitude()); // in m
			}
		
			m_plotter.clear();
			m_plotter.setLabelX("time [s]");
			m_plotter.setLabelY("altitude [m]");
			m_plotter.plotXY(x, y, "Altitude vs Time");
		}
	}
}

void FlightWindow::plot_varioVsTime()
{
	QByteArray igcData;
	QTime time;
	IGCFileParser igcParser;
	FlightPointList fpList;
	GnuPlot::TimeVectorType x;
	GnuPlot::PlotVectorType y;
	uint fpNr;
	uint fpListSize;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		if(m_pDb->igcFile(getTable()->text(row, Nr).toInt(), igcData))
		{
			igcParser.parse(igcData);
			fpList = igcParser.flightPointList();
			fpListSize = fpList.size() - 1;
			
			for(fpNr=0; fpNr<fpListSize; fpNr++)
			{
				x.push_back(fpList[fpNr].time);
				y.push_back(fpList.speedV(fpNr, fpNr+1)); // in m/s
			}
		
			m_plotter.clear();
			m_plotter.setLabelX("time [s]");
			m_plotter.setLabelY("vario [m/s]");
			m_plotter.plotXY(x, y, "Vario vs Time");
		}
	}
}

void FlightWindow::plot_OLC()
{
	QByteArray igcData;
	QString title;
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	OLCOptimizer::FlightPointIndexListType fpIndexList;
	FlightPointList fpList;
	uint fpNr;
	uint dist;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		if(m_pDb->igcFile(getTable()->text(row, Nr).toInt(), igcData))
		{
			igcParser.parse(igcData);

			m_plotter.clear();
			m_plotter.setLabelX("latitude [deg.min]");
			m_plotter.setLabelY("longitude [deg.min]");
			m_plotter.setLabelZ("altitude [m]");
			plotFlighPointList(igcParser.flightPointList(), "track");
			
			olcOptimizer.setFlightPoints(igcParser.flightPointList(), 5); // ignore deltaSpeeds under 5 m/s
			
			if(olcOptimizer.optimize())
			{
				// fai triangle
				dist = olcOptimizer.FAITriangle(fpIndexList);
				fpList.clear();
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[2]));
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[3]));
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
				title.sprintf("fai triangle: %.3f km (%.2f pts)", dist/1000.0, dist/1000.0*1.75);
				plotFlighPointList(fpList, title);
				
				// flat triangle
				dist = olcOptimizer.flatTriangle(fpIndexList);
				fpList.clear();
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[2]));
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[3]));
				fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
				title.sprintf("flat triangle: %.3f km (%.2f pts)", dist/1000.0, dist/1000.0*2.0);
				plotFlighPointList(fpList, title);
				
				// free distance
				dist = olcOptimizer.freeDistance(fpIndexList);
				fpList.clear();
				
				for(fpNr=0; fpNr<FLIGHT_POINT_INDEX_LIST_SIZE; fpNr++)
				{
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[fpNr]));
				}
				
				title.sprintf("free distance: %.3f km (%.2f pts)", dist/1000.0, dist/1000.0*1.5);
				plotFlighPointList(fpList, title);
			}
		}
	}
}

void FlightWindow::plotFlighPointList(FlightPointList &fpList, const QString& title)
{
	GnuPlot::PlotVectorType x;
	GnuPlot::PlotVectorType y;
	GnuPlot::PlotVectorType z;
	uint fpListSize;
	uint fpNr;
	
	fpListSize = fpList.size();
			
	for(fpNr=0; fpNr<fpListSize; fpNr++)
	{
		y.push_back(fpList[fpNr].wp.latitude());
		x.push_back(fpList[fpNr].wp.longitude());
		z.push_back(fpList[fpNr].wp.altitude());
	}
	
	m_plotter.plotXYZ(x, y, z, title);
}

void FlightWindow::plot_3d()
{
	QByteArray arr;
	QString fileName = "/tmp/track.igc";
	QFile file;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		if(m_pDb->igcFile(getTable()->text(row, Nr).toInt(), arr))
		{
			file.setName(fileName);
				
			if(file.open(IO_WriteOnly))
			{
				file.writeBlock(arr);
				file.close();
			
				// show flight
				m_gpligc.clearArguments();
				m_gpligc.addArgument("openGLIGCexplorer");
				m_gpligc.addArgument(fileName);
				Error::verify(m_gpligc.start(), Error::GPLIGC_OPEN);
			}
		}
	}
}
