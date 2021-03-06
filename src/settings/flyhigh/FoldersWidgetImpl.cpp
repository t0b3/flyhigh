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

#include <QFileDialog>
#include <QLineEdit>
#include "FoldersWidgetImpl.h"
#include "IFlyHighRC.h"

FoldersWidgetImpl::FoldersWidgetImpl(QWidget* parent, const QString& title)
  :QWidget(parent)
{
  setupUi(this);
  setWindowTitle(title);

  lineEdit_flyhigh->setText(IFlyHighRC::pInstance()->flyHighDir());
  lineEdit_airspace->setText(IFlyHighRC::pInstance()->airspaceDir());
  lineEdit_elevation->setText(IFlyHighRC::pInstance()->elevationDir());
  lineEdit_swisstopo->setText(IFlyHighRC::pInstance()->swissTopoDir());
}

FoldersWidgetImpl::~FoldersWidgetImpl()
{
}

void FoldersWidgetImpl::selectDirFlyHigh()
{
  QString path;

  if(selectDir(path))
  {
    lineEdit_flyhigh->setText(path);
    IFlyHighRC::pInstance()->setFlyHighDir(path);
  }
}

void FoldersWidgetImpl::selectDirAirspace()
{
  QString path;

  if(selectDir(path))
  {
    lineEdit_airspace->setText(path);
    IFlyHighRC::pInstance()->setAirspaceDir(path);
  }
}

void FoldersWidgetImpl::selectDirElevation()
{
  QString path;

  if(selectDir(path))
  {
    lineEdit_elevation->setText(path);
    IFlyHighRC::pInstance()->setElevationDir(path);
  }
}

void FoldersWidgetImpl::selectDirSwissTopo()
{
  QString path;

  if(selectDir(path))
  {
    lineEdit_swisstopo->setText(path);
    IFlyHighRC::pInstance()->setSwissTopoDir(path);
  }
}

bool FoldersWidgetImpl::selectDir(QString &path)
{
  QFileDialog dlg;
  bool accept;

  dlg.setFileMode(QFileDialog::Directory);
  dlg.setOption(QFileDialog::ShowDirsOnly, true);
  accept = dlg.exec();

  if(accept)
  {
    path = dlg.directory().path();
  }

  return accept;
}
