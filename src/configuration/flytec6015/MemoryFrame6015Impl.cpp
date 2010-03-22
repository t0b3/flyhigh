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

#include <qspinbox.h>
#include <qcombobox.h>
#include "MemoryFrame6015Impl.h"

MemoryFrame6015Impl::MemoryFrame6015Impl(QWidget* parent, const char* name, WFlags fl)
: MemoryFrame6015(parent,name,fl)
{
}

MemoryFrame6015Impl::~MemoryFrame6015Impl()
{
}

void MemoryFrame6015Impl::update(QByteArray &arr)
{
/*
	// Recording Interval
	spinBox_Intervall->setValue(arr[REC_INTERVAL_POS]);

	// Stop Mode
	comboBox_Mode->setCurrentItem(arr[REC_STOP_MODE_POS]);
*/
}

void MemoryFrame6015Impl::store(QByteArray &arr)
{
/*
	// Recording Interval
	arr[REC_INTERVAL_POS] = spinBox_Intervall->value();

	// Stop Mode
	arr[REC_STOP_MODE_POS] = comboBox_Mode->currentItem();
*/
}

#include "MemoryFrame6015Impl.moc"

