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

#ifndef GAMECOLORS_H
#define GAMECOLORS_H

#include <QColor>
#include <QObject>

const int nColors = 14;

class GameColors : public QObject
{
Q_OBJECT
public:
	GameColors();

	QColor color(int n);
	QColor color(int n, int h);
	QColor customColor(int n);
	void setColor(int n, QColor col);

	QList <QColor> colors();
	void setColors(QList <QColor> col);

	int preset() {return pres;}
	void setPreset(int n) {pres = n;}

	int numColors() {return nColors;}

public slots:
	int getBackgroundColorDegradation() {return backgroundColorDegradation;}
	void setBackgroundColorDegradation(int n) {backgroundColorDegradation = n;}
	double getBackgroundColorDegradationMultiplier() {return backgroundColorDegradationMultiplier;}
	void setBackgroundColorDegradationMultiplier(double n) {backgroundColorDegradationMultiplier = n;}
	int getGridColorDegradation() {return gridColorDegradation;}
	void setGridColorDegradation(int n) {gridColorDegradation = n;}
	double getGridColorDegradationMultiplier() {return gridColorDegradationMultiplier;}
	void setGridColorDegradationMultiplier(double n) {gridColorDegradationMultiplier = n;}
	int getPieceColorDegradation() {return pieceColorDegradation;}
	void setPieceColorDegradation(int n) {pieceColorDegradation = n;}
	double getPieceColorDegradationMultiplier() {return pieceColorDegradationMultiplier;}
	void setPieceColorDegradationMultiplier(double n) {pieceColorDegradationMultiplier = n;}

private:
	QList <QList <QColor> > cols;
	int pres; //preset

	int backgroundColorDegradation;
	double backgroundColorDegradationMultiplier;
	int gridColorDegradation;
	double gridColorDegradationMultiplier;
	int pieceColorDegradation;
	double pieceColorDegradationMultiplier;

	void initializeColors();
};

#endif // GAMECOLORS_H
