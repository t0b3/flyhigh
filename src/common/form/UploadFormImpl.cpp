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

#include <QCursor>
#include <QFileDialog>
#include "ISql.h"
#include "IFlyHighRC.h"
#include "IAccountForm.h"
#include "IOLCUploader.h"
#include "OLCUploaderFactory.h"
#include "Account.h"
#include "UploadFormImpl.h"

UploadFormImpl::UploadFormImpl(QWidget* parent, const QString &caption, Flight *pFlight)
  :QDialog(parent)
{
  setupUi(this);
  setWindowTitle(caption);
	setFlight(pFlight);
  updateAccount();
}

void UploadFormImpl::enableInput(bool b)
{
	buttonOk->setEnabled(b);
  pushButtonAccount->setEnabled(b);
  comboBoxModel->setEnabled(b);
  checkBoxFlightActive->setEnabled(b);

	if(b)
	{
		QWidget::unsetCursor();
	}
	else
	{
		QWidget::setCursor(QCursor(Qt::WaitCursor));
	}
}

void UploadFormImpl::setFlight(Flight *pFlight)
{
  m_pFlight = pFlight;
}

void UploadFormImpl::accept()
{

  Account* pAccount = new Account();
  IOLCUploader* pUploader = OLCUploaderFactory::getOLCUploader(pAccount);

  if(pUploader)
  {
    pUploader->uploadFlight(m_pFlight);
  }

  QDialog::accept();
}

void UploadFormImpl::newAccount()
{
  Account::AccountListType accounts;
  Account account;

  ISql::pInstance()->accountList(accounts);

  IAccountForm newAccount(this, "New Online Contest Account", &account, accounts);

  if(newAccount.exec())
  {
    ISql::pInstance()->add(account);
    updateAccount();
  }
}

void UploadFormImpl::updateAccount()
{
  Account::AccountListType::iterator it;
  QString entry;
  m_accountList.clear();
  ISql::pInstance()->accountList(m_accountList);
  comboBoxModel->clear();

  for(it=m_accountList.begin(); it!=m_accountList.end(); it++)
  {
    entry = it->username() + " (" + Account::typeAsString(it->type()) + ")";
    comboBoxModel->addItem(entry);
  }

}


#include "moc_UploadFormImpl.cxx"
