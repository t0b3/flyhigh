/***************************************************************************
 *   Copyright (C) 2010 by Alex Graf                                       *
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

#ifndef _WebMapWidget_h
#define _WebMapWidget_h

#include <QWidget>

class WebMap;
class WebMapOverlay;
class WebMapItem;

class WebMapWidget: public QWidget
{
	public:
		WebMapWidget(QWidget *pParent = NULL);

		~WebMapWidget();

		void loadMap();

		void addItem(WebMapItem *pItem);

		void removeItem(WebMapItem *pItem);

		void clear();

		WebMap* getMap();

	protected:
		void resizeEvent(QResizeEvent *pEvent);

	private:
		WebMap *m_pMap;
		WebMapOverlay *m_pOverlay;
};

#endif
