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

#include <qcombobox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include "DeviceFrameImpl.h"
#include "Flytec.h"
#include "Glider.h"
#include "IFlyHighRC.h"
#include "IGliderForm.h"
#include "ISql.h"

DeviceFrameImpl::DeviceFrameImpl(QWidget* parent)
: QWidget(parent)
{
  Flytec *pFlytec;

  pFlytec = static_cast<Flytec*>(IGPSDevice::pInstance());

  setupUi(this);
  comboBox_Language->setEnabled(pFlytec->deviceId() == IFlyHighRC::DevFlytec6020);
  updateGlider();
}

DeviceFrameImpl::~DeviceFrameImpl()
{
}

void DeviceFrameImpl::newGlider()
{
  Glider::GliderListType gliders;
	Glider glider;

  ISql::pInstance()->gliderList(gliders);
	IGliderForm newGlider(this, "New Glider", &glider, gliders);

	if(newGlider.exec())
	{
		ISql::pInstance()->add(glider);
		updateGlider();
	}
}

void DeviceFrameImpl::update()
{
  Flytec *pFlytec;
	Pilot dbPilot;
	QString dbPilotName;
	QString dbGlider;
	QString pilotName;
	QString callsign;
	QString glider;
	DeviceInfoType devInfo;
	int syncRes;

  // fetch data pointer for easy access
  pFlytec = static_cast<Flytec*>(IGPSDevice::pInstance());
  pFlytec->open();

	if(pFlytec->deviceInfo(devInfo))
	{
		lineEdit_SerialNr->setText(QString("%1").arg(devInfo.serialNr));
		lineEdit_SwVersion->setText(QString(devInfo.swVersion));
		lineEdit_DeviceIdent->setText(QString(devInfo.deviceIdent));
	}

	pFlytec->close();

	// pilot name
	pilotName = pFlytec->parRead(MemUndef, PILOT_NAME_POS, FtString).toString();

	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), dbPilot);
	dbPilot.fullName(dbPilotName);

	if(pilotName != dbPilotName.left(pilotName.length()))
	{
		syncRes = QMessageBox::question(this, "Different pilots",
        "Pilot on GPS differ from pilot in database. Set pilot?",
			 	"DB to GPS", "Ignore");

		switch(syncRes)
		{
			case 0: // From database
				pilotName = dbPilotName;
			break;
			default:
			break;
		}
	}

	lineEdit_PilotName->setText(pilotName);

	// glider
	glider = pFlytec->parRead(MemUndef, GLIDER_TYPE_POS, FtString).toString();
	dbGlider = m_gliderList[comboBoxModel->currentIndex()].model();

	if(glider != dbGlider)
	{
		syncRes = QMessageBox::question(this, "Different gliders",
				"Glider on GPS differ from glider in database. Set glider?",
				"DB to Dialog", "Ignore");

		switch(syncRes)
		{
			case 0: // from Pilot
				dbPilot.glider().fullName(dbGlider);
				selectGlider(dbGlider);
			break;
			default:
			break;
		}
	}

	// callsign
	callsign = pFlytec->parRead(MemUndef, GLIDER_ID_POS, FtString).toString();

	if(callsign != dbPilot.callSign().left(callsign.length()))
	{
		syncRes = QMessageBox::question(this, "Different callsigns",
        "Callsign on GPS differ from callsign in database. Set callsign?",
			 	"DB to GPS", "Ignore");

		switch(syncRes)
		{
			case 0: // from database
				callsign = dbPilot.callSign();
			break;
			default:
			break;
		}
	}

	// glider id
	lineEdit_GliderID->setText(callsign);

	// battery type
  comboBox_BattType->setCurrentIndex(pFlytec->parRead(MemUndef, BATT_TYPE_POS, FtUInt8).toInt());

  // language
  if(pFlytec->deviceId() == IFlyHighRC::DevFlytec6020)
  {
    comboBox_Language->setCurrentIndex(pFlytec->parRead(MemUndef, LANGUAGE_POS, FtUInt8).toInt());
  }
}

void DeviceFrameImpl::store()
{
  Flytec *pFlytec;
	QString pilotName;
	Pilot dbPilot;
	QString dbPilotName;
	QString dbGlider;
	QString callsign;
	QString rcCallsign;
	QString glider;
	QString rcGlider;
	int syncRes;

  pFlytec = static_cast<Flytec*>(IGPSDevice::pInstance());

	// pilot
	pilotName = lineEdit_PilotName->text();
	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), dbPilot);
	dbPilot.fullName(dbPilotName);

	if(pilotName != dbPilotName.left(pilotName.length()))
	{
		syncRes = QMessageBox::question(this, "Different pilots",
        "Pilot in database differ from pilot in dialog. Set pilot?",
				"DB to GPS", "Ignore");

		switch(syncRes)
		{
			case 0: // from database
				pilotName = dbPilotName;
				lineEdit_PilotName->setText(dbPilotName);
			break;
			default:
			break;
		}
	}

	pFlytec->parWrite(MemUndef, PILOT_NAME_POS, FtString, pilotName);

	// glider
	m_gliderList[comboBoxModel->currentIndex()].fullName(glider);
	dbPilot.glider().fullName(dbGlider);

	if(glider != dbGlider)
	{
		syncRes = QMessageBox::question(this, "Different gliders",
				"Glider in database differ from glider in dialog. Set glider?",
				"DB to GPS", "Ignore");

		switch(syncRes)
		{
      case 0: // From Pilot
        selectGlider(glider);
        glider = dbPilot.glider().model();
			break;
			default:
				glider = m_gliderList[comboBoxModel->currentIndex()].model();
			break;
		}
	}
	else
	{
		glider = m_gliderList[comboBoxModel->currentIndex()].model();
	}

	pFlytec->parWrite(MemUndef, GLIDER_TYPE_POS, FtString, glider);

	// callsign
	callsign = lineEdit_GliderID->text();

	if(callsign != dbPilot.callSign().left(callsign.length()))
	{
		syncRes = QMessageBox::question(this, "Different callsigns",
          "Callsign in database differ from callsign in dialog. Set callsign?");
//                ,"DB to GPS", "Ignore");

		switch(syncRes)
		{
      case QMessageBox::Ok: // from database
        callsign = dbPilot.callSign();
        lineEdit_PilotName->setText(callsign);
			break;
			default:
			break;
		}
	}

	// glider id
	pFlytec->parWrite(MemUndef, GLIDER_ID_POS, FtString, callsign);

	// battery type
	pFlytec->parWrite(MemUndef, BATT_TYPE_POS, FtUInt8, comboBox_BattType->currentIndex());

  // language
  if(pFlytec->deviceId() == IFlyHighRC::DevFlytec6020)
  {
    pFlytec->parWrite(MemUndef, LANGUAGE_POS, FtUInt8, comboBox_Language->currentIndex());
  }
}

void DeviceFrameImpl::updateGlider()
{
	QStringList list;
	QString gliderName;
	Glider::GliderListType::iterator it;
	Pilot dbPilot;

	comboBoxModel->clear();
	m_gliderList.clear();
	ISql::pInstance()->gliderList(m_gliderList);

	for(it=m_gliderList.begin(); it!=m_gliderList.end(); it++)
	{
		(*it).fullName(gliderName);
    comboBoxModel->addItem(gliderName);
	}

	ISql::pInstance()->pilot(IFlyHighRC::pInstance()->pilotId(), dbPilot);
	dbPilot.glider().fullName(gliderName);
	selectGlider(gliderName);
}

void DeviceFrameImpl::selectGlider(const QString &name)
{
	QString str;
	int index;
	int maxIndex;
	bool found = false;

	maxIndex = comboBoxModel->count();

	for(index=0; index<maxIndex; index++)
	{
    found = (comboBoxModel->itemText(index) == name);

		if(found)
		{
      comboBoxModel->setCurrentIndex(index);
			break;
		}
	}
}
