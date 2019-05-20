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

#include "gamecolors.h"


//public


GameColors::GameColors()
{
	initializeColors();
	pres = 0;

	backgroundColorDegradation = 100;
	backgroundColorDegradationMultiplier = 0.0;
	gridColorDegradation = 100;
	gridColorDegradationMultiplier = 0.0;
	pieceColorDegradation = 100;
	pieceColorDegradationMultiplier = 0.0;
}

//get a color from the current preset
QColor GameColors::color(int n)
{
	QColor color;
	switch(n)
	{
	case 0: //empty square
	case 50:
	case 100:
		color = cols[pres][0];
		break;
	case 1: //piece
	case 51:
	case 101:
		color = cols[pres][1];
		break;
	case 2: //piece
	case 52:
	case 102:
		color = cols[pres][2];
		break;
	case 3: //piece
	case 53:
	case 103:
		color = cols[pres][3];
		break;
	case 4: //piece
	case 54:
	case 104:
		color = cols[pres][4];
		break;
	case 5: //piece
	case 55:
	case 105:
		color = cols[pres][5];
		break;
	case 6: //piece
	case 56:
	case 106:
		color = cols[pres][6];
		break;
	case 7: //piece
	case 57:
	case 107:
		color = cols[pres][7];
		break;
	case 8: //background
	case 58:
	case 108:
		color = cols[pres][8];
		break;
	case 9: //grid
	case 59:
	case 109:
		color = cols[pres][9];
		break;
	case 10: //text
	case 60:
	case 110:
		color = cols[pres][10];
		break;
	case 11: //text background
	case 61:
	case 111:
		color = cols[pres][11];
		color.setAlpha(210);
		break;
	case 12: //piece shadow 1
	case 62:
	case 112:
		color = cols[pres][12];
		break;
	case 13: //piece shadow 2
	case 63:
	case 113:
		color = cols[pres][13];
		break;
	default:
		color = cols[pres][8];
		break;
	}

	//color for line destruction (darker)
	if(n >= 50 && n < 100)
		//color.setHsv(color.toHsv().hue(), color.toHsv().saturation(), 140, 255);
		color = color.darker(170);

	//color for ghost piece (translucent)
	if(n >= 100)
		color.setAlpha(65);
		//color = color.darker(130);

	return color;
}

//get a color from the current preset making it darker or lighter as h decreases
QColor GameColors::color(int n, int h)
{
	double factor = 100.0;

	if(n == 0) //background
		factor = backgroundColorDegradation + h * backgroundColorDegradationMultiplier;
	else if(n == 9) //grid
		factor = gridColorDegradation + h * gridColorDegradationMultiplier;
	else //pieces
		factor = pieceColorDegradation + h * pieceColorDegradationMultiplier;

	if(factor == 100.0)
		return color(n);
	else if(factor > 100.0)
		return color(n).lighter(factor);
	else if(factor < 100.0 && factor >= 1.0)
		return color(n).darker(200 - factor);
	else
		return color(n).lighter(1);
}

//get a color from the custom preset
QColor GameColors::customColor(int n)
{
	int presAux = pres;
	pres = 0;
	QColor col = color(n);
	pres = presAux;
	return col;
}

//set a color for the custom preset
void GameColors::setColor(int n, QColor col)
{
	cols[0][n] = col;
}

//get the colors from the current preset
QList <QColor> GameColors::colors()
{
	return cols[pres];
}

//set the colors for the custom preset
void GameColors::setColors(QList <QColor> col)
{
	cols[0] = col;
}


//private


void GameColors::initializeColors()
{
	QList <QColor> c;

	//custom preset
	c.clear();
	for(int i = 0; i < nColors; i++)
		c.append(QColor(0, 0, 0));
	cols.append(c);

	//default preset
	c.clear();
	c.append(QColor(90, 90, 90)); //0-empty square
	c.append(QColor(250, 70, 70)); //1-piece
	c.append(QColor(60, 60, 250)); //2-piece
	c.append(QColor(230, 240, 40)); //3-piece
	c.append(QColor(50, 180, 240)); //4-piece
	c.append(QColor(130, 20, 250)); //5-piece
	c.append(QColor(250, 180, 60)); //6-piece
	c.append(QColor(140, 240, 40)); //7-piece
	c.append(QColor(70, 70, 70)); //8-background
	c.append(QColor(0, 0, 0)); //9-grid
	c.append(QColor(250, 250, 250)); //10-text
	c.append(QColor(10, 10, 10)); //11-text background
	c.append(QColor(50, 50, 50)); //12-piece shadow 1
	c.append(QColor(100, 100, 100)); //13-piece shadow 2
	cols.append(c);

	//black preset
	c.clear();
	c.append(QColor(0, 0, 0)); //empty square
	c.append(QColor(250, 70, 70)); //piece
	c.append(QColor(60, 60, 250)); //piece
	c.append(QColor(230, 240, 40)); //piece
	c.append(QColor(50, 180, 240)); //piece
	c.append(QColor(130, 20, 250)); //piece
	c.append(QColor(250, 180, 60)); //piece
	c.append(QColor(140, 240, 40)); //piece
	c.append(QColor(0, 0, 0)); //background
	c.append(QColor(0, 0, 0)); //grid
	c.append(QColor(250, 250, 250)); //text
	c.append(QColor(10, 10, 10)); //text background
	c.append(QColor(50, 50, 50)); //piece shadow 1
	c.append(QColor(100, 100, 100)); //piece shadow 2
	cols.append(c);

	//dark preset
	c.clear();
	c.append(QColor(20, 20, 20)); //empty square
	c.append(QColor(250, 70, 70)); //piece
	c.append(QColor(60, 60, 250)); //piece
	c.append(QColor(230, 240, 40)); //piece
	c.append(QColor(50, 180, 240)); //piece
	c.append(QColor(130, 20, 250)); //piece
	c.append(QColor(250, 180, 60)); //piece
	c.append(QColor(140, 240, 40)); //piece
	c.append(QColor(0, 0, 0)); //background
	c.append(QColor(0, 0, 0)); //grid
	c.append(QColor(250, 250, 250)); //text
	c.append(QColor(10, 10, 10)); //text background
	c.append(QColor(50, 50, 50)); //piece shadow 1
	c.append(QColor(100, 100, 100)); //piece shadow 2
	cols.append(c);

	//clear preset
	c.clear();
	c.append(QColor(178, 178, 178)); //empty square
	c.append(QColor(250, 70, 70)); //piece
	c.append(QColor(60, 60, 250)); //piece
	c.append(QColor(230, 240, 40)); //piece
	c.append(QColor(50, 180, 240)); //piece
	c.append(QColor(130, 20, 250)); //piece
	c.append(QColor(250, 180, 60)); //piece
	c.append(QColor(140, 240, 40)); //piece
	c.append(QColor(220, 220, 220)); //background
	c.append(QColor(48, 48, 48)); //grid
	c.append(QColor(130, 130, 130)); //text
	c.append(QColor(220, 220, 220)); //text background
	c.append(QColor(100, 100, 100)); //piece shadow 1
	c.append(QColor(120, 120, 120)); //piece shadow 2
	cols.append(c);
}
