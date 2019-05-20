/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010, 2011 by Miguel Escudero Gestal
 *   miguelescudero@users.sourceforge.net
 *
 *   This file is part of tubularix
 *
 *   tubularix is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   tubularix is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#ifndef NETWORKCLIENTDIALOG_H
#define NETWORKCLIENTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class NetworkClientDialog;
}

class NetworkClientDialog : public QDialog {
    Q_OBJECT
public:
    NetworkClientDialog(QWidget *parent = 0);
    ~NetworkClientDialog();

private:
	Ui::NetworkClientDialog *m_ui;

private slots:
	void readSettings();
	void writeSettings();
	void acceptChanges();
	void rejectChanges();

protected:
    void changeEvent(QEvent *e);
};

#endif // NETWORKCLIENTDIALOG_H