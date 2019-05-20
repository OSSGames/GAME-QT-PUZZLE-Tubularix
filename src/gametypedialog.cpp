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

#include "gametypedialog.h"
#include "ui_gametypedialog.h"
#include "networkserverdialog.h"
#include "networkclientdialog.h"


//public


GameTypeDialog::GameTypeDialog(QWidget *parent, int gameType, int gameMode, bool customMode, int rows,
										 int columns, int startLevel,	bool samePieces, bool sendDestroyedLines) :
	QDialog(parent),
	m_ui(new Ui::GameTypeDialog)
{
	m_ui->setupUi(this);
	newConnection = false;

	connect(m_ui->buttonOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(m_ui->buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_ui->radioButton4, SIGNAL(clicked()), this, SLOT(setRowsAndColumns()));
	connect(m_ui->radioButton5, SIGNAL(clicked()), this, SLOT(setRowsAndColumns()));
	connect(m_ui->radioButton6, SIGNAL(clicked()), this, SLOT(setRowsAndColumns()));
	connect(m_ui->radioButton7, SIGNAL(clicked()), this, SLOT(setRowsAndColumns()));
	connect(m_ui->radioButton8, SIGNAL(clicked()), this, SLOT(setRowsAndColumns()));
	connect(m_ui->radioButton9, SIGNAL(clicked()), this, SLOT(setRowsAndColumns()));
	connect(m_ui->customMode, SIGNAL(clicked()), this, SLOT(setRowsAndColumns()));

	//network
	connect(m_ui->networkServerRadioButton, SIGNAL(clicked()), this, SLOT(showNetworkServerDialog()));
	connect(m_ui->networkServerRadioButton, SIGNAL(toggled(bool)),
		this, SLOT(setCheckedNetworkServerRadioButton(bool)));
	connect(m_ui->networkClientRadioButton, SIGNAL(clicked()), this, SLOT(showNetworkClientDialog()));
	connect(m_ui->networkClientRadioButton, SIGNAL(toggled(bool)),
		this, SLOT(setCheckedNetworkClientRadioButton(bool)));

	switch(gameType)
	{
	case singlePlayer:
		m_ui->radioButton1->setChecked(true);
		break;
	case twoPlayers:
		m_ui->radioButton2->setChecked(true);
		break;
	case networkServer:
		m_ui->networkServerRadioButton->setCheckable(true);
		m_ui->networkServerRadioButton->setChecked(true);
		break;
	case networkClient:
		m_ui->networkClientRadioButton->setCheckable(true);
		m_ui->networkClientRadioButton->setChecked(true);
		break;
	}

	switch(gameMode)
	{
	case tubularix:
		m_ui->radioButton4->setChecked(true);
		break;
	case invertedTubularix:
		m_ui->radioButton5->setChecked(true);
		break;
	case tetrisWithoutWalls:
		m_ui->radioButton6->setChecked(true);
		break;
	case invertedTetrisWithoutWalls:
		m_ui->radioButton7->setChecked(true);
		break;
	case tetris:
		m_ui->radioButton8->setChecked(true);
		break;
	case invertedTetris:
		m_ui->radioButton9->setChecked(true);
		break;
	}

	m_ui->customMode->setChecked(customMode);
	m_ui->rowsSpinBox->setValue(rows);
	m_ui->columnsSpinBox->setValue(columns);

	m_ui->startLevelSpinBox->setValue(startLevel);

	m_ui->checkBoxSamePieces->setChecked(samePieces);
	m_ui->checkBoxSendDestroyedLines->setChecked(sendDestroyedLines);
}

GameTypeDialog::~GameTypeDialog()
{
    delete m_ui;
}

int GameTypeDialog::selectedGameType()
{
	if(m_ui->radioButton1->isChecked())
		return singlePlayer;
	else if(m_ui->radioButton2->isChecked())
		return twoPlayers;
	else if(m_ui->networkServerRadioButton->isChecked())
		return networkServer;
	else if(m_ui->networkClientRadioButton->isChecked())
		return networkClient;
	else
		return 0;
}

int GameTypeDialog::selectedGameMode()
{
	if(m_ui->radioButton4->isChecked())
		return tubularix;
	else if(m_ui->radioButton5->isChecked())
		return invertedTubularix;
	else if(m_ui->radioButton6->isChecked())
		return tetrisWithoutWalls;
	else if(m_ui->radioButton7->isChecked())
		return invertedTetrisWithoutWalls;
	else if(m_ui->radioButton8->isChecked())
		return tetris;
	else if(m_ui->radioButton9->isChecked())
		return invertedTetris;
	else
		return 0;
}

bool GameTypeDialog::customMode()
{
	return m_ui->customMode->isChecked();
}

int GameTypeDialog::rows()
{
	return m_ui->rowsSpinBox->value();
}

int GameTypeDialog::columns()
{
	return m_ui->columnsSpinBox->value();
}

int GameTypeDialog::startLevel()
{
	return m_ui->startLevelSpinBox->value();
}

bool GameTypeDialog::samePieces()
{
	return m_ui->checkBoxSamePieces->isChecked();
}

bool GameTypeDialog::sendDestroyedLines()
{
	return m_ui->checkBoxSendDestroyedLines->isChecked();
}


//private slots


void GameTypeDialog::setRowsAndColumns()
{
	if(customMode())
		return;

	switch(selectedGameMode())
	{
		case tubularix:
		case invertedTubularix:
			m_ui->rowsSpinBox->setValue(18);
			m_ui->columnsSpinBox->setValue(12);
			break;
		case tetrisWithoutWalls:
		case invertedTetrisWithoutWalls:
		case tetris:
		case invertedTetris:
			m_ui->rowsSpinBox->setValue(23);
			m_ui->columnsSpinBox->setValue(10);
			break;
	}
}

void GameTypeDialog::setCheckedNetworkServerRadioButton(bool value)
{
	if(!value)
		m_ui->networkServerRadioButton->setCheckable(false);
}

void GameTypeDialog::showNetworkServerDialog()
{
	NetworkServerDialog dialog(this);

	if(dialog.exec())
	{
		m_ui->networkServerRadioButton->setCheckable(true);
		m_ui->networkServerRadioButton->setChecked(true);
		newConnection = true;
	}
}

void GameTypeDialog::setCheckedNetworkClientRadioButton(bool value)
{
	if(!value)
		m_ui->networkClientRadioButton->setCheckable(false);
}

void GameTypeDialog::showNetworkClientDialog()
{
	NetworkClientDialog dialog(this);

	if(dialog.exec())
	{
		m_ui->networkClientRadioButton->setCheckable(true);
		m_ui->networkClientRadioButton->setChecked(true);
		newConnection = true;
	}
}


//protected


void GameTypeDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
