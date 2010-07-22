/***************************************************************************
 *   Copyright (C) 2005 by Alex Graf                                       *
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

#ifndef _MainWindow_h_
#define _MainWindow_h_

#include <q3mainwindow.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <QCloseEvent>

class Q3PopupMenu;
class QWorkspace;
class MDIWindow;

class TrackWindow;

class MainWindow: public Q3MainWindow
{
	Q_OBJECT
	public:
		MainWindow();
		virtual ~MainWindow();

	protected:
		void closeEvent(QCloseEvent *);
		
	private slots:
		void flights_fromSQL();
		void flights_fromGPS();
		void flights_experience();
		void analysis_gliders();
		void analysis_servicing();
		void waypoints_fromSQL();
		void waypoints_fromGPS();
		void routes_fromSQL();
		void routes_fromGPS();
//		void airspaces_fromSQL();
		void airspaces_fromGPS();
		void airspaces_fromFile();
		void windows_tile_horizontally();
		void windows_activated(int id);
		void settings_port();
		void settings_device(int id);
		void settings_configure_device();
		void settings_pilotInfo();
		void help_about();
		void aboutToShow();
		
	private:
		QWorkspace* m_pWorkSpace;
		Q3PopupMenu* m_pWindowsMenu;
		Q3PopupMenu* m_pDevicesMenu;
		Q3PopupMenu* m_pSpecialMenu;
		MDIWindow* m_pActiveWin;
		int m_specialMenuId;

		void showWindow(Q3MainWindow *pWin);
		void setCurrentDevice(int id);
};

#endif
