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
 
#ifndef AirSpaceFormImpl_h
#define AirSpaceFormImpl_h

#include <QDialog>
#include <QPoint>
#include <QVector>
#include "ui_AirSpaceForm.h"

class QCloseEvent;
class QPaintEvent;
class AirSpace;

class AirSpaceFormImpl: public QDialog, public Ui::AirSpaceForm
{
	Q_OBJECT
	
	public:
		AirSpaceFormImpl(QWidget* parent, const QString &caption, AirSpace *pAirSpace);

		void setAirSpace(AirSpace *pAirSpace);
		
	protected slots:
		void tableChanged(int col, int row);

	protected:
		void closeEvent(QCloseEvent * e);

		void paintEvent(QPaintEvent *pEvent);
	
	private:
		typedef QVector<QPoint> PointArray;

		enum Fields{Use, Longitude, Latitude};
		
		QRect m_drawRect;
		AirSpace *m_pAirSpace;

		void translateEdgePts(PointArray &edgePts, int dx, int dy);

		void scaleEdgePts(PointArray &edgePts, double sx, double sy);
};

#endif
