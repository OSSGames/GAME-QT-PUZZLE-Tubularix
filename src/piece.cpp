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

#include "piece.h"
//#include <QTime>

Piece::Piece()
{
	leftLimit = 0;
	rightLimit = 11;
	setPosX(0);
	setPosY(0);
	rotPos = 0;

	setRotationStyle(0);
	resetPreviousProperties();
	//qsrand(QTime::currentTime().msec());
}

int Piece::selectPiece(int num)
{
	if(num == -1)
		selPiece = qrand() % 7;
	else
		selPiece = num;

	rotPos = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			pieceMatrix[i][j] = Pieces[rotationStyle][selPiece][rotPos][i][j];
		}
	}
	//resetPreviousProperties();

	return selPiece;
}

void Piece::setLimits(int left, int right)
{
	leftLimit = left;
	rightLimit = right;
}

void Piece::setRotationStyle(int style)
{
	rotationStyle = style;
}

//piece's current properties

void Piece::setPosX(int n, bool reset)
{
	if(reset)
		resetPreviousProperties();

	if(n < leftLimit)
		x = rightLimit;// - n - leftLimit;
	else if(n > rightLimit)
		x = leftLimit;// + n - rightLimit;
	else
		x = n;
}

void Piece::setPosY(int n, bool reset)
{
	if(reset)
		resetPreviousProperties();

	y = n;
}

void Piece::rotateClockwise()
{
	resetPreviousProperties();

	if(rotPos < 3)
		rotPos++;
	else
		rotPos = 0;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			pieceMatrix[i][j] = Pieces[rotationStyle][selPiece][rotPos][i][j];
		}
	}
}

void Piece::rotateCounterClockwise()
{
	resetPreviousProperties();

	if(rotPos > 0)
		rotPos--;
	else
		rotPos = 3;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			pieceMatrix[i][j] = Pieces[rotationStyle][selPiece][rotPos][i][j];
		}
	}
}

void Piece::setRotationPos(int newRotationPos)
{
	resetPreviousProperties();

	rotPos = newRotationPos;
	if(rotPos < 0)
		rotPos = 0;
	else if(rotPos > 3)
		rotPos = 3;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			pieceMatrix[i][j] = Pieces[rotationStyle][selPiece][rotPos][i][j];
		}
	}
}

int Piece::width()
{
	int w = 0;
	for(int j = 0; j < 4; j++)
	{
		for(int i = 0; i < 4; i++)
		{
			if(pieceMatrix[i][j] != 0)
			{
				w++;
				break;
			}
		}
	}
	return w;
}

int Piece::height()
{
	int h = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(pieceMatrix[i][j] != 0)
			{
				h++;
				break;
			}
		}
	}
	return h;
}

int Piece::firstLeft()
{
	for(int j = 0; j < 4; j++)
	{
		for(int i = 0; i < 4; i++)
		{
			if(pieceMatrix[i][j] != 0)
			{
				return j;
			}
		}
	}
	return 0;
}

int Piece::firstRight()
{
	for(int j = 3; j >= 0; j--)
	{
		for(int i = 0; i < 4; i++)
		{
			if(pieceMatrix[i][j] != 0)
			{
				return j;
			}
		}
	}
	return 0;
}

int Piece::firstUp()
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(pieceMatrix[i][j] != 0)
			{
				return i;
			}
		}
	}
	return 0;
}

int Piece::firstDown()
{
	for(int i = 3; i >= 0; i--)
	{
		for(int j = 0; j < 4; j++)
		{
			if(pieceMatrix[i][j] != 0)
			{
				return i;
			}
		}
	}
	return 0;
}

int Piece::firstLeftAtRow(int row)
{
	for(int j = 0; j < 4; j++)
	{
		if(pieceMatrix[row][j] != 0)
		{
			return j;
		}
	}
	return 0;
}

int Piece::firstRightAtRow(int row)
{
	for(int j = 3; j >= 0; j--)
	{
		if(pieceMatrix[row][j] != 0)
		{
			return j;
		}
	}
	return 0;
}

int Piece::firstUpAtColumn(int column)
{
	for(int i = 0; i < 4; i++)
	{
		if(pieceMatrix[i][column] != 0)
		{
			return i;
		}
	}
	return 0;
}

int Piece::firstDownAtColumn(int column)
{
	for(int i = 3; i >= 0; i--)
	{
		if(pieceMatrix[i][column] != 0)
		{
			return i;
		}
	}
	return 0;
}

//piece's previous properties

void Piece::setPreviousPosX(int n)
{
	if(n < leftLimit)
		previousX = rightLimit;
	else if(n > rightLimit)
		previousX = leftLimit;
	else
		previousX = n;
}

void Piece::setPreviousPosY(int n)
{
	previousY = n;
}

int Piece::previousWidth()
{
	int w = 0;
	for(int j = 0; j < 4; j++)
	{
		for(int i = 0; i < 4; i++)
		{
			if(previousPieceMatrix[i][j] != 0)
			{
				w++;
				break;
			}
		}
	}
	return w;
}

int Piece::previousHeight()
{
	int h = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(previousPieceMatrix[i][j] != 0)
			{
				h++;
				break;
			}
		}
	}
	return h;
}

void Piece::resetPreviousProperties()
{
	previousX = x;
	previousY = y;
	previousRotPos = rotPos;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			previousPieceMatrix[i][j] = pieceMatrix[i][j];//Pieces[selPiece][previousRotPos][i][j];
		}
	}
}

int Piece::previousFirstLeft()
{
	for(int j = 0; j < 4; j++)
	{
		for(int i = 0; i < 4; i++)
		{
			if(previousPieceMatrix[i][j] != 0)
			{
				return j;
			}
		}
	}
	return 0;
}

int Piece::previousFirstRight()
{
	for(int j = 3; j >= 0; j--)
	{
		for(int i = 0; i < 4; i++)
		{
			if(previousPieceMatrix[i][j] != 0)
			{
				return j;
			}
		}
	}
	return 0;
}

int Piece::previousFirstUp()
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(previousPieceMatrix[i][j] != 0)
			{
				return i;
			}
		}
	}
	return 0;
}

int Piece::previousFirstDown()
{
	for(int i = 3; i >= 0; i--)
	{
		for(int j = 0; j < 4; j++)
		{
			if(previousPieceMatrix[i][j] != 0)
			{
				return i;
			}
		}
	}
	return 0;
}
