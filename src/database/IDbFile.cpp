/***************************************************************************
 *   Copyright (C) 2019 by Alex Graf                                       *
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
#include <QDir>
#include <QStringList>
#include "AirSpaceList.h"
#include "Error.h"
#include "IDbFile.h"
#include "OpenAirFileParser.h"

IDbFile* IDbFile::m_pInst = NULL;

IDbFile::IDbFile()
{
}

IDbFile::~IDbFile()
{
}

IDbFile* IDbFile::pInstance()
{
  if(m_pInst == nullptr)
    m_pInst = new IDbFile();

  return m_pInst;
}

bool IDbFile::airspaceList(const QString &path, AirSpaceList &airspaceList)
{
  OpenAirFileParser parser;
  QDir dir;
  QStringList fileNames;
  bool success;

  airspaceList.clear();
  dir.setPath(path);
  fileNames = dir.entryList(QStringList() << "*.txt" << "*.TXT", QDir::Files);

  for(const QString &fileName: fileNames)
  {
    success = parser.parse(path + "/" + fileName, airspaceList);
    Error::verify(success, Error::FILE_PARSE);
  }

  airspaceList.sort();

  return (airspaceList.size() > 0);
}
