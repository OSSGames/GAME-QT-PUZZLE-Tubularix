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

#include "gameoptionsdialog.h"
#include "ui_gameoptionsdialog.h"
#include <QMessageBox>
#include <QSettings>


//public


GameOptionsDialog::GameOptionsDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::GameOptionsDialog)
{
	m_ui->setupUi(this);
	changed = false;
	waitingForNewKey = false;

	gameColors = new GameColors();
	pieceColorSelection = new PieceColorSelection(this);
	m_ui->scrollArea->setWidget(pieceColorSelection);

	for(int i = 0; i < 14; i++)
		keySelectionButton[i].setObjectName("keySelectionButton[" + QString::number(i) + "]");

	//Controls
	for(int i = 0; i < 14; i++)
	{
		connect(&keySelectionButton[i], SIGNAL(keyIntroduced(QString, int)), this, SLOT(setKeyCode(QString, int)));
		connect(&keySelectionButton[i], SIGNAL(introducingKey()), this, SLOT(setWaitingForNewKey()));
		connect(&keySelectionButton[i], SIGNAL(keyIntroductionRejected(bool)), this, SLOT(setWaitingForNewKey(bool)));
	}

	//Main buttons
	connect(m_ui->buttonOK, SIGNAL(clicked()), this, SLOT(acceptChanges()));
	connect(m_ui->buttonApply, SIGNAL(clicked()), this, SLOT(applyChanges()));
	connect(m_ui->buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_ui->buttonDefault, SIGNAL(clicked()), this, SLOT(restoreDefaultValues()));

	//Key repetition
	connect(m_ui->delaySlider, SIGNAL(valueChanged(int)), m_ui->delaySpinBox, SLOT(setValue(int)));
	connect(m_ui->delaySpinBox, SIGNAL(valueChanged(int)), m_ui->delaySlider, SLOT(setValue(int)));
	connect(m_ui->rateSlider, SIGNAL(valueChanged(int)), m_ui->rateSpinBox, SLOT(setValue(int)));
	connect(m_ui->rateSpinBox, SIGNAL(valueChanged(int)), m_ui->rateSlider, SLOT(setValue(int)));

	readSettings();

	//Set changed to true if settings are changed
	//Controls
	for(int i = 0; i < 14; i++)
		connect(&keySelectionButton[i], SIGNAL(clicked()), this, SLOT(settingsChanged()));

	//Key repetition
	connect(m_ui->movementKeyRepetitionCheckBox, SIGNAL(toggled(bool)), this, SLOT(settingsChanged()));
	connect(m_ui->rotationKeyRepetitionCheckBox, SIGNAL(toggled(bool)), this, SLOT(settingsChanged()));
	connect(m_ui->delaySlider, SIGNAL(valueChanged(int)), this, SLOT(settingsChanged()));
	connect(m_ui->rateSlider, SIGNAL(valueChanged(int)), this, SLOT(settingsChanged()));

	//Colors
	connect(pieceColorSelection, SIGNAL(gameColorsChanged(GameColors*)), this, SLOT(setGameColors(GameColors*)));
	connect(pieceColorSelection, SIGNAL(gameColorsChanged(GameColors*)), this, SLOT(settingsChanged()));
	connect(m_ui->colorPresetComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setColorPreset(int)));

	//Color degradations
	connect(m_ui->backgroundColorDegradationSpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(setBackgroundColorDegradation(int)));
	connect(m_ui->backgroundColorDegradationMultiplierSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(setBackgroundColorDegradationMultiplier(double)));
	connect(m_ui->gridColorDegradationSpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(setGridColorDegradation(int)));
	connect(m_ui->gridColorDegradationMultiplierSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(setGridColorDegradationMultiplier(double)));
	connect(m_ui->pieceColorDegradationSpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(setPieceColorDegradation(int)));
	connect(m_ui->pieceColorDegradationMultiplierSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(setPieceColorDegradationMultiplier(double)));
}

GameOptionsDialog::~GameOptionsDialog()
{
	delete m_ui;
}


//public slots


void GameOptionsDialog::setGameColors(GameColors *cols)
{
	gameColors->setColors(cols->colors());
	if(m_ui->colorPresetComboBox->currentIndex() != 0)
		m_ui->colorPresetComboBox->setCurrentIndex(0); //set preset to custom when colors are changed
}

void GameOptionsDialog::setColorPreset(int n)
{
	gameColors->setPreset(n);
	pieceColorSelection->setGameColors(gameColors);
}


//private


void GameOptionsDialog::setColorDegradationSpinBoxesValues()
{
	m_ui->backgroundColorDegradationSpinBox->setValue(
		gameColors->getBackgroundColorDegradation());
	m_ui->backgroundColorDegradationMultiplierSpinBox->setValue(
		gameColors->getBackgroundColorDegradationMultiplier());
	m_ui->gridColorDegradationSpinBox->setValue(
		gameColors->getGridColorDegradation());
	m_ui->gridColorDegradationMultiplierSpinBox->setValue(
		gameColors->getGridColorDegradationMultiplier());
	m_ui->pieceColorDegradationSpinBox->setValue(
		gameColors->getPieceColorDegradation());
	m_ui->pieceColorDegradationMultiplierSpinBox->setValue(
		gameColors->getPieceColorDegradationMultiplier());
}


//private slots


void GameOptionsDialog::readSettings()
{
	QSettings settings;
	settings.beginGroup("GameOptions");

	//Controls
	keySelectionButton[0].setText(settings.value("pause", "P").toString());
	keySelectionButton[1].setText(settings.value("restart", "R").toString());
	keySelectionButton[2].setText(settings.value("left1", "Left").toString());
	keySelectionButton[3].setText(settings.value("right1", "Right").toString());
	keySelectionButton[4].setText(settings.value("down1", "Down").toString());
	keySelectionButton[5].setText(settings.value("drop1", "B").toString());
	keySelectionButton[6].setText(settings.value("rotateCC1", "Up").toString());
	keySelectionButton[7].setText(settings.value("rotateC1", "Space").toString());
	keySelectionButton[8].setText(settings.value("left2", "A").toString());
	keySelectionButton[9].setText(settings.value("right2", "D").toString());
	keySelectionButton[10].setText(settings.value("down2", "S").toString());
	keySelectionButton[11].setText(settings.value("drop2", "X").toString());
	keySelectionButton[12].setText(settings.value("rotateCC2", "W").toString());
	keySelectionButton[13].setText(settings.value("rotateC2", "Q").toString());
	for(int i = 0; i < 2; i++)
		m_ui->gridLayoutGeneral->addWidget(&keySelectionButton[i], i + 2, 1);
	for(int i = 2; i < 8; i++)
		m_ui->gridLayoutPlayer1->addWidget(&keySelectionButton[i], i - 2, 1);
	for(int i = 8; i < 14; i++)
		m_ui->gridLayoutPlayer2->addWidget(&keySelectionButton[i], i - 6, 1);

	pauseKey = settings.value("pauseCode", Qt::Key_P).toInt();
	restartKey = settings.value("restartCode", Qt::Key_R).toInt();
	left1Key = settings.value("left1Code", Qt::Key_Left).toInt();
	right1Key = settings.value("rigt1Code", Qt::Key_Right).toInt();
	down1Key = settings.value("down1Code", Qt::Key_Down).toInt();
	drop1Key = settings.value("drop1Code", Qt::Key_B).toInt();
	rotateCC1Key = settings.value("rotateCC1Code", Qt::Key_Up).toInt();
	rotateC1Key = settings.value("rotateC1Code", Qt::Key_Space).toInt();
	left2Key = settings.value("left2Code", Qt::Key_A).toInt();
	right2Key = settings.value("right2Code", Qt::Key_D).toInt();
	down2Key = settings.value("down2Code", Qt::Key_S).toInt();
	drop2Key = settings.value("drop2Code", Qt::Key_X).toInt();
	rotateCC2Key = settings.value("rotateCC2Code", Qt::Key_W).toInt();
	rotateC2Key = settings.value("rotateC2Code", Qt::Key_Q).toInt();

	//Rotation style
	switch(settings.value("rotationStyle", 0).toInt())
	{
	case 0:
		m_ui->radioButtonDefalutRotation->setChecked(true);
		break;
	case 1:
		m_ui->radioButtonClassicalRotation->setChecked(true);
		break;
	case 2:
		m_ui->radioButtonClassicalModifiedRotation->setChecked(true);
		break;
	case 3:
		m_ui->radioButtonOldRotation->setChecked(true);
		break;
	}

	//Key repetition
	m_ui->movementKeyRepetitionCheckBox->setChecked(settings.value("activateMovementKeyRepetition", true).toBool());
	m_ui->rotationKeyRepetitionCheckBox->setChecked(settings.value("activateRotationKeyRepetition", false).toBool());
	m_ui->delaySlider->setValue(settings.value("repetitionDelay", 205).toInt());
	m_ui->rateSlider->setValue(settings.value("repetitionRate", 25).toInt());

	settings.endGroup();

	//Colors
	settings.beginGroup("Colors");
	gameColors->setColor(0, QColor(settings.value("color0", QColor(90, 90, 90).name()).toString())); //empty square
	gameColors->setColor(1, QColor(settings.value("color1", QColor(250, 70, 70).name()).toString())); //piece
	gameColors->setColor(2, QColor(settings.value("color2", QColor(60, 60, 250).name()).toString())); //piece
	gameColors->setColor(3, QColor(settings.value("color3", QColor(230, 240, 40).name()).toString())); //piece
	gameColors->setColor(4, QColor(settings.value("color4", QColor(50, 180, 240).name()).toString())); //piece
	gameColors->setColor(5, QColor(settings.value("color5", QColor(130, 20, 250).name()).toString())); //piece
	gameColors->setColor(6, QColor(settings.value("color6", QColor(250, 180, 60).name()).toString())); //piece
	gameColors->setColor(7, QColor(settings.value("color7", QColor(140, 240, 40).name()).toString())); //piece
	gameColors->setColor(8, QColor(settings.value("color8", QColor(70, 70, 70).name()).toString())); //background
	gameColors->setColor(9, QColor(settings.value("color9", QColor(0, 0, 0).name()).toString())); //grid
	gameColors->setColor(10, QColor(settings.value("color10", QColor(250, 250, 250).name()).toString())); //text
	gameColors->setColor(11, QColor(settings.value("color11", QColor(10, 10, 10).name()).toString())); //text background
	gameColors->setColor(12, QColor(settings.value("color12", QColor(50, 50, 50).name()).toString())); //piece shadow 1
	gameColors->setColor(13, QColor(settings.value("color13", QColor(100, 100, 100).name()).toString())); //piece shadow 2

	gameColors->setBackgroundColorDegradation(settings.value("backgroundColorDegradation", 100).toInt());
	gameColors->setBackgroundColorDegradationMultiplier(
		settings.value("backgroundColorDegradationMultiplier", 0.0).toDouble());
	gameColors->setGridColorDegradation(settings.value("gridColorDegradation", 100).toInt());
	gameColors->setGridColorDegradationMultiplier(
		settings.value("gridColorDegradationMultiplier", 0.0).toDouble());
	gameColors->setPieceColorDegradation(settings.value("pieceColorDegradation", 100).toInt());
	gameColors->setPieceColorDegradationMultiplier(
		settings.value("pieceColorDegradationMultiplier", 0.0).toDouble());
	setColorDegradationSpinBoxesValues();

	setColorPreset(settings.value("preset", 1).toInt());
	m_ui->colorPresetComboBox->setCurrentIndex(gameColors->preset());
	settings.endGroup();

}

void GameOptionsDialog::writeSettings()
{
	QSettings settings;
	settings.beginGroup("GameOptions");

	//Controls
	settings.setValue("pause", keySelectionButton[0].text());
	settings.setValue("restart", keySelectionButton[1].text());
	settings.setValue("left1", keySelectionButton[2].text());
	settings.setValue("right1", keySelectionButton[3].text());
	settings.setValue("down1", keySelectionButton[4].text());
	settings.setValue("drop1", keySelectionButton[5].text());
	settings.setValue("rotateCC1", keySelectionButton[6].text());
	settings.setValue("rotateC1", keySelectionButton[7].text());
	settings.setValue("left2", keySelectionButton[8].text());
	settings.setValue("right2", keySelectionButton[9].text());
	settings.setValue("down2", keySelectionButton[10].text());
	settings.setValue("drop2", keySelectionButton[11].text());
	settings.setValue("rotateCC2", keySelectionButton[12].text());
	settings.setValue("rotateC2", keySelectionButton[13].text());

	settings.setValue("pauseCode", pauseKey);
	settings.setValue("restartCode", restartKey);
	settings.setValue("left1Code", left1Key);
	settings.setValue("rigt1Code", right1Key);
	settings.setValue("down1Code", down1Key);
	settings.setValue("drop1Code", drop1Key);
	settings.setValue("rotateCC1Code", rotateCC1Key);
	settings.setValue("rotateC1Code", rotateC1Key);
	settings.setValue("left2Code", left2Key);
	settings.setValue("right2Code", right2Key);
	settings.setValue("down2Code", down2Key);
	settings.setValue("drop2Code", drop2Key);
	settings.setValue("rotateCC2Code", rotateCC2Key);
	settings.setValue("rotateC2Code", rotateC2Key);

	//Rotation style
	if(m_ui->radioButtonDefalutRotation->isChecked())
		settings.setValue("rotationStyle", 0);
	else if(m_ui->radioButtonClassicalRotation->isChecked())
		settings.setValue("rotationStyle", 1);
	else if(m_ui->radioButtonClassicalModifiedRotation->isChecked())
		settings.setValue("rotationStyle", 2);

	//Key repetition
	settings.setValue("activateMovementKeyRepetition", m_ui->movementKeyRepetitionCheckBox->isChecked());
	settings.setValue("activateRotationKeyRepetition", m_ui->rotationKeyRepetitionCheckBox->isChecked());
	settings.setValue("repetitionDelay", m_ui->delaySlider->value());
	settings.setValue("repetitionRate", m_ui->rateSlider->value());

	settings.endGroup();

	//Colors
	settings.beginGroup("Colors");
	for(int i = 0; i < gameColors->numColors(); i++)
		settings.setValue("color" + QString::number(i), gameColors->customColor(i).name());

	settings.setValue("backgroundColorDegradation", gameColors->getBackgroundColorDegradation());
	settings.setValue("backgroundColorDegradationMultiplier", gameColors->getBackgroundColorDegradationMultiplier());
	settings.setValue("gridColorDegradation", gameColors->getGridColorDegradation());
	settings.setValue("gridColorDegradationMultiplier", gameColors->getGridColorDegradationMultiplier());
	settings.setValue("pieceColorDegradation", gameColors->getPieceColorDegradation());
	settings.setValue("pieceColorDegradationMultiplier", gameColors->getPieceColorDegradationMultiplier());

	settings.setValue("preset", gameColors->preset());
	settings.endGroup();
}

void GameOptionsDialog::acceptChanges()
{
	writeSettings();
	changed = false;
	accept();
}

void GameOptionsDialog::applyChanges()
{
	writeSettings();
	changed = false;
	emit changesApplied();
}

void GameOptionsDialog::restoreDefaultValues()
{
	//Restore only the values of the current tab
	//The user should save changes before closing the options dialog
	QString tabText = m_ui->tabWidget->tabText(m_ui->tabWidget->currentIndex());
	if(tabText == tr("Controls"))
	{
		keySelectionButton[0].setText("P");
		keySelectionButton[1].setText("R");
		keySelectionButton[2].setText("Left");
		keySelectionButton[3].setText("Right");
		keySelectionButton[4].setText("Down");
		keySelectionButton[5].setText("B");
		keySelectionButton[6].setText("Up");
		keySelectionButton[7].setText("Space");
		keySelectionButton[8].setText("A");
		keySelectionButton[9].setText("D");
		keySelectionButton[10].setText("S");
		keySelectionButton[11].setText("X");
		keySelectionButton[12].setText("W");
		keySelectionButton[13].setText("Q");

		pauseKey = Qt::Key_P;
		restartKey = Qt::Key_R;
		left1Key = Qt::Key_Left;
		right1Key = Qt::Key_Right;
		down1Key = Qt::Key_Down;
		drop1Key = Qt::Key_B;
		rotateCC1Key = Qt::Key_Up;
		rotateC1Key = Qt::Key_Space;
		left2Key = Qt::Key_A;
		right2Key = Qt::Key_D;
		down2Key = Qt::Key_S;
		drop2Key = Qt::Key_X;
		rotateCC2Key = Qt::Key_W;
		rotateC2Key = Qt::Key_Q;
	}
	else if(tabText == tr("Key repetition"))
	{
		m_ui->movementKeyRepetitionCheckBox->setChecked(true);
		m_ui->rotationKeyRepetitionCheckBox->setChecked(true);
		m_ui->delaySlider->setValue(205);
		m_ui->rateSlider->setValue(25);
	}
	else if(tabText == tr("Colors"))
	{
		gameColors->setColor(0, QColor(90, 90, 90)); //empty square
		gameColors->setColor(1, QColor(250, 70, 70)); //piece
		gameColors->setColor(2, QColor(60, 60, 250)); //piece
		gameColors->setColor(3, QColor(230, 240, 40)); //piece
		gameColors->setColor(4, QColor(50, 180, 240)); //piece
		gameColors->setColor(5, QColor(130, 20, 250)); //piece
		gameColors->setColor(6, QColor(250, 180, 60)); //piece
		gameColors->setColor(7, QColor(140, 240, 40)); //piece
		gameColors->setColor(8, QColor(70, 70, 70)); //background
		gameColors->setColor(9, QColor(0, 0, 0)); //grid
		gameColors->setColor(10, QColor(250, 250, 250)); //text
		gameColors->setColor(11, QColor(10, 10, 10)); //text background
		gameColors->setColor(12, QColor(50, 50, 50)); //piece shadow 1
		gameColors->setColor(13, QColor(100, 100, 100)); //piece shadow 2

		gameColors->setBackgroundColorDegradation(100);
		gameColors->setBackgroundColorDegradationMultiplier(0.0);
		gameColors->setGridColorDegradation(100);
		gameColors->setGridColorDegradationMultiplier(0.0);
		gameColors->setPieceColorDegradation(100);
		gameColors->setPieceColorDegradationMultiplier(0.0);
		setColorDegradationSpinBoxesValues();

		if(m_ui->colorPresetComboBox->currentIndex() != 0)
			m_ui->colorPresetComboBox->setCurrentIndex(0);
		else
			setColorPreset(0);
	}
}

void GameOptionsDialog::settingsChanged()
{
	changed = true;
}

void GameOptionsDialog::setKeyCode(QString name, int code)
{
	//wouldn't it be better to put else if's?
	if(name == "keySelectionButton[0]")
		pauseKey = code;
	if(name == "keySelectionButton[1]")
		restartKey = code;
	if(name == "keySelectionButton[2]")
		left1Key = code;
	if(name == "keySelectionButton[3]")
		right1Key = code;
	if(name == "keySelectionButton[4]")
		down1Key = code;
	if(name == "keySelectionButton[5]")
		drop1Key = code;
	if(name == "keySelectionButton[6]")
		rotateCC1Key = code;
	if(name == "keySelectionButton[7]")
		rotateC1Key = code;
	if(name == "keySelectionButton[8]")
		left2Key = code;
	if(name == "keySelectionButton[9]")
		right2Key = code;
	if(name == "keySelectionButton[10]")
		down2Key = code;
	if(name == "keySelectionButton[11]")
		drop2Key = code;
	if(name == "keySelectionButton[12]")
		rotateCC2Key = code;
	if(name == "keySelectionButton[13]")
		rotateC2Key = code;

	setWaitingForNewKey(false);
}

void GameOptionsDialog::setWaitingForNewKey(bool value)
{
	waitingForNewKey = value;
}


//protected


void GameOptionsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void GameOptionsDialog::closeEvent(QCloseEvent *event)
{
	//ask if the user wants to save unsaved changes (yes/no/cancel)
	if(changed)
	{
		QMessageBox m;
		m.setText(tr("There are unsaved changes"));
		m.setInformativeText(tr("Do you want to save them or discard them?"));
		m.setIcon(QMessageBox::Warning);
		m.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		m.setDefaultButton(QMessageBox::Save);

		switch(m.exec())
		{
		case QMessageBox::Save:
			acceptChanges();
			break;
		case QMessageBox::Discard:
			event->accept();
			break;
		case QMessageBox::Cancel:
			event->ignore();
			return;
		}
	}

	QDialog::closeEvent(event);
}
