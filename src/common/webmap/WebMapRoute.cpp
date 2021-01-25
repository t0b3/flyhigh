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
#include "WebMap.h"
#include "WebMapRoute.h"

WebMapRoute::WebMapRoute(WebMap *pWebMap)
{
	m_pWebMap = pWebMap;
    m_pPage = m_pWebMap->page();
	m_pRoute = NULL;
}

WebMapRoute::~WebMapRoute()
{
}

void WebMapRoute::init()
{
	QString code = "rt_init();";
    m_pPage->runJavaScript(code);
}

void WebMapRoute::setName(const QString &name)
{
	QString code = "rt_setName('%1');";
	QString locName;

	locName = WebMap::escape(name);
    m_pPage->runJavaScript(code.arg(locName));
}

QString WebMapRoute::name() const
{
	QString code = "rt_getName();";
	QString name;

/*  TODO: reimplement broken functionality, i.e. via QWebChannel - JS calling Qt slot */
//  PREVIOUS CODE with QtWebKit
//	name = m_pPage->evaluateJavaScript(code).toString();
    m_pPage->runJavaScript(QString("console.log(rt_getName())")); //DEBUG
//  TODO: port to foo.someattr = "somedata" // see https://myprogrammingnotes.com/communication-c-javascript-qt-webengine.html

	return name;
}

void WebMapRoute::setTurnPointList(const WayPoint::WayPointListType &tpList)
{
	QString code = "rt_setTurnPts(%1);";
	QString arg = "[";
	QString argElem = "[%1,%2]";
	int tpNr;
	bool first = true;

	for(tpNr=0; tpNr<tpList.size(); tpNr++)
	{
		if(!first)
		{
			arg += ",";
		}

		first = false;
		arg += argElem.arg(tpList.at(tpNr).pos().lat()).arg(tpList.at(tpNr).pos().lon());
	}

	arg += "]";
    m_pPage->runJavaScript(code.arg(arg));
}

void WebMapRoute::turnPointList(WayPoint::WayPointListType &tpList) const
{
  QString code = "rt_getTurnPts();";
  QString wpName;
  QVariantList tpArray;
  QVariantList tpLatLng;
  QVariantList::iterator it;
  WayPoint wp;
  int wpNr = 0;

	wpName = "%1_" + name();
	tpList.clear();
/*  TODO: reimplement broken functionality, i.e. via QWebChannel - JS calling Qt slot */
//  PREVIOUS CODE with QtWebKit
//  tpArray = m_pPage->evaluateJavaScript(code).toList();
    m_pPage->runJavaScript(QString("alert(rt_getTurnPts())")); // DEBUG
//  TODO: port to foo.someattr = "somedata" // see https://myprogrammingnotes.com/communication-c-javascript-qt-webengine.html

  for(it=tpArray.begin(); it!=tpArray.end(); it++)
  {
    tpLatLng = (*it).toList();

    if(tpLatLng.size() == 3)
    {
      wp.setName(wpName.arg(wpNr));
      wp.setLat(tpLatLng[0].toFloat());
      wp.setLon(tpLatLng[1].toFloat());
      wp.setAlt(tpLatLng[2].toInt());
      wp.setType(WayPoint::TypeTurnPoint);
      tpList.push_back(wp);
      wpNr++;
    }
  }
}

void WebMapRoute::setEditable(bool en)
{
	QString code = "rt_setEditable(%1);";
    m_pPage->runJavaScript(code.arg(en));
}

void WebMapRoute::setGlueToCenter(bool en)
{
	QString code = "rt_setGlueToCenter(%1);";
    m_pPage->runJavaScript(code.arg(en));
}

Route::Type WebMapRoute::type() const
{
	QString code = "rt_getType();";
	int type;

/*  TODO: reimplement broken functionality, i.e. via QWebChannel - JS calling Qt slot */
//  PREVIOUS CODE with QtWebKit
//	type = m_pPage->evaluateJavaScript(code).toInt();
    m_pPage->runJavaScript(QString("alert(rt_getType())")); // DEBUG
//  TODO: port to foo.someattr = "somedata" // see https://myprogrammingnotes.com/communication-c-javascript-qt-webengine.html

	return (Route::Type)type;
}
