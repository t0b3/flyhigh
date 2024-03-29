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
#include "AirSpace.h"
#include "WebMap.h"
#include "WebMapAirSpace.h"

WebMapAirSpace::WebMapAirSpace(WebMap *pWebMap)
{
	m_pWebMap = pWebMap;
}

WebMapAirSpace::~WebMapAirSpace()
{
}

void WebMapAirSpace::init()
{
	QString code = "as_init();";
	m_pWebMap->page()->runJavaScript(code);
}

void WebMapAirSpace::pushAirSpace(AirSpace *pAirSpace)
{
	QString code = "as_pushAirSpace(%1, {id:%2, name:'%3', low:%4, high:%5, airclass:'%6'});";
	QString coords = "[";
	QString coord = "[%1,%2]";
	uint wpNr;
	uint wpListSize;
	float lat;
	float lon;
  float endLat;
	float endLon;
  int id;
  QString name;
  int low;
  int high;
  QString airClass;
	bool first = true;

  wpListSize = pAirSpace->pointList().size();

  if(wpListSize > 0)
  {
    for(wpNr=0; wpNr<wpListSize; wpNr++)
    {
      if(!first)
      {
        coords += ",";
      }

      first = false;
      lat = pAirSpace->pointList().at(wpNr).lat();
      lon = pAirSpace->pointList().at(wpNr).lon();
      coords += coord.arg(lat).arg(lon);
    }

    lat = pAirSpace->pointList().at(0).lat();
    lon = pAirSpace->pointList().at(0).lon();
    endLat = pAirSpace->pointList().at(wpListSize - 1).lat();
    endLon = pAirSpace->pointList().at(wpListSize - 1).lon();

    if((lat != endLat) || (lon !=endLon))
    {
      coords += ",";
      coords += coord.arg(lat).arg(lon);
    }

    coords += "]";
  }

  id = pAirSpace->id();
  name = WebMap::escape(pAirSpace->name());
  low = pAirSpace->low();
  high = pAirSpace->high();
  airClass = WebMap::escape(pAirSpace->airspaceClass());
  m_pWebMap->page()->runJavaScript(code.arg(coords).arg(id).arg(name).arg(low)
                                       .arg(high).arg(airClass));
}

void WebMapAirSpace::selectAirSpace(int id)
{
	QString code = "as_selectAirSpace(%1);";
  m_pWebMap->page()->runJavaScript(code.arg(id));
}
