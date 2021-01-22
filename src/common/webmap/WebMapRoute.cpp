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
	m_pRoute = NULL;
}

WebMapRoute::~WebMapRoute()
{
}

void WebMapRoute::init()
{
	QString code = "rt_init();";
	QWebEnginePage *pFrame;

	pFrame = m_pWebMap->page();
	pFrame->runJavaScript(code);
}

void WebMapRoute::setName(const QString &name)
{
	QString code = "rt_setName('%1');";
	QString locName;
	QWebEnginePage *pFrame;

	pFrame = m_pWebMap->page();
	locName = WebMap::escape(name);
	pFrame->runJavaScript(code.arg(locName));
}

QString WebMapRoute::name() const
{
	QString code = "rt_getName();";
	QWebEnginePage *pFrame;
	QString name;

	pFrame = m_pWebMap->page();
//	name = pFrame->runJavaScript(code, [](const QVariant &v) { qDebug() << v.toString(); }); // TODO: port to foo.someattr = "somedata" // see https://myprogrammingnotes.com/communication-c-javascript-qt-webengine.html
//	name = pFrame->evaluateJavaScript(code).toString();

	return name;
}

void WebMapRoute::setTurnPointList(const WayPoint::WayPointListType &tpList)
{
	QString code = "rt_setTurnPts(%1);";
	QString arg = "[";
	QString argElem = "[%1,%2]";
	QWebEnginePage *pFrame;
	int tpNr;
	bool first = true;

	pFrame = m_pWebMap->page();

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
	pFrame->runJavaScript(code.arg(arg));
}

void WebMapRoute::turnPointList(WayPoint::WayPointListType &tpList) const
{
  QString code = "rt_getTurnPts();";
  QString wpName;
  QWebEnginePage *pFrame;
  QVariantList tpArray;
  QVariantList tpLatLng;
  QVariantList::iterator it;
  WayPoint wp;
  int wpNr = 0;

	wpName = "%1_" + name();
	tpList.clear();
  pFrame = m_pWebMap->page();
//  tpArray = pFrame->runJavaScript(code, [](const QVariant &v) { qDebug() << v.toString(); }); // TODO: port to foo.someattr = "somedata" // see https://myprogrammingnotes.com/communication-c-javascript-qt-webengine.html
//  tpArray = pFrame->evaluateJavaScript(code).toList();

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
	QWebEnginePage *pFrame;

	pFrame = m_pWebMap->page();
	pFrame->runJavaScript(code.arg(en));
}

void WebMapRoute::setGlueToCenter(bool en)
{
	QString code = "rt_setGlueToCenter(%1);";
	QWebEnginePage *pFrame;

	pFrame = m_pWebMap->page();
	pFrame->runJavaScript(code.arg(en));
}

Route::Type WebMapRoute::type() const
{
	QString code = "rt_getType();";
	QWebEnginePage *pFrame;
	int type;

	pFrame = m_pWebMap->page();
//	type = pFrame->evaluateJavaScript(code, [](const QVariant &v) { qDebug() << v.toString(); }); // TODO: port to foo.someattr = "somedata" // see https://myprogrammingnotes.com/communication-c-javascript-qt-webengine.html
//	type = pFrame->evaluateJavaScript(code).toInt();

	return (Route::Type)type;
}
