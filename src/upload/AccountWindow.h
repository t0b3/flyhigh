/***************************************************************************
 *   Copyright (C) 2005 by Alex Graf                                       *
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
#ifndef AccountWindow_h
#define AccountWindow_h

#include "Account.h"
#include "TableWindow.h"

class QWidget;
class IDataBase;

class AccountWindow: public TableWindow
{
	Q_OBJECT
	public:
    AccountWindow(QWidget* parent, const QString &name);

	private:
    void setAccountToRow(uint row, Account &account);

	private slots:
		void file_new();

		void file_edit();

		void file_delete();

		void file_update();

	private:
    enum Fields{Username, Contest, Description};

		IDataBase *m_pDb;
    Account::AccountListType m_accountList;
};

#endif
