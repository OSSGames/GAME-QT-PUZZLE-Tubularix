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

#ifndef PIECECOLORSELECTION_H
#define PIECECOLORSELECTION_H

#include "gamecolors.h"
#include "piece.h"
#include <QWidget>

const int piecesMatrix[10][9] =
{{0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {1,1,0,2,0,0,0,0,0},
 {1,1,0,2,0,0,0,0,0},
 {5,4,0,2,0,0,0,3,6},
 {5,4,4,2,7,0,3,3,6},
 {5,5,4,7,7,7,3,6,6}};


class PieceColorSelection : public QWidget
{
Q_OBJECT
public:
	PieceColorSelection(QWidget *parent = 0);

public slots:
	void setGameColors(GameColors *cols);

private:
	Piece *piece;
	GameColors *gameColors;

	void changeColor(int n = 0);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);

signals:
	void gameColorsChanged(GameColors *cols);
};

#endif // PIECECOLORSELECTION_H
