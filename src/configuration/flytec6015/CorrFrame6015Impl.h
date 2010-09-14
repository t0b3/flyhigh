/***************************************************************************
 *   Copyright (C) 2004 by Alex Graf                                       *
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

#ifndef CorrFrame6015Impl_h
#define CorrFrame6015Impl_h

#include "Frame6015.h"
#include "ui_CorrFrame6015.h"

class CorrFrame6015Impl: public QWidget, public Ui::CorrFrame6015, public Frame6015
{
	Q_OBJECT

	public:
		CorrFrame6015Impl(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0);

		~CorrFrame6015Impl();

		void update();

		void store();
};

#endif

