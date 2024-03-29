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
#ifndef Device_h
#define Device_h

#include <chrono>
#include <QtCore/QString>

class QSerialPort;

class Device
{
  public:
    Device(bool flow);

    ~Device();

    bool openDevice(const QString &dev, int baud);

    bool isOpen();

    void closeDevice();

    bool recieveTlg(int tout, bool head);

    const QString& getTlg();

    bool sendTlg(const QString &tlg);

    void flush();

  private:
    typedef enum State{SearchHead, ReadTlg}State;

    enum {MaxTlgSize = 255};

    QSerialPort *m_serialPort;
    QString m_tlg;
    std::chrono::time_point<std::chrono::system_clock> m_tout;

    bool getChar(char &ch);

    bool writeBuffer(const char *pBuff, int len);

    void startTimer(int tout);

    bool isElapsed();
};

#endif
