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

#include <qcombobox.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qslider.h>
#include "DisplayFrame6015Impl.h"
#include "Flytec6015.h"

DisplayFrame6015Impl::DisplayFrame6015Impl(QWidget* parent, const char* name, Qt::WFlags fl)
: QWidget(parent)
{
  setupUi(this);
}

DisplayFrame6015Impl::~DisplayFrame6015Impl()
{
}

void DisplayFrame6015Impl::update(QByteArray &arr)
{
	Flytec6015 *pDev;
	uint uiValue;

	pDev = static_cast<Flytec6015*>(IGPSDevice::pInstance());

	// Alti, Line 4, Alt2, Alt3
	uiValue = pDev->memoryRead(MemFa, DIV_FLAGS, UInt16).toUInt();
	comboBox_AltDisp->setCurrentItem((uiValue & MASK_ALT_DISP) >> 4);
	comboBox_Line4->setCurrentItem((uiValue & MASK_LINE4_DISP) >> 5);
	comboBox_Alt2Mode->setCurrentItem((uiValue & MASK_ALT2_MODE) >> 6);
	comboBox_Alt3Mode->setCurrentItem((uiValue & MASK_ALT3_MODE) >> 9);
}

void DisplayFrame6015Impl::store(QByteArray &arr)
{
/*
	int16_t i16value;

	// LCD Contrast
	i16value = slider_Contrast->value();
	arr[LCD_CONTRAST_POS] = (u_char)(i16value >> 8);
	arr[LCD_CONTRAST_POS+1] = (u_char)(i16value & 0xFF);
	
	// User Field 0
	arr[USERFIELD_0_POS] = comboBox_UserField_0_0->currentItem();
	arr[USERFIELD_0_POS+1] = comboBox_UserField_0_1->currentItem();
	arr[USERFIELD_0_POS+2] = comboBox_UserField_0_2->currentItem();
	
	// User Field 1
	arr[USERFIELD_1_POS] = comboBox_UserField_1_0->currentItem();
	arr[USERFIELD_1_POS+1] = comboBox_UserField_1_1->currentItem();
	arr[USERFIELD_1_POS+2] = comboBox_UserField_1_2->currentItem();

	// User Field 2
	arr[USERFIELD_2_POS] = comboBox_UserField_2_0->currentItem();
	arr[USERFIELD_2_POS+1] = comboBox_UserField_2_1->currentItem();
	arr[USERFIELD_2_POS+2] = comboBox_UserField_2_2->currentItem();
*/
}

void DisplayFrame6015Impl::insertItems( QComboBox * pItem )
{
	pItem->clear();
	pItem->insertItem( tr( "Wind spd" ) );
	pItem->insertItem( tr( "Time" ) );
	pItem->insertItem( tr( "Fl.Time" ) );
	pItem->insertItem( tr( "Gnd spd" ) );
	pItem->insertItem( tr( "Speed diff" ) );
	pItem->insertItem( tr( "Dist WP" ) );
	pItem->insertItem( tr( "Bearing" ) );
	pItem->insertItem( tr( "Track" ) );
	pItem->insertItem( tr( "Temp" ) );
	pItem->insertItem( tr( "Alt 2" ) );
	pItem->insertItem( tr( "Alt 3" ) );
	pItem->insertItem( tr( "QNH hPa" ) );
	pItem->insertItem( tr( "L/D Gnd" ) );
	pItem->insertItem( tr( "L/D air" ) );
	pItem->insertItem( tr( "L/D Req" ) );
	pItem->insertItem( tr( "Dist to ^" ) );
	pItem->insertItem( QString::null );
	pItem->insertItem( tr( "Alt @ BG" ) );
	pItem->insertItem( tr( "FL (ft)" ) );
	pItem->insertItem( tr( "Air spd" ) );
	pItem->insertItem( tr( "Alt @ goal" ) );
	pItem->insertItem( tr( "Dist goal" ) );
	pItem->insertItem( tr( "Vario" ) );
	pItem->insertItem( tr( "Alt 1" ) );
	pItem->insertItem( tr( "SMS p/t" ) );
	pItem->insertItem( tr( "Dist toff" ) );
	pItem->insertItem( tr( "Dist cyl" ) );
	pItem->insertItem( tr( "L/D rel goal" ) );
	pItem->insertItem( tr( "Race start" ) );
}



#include "moc_DisplayFrame6015Impl.cxx"

