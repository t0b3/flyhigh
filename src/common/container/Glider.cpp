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

#include "Glider.h"

Glider::Glider()
{
  m_id = -1;
  m_manufacturer = "";
  m_model = "";
  m_airtime = 0;
  m_flights = 0;
  m_passengers = 0;
}

int Glider::id() const
{
  return m_id;
}

void Glider::setId(int id)
{
  m_id = id;
}

const QString& Glider::manufacturer() const
{
  return m_manufacturer;
}

void Glider::setManufacturer(const QString &manu)
{
  m_manufacturer = manu;
}

const QString& Glider::model() const
{
  return m_model;
}

void Glider::setModel(const QString &model)
{
  m_model = model;
}

void Glider::fullName(QString &name) const
{
  name = m_model;

  if(m_manufacturer != "")
  {
   name += " (" + m_manufacturer + ")";
  }
}

void Glider::olcName(QString &name) const
{
  name = m_manufacturer + " " + m_model;
}

void Glider::setSerial(const QString &serial)
{
  m_serial = serial;
}

const QString& Glider::serial() const
{
  return m_serial;
}

void Glider::setAirtime(uint time)
{
  m_airtime = time;
}

uint Glider::airtime() const
{
  return m_airtime;
}

void Glider::setFlights(uint n)
{
  m_flights = n;
}

uint Glider::fligths() const
{
  return m_flights;
}

void Glider::setPassengers(uint count)
{
  m_passengers = count;
}

uint Glider::passengers() const
{
  return m_passengers;
}

Glider& Glider::operator=(const Glider &glider)
{
  m_id = glider.m_id;
  m_manufacturer = glider.m_manufacturer;
  m_model = glider.m_model;
  m_serial = glider.m_serial;
  m_airtime = glider.m_airtime;
  m_flights = glider.m_flights;
  m_passengers = glider.m_passengers;

  return *this;
}

bool Glider::operator==(const Glider &glider)
{
  return ((m_manufacturer == glider.m_manufacturer) &&
          (m_model == glider.m_model) &&
          (m_serial == glider.m_serial));
}
