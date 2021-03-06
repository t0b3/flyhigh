/***************************************************************************
 *   Copyright (C) 2005 by Alex Graf                                       *
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

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QStatusBar>
#include "AirSpaceWindow.h"
#include "CredentialsDlg.h"
#include "DeviceConnectionParameters.h"
#include "GliderWindow.h"
#include "AccountWindow.h"
#include "FlightWindow.h"
#include "FlightExpWindow.h"
#include "FlyHighSettingsDlg.h"
#include "IDataBase.h"
#include "IGPSDevice.h"
#include "IFlightForm.h"
#include "IFlytecConfig.h"
#include "IFlytec6015Config.h"
#include "IFlyHighRC.h"
#include "IPilotInfoFrame.h"
#include "IRouteForm.h"
#include "ISql.h"
#include "MDIWindow.h"
#include "MigrationDlg.h"
#include "ProgressDlg.h"
#include "RouteWindow.h"
#include "ServicingWindow.h"
#include "WayPointWindow.h"
#include "WebMapRouteView.h"
#include "MainWindow.h"

MainWindow::MainWindow()
//	:QMainWindow(0, Qt::WDestructiveClose)
  :QMainWindow(0)
{
  QPalette palette;
  QAction *pAction;

  createAndConnectDb();

  // Workspace
  m_pMdiArea = new QMdiArea(this);
  setCentralWidget(m_pMdiArea);

  palette.setColor(m_pMdiArea->backgroundRole(), Qt::lightGray);
  m_pMdiArea->setPalette(palette);

  connect(m_pMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowActivated(QMdiSubWindow*)));

  m_pWinMapper = new QSignalMapper(this);
  connect(m_pWinMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

  QMainWindow::setWindowTitle("FlyHigh");

  // Menu File
  m_pMenuFile = menuBar()->addMenu(tr("&File"));

  pAction = new QAction(tr("New &Flight..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(file_newFlight()));
  m_pMenuFile->addAction(pAction);

  pAction = new QAction(tr("New &Route..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(file_newRoute()));
  m_pMenuFile->addAction(pAction);
  m_pMenuFile->addSeparator();

  pAction = new QAction(tr("&Quit"), this);
  connect(pAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
  m_pMenuFile->addAction(pAction);

  // Analysis
  m_pMenuAnalysis = menuBar()->addMenu(tr("&Analysis"));

  pAction = new QAction(tr("&Flights (DB)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(flights_fromSQL()));
  m_pMenuAnalysis->addAction(pAction);

  pAction = new QAction(tr("Flights (G&PS)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(flights_fromGPS()));
  m_pMenuAnalysis->addAction(pAction);
  m_pMenuAnalysis->addSeparator();

  pAction = new QAction(tr("Flight &Stats"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(flights_experience()));
  m_pMenuAnalysis->addAction(pAction);
  m_pMenuAnalysis->addSeparator();

  pAction = new QAction(tr("&Gliders"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(analysis_gliders()));
  m_pMenuAnalysis->addAction(pAction);

  pAction = new QAction(tr("Se&rvicing"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(analysis_servicing()));
  m_pMenuAnalysis->addAction(pAction);

  // Preparation
  m_pMenuPreparation = menuBar()->addMenu(tr("&Preparation"));

  pAction = new QAction(tr("&Start/Land (DB)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(startLand_fromSQL()));
  m_pMenuPreparation->addAction(pAction);

  pAction = new QAction(tr("&Waypoints (DB)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(buoys_fromSQL()));
  m_pMenuPreparation->addAction(pAction);

  pAction = new QAction(tr("Waypoints (G&PS)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(buoys_fromGPS()));
  m_pMenuPreparation->addAction(pAction);

  pAction = new QAction(tr("Waypoints (&File)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(buoys_fromFile()));
  m_pMenuPreparation->addAction(pAction);

  // routes
  m_pMenuPreparation->addSeparator();

  pAction = new QAction(tr("&Routes (DB)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(routes_fromSQL()));
  m_pMenuPreparation->addAction(pAction);

  pAction = new QAction(tr("Routes (&GPS)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(routes_fromGPS()));
  m_pMenuPreparation->addAction(pAction);
  m_pMenuPreparation->addSeparator();

  pAction = new QAction(tr("&Airspaces (DB)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(airspaces_fromSQL()));
  m_pMenuPreparation->addAction(pAction);

  pAction = new QAction(tr("Air&spaces (GPS)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(airspaces_fromGPS()));
  m_pMenuPreparation->addAction(pAction);

  pAction = new QAction(tr("A&irspaces (File)"), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(airspaces_fromFile()));
  m_pMenuPreparation->addAction(pAction);

  // Menu Configuration
  m_pMenuSettings = menuBar()->addMenu(tr("&Settings"));

  pAction = new QAction(tr("&Configure FlyHigh..."), this);
  connect(pAction, SIGNAL(triggered()),this, SLOT(settings_configure_flyhigh()));
  m_pMenuSettings->addAction(pAction);

  pAction = new QAction(tr("&Setup Device..."), this);
  connect(pAction, SIGNAL(triggered()), SLOT(settings_configure_device()));
  m_pMenuSettings->addAction(pAction);

  pAction = new QAction(tr("Pilot &Info..."), this);
  connect(pAction, SIGNAL(triggered()), SLOT(settings_pilotInfo()));
  m_pMenuSettings->addAction(pAction);

  pAction = new QAction(tr("&Accounts..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(settings_accounts()));
  m_pMenuSettings->addAction(pAction);

  // Menu Tools
  m_pMenuTools = menuBar()->addMenu(tr("&Tools"));
  pAction = new QAction(tr("&Database migration..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(tools_migrateDB()));
  m_pMenuTools->addAction(pAction);

  // Menu Windows
  m_pMenuWindows = menuBar()->addMenu(tr("&Windows"));

  m_pCascade = new QAction(tr("&Cascade"), this);
  connect(m_pCascade, SIGNAL(triggered()), this, SLOT(cascadeSubWindows()));

  m_pTile = new QAction(tr("&Tile"), this);
  connect(m_pTile, SIGNAL(triggered()), m_pMdiArea, SLOT(tileSubWindows()));

  m_pTileHor = new QAction(tr("Tile &Horizontally"), this);
  connect(m_pTileHor, SIGNAL(triggered()), SLOT(windows_tile_horizontally()));

  m_pWinSeparator = new QAction(this);
  m_pWinSeparator->setSeparator(true);

  connect(m_pMenuWindows, SIGNAL(aboutToShow()), this, SLOT(updateMenuWindow()));

  // Menu Help
  menuBar()->addSeparator();
  m_pMenuHelp = menuBar()->addMenu(tr("&Help"));

  pAction = new QAction(tr("&About"), this);
  connect(pAction, SIGNAL(triggered()), SLOT(help_about()));
  m_pMenuHelp->addAction(pAction);

  statusBar()->showMessage(tr("Ready"), 2000);

  // if pilot info is not set
  if(IFlyHighRC::pInstance()->pilotId() < 0)
  {
    settings_pilotInfo();
  }
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *pEvent)
{
  QList<QMdiSubWindow*> winList;
  QWidget *pWin;
  uint nofWin;
  uint winNr;

  winList = m_pMdiArea->subWindowList();
  nofWin = winList.count();

  if(nofWin > 0)
  {
    for(winNr=0; winNr<nofWin; winNr++)
    {
      pWin = winList.at(winNr);

      if(!pWin->close())
      {
        pEvent->ignore();
        return;
      }
    }
  }

  QMainWindow::closeEvent(pEvent);
}

void MainWindow::file_newFlight()
{
  IFlightForm newFlightForm(this, tr("New Flight"));
  Pilot pilot;
  Flight flight;
  IDataBase *pDb;
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
  pDb = ISql::pInstance();

  if(newFlightForm.exec() && pDb->open())
  {
    ISql::pInstance()->add(flight);
    pDb->close();
  }
}

void MainWindow::file_newRoute()
{
  Route route;
  Route *pLatest = NULL;
  Route::RouteListType routeList;
  Route::RouteListType::iterator it;
  WayPoint wp;
  ProgressDlg progDlg(this);
  IDataBase *pDb;
  int maxId = -1;

  route.setName("New route");
  route.setType(Route::Straight);
  pDb = ISql::pInstance();

  if(pDb->open())
  {
    progDlg.beginProgress("read routes...", pDb);
    pDb->routeList(routeList);
    progDlg.endProgress();
    pDb->close();
  }

  for(it=routeList.begin(); it!=routeList.end(); it++)
  {
    if(maxId < (*it).id())
    {
      maxId = (*it).id();
      pLatest = &(*it);
    }
  }

  if((pLatest != NULL) && (pLatest->wayPointList().size() > 0))
  {
    // take two points around start of first route as initial
    wp = pLatest->wayPointList().at(0);
    wp.setLon(wp.lon() - 0.25);
    route.wayPointList().push_back(wp);
    wp = pLatest->wayPointList().at(0);
    wp.setLon(wp.lon() + 0.25);
    route.wayPointList().push_back(wp);
  }
  else
  {
    // default
    route.wayPointList().push_back(WayPoint(47.0, 8.5));
    route.wayPointList().push_back(WayPoint(47.0, 9.0));
  }

  newRoute(route, true);
}

void MainWindow::flights_fromSQL()
{
  MDIWindow *pWin;

  pWin = new FlightWindow(m_pMdiArea, "Flights (DB)", IDataBase::SqlDB);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::flights_fromGPS()
{
  MDIWindow* pWin;

  pWin = new FlightWindow(m_pMdiArea, "Flights (GPS)", IDataBase::GPSdevice);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::flights_experience()
{
  MDIWindow *pWin;

  pWin = new FlightExpWindow(m_pMdiArea, "Flight Experience");
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::analysis_gliders()
{
  MDIWindow* pWin;

  pWin = new GliderWindow(m_pMdiArea, "Glider");
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::analysis_servicing()
{
  MDIWindow* pWin;

  pWin = new ServicingWindow(m_pMdiArea, "Servicing");
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::startLand_fromSQL()
{
  MDIWindow* pWin;

  pWin = new WayPointWindow(m_pMdiArea, "Start/Land (DB)", IDataBase::SqlDB, WayPoint::TypeStartLand);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::buoys_fromSQL()
{
  MDIWindow* pWin;

  pWin = new WayPointWindow(m_pMdiArea, "Waypoints (DB)", IDataBase::SqlDB, WayPoint::TypeBuoy);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::buoys_fromFile()
{
  MDIWindow* pWin;

  pWin = new WayPointWindow(m_pMdiArea, "Waypoints (File)", IDataBase::File, WayPoint::TypeBuoy);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::buoys_fromGPS()
{
  MDIWindow* pWin;

  pWin = new WayPointWindow(m_pMdiArea, "Waypoints (GPS)", IDataBase::GPSdevice, WayPoint::TypeBuoy);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::routes_fromSQL()
{
  MDIWindow* pWin;

  pWin = new RouteWindow(m_pMdiArea, "Routes (DB)", IDataBase::SqlDB);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::routes_fromGPS()
{
  MDIWindow* pWin;

  pWin = new RouteWindow(m_pMdiArea, "Routes (GPS)", IDataBase::GPSdevice);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::airspaces_fromSQL()
{
  MDIWindow* pWin;

  pWin = new AirSpaceWindow(m_pMdiArea, "Airspaces (DB)", IDataBase::SqlDB);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::airspaces_fromGPS()
{
  MDIWindow* pWin;

  pWin = new AirSpaceWindow(m_pMdiArea, "Airspaces (GPS)", IDataBase::GPSdevice);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::airspaces_fromFile()
{
  MDIWindow* pWin;

  pWin = new AirSpaceWindow(m_pMdiArea, "Airspaces (File)", IDataBase::File);
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);
}

void MainWindow::settings_configure_device()
{
  IFlytecConfig *pFrame;
  IFlytec6015Config *pConfig6015;

  switch(IFlyHighRC::pInstance()->deviceNameIndex())
  {
    case IFlyHighRC::DevFlytec5020:
    case IFlyHighRC::DevFlytec6020:
      pFrame = new IFlytecConfig();
      pFrame->show();
    break;
    case IFlyHighRC::DevFlytec6015:
      pConfig6015 = new IFlytec6015Config();
      pConfig6015->show();
    break;
      /*
      handle here other devices
      case 1:
      break;
      */
  }
}

void MainWindow::settings_configure_flyhigh()
{
  FlyHighSettingsDlg dlg(this);

  if(dlg.exec() == QDialog::Accepted)
  {
    // save values
    const DatabaseParameters dbparams(dlg.getDBParameters());
    dbparams.writeToConfig();
    const DeviceConnectionParameters devparams(dlg.getDeviceConnectionParameters());
    devparams.writeToConfig();

    // notify database mngr
    ISql::pInstance()->setDBParameters(dbparams);
    createAndConnectDb();

    // todo: notify open windows
  }
}

void MainWindow::settings_pilotInfo()
{
  IPilotInfoFrame rcFrame;

  rcFrame.show();
}

void MainWindow::settings_accounts()
{
  MDIWindow* pWin;

  pWin = new AccountWindow(m_pMdiArea, "Accounts");
  connect(pWin, SIGNAL(message(const QString&, int)), statusBar(), SLOT(showMessage(const QString&, int)));
  showWindow(pWin);

}

void MainWindow::tools_migrateDB()
{
  MigrationDlg dlg(this);

  dlg.exec();
}

void MainWindow::windows_tile_horizontally()
{
  QList<QMdiSubWindow*> winList;
  QWidget *pWin;
  uint winNr;
  uint nofWin;
  int heightForEach;
  int y = 0;

  winList = m_pMdiArea->subWindowList();

  // if 0 or 1 windows do normal tiling
  if(winList.count() < 2)
  {
    m_pMdiArea->tileSubWindows();
    return;
  }

  // primitive horizontal tiling
  nofWin = winList.count();
  heightForEach = m_pMdiArea->height() / nofWin;

  for(winNr= 0; winNr<nofWin; winNr++)
  {
    pWin = winList.at(winNr);

    if(pWin->isMaximized())
    {
      // prevent flicker
      pWin->hide();
      pWin->showNormal();
    }

    // set new height
    pWin->resize(m_pMdiArea->width(), heightForEach);
    pWin->move(0,y);
    y += heightForEach;
  }
}

void MainWindow::help_about()
{
  QMessageBox::about(this, IFlyHighRC::pInstance()->versionInfo(),
  IFlyHighRC::pInstance()->copyInfo());
}

void MainWindow::updateMenuWindow()
{
  QList<QMdiSubWindow*> winList;
  QString text;
  QAction *pAction;
  MDIWindow *pChild;
  int childNr;
  bool hasMdiChild;

  m_pMenuWindows->clear();

  // window behaviour
  m_pMenuWindows->addAction(m_pCascade);
  m_pMenuWindows->addAction(m_pTile);
  m_pMenuWindows->addAction(m_pTileHor);
  m_pMenuWindows->addAction(m_pWinSeparator);

  // set enabled
  hasMdiChild = !m_pMdiArea->subWindowList().isEmpty();
  m_pCascade->setEnabled(hasMdiChild);
  m_pTile->setEnabled(hasMdiChild);
  m_pTileHor->setEnabled(hasMdiChild);

  // windows list
  winList = m_pMdiArea->subWindowList();
  m_pWinSeparator->setVisible(!winList.isEmpty());

  for(childNr=0; childNr<winList.size(); childNr++)
  {
    pChild = qobject_cast<MDIWindow*>(winList.at(childNr));

    if(childNr < 9)
    {
      text = QString("&%1 %2").arg(childNr + 1).arg(pChild->windowTitle());
    }
    else
    {
      text = QString("%1 %2").arg(childNr + 1).arg(pChild->windowTitle());
    }

    pAction = m_pMenuWindows->addAction(text);
    pAction->setCheckable(true);
    pAction ->setChecked(pChild == activeMdiChild());
    connect(pAction, SIGNAL(triggered()), m_pWinMapper, SLOT(map()));
    m_pWinMapper->setMapping(pAction, winList.at(childNr));
  }
}

void MainWindow::setActiveSubWindow(QWidget *pWin)
{
  if(pWin != NULL)
  {
    m_pMdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(pWin));
  }
}

void MainWindow::subWindowActivated(QMdiSubWindow *pSubWin)
{
  MDIWindow *pWin;

  menuBar()->clear();
  menuBar()->addMenu(m_pMenuFile);
  pWin = qobject_cast<MDIWindow*>(pSubWin);

  if(pWin != NULL)
  {
    menuBar()->addMenu(pWin->menu());
  }

  menuBar()->addMenu(m_pMenuAnalysis);
  menuBar()->addMenu(m_pMenuPreparation);
  menuBar()->addMenu(m_pMenuSettings);
  menuBar()->addMenu(m_pMenuTools);
  menuBar()->addMenu(m_pMenuWindows);
  menuBar()->addMenu(m_pMenuHelp);
}

void MainWindow::cascadeSubWindows()
{
  QList<QMdiSubWindow*> winList;
  MDIWindow *pWin;
  uint winNr;
  uint nofWin;

  m_pMdiArea->cascadeSubWindows();
  winList = m_pMdiArea->subWindowList();
  nofWin = winList.count();

  for(winNr= 0; winNr<nofWin; winNr++)
  {
    pWin = qobject_cast<MDIWindow*>(winList.at(winNr));
    resizeSubWindow(pWin);
  }
}

void MainWindow::showWindow(MDIWindow *pWin)
{
  MDIWindow *pActWin = NULL;

  pActWin = activeMdiChild();
  m_pMdiArea->addSubWindow(pWin);
  resizeSubWindow(pWin);

  if((pActWin == NULL) || pActWin->isMaximized())
  {
    pWin->showMaximized();
  }
  else
  {
    pWin->show();
  }
}

MDIWindow* MainWindow::activeMdiChild()
{
  MDIWindow *pWin = NULL;
  QMdiSubWindow *pActWin;

  pActWin = m_pMdiArea->activeSubWindow();

  if(pActWin != NULL)
  {
    pWin = qobject_cast<MDIWindow*>(pActWin);
  }

  return pWin;
}

void MainWindow::resizeSubWindow(MDIWindow *pWin)
{
  pWin->resize(m_pMdiArea->width() / 2, m_pMdiArea->height() / 2);
}

bool MainWindow::createAndConnectDb()
{
  bool success = false;

  if(!ISql::pInstance()->connectDb())
  {
    if(ISql::pInstance()->reqPwdForCreateDb())
    {
      CredentialsDlg dlg(NULL, QObject::tr("Please enter MySQL administrator credentials"));
      dlg.setUsername("root");

      if(dlg.exec() == QDialog::Accepted)
      {
        ISql::pInstance()->createDb(dlg.getUsername(), dlg.getPassword());
      }
    }
    else
    {
      ISql::pInstance()->createDb();
    }

    success = ISql::pInstance()->connectDb();

    if(!success)
    {
      qDebug() << "Database connection failed";
    }
  }

  return success;
}

void MainWindow::newRoute(Route &route, bool glue)
{
  WebMapRouteView *pView;
  AirSpaceList airSpaceList;
  IDataBase *pDb;

  pView = new WebMapRouteView(tr("Add Route to DB"));
  pView->setRoute(&route);
  ISql::pInstance()->airspaceList(airSpaceList);
  pView->setAirSpaceList(&airSpaceList);
  pView->setGlueToCenter(glue);
  pView->loadMap();
  pDb = ISql::pInstance();

  if((pView->exec() == QDialog::Accepted) && pDb->open())
  {
    // save new route
    pDb->add(route.wayPointList());
    pDb->add(route);
    pDb->close();
  }
}
