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

#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QVariant>
#include <QWebEnginePage>
#include "WebMap.h"
#include "WebMapPage.h"
#include "WebMapRoute.h"

WebMapRoute::WebMapRoute(WebMap *pWebMap)
{
	m_pWebMap = pWebMap;
	m_pRoute = nullptr;
}

WebMapRoute::~WebMapRoute()
{
}

void WebMapRoute::init()
{
	QString code = "rt_init();";
	m_pWebMap->page()->runJavaScript(code);
}

void WebMapRoute::setName(const QString &name)
{
	QString code = "rt_setName('%1');";
	QString locName = WebMap::escape(name);
	m_pWebMap->page()->runJavaScript(code.arg(locName));
}

QString WebMapRoute::name() const
{
  WebMapPage *webMapPage = static_cast<WebMapPage*>(m_pWebMap->page());
	QString code = "rt_getName();";
	QVariant value;
  QString name;

/**
	pFrame = m_pWebMap->page()->mainFrame();
	name = pFrame->evaluateJavaScript(code).toString();
*/
  webMapPage->runJavaScriptRet(code, value);
  name = value.toString();

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
			arg += ",";

		first = false;
		arg += argElem.arg(tpList.at(tpNr).pos().lat()).arg(tpList.at(tpNr).pos().lon());
	}

	arg += "]";
	m_pWebMap->page()->runJavaScript(code.arg(arg));
}

void WebMapRoute::turnPointList(WayPoint::WayPointListType &tpList) const
{
  WebMapPage *webMapPage = static_cast<WebMapPage*>(m_pWebMap->page());
  QString code = "rt_getTurnPts();";
  QVariant opts;
  QJsonDocument doc;
  QJsonArray::iterator it;
  QJsonArray values;
  QString wpName;
  WayPoint wp;
  int wpNr = 0;

	wpName = "%1_" + name();
	tpList.clear();

///  tpArray = pFrame->evaluateJavaScript(code).toList();
  webMapPage->runJavaScriptRet(code, opts);
  doc = QJsonDocument::fromJson(opts.toString().toUtf8());

  if(doc.isArray())
  {
    for(const QJsonValue &tp: doc.array())
    {
      if(tp.isArray())
      {
        values = tp.toArray();

        if(values.size() == 3)
        {
          wp.setName(wpName.arg(wpNr));
          wp.setLat(values[0].toDouble());
          wp.setLon(values[1].toDouble());
          wp.setAlt(values[2].toInt());
          wp.setType(WayPoint::TypeTurnPoint);
          tpList.push_back(wp);
          wpNr++;
        }
      }
    }
  }
/**
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
*/
}

void WebMapRoute::setEditable(bool en)
{
	QString code = "rt_setEditable(%1);";
	m_pWebMap->page()->runJavaScript(code.arg(en));
}

void WebMapRoute::setGlueToCenter(bool en)
{
	QString code = "rt_setGlueToCenter(%1);";
	m_pWebMap->page()->runJavaScript(code.arg(en));
}

Route::Type WebMapRoute::type() const
{
  WebMapPage *webMapPage = static_cast<WebMapPage*>(m_pWebMap->page());
	QString code = "rt_getType();";
  QVariant value;
	int type = -1;

//	type = pFrame->evaluateJavaScript(code).toInt();
  webMapPage->runJavaScriptRet(code, value);
  type = value.toInt();

	return (Route::Type)type;
}
