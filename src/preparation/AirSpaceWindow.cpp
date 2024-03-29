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
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include "AirSpace.h"
#include "AirSpaceWindow.h"
#include "Error.h"
#include "IGPSDevice.h"
#include "IAirSpaceForm.h"
#include "IFlyHighRC.h"
#include "ISql.h"
#include "OpenAirFileParser.h"
#include "ProgressDlg.h"
#include "WebMapAirSpaceView.h"

AirSpaceWindow::AirSpaceWindow(QWidget* parent, const QString &name, IDataBase::SourceType src)
  :TableWindow(parent, name)
{
  QStringList nameList;
  QAction* pAction;
  QTableWidget *pTable;

  pTable = TableWindow::getTable();
  m_pWebMapView = nullptr;
  m_pAirSpaceView = nullptr;
  m_externSelect = false;

  switch(src)
  {
    case IDataBase::SqlDB:
      m_pDb = ISql::pInstance();

      connect(m_pDb, SIGNAL(airSpacesChanged()), this, SLOT(file_update()));

      pAction = new QAction(tr("&Edit"), this);
      connect(pAction, SIGNAL(triggered()), this, SLOT(file_edit()));
      MDIWindow::addAction(pAction, true);

      pAction = new QAction(tr("&Delete"), this);
      connect(pAction, SIGNAL(triggered()), this, SLOT(file_delete()));
      MDIWindow::addAction(pAction);

      pAction = new QAction(tr("&Add to GPS..."), this);
      connect(pAction, SIGNAL(triggered()), this, SLOT(file_AddToGPS()));
      MDIWindow::addAction(pAction, true);
    break;
    case IDataBase::GPSdevice:
    {
      m_pDb = IGPSDevice::pInstance();

      connect(m_pDb, SIGNAL(airSpacesChanged()), this, SLOT(file_update()));

      pAction = new QAction(tr("&Delete"), this);
      connect(pAction, SIGNAL(triggered()), this, SLOT(file_delete()));
      MDIWindow::addAction(pAction);

      pAction = new QAction(tr("&Update"), this);
      connect(pAction, SIGNAL(triggered()), this, SLOT(file_update()));
      MDIWindow::addAction(pAction);
    }
    break;
    case IDataBase::File:
    {
      m_pDb = nullptr;

      pAction = new QAction(tr("&Add to GPS..."), this);
      connect(pAction, SIGNAL(triggered()), this, SLOT(file_AddToGPS()));
      MDIWindow::addAction(pAction, true);
    }
    break;
  }

  if(src != IDataBase::SqlDB)
  {
    pAction = new QAction(tr("Add to DB..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_AddToSqlDB()));
    MDIWindow::addAction(pAction, true);

    pAction = new QAction(tr("&Update"), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_update()));
    MDIWindow::addAction(pAction);
  }

  // import/export
  pAction = new QAction(this);
  pAction->setSeparator(true);
  MDIWindow::addAction(pAction);

  if(src == IDataBase::File)
  {
    pAction = new QAction(tr("&Import..."), this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(file_open()));
    MDIWindow::addAction(pAction);
  }

  pAction = new QAction(tr("&Export all..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(exportTable()));
  MDIWindow::addAction(pAction);

  // view
  pAction = new QAction(this);
  pAction->setSeparator(true);
  MDIWindow::addAction(pAction);

  pAction = new QAction(tr("&View..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(file_viewAirSpace()));
  MDIWindow::addAction(pAction);

  pAction = new QAction(tr("View &Web Map..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(file_viewWebMap()));
  MDIWindow::addAction(pAction, true);

  TableWindow::setWindowIcon(QIcon(":/document.xpm"));

  // configure the table
  pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  pTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // header
  nameList += tr("Name");
  nameList += tr("Low [m]");
  nameList += tr("High [m]");
  nameList += tr("Class");
  nameList += tr("Comment");
  setupHeader(nameList);

  pTable->setColumnWidth(Name, 200);
  pTable->setColumnWidth(High, 100);
  pTable->setColumnWidth(Low, 100);
  pTable->setColumnWidth(Class, 80);
  pTable->setColumnWidth(Comment, 750);

  connect(m_pDb, SIGNAL(airSpacesChanged()), this, SLOT(file_update()));

  if(src == IDataBase::File)
  {
    file_open();
  }
  else
  {
    file_update();
  }
}

AirSpaceWindow::~AirSpaceWindow()
{
}

void AirSpaceWindow::closeEvent(QCloseEvent *pEvent)
{
  (void)pEvent;

  if(m_pAirSpaceView != nullptr)
  {
    delete m_pAirSpaceView;
    m_pAirSpaceView = nullptr;
  }

  if(m_pWebMapView != nullptr)
  {
    delete m_pWebMapView;
    m_pWebMapView = nullptr;
  }
}

void AirSpaceWindow::file_update()
{
  ProgressDlg progDlg(this);
  uint airspaceNr;
  uint maxAirspaceNr;

  if(m_pDb != nullptr)
  {
    m_airSpaceList.clear();
    TableWindow::setNumRows(0);

    if(m_pDb->open())
    {
      TableWindow::setCursor(QCursor(Qt::WaitCursor));
      progDlg.beginProgress("read airspaces...", m_pDb);
      m_pDb->airspaceList(m_airSpaceList);
      progDlg.endProgress();
      maxAirspaceNr = m_airSpaceList.size();
      TableWindow::setNumRows(maxAirspaceNr);

      for(airspaceNr=0; airspaceNr<maxAirspaceNr; airspaceNr++)
      {
        setAirSpaceToRow(airspaceNr, m_airSpaceList.at(airspaceNr));
      }

      TableWindow::selectRow(0);
      selectionChanged();
      m_pDb->close();
      TableWindow::unsetCursor();
    }
  }
}

void AirSpaceWindow::file_open()
{
  QString fileName;
  OpenAirFileParser parser;
  uint airspaceNr;
  uint maxAirspaceNr;
  bool success;

  fileName = QFileDialog::getOpenFileName(this,
                                          tr("Open OpenAir File"),
                                          IFlyHighRC::pInstance()->lastDir(),
                                          "OpenAir Files (*.txt);;"
                                          "Flytec Files (*.faf *.fas)");

  if(fileName != "")
  {
    IFlyHighRC::pInstance()->setLastDir(QFileInfo(fileName).absoluteDir().absolutePath());
    TableWindow::setCursor(QCursor(Qt::WaitCursor));

    if(fileName.contains(QRegExp("faf$|fas$")))
    {
      parser.setDefaultUnit(OpenAirFileParser::Meter);
    }

    success = parser.parse(fileName, m_airSpaceList);
    Error::verify(success, Error::FILE_PARSE);

    if(success)
    {
      m_airSpaceList.sort();
      maxAirspaceNr = m_airSpaceList.size();
      TableWindow::setNumRows(maxAirspaceNr);

      for(airspaceNr=0; airspaceNr<maxAirspaceNr; airspaceNr++)
      {
        setAirSpaceToRow(airspaceNr, m_airSpaceList.at(airspaceNr));
      }

      TableWindow::selectRow(0);
      selectionChanged();
    }

    TableWindow::unsetCursor();
  }
}

void AirSpaceWindow::file_edit()
{
  IAirSpaceForm airspaceForm(this, tr("Edit AirSpace"));
  int row;

  row = getTable()->currentRow();

  if(row >= 0)
  {
    airspaceForm.setAirSpace(m_airSpaceList[row]);

    if(airspaceForm.exec() && m_pDb->open())
    {
      ISql::pInstance()->update(*m_airSpaceList[row]);
      m_pDb->close();
    }
  }
}

void AirSpaceWindow::file_delete()
{
  AirSpaceList airspaceList(false);
  ProgressDlg progDlg(this);

  selectionToList(airspaceList);

  if((airspaceList.size() > 0) && m_pDb->open())
  {
    TableWindow::setCursor(QCursor(Qt::WaitCursor));
    progDlg.beginProgress(tr("delete airspaces..."), m_pDb);
    m_pDb->delAirSpaces(airspaceList);
    progDlg.endProgress();
    TableWindow::unsetCursor();
    m_pDb->close();
  }
}

void AirSpaceWindow::file_AddToSqlDB()
{
  AirSpaceList airspaceList(false);
  ProgressDlg progDlg(this);

  selectionToList(airspaceList);

  if((airspaceList.size() > 0) && ISql::pInstance()->open())
  {
    TableWindow::setCursor(QCursor(Qt::WaitCursor));
    progDlg.beginProgress(tr("add airspaces..."), ISql::pInstance());
    ISql::pInstance()->add(airspaceList);
    progDlg.endProgress();
    TableWindow::unsetCursor();
    ISql::pInstance()->close();
  }
}

void AirSpaceWindow::file_AddToGPS()
{
  AirSpaceList airspaceList(false);
  ProgressDlg progDlg(this);

  selectionToList(airspaceList);

  if((airspaceList.size() > 0) && IGPSDevice::pInstance()->open())
  {
    TableWindow::setCursor(QCursor(Qt::WaitCursor));
    progDlg.beginProgress("add airspace...", IGPSDevice::pInstance());
    IGPSDevice::pInstance()->add(airspaceList);
    progDlg.endProgress();
    TableWindow::unsetCursor();
    IGPSDevice::pInstance()->close();
  }
}

void AirSpaceWindow::file_viewAirSpace()
{
  if((m_airSpaceList.size() > 0) && (m_pAirSpaceView == nullptr))
  {
    m_pAirSpaceView = new AirSpaceView();
    m_pAirSpaceView->setGeometry(QRect(0, 0, 800, 800));
    m_pAirSpaceView->setWindowTitle(tr("View AirSpaces"));
    m_pAirSpaceView->setAirSpaceList(&m_airSpaceList, 0);
    connect(m_pAirSpaceView, SIGNAL(finished(int)), this, SLOT(airSpaceViewFinished(int)));
    m_pAirSpaceView->show();
    selectionChanged();
  }
}

void AirSpaceWindow::file_viewWebMap()
{
  if((m_airSpaceList.size() > 0) && (m_pWebMapView == nullptr))
  {
    menu()->setEnabled(false);
    m_pWebMapView = new WebMapAirSpaceView(tr("View AirSpaces"));
    m_pWebMapView->setAirSpaceList(&m_airSpaceList);
    m_pWebMapView->loadMap();
    connect(m_pWebMapView, SIGNAL(airSpaceChanged(int)), this, SLOT(airSpaceChanged(int)));
    connect(m_pWebMapView, SIGNAL(finished(int)), this, SLOT(webMapFinished(int)));
    m_pWebMapView->setWindowModality(Qt::NonModal);
    m_pWebMapView->show();
    selectionChanged();
  }
}

void AirSpaceWindow::selectionChanged()
{
  int row;

  row = getTable()->currentRow();

  if(row >= 0)
  {
    if(m_pAirSpaceView != nullptr)
    {
      m_pAirSpaceView->setSelected(row);
    }

    if((m_pWebMapView != nullptr) && !m_externSelect)
    {
      m_pWebMapView->selectAirSpace(m_airSpaceList[row]->id());
    }
  }
}

void AirSpaceWindow::airSpaceViewFinished(int res)
{
  m_pAirSpaceView = nullptr;
}

void AirSpaceWindow::webMapFinished(int res)
{
  m_pWebMapView = nullptr;
  menu()->setEnabled(true);
}

void AirSpaceWindow::airSpaceChanged(int id)
{
  uint row;

  m_externSelect = true;

  for(row=0; row<m_airSpaceList.size(); row++)
  {
    if(m_airSpaceList[row]->id() == id)
    {
      TableWindow::selectRow(row);
      break;
    }
  }

  m_externSelect = false;
}

void AirSpaceWindow::setAirSpaceToRow(uint row, const AirSpace *pAirSpace)
{
  QTableWidget *pTable = TableWindow::getTable();
  QString str = "%1";

  pTable->item(row, Name)->setText(pAirSpace->name());
  pTable->item(row, High)->setText(str.arg(pAirSpace->high()));

  if(pAirSpace->low() == 0)
  {
    pTable->item(row, Low)->setText(tr("GND"));
  }
  else
  {
    pTable->item(row, Low)->setText(str.arg(pAirSpace->low()));
  }

  pTable->item(row, Class)->setText(pAirSpace->airspaceClass());
  pTable->item(row, Comment)->setText(pAirSpace->remark());
}

void AirSpaceWindow::selectionToList(AirSpaceList &airspaceList)
{
  QList<QTableWidgetSelectionRange> selRange;
  QList<QTableWidgetSelectionRange>::iterator rangeIt;
  QTableWidget *pTable;
  int topRow;
  int bottomRow;
  int row;

  pTable = TableWindow::getTable();
  selRange = pTable->selectedRanges();

  for(rangeIt=selRange.begin(); rangeIt!=selRange.end(); rangeIt++)
  {
    topRow = (*rangeIt).topRow();
    bottomRow = (*rangeIt).bottomRow();

    for(row=topRow; row<=bottomRow; row++)
    {
      airspaceList.push_back(m_airSpaceList[row]);
    }
  }
}
