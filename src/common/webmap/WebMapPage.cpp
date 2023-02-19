/***************************************************************************
 *   Copyright (C) 2023 by Alex Graf                                       *
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <QVariant>
#include <QWebEnginePage>
#include "WebMapPage.h"

WebMapPage::WebMapPage(QObject *parent)
  :QWebEnginePage(parent)
{
}

WebMapPage::~WebMapPage()
{
}

void WebMapPage::runJavaScriptRet(const QString &scriptSource, QVariant &value)
{
  struct RetValue
  {
    QVariant &value;
    QAtomicInt guard;
  };

  RetValue retValue = {value, -1};

  QWebEnginePage::runJavaScript(scriptSource,
                                [&retValue](const QVariant &result)
                                {
                                  retValue.value = result;
                                  retValue.guard = 1;
                                });

  while(retValue.guard == -1)
    QApplication::processEvents();
}

bool WebMapPage::javaScriptConfirm(const QUrl &securityOrigin, const QString &msg)
{
  (void)securityOrigin;
  QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
  QJsonObject values;
  QString func;
  bool success = doc.isObject();

  if(success)
  {
    values = doc.object();
    func = values["func"].toString();

    if(func == "appReady")
    {
      QTimer::singleShot(0, this, [this, values] ()
      {
        emit appReady();
      });
    }
    else if(func == "setLine")
    {
      QTimer::singleShot(0, this, [this, values] ()
      {
        emit setLine(values["line"].toInt());
      });
    }
    else if(func == "setOk")
    {
      QTimer::singleShot(0, this, [this, values] ()
      {
        emit setOk(values["ok"].toBool());
      });
    }
    else if(func == "netRequest")
    {
      QTimer::singleShot(0, this, [this, values] ()
      {
        emit netRequest(values["id"].toInt(),
                        values["req"].toString(),
                        values["cb"].toString());
      });
    }
    else
      success = false;
  }

  return success;
}
