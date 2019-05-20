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

#ifndef GAMETYPEDIALOG_H
#define GAMETYPEDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
	class GameTypeDialog;
}

class GameTypeDialog : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY(GameTypeDialog)

public:
	explicit GameTypeDialog(QWidget *parent = 0, int gameType = 1, int gameMode = 1, bool customMode = false,
		int rows = 18, int columns = 12, int startLevel = 1, bool samePieces = true,
		bool sendDestroyedLines = true);
	virtual ~GameTypeDialog();

	int selectedGameType();
	int selectedGameMode();
	bool customMode();
	int rows();
	int columns();
	int startLevel();
	bool samePieces();
	bool sendDestroyedLines();
	bool newConnectionRequested() {return newConnection;}

private:
	enum {singlePlayer = 1, twoPlayers, networkServer,	networkClient}; //game type
	enum {tubularix = 1, invertedTubularix, tetrisWithoutWalls, invertedTetrisWithoutWalls,
		tetris, invertedTetris}; //game mode

	Ui::GameTypeDialog *m_ui;

	bool newConnection;

private slots:
	void setRowsAndColumns();
	void setCheckedNetworkServerRadioButton(bool value);
	void showNetworkServerDialog();
	void setCheckedNetworkClientRadioButton(bool value);
	void showNetworkClientDialog();

protected:
	virtual void changeEvent(QEvent *e);
};

#endif // GAMETYPEDIALOG_H
