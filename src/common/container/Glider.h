/***************************************************************************
 *   Copyright (C) 2004 by Alex Graf                                       *
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

#ifndef Glider_h
#define Glider_h

#include <QString>
#include <QVector>

class Glider
{
  public:
    typedef QVector<Glider> GliderListType;

    Glider();

    int id() const;

    void setId(int id);

    const QString& manufacturer() const;

    void setManufacturer(const QString &manu);

    const QString& model() const;

    void setModel(const QString &model);

    void fullName(QString &name) const;

    void olcName(QString &name) const;

    void setSerial(const QString &serial);

    const QString& serial() const;

    void setAirtime(uint time); // in seconds

    uint airtime() const;

    void setFlights(uint n);

    uint fligths() const;

    void setPassengers(uint count);

    uint passengers() const;

    Glider& operator=(const Glider &glider);

    bool operator==(const Glider &glider);

  private:
    int m_id;
    QString m_manufacturer;
    QString m_model;
    QString m_serial;
    uint m_airtime;
    uint m_flights;
    uint m_passengers;
};

#endif
