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
#include <qdir.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtable.h>
#include <qdatetime.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qtimer.h>
#include <qwidget.h>
#include "IFlyHighRC.h"
#include "Error.h"
#include "FlightWindow.h"
#include "Glider.h"
#include "WayPoint.h"
#include "IGPSDevice.h"
#include "IGliderForm.h"
#include "IFlightForm.h"
#include "IFlyHighRC.h"
#include "IWayPointForm.h"
#include "ISql.h"
#include "Flight.h"
#include "Images.h"
#include "IGCFileParser.h"
#include "KmlWriter.h"
#include "OLCOptimizer.h"
#include "OLCWebForm.h"
#include "ProgressDlg.h"
#include "MapView.h"

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
	
	pMenu->insertItem("&Export IGC...", this, SLOT(file_exportIGC()));
	pMenu->insertItem("&Export KML...", this, SLOT(file_exportKML()));
	pMenu->insertItem("&Export all...", this, SLOT(exportTable()));
	
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
	pMenu->insertItem("&Map View", this, SLOT(showOnMap()));
	
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
	pTable->setColumnWidth(Date, 115);
	pTable->setColumnWidth(Time, 95);
	pTable->setColumnWidth(Duration, 95);
	pTable->setColumnWidth(Model, 175);
	pTable->setColumnWidth(StartPt, 150);
	pTable->setColumnWidth(LandPt, 150);
	pTable->setColumnWidth(Distance, 70);
	pTable->setColumnWidth(Comment, 1000);
	
	// menu
/*
	pMenu = new QPopupMenu(this);
	menuBar()->insertItem("&Flights", pMenu);
	pMenu->insertItem("Update", this, SLOT(tracks_update()));
	pMenu->insertItem("Read...", this, SLOT(tracks_read()));
*/

	m_fileName = "";
	m_lastModified = 0;
}

bool FlightWindow::periodicalUpdate()
{
	int lastModified;

	lastModified = m_pDb->flightsLastModified();
	
	if(m_lastModified < lastModified)
	{
		file_update();
		m_lastModified = lastModified;
	}
	
	return true;
}

void FlightWindow::file_update()
{
	Pilot pilot;
	QTable *pTable = TableWindow::getTable();
	ProgressDlg progDlg(this);
	uint flightNr;
	uint maxFlightNr;
	
	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), pilot);
	m_flightList.clear();
	progDlg.beginProgress("reading flights...", m_pDb);
	
	if(m_pDb->flightList(pilot, m_flightList))
	{
		maxFlightNr = m_flightList.size();
		pTable->setNumRows(maxFlightNr);
		
		for(flightNr=0; flightNr<maxFlightNr; flightNr++)
		{
			setFlightToRow(flightNr, m_flightList[flightNr]);
		}
	}
	
	progDlg.endProgress();
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

	flight.glider().fullName(str);
	pTable->setText(row, Model, str);
	flight.startPt().fullName(str);
	pTable->setText(row, StartPt, str);
	flight.landPt().fullName(str);
	pTable->setText(row, LandPt, str);
	
	str.sprintf("%.3f", flight.distance()/1000.0);
	pTable->setText(row, Distance, str);
	
	pTable->setText(row, Comment, flight.comment());
}

void FlightWindow::file_AddToSqlDB()
{
	IFlightForm newFlightForm(this, "New Flight");
	ProgressDlg progDlg(this);
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	QTime time;
	QDate date;
	Flight flight;
	Glider glider;
	Pilot pilot;
	WayPoint wp;
	OLCOptimizer::FlightPointIndexListType fpIndexList;
	QString str;
	int row;
	uint gpsFlightNr;
	uint newFlightNr;
	int id;

	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		// pilot info
		ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), pilot);
	
		// load igc data
		progDlg.beginProgress("read igc file...", m_pDb);
		m_pDb->loadIGCFile(m_flightList[row]);
		progDlg.endProgress();
	
		// parse and optimize
		igcParser.parse(m_flightList[row].igcData());
		olcOptimizer.setFlightPoints(igcParser.flightPointList(), 100, 200);
		progDlg.beginProgress("optimize flight...", &olcOptimizer);
		
		if(olcOptimizer.optimize())
		{
			// distance
			m_flightList[row].setDistance(olcOptimizer.freeDistance(fpIndexList));
		}
		
		progDlg.endProgress();
		
		// nr
		gpsFlightNr = getTable()->text(row, Nr).toUInt(); // store this for restore
		newFlightNr = ISql::pInstance()->newFlightNr(pilot);
		m_flightList[row].setNumber(newFlightNr);
		
		// start
		id = igcParser.flightPointList().firstValidFlightData();
		
		if(id >= 0)
		{
			// start time
			time = igcParser.flightPointList().at(id).time.addSecs(IFlyHighRC::pInstance()->utcOffset() * 3600);
			m_flightList[row].setTime(time);
		
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
			m_flightList[row].setStartPt(wp);
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
			m_flightList[row].setLandPt(wp);
		}

		// glider
		if(!ISql::pInstance()->glider(igcParser.model(), glider))
		{
			IGliderForm newGlider(this, "New Glider", &glider);
			
			if(newGlider.exec())
			{
				ISql::pInstance()->add(glider);
			}
		}

		m_flightList[row].setGlider(glider);
		
		// a new flight
		newFlightForm.setFlight(&m_flightList[row]);
		
		if(newFlightForm.exec())
		{
			ISql::pInstance()->add(m_flightList[row]);
			m_flightList[row].setNumber(gpsFlightNr); // restore original track nr or gps will be confused
			setFlightToRow(row, m_flightList[row]);
		}
	}
}

void FlightWindow::file_import()
{
	IFlightForm newFlightForm(this, "New Flight");
	ProgressDlg progDlg(this);
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	const QDir *pDir;
	QTime time;
	QDate date;
	Flight flight;
	Glider glider;
	Pilot pilot;
	WayPoint wp;
	FlightPointList fpList;
	OLCOptimizer::FlightPointIndexListType fpIndexList;
	QString fileName;
	QFile file;
	int nr;
	int startPtId;
	int landPtId;
	int duration;

	QFileDialog fileDlg(IFlyHighRC::pInstance()->lastDir(), "IGC Files (*.igc; *.IGC)", this,
			"IGC file import", true);

	if(fileDlg.exec() == QDialog::Accepted)
	{
		// pilot info
		ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), pilot);

		pDir = fileDlg.dir();
		IFlyHighRC::pInstance()->setLastDir(pDir->absPath());
		delete pDir;
		file.setName(fileDlg.selectedFile());
		
		if(file.open(IO_ReadOnly))
		{
			flight.setIgcData(file.readAll());
			file.close();
		
			// parse and optimize
			igcParser.parse(flight.igcData());
			olcOptimizer.setFlightPoints(igcParser.flightPointList(), 100, 200);
			progDlg.beginProgress("optimize flight...", &olcOptimizer);
			
			if(olcOptimizer.optimize())
			{
				// distance
				flight.setDistance(olcOptimizer.freeDistance(fpIndexList));
			}
			
			progDlg.endProgress();
	
			// nr
			nr = ISql::pInstance()->newFlightNr(pilot);
			flight.setNumber(nr);
			
			// date
			flight.setDate(igcParser.date());
		
			// start
			startPtId = igcParser.flightPointList().firstValidFlightData();
			
			if(startPtId >= 0)
			{
				// add UTC offset to start time
				time = igcParser.flightPointList().at(startPtId).time.addSecs(IFlyHighRC::pInstance()->utcOffset() * 3600);
				flight.setTime(time);
				
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
				flight.setStartPt(wp);
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
				flight.setLandPt(wp);
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
			flight.setGlider(glider);
		
			// a new flight
			newFlightForm.setFlight(&flight);
			
			if(newFlightForm.exec())
			{
				ISql::pInstance()->add(flight);
			}
		}
	}
}

void FlightWindow::file_exportIGC()
{
	const QDir *pDir;
	QFile file;
	QString name;
	QString fileName;
	QString line;
	QString comment;
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	OLCOptimizer::FlightPointIndexListType fpIndexListFree;
	OLCOptimizer::FlightPointIndexListType fpIndexListFAI;
	OLCOptimizer::FlightPointIndexListType fpIndexListFlat;
	OLCWebForm olcWebForm;
	FlightPointList fpList;
	ProgressDlg progDlg(this);
	double distFree;
	double distFAI;
	double distFlat;
	double ptsFree;
	double ptsFAI;
	double ptsFlat;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		if(m_pDb->loadIGCFile(m_flightList[row]))
		{
			// IGC file
			igcParser.parse(m_flightList[row].igcData());
			
			QFileDialog fileDlg(IFlyHighRC::pInstance()->lastDir(), "IGC Files (*.igc)", this,
					"IGC file export", true);

			olcWebForm.setFlight(m_flightList[row]);
			olcWebForm.olcFileName(fileName);
			fileDlg.setSelection(fileName);
			fileDlg.setMode(QFileDialog::AnyFile);

			if(fileDlg.exec() == QDialog::Accepted)
			{
				pDir =  fileDlg.dir();
				IFlyHighRC::pInstance()->setLastDir(pDir->absPath());
				delete pDir;
				fileName = fileDlg.selectedFile();
				file.setName(fileName + ".igc");
				
				if(file.open(IO_WriteOnly))
				{
					file.writeBlock(m_flightList[row].igcData());
					file.close();
				}
				
				// OLC file
				olcOptimizer.setFlightPoints(igcParser.flightPointList(), 100, 200);
				progDlg.beginProgress("optimize flight...", &olcOptimizer);
				
				if(olcOptimizer.optimize())
				{
					distFree = olcOptimizer.freeDistance(fpIndexListFree) / 1000.0;
					ptsFree = distFree * 1.5;
					distFAI = olcOptimizer.FAITriangle(fpIndexListFAI) / 1000.0;
					ptsFAI = distFAI  * 2.0;
					distFlat = olcOptimizer.flatTriangle(fpIndexListFlat) / 1000.0;
					ptsFlat = distFlat * 1.75;
					
					if((ptsFree > ptsFAI) && (ptsFree > ptsFlat))
					{
						olcWebForm.setDeparture(olcOptimizer.flyPointList().at(fpIndexListFree[0]));
						olcWebForm.set1stWayPoint(olcOptimizer.flyPointList().at(fpIndexListFree[1]).wp);
						olcWebForm.set2ndWayPoint(olcOptimizer.flyPointList().at(fpIndexListFree[2]).wp);
						olcWebForm.set3rdWayPoint(olcOptimizer.flyPointList().at(fpIndexListFree[3]).wp);
						olcWebForm.setFinish(olcOptimizer.flyPointList().at(fpIndexListFree[4]));
					}
					else if(ptsFlat > ptsFAI)
					{
						olcWebForm.setDeparture(olcOptimizer.flyPointList().at(fpIndexListFlat[0]));
						olcWebForm.set1stWayPoint(olcOptimizer.flyPointList().at(fpIndexListFlat[1]).wp);
						olcWebForm.set2ndWayPoint(olcOptimizer.flyPointList().at(fpIndexListFlat[2]).wp);
						olcWebForm.set3rdWayPoint(olcOptimizer.flyPointList().at(fpIndexListFlat[3]).wp);
						olcWebForm.setFinish(olcOptimizer.flyPointList().at(fpIndexListFlat[4]));
					}
					else
					{
						olcWebForm.setDeparture(olcOptimizer.flyPointList().at(fpIndexListFAI[0]));
						olcWebForm.set1stWayPoint(olcOptimizer.flyPointList().at(fpIndexListFAI[1]).wp);
						olcWebForm.set2ndWayPoint(olcOptimizer.flyPointList().at(fpIndexListFAI[2]).wp);
						olcWebForm.set3rdWayPoint(olcOptimizer.flyPointList().at(fpIndexListFAI[3]).wp);
						olcWebForm.setFinish(olcOptimizer.flyPointList().at(fpIndexListFAI[4]));
					}
					
					olcWebForm.save(fileName + ".html");
				}
				
				progDlg.endProgress();
			}
		}
	}
}

void FlightWindow::file_exportKML()
{
	const QDir *pDir;
	QFile file;
	QString name;
	QString fileName;
	QString line;
	QString comment;
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	OLCOptimizer::FlightPointIndexListType fpIndexListFree;
	OLCOptimizer::FlightPointIndexListType fpIndexListFAI;
	OLCOptimizer::FlightPointIndexListType fpIndexListFlat;
	KmlWriter kmlWriter;
	FlightPointList fpList;
	
	ProgressDlg progDlg(this);
	double distFree;
	double distFAI;
	double distFlat;
	double ptsFree;
	double ptsFAI;
	double ptsFlat;
	int row;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		if(m_pDb->loadIGCFile(m_flightList[row]))
		{
			// IGC file
			igcParser.parse(m_flightList[row].igcData());
			QFileDialog fileDlg(IFlyHighRC::pInstance()->lastDir(), "KML Files (*.kml)", this,
					"KML file export", true);

			fileName = m_flightList[row].startPt().name();
			fileName += m_flightList[row].date().toString("_dd_MM_yyyy");
			fileDlg.setSelection(fileName);
			fileDlg.setMode(QFileDialog::AnyFile);

			if(fileDlg.exec() == QDialog::Accepted)
			{
				pDir =  fileDlg.dir();
				IFlyHighRC::pInstance()->setLastDir(pDir->absPath());
				delete pDir;
				fileName = fileDlg.selectedFile();

				// OLC file
				olcOptimizer.setFlightPoints(igcParser.flightPointList(), 100, 200);
				progDlg.beginProgress("optimize flight...", &olcOptimizer);
				
				if(olcOptimizer.optimize())
				{
					distFree = olcOptimizer.freeDistance(fpIndexListFree) / 1000.0;
					ptsFree = distFree * 1.5;
					distFAI = olcOptimizer.FAITriangle(fpIndexListFAI) / 1000.0;
					ptsFAI = distFAI  * 2.0;
					distFlat = olcOptimizer.flatTriangle(fpIndexListFlat) / 1000.0;
					ptsFlat = distFlat * 1.75;
					
					if((ptsFree > ptsFAI) && (ptsFree > ptsFlat))
					{
						kmlWriter.setDeparture(olcOptimizer.flyPointList().at(fpIndexListFree[0]));
						kmlWriter.set1stWayPoint(olcOptimizer.flyPointList().at(fpIndexListFree[1]).wp);
						kmlWriter.set2ndWayPoint(olcOptimizer.flyPointList().at(fpIndexListFree[2]).wp);
						kmlWriter.set3rdWayPoint(olcOptimizer.flyPointList().at(fpIndexListFree[3]).wp);
						kmlWriter.setFinish(olcOptimizer.flyPointList().at(fpIndexListFree[4]));
						kmlWriter.setTriangle(false);
					}
					else if(ptsFlat > ptsFAI)
					{
						kmlWriter.setDeparture(olcOptimizer.flyPointList().at(fpIndexListFlat[0]));
						kmlWriter.set1stWayPoint(olcOptimizer.flyPointList().at(fpIndexListFlat[1]).wp);
						kmlWriter.set2ndWayPoint(olcOptimizer.flyPointList().at(fpIndexListFlat[2]).wp);
						kmlWriter.set3rdWayPoint(olcOptimizer.flyPointList().at(fpIndexListFlat[3]).wp);
						kmlWriter.setFinish(olcOptimizer.flyPointList().at(fpIndexListFlat[4]));
						kmlWriter.setTriangle(true);
					}
					else
					{
						kmlWriter.setDeparture(olcOptimizer.flyPointList().at(fpIndexListFAI[0]));
						kmlWriter.set1stWayPoint(olcOptimizer.flyPointList().at(fpIndexListFAI[1]).wp);
						kmlWriter.set2ndWayPoint(olcOptimizer.flyPointList().at(fpIndexListFAI[2]).wp);
						kmlWriter.set3rdWayPoint(olcOptimizer.flyPointList().at(fpIndexListFAI[3]).wp);
						kmlWriter.setFinish(olcOptimizer.flyPointList().at(fpIndexListFAI[4]));
						kmlWriter.setTriangle(true);
					}

					kmlWriter.setFlight(m_flightList[row]);
					kmlWriter.setFlightPoints(igcParser.flightPointList());
					kmlWriter.save(fileName + ".kml");
				}
				
				progDlg.endProgress();
			}
		}
	}
}

void FlightWindow::file_new()
{
	IFlightForm newFlightForm(this, "New Flight");
	Pilot pilot;
	Flight flight;
	int nr;

	// pilot info
	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), pilot);
	
	// nr
	nr = ISql::pInstance()->newFlightNr(pilot);
	flight.setNumber(nr);
	
	// current date and time
	flight.setDate(QDate::currentDate());
	flight.setTime(QTime::currentTime());
	flight.setDuration(QTime(0, 0, 0));

	// current glider
	flight.setGlider(pilot.glider());
	
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
		m_pDb->delFlight(m_flightList[row]);
	}
}

void FlightWindow::plot_speedVsTime()
{
	QTime time;
	IGCFileParser igcParser;
	FlightPointList fpList;
	ProgressDlg progDlg(this);
	GnuPlot::TimeVectorType x;
	GnuPlot::PlotVectorType y;
	uint fpNr;
	uint fpListSize;
	int row;
	bool success;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		progDlg.beginProgress("read igc file...", m_pDb);
		success = m_pDb->loadIGCFile(m_flightList[row]);
		progDlg.endProgress();
	
		if(success)
		{
			igcParser.parse(m_flightList[row].igcData());
			fpList = igcParser.flightPointList();
			fpListSize = fpList.size();
			
			if(fpListSize > 0)
			{
				for(fpNr=0; fpNr<fpListSize; fpNr++)
				{
					x.push_back(fpList[fpNr].time);
					y.push_back(fpList.speedH(fpNr, fpNr+1)*3.6); // in km/h
				}
			
				m_plotter.clear();
				m_plotter.setLabelX("time [s]");
				m_plotter.setLabelY("speed [km/h]");
				m_plotter.plotXY(x, y, "Speed vs Time");
			}
		}
		
		TableWindow::unsetCursor();
	}
}

void FlightWindow::plot_altVsTime()
{
	QTime time;
	IGCFileParser igcParser;
	FlightPointList fpList;
	ProgressDlg progDlg(this);
	GnuPlot::TimeVectorType x;
	GnuPlot::PlotVectorType y;
	uint fpNr;
	uint fpListSize;
	int row;
	bool success;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		progDlg.beginProgress("read igc file...", m_pDb);
		success = m_pDb->loadIGCFile(m_flightList[row]);
		progDlg.endProgress();
		
		if(success)
		{
			igcParser.parse(m_flightList[row].igcData());
			fpList = igcParser.flightPointList();
			fpListSize = fpList.size();
			
			if(fpListSize > 0)
			{
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
		
		TableWindow::unsetCursor();
	}
}

void FlightWindow::plot_varioVsTime()
{
	QTime time;
	IGCFileParser igcParser;
	FlightPointList fpList;
	GnuPlot::TimeVectorType x;
	GnuPlot::PlotVectorType y;
	ProgressDlg progDlg(this);
	uint fpNr;
	uint fpListSize;
	int row;
	bool success;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		progDlg.beginProgress("read igc file...", m_pDb);
		success = m_pDb->loadIGCFile(m_flightList[row]);
		progDlg.endProgress();
		
		if(success)
		{
			igcParser.parse(m_flightList[row].igcData());
			fpList = igcParser.flightPointList();
			fpListSize = fpList.size();
			
			if(fpListSize > 0)
			{
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
		
		TableWindow::unsetCursor();
	}
}

void FlightWindow::plot_OLC()
{
	QString title;
	IGCFileParser igcParser;
	OLCOptimizer olcOptimizer;
	OLCOptimizer::FlightPointIndexListType fpIndexList;
	FlightPointList fpList;
	ProgressDlg progDlg(this);
	uint fpNr;
	uint dist;
	int row;
	bool success;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		progDlg.beginProgress("read igc file...", m_pDb);
		success = m_pDb->loadIGCFile(m_flightList[row]);
		progDlg.endProgress();
		
		if(success)
		{
			igcParser.parse(m_flightList[row].igcData());
	
			if(igcParser.flightPointList().size() > 0)
			{
				m_plotter.clear();
				m_plotter.setLabelX("latitude [deg.min]");
				m_plotter.setLabelY("longitude [deg.min]");
				m_plotter.setLabelZ("altitude [m]");
				plotFlighPointList(igcParser.flightPointList(), "track");
				
				olcOptimizer.setFlightPoints(igcParser.flightPointList(), 100, 200);
				progDlg.beginProgress("optimize flight...", &olcOptimizer);
				
				if(olcOptimizer.optimize())
				{
					// fai triangle
					dist = olcOptimizer.FAITriangle(fpIndexList);
					fpList.clear();
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[2]));
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[3]));
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
					title.sprintf("fai triangle: %.3f km (%.2f pts)", dist/1000.0, dist/1000.0*1.4);
					plotFlighPointList(fpList, title);
					
					// flat triangle
					dist = olcOptimizer.flatTriangle(fpIndexList);
					fpList.clear();
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[2]));
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[3]));
					fpList.add(olcOptimizer.flyPointList().at(fpIndexList[1]));
					title.sprintf("flat triangle: %.3f km (%.2f pts)", dist/1000.0, dist/1000.0*1.2);
					plotFlighPointList(fpList, title);
					
					// free distance
					dist = olcOptimizer.freeDistance(fpIndexList);
					fpList.clear();
					
					for(fpNr=0; fpNr<FLIGHT_POINT_INDEX_LIST_SIZE; fpNr++)
					{
						fpList.add(olcOptimizer.flyPointList().at(fpIndexList[fpNr]));
					}
					
					title.sprintf("free distance: %.3f km (%.2f pts)", dist/1000.0, dist/1000.0*1.0);
					plotFlighPointList(fpList, title);
				}

				progDlg.endProgress();
			}
		}
		
		TableWindow::unsetCursor();
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

void FlightWindow::showOnMap()
{
	QString title;
	IGCFileParser igcParser;
	ProgressDlg progDlg(this);
	int row;
	bool success;
	MapView *pView;
	WayPoint::WayPointListType wpList;
	uint fpListSize;
	uint fpNr;
	
	row = getTable()->currentRow();
	
	if(row >= 0)
	{
		progDlg.beginProgress("read igc file...", m_pDb);
		success = m_pDb->loadIGCFile(m_flightList[row]);
		progDlg.endProgress();
		
		if(success)
		{
			igcParser.parse(m_flightList[row].igcData());
			fpListSize = igcParser.flightPointList().size();

			if(fpListSize > 0)
			{
				for(fpNr=0; fpNr<fpListSize; fpNr++)
				{
					wpList.push_back(igcParser.flightPointList().at(fpNr).wp);
				}

				pView = new MapView();
				pView->shwoWayPointList(wpList);
				pView->showMaximized();
			}
		}
	}
}