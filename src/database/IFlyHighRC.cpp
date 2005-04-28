/***************************************************************************
 *   Copyright (C) 2005 by Alex Graf                                     *
 *   grafal@sourceforge.net                                                         *
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

#include <qdir.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include "IFlyHighRC.h"

#define MAX_LINE_SIZE 255
#define MAX_VAR_SIZE 20

const QString DeviceTag = "[device]\n";
const QString DeviceNameVar = "name=";
const QString DeviceLineVar = "line=";
const QString DeviceSpeedVar = "speed=";
const QString DateTimeTag = "[datetime]\n";
const QString DateTimeUtcVar = "utc=";
const QString DirectoryTag = "[directory]\n";
const QString DirectoryLastVar = "last=";

IFlyHighRC *IFlyHighRC::m_pInstance = NULL;

IFlyHighRC* IFlyHighRC::pInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new IFlyHighRC;
	}
	
	return m_pInstance;
}

IFlyHighRC::IFlyHighRC()
{
	m_deviceNameList = "Flytec5020";
//	m_deviceNameList += "Flytec5030";
	m_deviceSpeedList = "57600";
//	m_deviceSpeedList += "115200";

	m_rcFile.setName("/home/grafal/.flyhighrc");
}

uint IFlyHighRC::deviceName()
{
	return m_deviceName;
}

void IFlyHighRC::setDeviceName(uint index)
{
	m_deviceName = index;
}

const QString& IFlyHighRC::deviceLine()
{
	return m_deviceLine;
}

void IFlyHighRC::setDeviceLine(const QString &line)
{
	m_deviceLine = line;
}

uint IFlyHighRC::deviceSpeed()
{
	return m_deviceSpeed;
}

void IFlyHighRC::setDeviceSpeed(uint index)
{
	m_deviceSpeed = index;
}

char IFlyHighRC::utcOffset()
{
	return m_utcOffset;
}

void IFlyHighRC::setUtcOffset(char offset)
{
	if(offset > 12)
	{
		offset = 12;
	}
	else if(offset < -12)
	{
		offset = -12;
	}
	
	m_utcOffset = offset;
}

const QString& IFlyHighRC::lastDir()
{
	return m_lastDir;
}

void IFlyHighRC::setLastDir(const QString& name)
{
	m_lastDir = name;
}

const QString& IFlyHighRC::versionInfo()
{
	return m_versionInfo;
}

const QStringList& IFlyHighRC::deviceNameList()
{
	return m_deviceNameList;
}

const QStringList& IFlyHighRC::deviceSpeedList()
{
	return m_deviceSpeedList;
}

void IFlyHighRC::loadRC()
{
	QByteArray rcFileData;
	QBuffer buff;
	char line[MAX_LINE_SIZE];
	
	if(m_rcFile.open(IO_ReadOnly))
	{
		rcFileData = m_rcFile.readAll();
		m_rcFile.close();
		buff.setBuffer(rcFileData);
		buff.open(IO_ReadOnly);
		
		while(buff.readLine(line, MAX_LINE_SIZE) > 0)
		{
			if(line == DeviceTag)
			{
				parseSerialLine(buff);
			}
			else if(line ==  DateTimeTag)
			{
				parseDateTime(buff);
			}
			else if(line == DirectoryTag)
			{
				parseDirectory(buff);
			}
		}
		buff.close();
	}
	else
	{
		m_deviceName = 0;
		m_deviceLine = "/dev/ttyS0";
		m_deviceSpeed = 0;
		m_utcOffset = 0;
		m_lastDir = QDir::homeDirPath();
	}
	
	m_versionInfo = "FlyHigh Version 0.3";
}

void IFlyHighRC::saveRC()
{
	if(m_rcFile.open(IO_WriteOnly | IO_Truncate))
	{
		QTextStream stream(&m_rcFile);
		
		saveSerialLine(stream);
		saveDateTime(stream);
		saveDirectory(stream);
		
		m_rcFile.close();
	}
}

void IFlyHighRC::parseSerialLine(QBuffer &buff)
{
	char line[MAX_LINE_SIZE];
	QString var;
	QString val;
	
	while(buff.readLine(line, MAX_LINE_SIZE) > 0)
	{
		parseValue(line, var, val);
		
		if(DeviceLineVar.find(var) == 0)
		{
			setDeviceLine(val);
		}
		else if(DeviceSpeedVar.find(var) == 0)
		{
			setDeviceSpeed(m_deviceSpeedList.findIndex(val));
		}
		else if(DeviceNameVar.find(var) == 0)
		{
			setDeviceName(m_deviceNameList.findIndex(val));
		}
		else
		{
			break;
		}
	}
}

void IFlyHighRC::saveSerialLine(QTextStream &stream)
{
	QString str;
	
	stream << DeviceTag;
	str = m_deviceNameList[m_deviceName];
	stream << DeviceNameVar << str << "\n";
	stream << DeviceLineVar << m_deviceLine << "\n";
	str = m_deviceSpeedList[m_deviceSpeed];
	stream << DeviceSpeedVar << str << "\n";
	stream << "\n";
}

void IFlyHighRC::parseDateTime(QBuffer &buff)
{
	char line[MAX_LINE_SIZE];
	QString var;
	QString val;
	
	while(buff.readLine(line, MAX_LINE_SIZE) > 0)
	{
		parseValue(line, var, val);
		
		if(DateTimeUtcVar.find(var) == 0)
		{
			setUtcOffset(atoi(val));
		}
		else
		{
			break;
		}
	}
}

void IFlyHighRC::saveDateTime(QTextStream &stream)
{
	QString str;
	
	stream << DateTimeTag;
	str.sprintf("%i", m_utcOffset);
	stream << DateTimeUtcVar << str << "\n";
	stream << "\n";
}

void IFlyHighRC::parseDirectory(QBuffer &buff)
{
	char line[MAX_LINE_SIZE];
	QString var;
	QString val;
	
	while(buff.readLine(line, MAX_LINE_SIZE) > 0)
	{
		parseValue(line, var, val);
		
		if(DirectoryLastVar.find(var) == 0)
		{
			m_lastDir = val;
		}
		else
		{
			break;
		}
	}
}

void IFlyHighRC::saveDirectory(QTextStream &stream)
{
	stream << DirectoryTag;
	stream << DirectoryLastVar << m_lastDir << "\n";
	stream << "\n";
}

void IFlyHighRC::parseValue(char *line, QString &var, QString &val)
{
	QString str = line;
	int valEnd;
	int varEnd;
	
	varEnd = str.find('=');
	valEnd = str.find('\n');
	var = str.left(varEnd);
	val = str.mid(varEnd + 1, valEnd -  varEnd - 1);
}
