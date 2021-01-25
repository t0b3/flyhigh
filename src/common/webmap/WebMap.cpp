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
#include <QDialog>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressBar>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QtWebEngine>
#include <math.h>
#include "WebMap.h"
#include "WebMapAirSpace.h"
#include "WebMapFlight.h"
#include "WebMapRoute.h"
#include "WebMapWayPoint.h"

#include <QDebug>

WebMap::WebMap(QWidget *pParent, MapType type)
	:QWebEngineView(pParent)
{
    m_pPage = this->page();

  m_pAirSpace = NULL;
  m_pFlight = NULL;
  m_pRoute = NULL;
  m_pWayPoint = NULL;
	m_mapReady = false;
	m_mapType = type;
	m_pNetMgr = new QNetworkAccessManager(this);
	m_pProgress = new QProgressBar(this);
	m_pProgress->setGeometry(LeftWidth, 0, ProgressW, ProgressH);
	m_pProgress->show();

    m_pPage->settings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);

	// JavaScript can access this WebMap via channel.objects.WebMap
	QWebChannel *channel = new QWebChannel();
	channel->registerObject("WebMap", this);
	// Set up the web channel so the page can access Qt objects
    m_pPage->setWebChannel(channel);

	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
	connect(this, SIGNAL(loadProgress(int)), m_pProgress, SLOT(setValue(int)));
  connect(m_pNetMgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(netReply(QNetworkReply*)));
}

WebMap::~WebMap()
{
  if(m_pAirSpace != NULL)
  {
    delete m_pAirSpace;
  }

  if(m_pFlight != NULL)
  {
    delete m_pFlight;
  }

  if(m_pRoute != NULL)
  {
    delete m_pRoute;
  }

  if(m_pWayPoint != NULL)
  {
    delete m_pWayPoint;
  }
}

WebMapAirSpace* WebMap::getAirSpace()
{
  if(m_pAirSpace == NULL)
  {
    m_pAirSpace = new WebMapAirSpace(this);
  }

  return m_pAirSpace;
}

WebMapFlight* WebMap::getFlight()
{
  if(m_pFlight == NULL)
  {
    m_pFlight = new WebMapFlight(this);
  }

  return m_pFlight;
}

WebMapRoute* WebMap::getRoute()
{
  if(m_pRoute == NULL)
  {
    m_pRoute = new WebMapRoute(this);
  }

  return m_pRoute;
}

WebMapWayPoint* WebMap::getWayPoint()
{
  if(m_pWayPoint == NULL)
  {
    m_pWayPoint = new WebMapWayPoint(this);
  }

  return m_pWayPoint;
}

void WebMap::loadUrl(const QString &url)
{
	m_mapReady = false;
	load(QUrl(url));
}

bool WebMap::isMapReady() const
{
	return m_mapReady;
}

void WebMap::resizeEvent(QResizeEvent *pEvent)
{
	QWebEngineView::resizeEvent(pEvent);
	setSize(width(), height());
}

void WebMap::setSize(uint width, uint height)
{
	QString code;

	width = (width - LeftWidth - Margin);

	switch(m_mapType)
	{
	  case MapFlight:
      //TODO: delay calling as wm_setMapSize() and fl_setPlotSize() are not present during early stage
      code = "wm_setMapSize(%1, %2);";
      m_pPage->runJavaScript(code.arg(width).arg(height - PlotHeight));
      code = "fl_setPlotSize(%1, %2);";
      m_pPage->runJavaScript(code.arg(width).arg(PlotHeight));
	  break;
	  case MapRoute:
      code = "wm_setMapSize(%1, %2);";
      m_pPage->runJavaScript(code.arg(width).arg(height));
	  break;
	  case MapWayPoint:
      code = "wm_setMapSize(%1, %2);";
      m_pPage->runJavaScript(code.arg(width).arg(height));
	  break;
	  case MapAirSpace:
	  	code = "wm_setMapSize(%1, %2);";
      m_pPage->runJavaScript(code.arg(width).arg(height));
	  break;
	}
}

QString WebMap::escape(const QString &str)
{
  QString locStr;

  locStr = str;
  locStr.replace(QRegExp("('|\")"), "\\\\1");

  return locStr;
}

void WebMap::loadFinished(bool ok)
{
	m_mapReady = true;
	setSize(width(), height());
	emit mapReady();
	m_pProgress->hide();
}

void WebMap::netReply(QNetworkReply *pReply)
{
  QString code;
  QString replyStr(pReply->readAll());
  int id;

  id = m_netReqList.front().id;
  code = m_netReqList.front().callback + "(%1, %2);";
  m_netReqList.pop_front();
  m_pPage->runJavaScript(code.arg(id).arg(replyStr));
}

void WebMap::emitAppReady()
{
    emit appReady();
}

void WebMap::setOk(bool ok)
{
	if(ok)
	{
		emit finished(QDialog::Accepted);
	}
	else
	{
		emit finished(QDialog::Rejected);
	}
}

void WebMap::setLine(int line)
{
  emit lineChanged(line);
}

void WebMap::netRequest(int id, const QString &request, const QString &callback)
{
  QNetworkRequest netReq(request);
  NetRequest netReqPar;

  netReqPar.id = id;
  netReqPar.callback = callback;
  m_netReqList.push_back(netReqPar);
  m_pNetMgr->get(netReq);
}
