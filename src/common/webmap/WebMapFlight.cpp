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

#include <QObject>
#include <QWebEnginePage>
#include <math.h>
#include "WebMap.h"
#include "WebMapFlight.h"

WebMapFlight::WebMapFlight(WebMap *pWebMap)
{
  m_pWebMap = pWebMap;
  m_plotEn = false;
}

WebMapFlight::~WebMapFlight()
{
}

void WebMapFlight::init()
{
  QString code = "fl_init();";
  m_pWebMap->page()->runJavaScript(code);
}

void WebMapFlight::setFlightPointList(const QDate &date,
                                      const FlightPointList *pFpList)
{
  QString code;
  QString value = "%1";
  QString latLonArg = "[%1,%2]";
  QString strTime = "";
  QString strAlt = "";
  QString strElevation = "";
  QString strLatLon = "";
  QTime time;
  uint fpNr;
  uint begin;
  uint end;
  uint start;
  uint duration;
  double alt;
  double minAlt;
  double maxAlt;
  int epochDate;
  int epochTime;
  int secsOfDay;
  int prevSecsOfDay;
  bool first = true;

  begin = pFpList->firstValidFlightData();
  end = pFpList->lastValidFlightData();

  if(end > 0)
  {
    minAlt = pFpList->at(begin)->alt();
    maxAlt = minAlt;
    time = pFpList->at(begin)->time();
    start = time.hour() * 3600 + time.minute() * 60 + time.second();
    time = pFpList->at(end - 1)->time();
    duration = time.hour() * 3600 + time.minute() * 60 + time.second() - start;
    epochDate = QDateTime(date).toTime_t();
    prevSecsOfDay = start;

    for(fpNr=begin; fpNr<end; fpNr++)
    {
      if(!first)
      {
        strTime += ",";
        strAlt += ",";
        strElevation += ",";
        strLatLon += ",";
      }

      first = false;

      // time
      time = pFpList->at(fpNr)->time();
      secsOfDay = time.hour() * 3600 + time.minute() * 60 + time.second();

      if(secsOfDay < prevSecsOfDay)
      {
        epochDate += 86400; // next day
      }

      epochTime = (epochDate + secsOfDay);
      prevSecsOfDay = secsOfDay;
      strTime += value.arg(epochTime);

      // altitude
      alt = pFpList->at(fpNr)->alt();
      minAlt = qMin(alt, minAlt);
      maxAlt = qMax(alt, maxAlt);
      strAlt += value.arg(alt);

      // elevation
      strElevation += value.arg(pFpList->at(fpNr)->elevation());

      // lat, lon
      strLatLon += latLonArg.arg(pFpList->at(fpNr)->pos().lat())
                            .arg(pFpList->at(fpNr)->pos().lon());
    }

    minAlt = floor(minAlt / 100.0) * 100;
    maxAlt = ceil(maxAlt / 100.0) * 100;

    code = "fl_setFlightTime([%1], %2, %3);";
    m_pWebMap->page()->runJavaScript(code.arg(strTime).arg(start).arg(duration));
    code = "fl_setFlightAlt([%1], %2, %3);";
    m_pWebMap->page()->runJavaScript(code.arg(strAlt).arg(minAlt).arg(maxAlt));
    code = "fl_setFlightElevation([%1]);";
    m_pWebMap->page()->runJavaScript(code.arg(strElevation));
    code = "fl_setFlightLatLon([%1]);";
    m_pWebMap->page()->runJavaScript(code.arg(strLatLon));
  }
}

void WebMapFlight::setSogList(const FlightPointList::SogListType &sogList,
                              uint begin, uint end)
{
  QString code = "fl_setSog([%1]);";
  QString value = "%1";
  QString strSog = "";
  uint itemNr;
  uint listSize;
  bool first = true;

  listSize = std::min((uint)sogList.size(), end);

  if(listSize > 0)
  {
    for(itemNr=begin; itemNr<listSize; itemNr++)
    {
      if(!first)
      {
        strSog += ",";
      }

      first = false;

      // sog
      strSog += value.arg(round(sogList.at(itemNr) * 10.0) / 10.0);
    }

    m_pWebMap->page()->runJavaScript(code.arg(strSog));
  }
}

void WebMapFlight::setVarioList(const FlightPointList::VarioListType &varioList,
                                uint begin, uint end)
{
  QString code = "fl_setVario([%1]);";
  QString value = "%1";
  QString strVario = "";
  uint itemNr;
  uint listSize;
  bool first = true;

  listSize = std::min((uint)varioList.size(), end);

  if(listSize > 0)
  {
    for(itemNr=begin; itemNr<listSize; itemNr++)
    {
      if(!first)
      {
        strVario += ",";
      }

      first = false;

      // sog
      strVario += value.arg(round(varioList.at(itemNr) * 10.0) / 10.0);
    }

    m_pWebMap->page()->runJavaScript(code.arg(strVario));
  }
}

// qrc:/home/grafal/pictures/rigi_20131101/IMG_1483.JPG

void WebMapFlight::setPhotoList(const Photo::PhotoListType &photoList)
{
  QString code = "fl_pushPhoto({lat: %1, lng: %2, path: '%3'});";
  uint listSize;
  uint itemNr;
  QString path;
  double lat;
  double lon;

  listSize = photoList.size();

  if(listSize > 0)
  {
    for(itemNr=0; itemNr<listSize; itemNr++)
    {
      lat = photoList.at(itemNr).pos().lat();
      lon = photoList.at(itemNr).pos().lon();
      path = photoList.at(itemNr).path();
      m_pWebMap->page()->runJavaScript(code.arg(lat).arg(lon).arg(path));
    }
  }
}

void WebMapFlight::showPlot()
{
  QString code = "fl_showPlot();";

  if(m_plotEn)
    m_pWebMap->page()->runJavaScript(code);
}

void WebMapFlight::setPlotEnable(bool en)
{
  m_plotEn = en;
}
