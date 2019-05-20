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

#include "networkserverdialog.h"
#include "ui_networkserverdialog.h"
#include <QSettings>


//public


NetworkServerDialog::NetworkServerDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NetworkServerDialog)
{
    m_ui->setupUi(this);

	connect(m_ui->okButton, SIGNAL(clicked()), this, SLOT(acceptChanges()));
	connect(m_ui->cancelButton, SIGNAL(clicked()), this, SLOT(rejectChanges()));

	readSettings();
}

NetworkServerDialog::~NetworkServerDialog()
{
    delete m_ui;
}


//private slots


void NetworkServerDialog::readSettings()
{
	QSettings settings;
	settings.beginGroup("NetworkServer");
	//m_ui->ipLineEdit->setText(settings.value("ip", "127.0.0.1").toString());
	m_ui->portLineEdit->setText(settings.value("port", 5124).toString());
	settings.endGroup();
}

void NetworkServerDialog::writeSettings()
{
	QSettings settings;
	settings.beginGroup("NetworkServer");
	//settings.setValue("ip", m_ui->ipLineEdit->text());
	settings.setValue("port", m_ui->portLineEdit->text());
	settings.endGroup();
}

void NetworkServerDialog::acceptChanges()
{
	writeSettings();
	accept();
}

void NetworkServerDialog::rejectChanges()
{
	reject();
}


//protected


void NetworkServerDialog::changeEvent(QEvent *event)
{
	QDialog::changeEvent(event);
	switch (event->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
