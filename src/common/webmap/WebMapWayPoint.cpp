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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "WebMap.h"
#include "WebMapPage.h"
#include "WebMapWayPoint.h"

WebMapWayPoint::WebMapWayPoint(WebMap *pWebMap)
{
	m_pWebMap = pWebMap;
}

WebMapWayPoint::~WebMapWayPoint()
{
}

void WebMapWayPoint::init()
{
	QString code = "wp_init();";
	m_pWebMap->page()->runJavaScript(code);
}

void WebMapWayPoint::pushWayPoint(const WayPoint &wp)
{
	QString code = "wp_pushWayPoint({id: %1, name: '%2', spot: '%3', country: '%4',"
                 " lat: %5, lng: %6, alt: %7});";
	QString name;
	QString country;
	QString spot;
	float lat;
	float lon;
	int id;
	int alt;

  id = wp.id();
  name = WebMap::escape(wp.name());
  spot = WebMap::escape(wp.spot());
  country = WebMap::escape(wp.country());
  lat = wp.lat();
  lon = wp.lon();
  alt = wp.alt();
  m_pWebMap->page()->runJavaScript(code.arg(id).arg(name).arg(spot).arg(country)
                                       .arg(lat).arg(lon).arg(alt));
}

void WebMapWayPoint::selectWayPoint(uint id)
{
	QString code = "wp_selectWayPoint(%1);";
  m_pWebMap->page()->runJavaScript(code.arg(id));
}

/**
void WebMapWayPoint::populateObject()
{
	m_pWebMap->page()->mainFrame()->addToJavaScriptWindowObject("WebMapWayPoint", this);
}
*/

void WebMapWayPoint::setEditable(bool en)
{
	QString code = "wp_setEditable(%1);";
  m_pWebMap->page()->runJavaScript(code.arg(en));
}

bool WebMapWayPoint::getNextModified(WayPoint &wp)
{
  WebMapPage *webMapPage = static_cast<WebMapPage*>(m_pWebMap->page());
  QString code = "wp_getNextModified();";
  QVariant opts;
  QJsonDocument doc;
  QJsonObject values;
  bool valid;

  webMapPage->runJavaScriptRet(code, opts);
  doc = QJsonDocument::fromJson(opts.toString().toUtf8());
  valid = doc.isObject();

  if(valid)
  {
    values = doc.object();
    wp.setId(values["id"].toInt());
    wp.setName(values["name"].toString());
    wp.setSpot(values["spot"].toString());
    wp.setCountry(values["country"].toString());
    wp.setCoordinates(values["lat"].toDouble(), values["lng"].toDouble(),
                      values["alt"].toInt());
  }

  return valid;

/** @TODO: find a proper solution
  QString code = "wp_getNextModified();";
	QVariantMap wpMap;
	bool valid;

	wpMap = pFrame->evaluateJavaScript(code).toMap();
	valid = !wpMap.empty();

  if(valid)
	{
    wp.setId(wpMap["id"].toInt());
    wp.setName(wpMap["name"].toString());
    wp.setSpot(wpMap["spot"].toString());
    wp.setCountry(wpMap["country"].toString());
    wp.setCoordinates(wpMap["lat"].toDouble(), wpMap["lng"].toDouble(), wpMap["alt"].toInt());
	}

	return valid;
*/
}
