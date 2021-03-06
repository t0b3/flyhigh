/***************************************************************************
 *   Copyright (C) 2013 by Alex Graf                                       *
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
#ifndef _ELEVATIONTILELIST_H
#define _ELEVATIONTILELIST_H

#include "ElevationTile.h"

class ElevationTileList
{
  public:
    typedef QVector<ElevationTile*> TileList;

    typedef TileList::iterator iterator;

    ElevationTileList(bool dataOwner = true);

    ~ElevationTileList();

    void push_back(ElevationTile *pTile);

    void clear();

    iterator begin();

    iterator end();

  private:
    TileList m_tileList;
    bool m_dataOwner;
};
#endif
