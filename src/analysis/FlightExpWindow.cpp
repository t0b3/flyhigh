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
#include <QCursor>
#include <QStringList>
#include <QTableWidget>
#include <QWidget>
#include "ContainerDef.h"
#include "FlightExpWindow.h"
#include "IDataBase.h"
#include "IFlyHighRC.h"
#include "ISql.h"

FlightExpWindow::FlightExpWindow(QWidget* parent, const QString &name)
  :TableWindow(parent, name)
{
  QStringList nameList;
  QAction* pAction;
  QTableWidget *pTable;

  pTable = TableWindow::getTable();

  pAction = new QAction(tr("&Export all..."), this);
  connect(pAction, SIGNAL(triggered()), this, SLOT(exportTable()));
  MDIWindow::addAction(pAction);

  TableWindow::setWindowTitle(tr("Flight statistics"));
  TableWindow::setWindowIcon(QIcon(":/document.xpm"));

  // configure the table
  pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  pTable->setSelectionMode(QAbstractItemView::SingleSelection);
  m_pDb = ISql::pInstance();
  connect(m_pDb, SIGNAL(flightsChanged()), this, SLOT(file_update()));

  // header
  nameList += tr("Year");
  nameList += tr("Solo");
  nameList += tr("Solo [h]");
  nameList += tr("Tandem");
  nameList += tr("Tandem [h]");
  nameList += tr("Total");
  nameList += tr("Total [h]");

  setupHeader(nameList);

  pTable->setColumnWidth(Year, 60);
  pTable->setColumnWidth(FlightsSolo, 100);
  pTable->setColumnWidth(AirtimeSolo, 100);
  pTable->setColumnWidth(FlightsTandem, 100);
  pTable->setColumnWidth(AirtimeTandem, 100);
  pTable->setColumnWidth(FlightsTotal, 100);
  pTable->setColumnWidth(AirtimeTotal, 100);

  file_update();
}

void FlightExpWindow::file_update()
{
  FlightsPerYearListType fpyList;
  Pilot pilot;
  QTableWidget *pTable = TableWindow::getTable();
  QString str;
  uint flightsSolo = 0;
  uint airtimeSolo = 0;
  uint flightsTandem = 0;
  uint airtimeTandem = 0;
  uint flightsTotal = 0;
  uint airtimeTotal = 0;
  uint fpyCount;
  uint fpyNr;
  uint itemNr;

  TableWindow::setCursor(QCursor(Qt::WaitCursor));

  // pilot info
  ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), pilot);

  if(m_pDb->flightsPerYear(pilot, fpyList))
  {
    fpyCount = fpyList.size();
    TableWindow::setNumRows(fpyCount + 2);

    if(fpyCount > 0)
    {
      // statistics, newest first
      for(fpyNr=0; fpyNr<fpyCount; fpyNr++)
      {
        itemNr = (fpyCount - fpyNr - 1);
        str = QString::number(fpyList[fpyNr].year);
        pTable->item(itemNr, Year)->setText(str);

        str = QString::number(fpyList[fpyNr].flightsSolo);
        pTable->item(itemNr, FlightsSolo)->setText(str);
        str = QString::number(fpyList[fpyNr].airTimeSolo / 3600.0, 'f', 2);
        pTable->item(itemNr, AirtimeSolo)->setText(str);
        flightsSolo += fpyList[fpyNr].flightsSolo;
        airtimeSolo += fpyList[fpyNr].airTimeSolo;

        str = QString::number(fpyList[fpyNr].flightsTandem);
        pTable->item(itemNr, FlightsTandem)->setText(str);
        str = QString::number(fpyList[fpyNr].airTimeTandem / 3600.0, 'f', 2);
        pTable->item(itemNr, AirtimeTandem)->setText(str);
        flightsTandem += fpyList[fpyNr].flightsTandem;
        airtimeTandem += fpyList[fpyNr].airTimeTandem;

        str = QString::number(fpyList[fpyNr].flightsTotal);
        pTable->item(itemNr, FlightsTotal)->setText(str);
        str = QString::number(fpyList[fpyNr].airTimeTotal / 3600.0, 'f', 2);
        pTable->item(itemNr, AirtimeTotal)->setText(str);
        flightsTotal += fpyList[fpyNr].flightsTotal;
        airtimeTotal += fpyList[fpyNr].airTimeTotal;
      }

      // separator
      itemNr = fpyCount;
      pTable->item(itemNr, Year)->setText("_______");
      pTable->item(itemNr, FlightsSolo)->setText("____________");
      pTable->item(itemNr, AirtimeSolo)->setText("____________");
      pTable->item(itemNr, FlightsTandem)->setText("____________");
      pTable->item(itemNr, AirtimeTandem)->setText("____________");
      pTable->item(itemNr, FlightsTotal)->setText("____________");
      pTable->item(itemNr, AirtimeTotal)->setText("____________");

      // sum
      itemNr++;
      str = QString::number(fpyList[fpyCount-1].year - fpyList[0].year + 1);
      pTable->item(itemNr, Year)->setText(str);

      str = QString::number(flightsSolo);
      pTable->item(itemNr, FlightsSolo)->setText(str);
      str = QString::number(airtimeSolo / 3600.0, 'f', 2);
      pTable->item(itemNr, AirtimeSolo)->setText(str);

      str = QString::number(flightsTandem);
      pTable->item(itemNr, FlightsTandem)->setText(str);
      str = QString::number(airtimeTandem / 3600.0, 'f', 2);
      pTable->item(itemNr, AirtimeTandem)->setText(str);

      str = QString::number(flightsTotal);
      pTable->item(itemNr, FlightsTotal)->setText(str);
      str = QString::number(airtimeTotal / 3600.0, 'f', 2);
      pTable->item(itemNr, AirtimeTotal)->setText(str);
    }
  }

  TableWindow::unsetCursor();
}
