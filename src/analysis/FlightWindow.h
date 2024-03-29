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
#ifndef FlightWindow_h
#define FlightWindow_h

#include "FlightPointList.h"
#include "IDataBase.h"
#include "GnuPlot.h"
#include "Photo.h"
#include "TableWindow.h"

class QWidget;
class Elevation;
class OLCOptimizer;

class FlightWindow: public TableWindow
{
	Q_OBJECT
	public:
		FlightWindow(QWidget* parent, const QString &name, IDataBase::SourceType src);

	private slots:
		void file_update();

		void file_AddToSqlDB();

		void file_new();

		void file_edit();

		void file_delete();

		void file_import();

		void file_exportIGC();

		void file_exportKML();

		void plot_airData();

		void plot_OLC();

		void showOnMap();

		void showOnWebMap();

		void showPhotos();

    void upload2OLC();

		void confirmDownload(Elevation *pElevation, const QString &question);

	private:
		enum Fields{Id, Nr, Icon, Date, Time, Duration, Model, StartPt, LandPt, Distance, Comment};

		IDataBase *m_pDb;
		QString m_fileName;
		GnuPlot m_plotter;
		Flight::FlightListType m_flightList;

    void setFlightToRow(uint row, Flight &flight);

    int getCurrentFlightIndex();

		void plotFlighPointList(FlightPointList &fpList, const QString& title);

    Route::Type getBestOlcTurnPts(const OLCOptimizer &optimizer,
                           WayPoint::WayPointListType &tpList,
                           uint &dist, float &score);

    void resolvePhotos(const QString &path, const FlightPointList &fpList,
                       Photo::PhotoListType &photoList);
};

#endif
