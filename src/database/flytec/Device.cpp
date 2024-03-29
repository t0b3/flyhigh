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

#include <QtCore/QString>
#include <QtSerialPort/QSerialPort>
#include <chrono>
#include "Device.h"

#include <QtCore/QDebug>

Device::Device(bool flow)
{
  m_tlg = "";
  m_tout = std::chrono::system_clock::now();

  m_serialPort = new QSerialPort();

  if(flow)
  {
    m_serialPort->setFlowControl(QSerialPort::SoftwareControl);
  }
  else
  {
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
  }

  m_serialPort->setParity(QSerialPort::NoParity);
  m_serialPort->setDataBits(QSerialPort::Data8);
  m_serialPort->setStopBits(QSerialPort::OneStop);
}

Device::~Device()
{
  delete m_serialPort;
}

bool Device::openDevice(const QString &dev, int baud)
{
  bool success;

  m_serialPort->setPortName(dev);
  m_serialPort->setBaudRate(baud);
  success = m_serialPort->open(QIODevice::ReadWrite);

  return success;
}

bool Device::isOpen()
{
  return m_serialPort->isOpen();
}

void Device::closeDevice()
{
  m_serialPort->close();
}

bool Device::recieveTlg(int tout, bool head)
{
  int charNr = 0;
  bool validTlg = false;
  char ch;
  State state;

  if(head)
  {
    state = SearchHead;
  }
  else
  {
    state = ReadTlg;
  }

  m_tlg = "";
  startTimer(tout);

  do
  {
    if(getChar(ch))
    {
      if(state == SearchHead)
      {
        if(ch == '$')
        {
          state = ReadTlg;
          m_tlg = ch;
        }
      }
      else
      {
        charNr++;
        m_tlg += ch;
        validTlg = (ch == '\n');

        if(charNr > MaxTlgSize)
        {
          m_tlg = "";
        }

        if(validTlg || (charNr > MaxTlgSize))
        {
          break;
        }
      }
    }
  }while(!isElapsed());

  return validTlg;
}

const QString& Device::getTlg()
{
  return m_tlg;
}

bool Device::sendTlg(const QString &tlg)
{
  return writeBuffer(tlg.toLatin1().constData(), tlg.length());
}

void Device::flush()
{
  m_tlg = "";
  m_serialPort->clear();
}

bool Device::getChar(char &ch)
{
  enum {XON = 0x11, XOFF = 0x13};

  bool success = (m_serialPort->bytesAvailable() > 0);

  if(!success)
    success = m_serialPort->waitForReadyRead(100);

  if(success)
  {
    success = m_serialPort->getChar(&ch);
    success &= (ch != XON) && (ch != XOFF);
  }

  return success;
}

bool Device::writeBuffer(const char *pBuff, int len)
{
  int nWrite;

  nWrite = m_serialPort->write(pBuff, len);

  return (nWrite == len);
}

void Device::startTimer(int tout)
{
  m_tout = std::chrono::system_clock::now() + std::chrono::milliseconds(tout);
}

bool Device::isElapsed()
{
  bool elapsed = (std::chrono::system_clock::now() > m_tout);

  return (elapsed);
}
