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

#include "piececolorselection.h"
#include <QColorDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QSettings>


//public


PieceColorSelection::PieceColorSelection(QWidget *parent)
		: QWidget(parent)
{
	setMouseTracking(true);

	piece = new Piece;
	gameColors = new GameColors();

	//background
	setPalette(QPalette(gameColors->color(8)));
	setAutoFillBackground(true);
}


//public slots


void PieceColorSelection::setGameColors(GameColors *cols)
{
	gameColors->setPreset(cols->preset());
	gameColors->setColors(cols->colors());

	gameColors->setBackgroundColorDegradation(cols->getBackgroundColorDegradation());
	gameColors->setBackgroundColorDegradationMultiplier(cols->getBackgroundColorDegradationMultiplier());
	gameColors->setGridColorDegradation(cols->getGridColorDegradation());
	gameColors->setGridColorDegradationMultiplier(cols->getGridColorDegradationMultiplier());
	gameColors->setPieceColorDegradation(cols->getPieceColorDegradation());
	gameColors->setPieceColorDegradationMultiplier(cols->getPieceColorDegradationMultiplier());

	setPalette(QPalette(gameColors->color(8)));
	update();
}


//private


void PieceColorSelection::changeColor(int n)
{
	QColor color = QColorDialog::getColor(gameColors->color(n), this, "Select color", QColorDialog::DontUseNativeDialog);
	if(color.isValid())
	{
		gameColors->setPreset(0); //set preset to custom when colors are changed
		gameColors->setColor(n, color);
		if(n == 8)
			setPalette(QPalette(gameColors->color(8)));

		update();
		emit gameColorsChanged(gameColors);
	}
}


//protected


void PieceColorSelection::paintEvent(QPaintEvent */*event*/)
{
	QPainter painter(this);

	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 9; j++)
		{
			painter.setPen(gameColors->color(9, i));
			painter.setBrush(gameColors->color(piecesMatrix[i][j], i));
			painter.drawRect(24 + j * 30, 200 + i * 30, 30, 30);
		}
	}
	painter.setPen(gameColors->color(9));

	QFont textFont;
	textFont.setPointSize(11);
	painter.setFont(textFont);
	painter.setPen(gameColors->color(10));
	painter.drawText(24, 30, tr("Click on a piece to change its color"));
	painter.drawText(24, 50, tr("You can also change background "));
	painter.drawText(24, 70, tr("and scenario colors"));

	painter.drawText(24, 100, tr("Grid:"));
	painter.drawText(24, 120, tr("Text:"));
	painter.drawText(24, 140, tr("Text background:"));
	painter.drawText(24, 160, tr("Piece shadow 1:"));
	painter.drawText(24, 180, tr("Piece shadow 2:"));
	painter.setPen(gameColors->color(9));
	painter.setBrush(gameColors->color(9));
	painter.drawRect(24 + 255, 88, 15, 15);
	painter.setBrush(gameColors->color(10));
	painter.drawRect(24 + 255, 108, 15, 15);
	painter.setBrush(gameColors->color(11));
	painter.drawRect(24 + 255, 128, 15, 15);
	painter.setBrush(gameColors->color(12));
	painter.drawRect(24 + 255, 148, 15, 15);
	painter.setBrush(gameColors->color(13));
	painter.drawRect(24 + 255, 168, 15, 15);
}

void PieceColorSelection::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		//scenario or pieces
		if(event->x() >= 24 && event->x() < (24 + 9 * 30) && event->y() > 200 && event->y() <= (200 + 10 * 30))
		{
			for(int i = 0; i < 10; i++)
			{
				for(int j = 0; j < 9; j++)
				{
					if(event->x() >= (24 + j * 30) && event->x() < (24 + (j + 1) * 30)
						&& event->y() > (200 + i * 30) && event->y() <= (200 + (i + 1) * 30))
					{
						changeColor(piecesMatrix[i][j]);
						return;
					}
				}
			}
		}
		else if(event->x() >= (24 + 255) && event->x() < (24 + 255 + 15) &&
			event->y() > 88 && event->y() <= (88 + 15)) //grid
			changeColor(9);
		else if(event->x() >= (24 + 255) && event->x() < (24 + 255 + 15) &&
			event->y() > 108 && event->y() <= (108 + 15)) //text
			changeColor(10);
		else if(event->x() >= (24 + 255) && event->x() < (24 + 255 + 15) &&
			event->y() > 128 && event->y() <= (128 + 15)) //text background
			changeColor(11);
		else if(event->x() >= (24 + 255) && event->x() < (24 + 255 + 15) &&
			event->y() > 148 && event->y() <= (148 + 15)) //piece shadow 1
			changeColor(12);
		else if(event->x() >= (24 + 255) && event->x() < (24 + 255 + 15) &&
			event->y() > 168 && event->y() <= (168 + 15)) //piece shadow 2
			changeColor(13);
		else //background
			changeColor(8);
	}
}
