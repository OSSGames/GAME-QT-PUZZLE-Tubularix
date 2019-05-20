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

#ifndef GAMEOPTIONSDIALOG_H
#define GAMEOPTIONSDIALOG_H

#include "gamecolors.h"
#include "keyselectionbutton.h"
#include "piececolorselection.h"
#include <QtGui/QDialog>
#include <QCloseEvent>

namespace Ui {
    class GameOptionsDialog;
}

class GameOptionsDialog : public QDialog
{
Q_OBJECT
public:
    GameOptionsDialog(QWidget *parent = 0);
    ~GameOptionsDialog();

public slots:
	void setGameColors(GameColors *cols);
	void setColorPreset(int n);

	void setBackgroundColorDegradation(int n) {gameColors->setBackgroundColorDegradation(n);
		pieceColorSelection->setGameColors(gameColors);}
	void setBackgroundColorDegradationMultiplier(double n) {gameColors->setBackgroundColorDegradationMultiplier(n);
		pieceColorSelection->setGameColors(gameColors);}
	void setGridColorDegradation(int n) {gameColors->setGridColorDegradation(n);
		pieceColorSelection->setGameColors(gameColors);}
	void setGridColorDegradationMultiplier(double n) {gameColors->setGridColorDegradationMultiplier(n);
		pieceColorSelection->setGameColors(gameColors);}
	void setPieceColorDegradation(int n) {gameColors->setPieceColorDegradation(n);
		pieceColorSelection->setGameColors(gameColors);}
	void setPieceColorDegradationMultiplier(double n) {gameColors->setPieceColorDegradationMultiplier(n);
		pieceColorSelection->setGameColors(gameColors);}

private:
	Ui::GameOptionsDialog *m_ui;
	PieceColorSelection *pieceColorSelection;
	bool changed;
	bool waitingForNewKey;
	GameColors *gameColors;

	KeySelectionButton keySelectionButton[14];
	int pauseKey;
	int restartKey;
	int left1Key;
	int right1Key;
	int down1Key;
	int drop1Key;
	int rotateCC1Key;
	int rotateC1Key;
	int left2Key;
	int right2Key;
	int down2Key;
	int drop2Key;
	int rotateCC2Key;
	int rotateC2Key;

	void setColorDegradationSpinBoxesValues();

private slots:
	void readSettings();
	void writeSettings();
	void acceptChanges();
	void applyChanges();
	void restoreDefaultValues();
	void settingsChanged();
	void setKeyCode(QString name, int code);
	void setWaitingForNewKey(bool value = true);

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);

signals:
	void changesApplied();
};

#endif // GAMEOPTIONSDIALOG_H
