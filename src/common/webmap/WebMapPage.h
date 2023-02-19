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

#ifndef _WebMapPage_h
#define _WebMapPage_h

#include <QWebEnginePage>

class WebMapPage: public QWebEnginePage
{
  Q_OBJECT

  public:
    WebMapPage(QObject *parent);

    virtual ~WebMapPage();

    void runJavaScriptRet(const QString &scriptSource, QVariant &value);

  signals:
    void appReady();

    void setOk(bool ok);

    void setLine(int line);

    void netRequest(int id, const QString &request, const QString &callback);
  protected:
    /**
      Called on JavaScript confirm("msg")
    */
    bool javaScriptConfirm(const QUrl &securityOrigin, const QString &msg) override;

};
#endif
