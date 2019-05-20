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

#include "gameboard.h"
#include <QAction>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <math.h>


//public

GameBoard::GameBoard(QWidget *parent)
	: QWidget(parent)
{
	setMouseTracking(true);
	//frameCount = 0;

	cursorTimer = new QTimer(this);
	connect(cursorTimer, SIGNAL(timeout()), this, SLOT(hideCursor()));

	antialias = false;
	showMap = false;
	piecesAppearGradually = false;
	showGhostPiece = false;
	rotationStyle = 0;
	mouseControlType = 0;
	pieceGrabbed = false;
	piecePosClicked = 0;
	gameColors = new GameColors();

	rows = 18;
	columns = 12;

	piecePaintingNeeded = true;
	scenarioPaintingNeeded = true;
	scenarioChangesPaintingNeeded = true;

	networkedGameboard = false;

	//background
	//setPalette(QColor(0, 0, 0));
	//setAutoFillBackground(true);
	setAttribute(Qt::WA_OpaquePaintEvent, true);
	//createScenarioBackground();

	currentPiece = new Piece();
	nextPiece = new Piece();

	pieceTimer = new QTimer(this);
	connect(pieceTimer, SIGNAL(timeout()), this, SLOT(gravity()));
	animTimer = new QTimer(this);
	connect(animTimer, SIGNAL(timeout()), this, SLOT(doAnimations()));
}

void GameBoard::setNetworkedGameboard(bool value)
{
	networkedGameboard = value;
	if(value)
	{
		//pieceTimer->stop();
		//disconnect(pieceTimer, SIGNAL(timeout()));
		pieceTimer->disconnect(this, 0);
	}
	else
	{
		//disconnect(pieceTimer, SIGNAL(timeout()));
		pieceTimer->disconnect(this, 0);
		connect(pieceTimer, SIGNAL(timeout()), this, SLOT(gravity()));
	}

}

void GameBoard::generatePieces()
{
	switch(gameMode)
	{
	case tubularix:
	case invertedTubularix:
	case tetrisWithoutWalls:
	case invertedTetrisWithoutWalls:
		currentPiece->setPosX(currentPiece->posX() + currentPiece->firstLeft() - nextPiece->firstLeft(), false);
		break;
	case tetris:
	case invertedTetris:
		currentPiece->setPosX(columns / 2 - 1 - nextPiece->firstLeft(), false);
		break;
	}
	//currentPiece->resetPreviousProperties();

	currentPiece->selectPiece(nextPiece->selectedPiece());
	
	//Make piece appear gradually or not
	if(piecesAppearGradually)
		currentPiece->setPosY(0 - currentPiece->firstDown(), false);
	else
		currentPiece->setPosY(0 - currentPiece->firstUp(), false);
	
	if(!nextPieces.isEmpty())
	{
		nextPiece->selectPiece(nextPieces.first());
		nextPieces.removeFirst();
	}
	else
		nextPiece->selectPiece();
	emit nextPieceNeeded();

	if(checkCollisions(currentPiece->posX(), currentPiece->posY()))
	{
		currentPiece->setPosY(-5);
		currentPiece->resetPreviousProperties();
		stopGame();
		return;
	}

	//needed for unpainting the shadow
	piecePaintingNeeded = false;
	//scenarioPaintingNeeded = false;
	scenarioChangesPaintingNeeded = false;
	repaint();

	currentPiece->resetPreviousProperties();

	piecePaintingNeeded = true;
	//scenarioPaintingNeeded = true;
	scenarioChangesPaintingNeeded = true;
	repaint();
}

void GameBoard::updateScenario(bool deletePiece)
{
	int offset = 0;
	if(currentPiece->posY() < 0)
		offset = 0 - currentPiece->posY();

	for(int i = offset; i <= currentPiece->firstDown(); i++)
	{
		for(int j = currentPiece->firstLeft(); j <= currentPiece->firstRight(); j++)
		{
			if(currentPiece->pieceMatrix[i][j] == 0)
				continue;

			if(deletePiece == true) //delete piece
			{
				if(currentPiece->posX() + j > columns - 1)
				{
					scenario[(currentPiece->posY() + i) * columns + currentPiece->posX() + j - columns] = 0;
					scenarioChanges[(currentPiece->posY() + i) * columns + currentPiece->posX() + j - columns] = 1;
				}
				else
				{
					scenario[(currentPiece->posY() + i) * columns + currentPiece->posX() + j] = 0;
					scenarioChanges[(currentPiece->posY() + i) * columns + currentPiece->posX() + j] = 1;
				}
			}
			else //insert piece
			{
				if(currentPiece->posX() + j > columns - 1)
				{
					scenario[(currentPiece->posY() + i) * columns + currentPiece->posX() + j - columns]
						= currentPiece->pieceMatrix[i][j];
					scenarioChanges[(currentPiece->posY() + i) * columns + currentPiece->posX() + j - columns] = 1;
				}
				else
				{
					scenario[(currentPiece->posY() + i) * columns + currentPiece->posX() + j]
						= currentPiece->pieceMatrix[i][j];
					scenarioChanges[(currentPiece->posY() + i) * columns + currentPiece->posX() + j] = 1;
				}
			}
		}
	}

	//scenarioPaintingNeeded = true;
	scenarioChangesPaintingNeeded = true;
	repaint();
}

bool GameBoard::moveLeft()
{
	if(!pieceTimer->isActive())
		return false;

	if(!checkCollisions(currentPiece->posX() - 1, currentPiece->posY()))
	{
		currentPiece->setPosX(currentPiece->posX() - 1);
		piecePaintingNeeded = true;
		repaint();
	}
	return true;
}

bool GameBoard::moveRight()
{
	if(!pieceTimer->isActive())
		return false;

	if(!checkCollisions(currentPiece->posX() + 1, currentPiece->posY()))
	{
		currentPiece->setPosX(currentPiece->posX() + 1);
		piecePaintingNeeded = true;
		repaint();
	}
	return true;
}

bool GameBoard::moveToCol(int col)
{
	if(!pieceTimer->isActive())
		return false;

	if(!checkCollisions(col, currentPiece->posY()))
	{
		currentPiece->setPosX(col);
		piecePaintingNeeded = true;
		repaint();
	}
	return true;
}

bool GameBoard::moveDown()
{
	if(!pieceTimer->isActive())
		return false;

	if(!checkCollisions(currentPiece->posX(), currentPiece->posY() + 1))
	{
		currentPiece->setPosY(currentPiece->posY() + 1);
		piecePaintingNeeded = true;
		repaint();
	}
	return true;
}

void GameBoard::rotatePieceClockwise()
{
	if(!pieceTimer->isActive())
		return;

	currentPiece->rotateClockwise();

	//cancel rotation if a collision happened
	if(checkCollisions(currentPiece->posX(), currentPiece->posY()))
	{
		currentPiece->rotateCounterClockwise();
		currentPiece->resetPreviousProperties();
	}
	else
	{
		piecePaintingNeeded = true;
		repaint();
	}
}

void GameBoard::rotatePieceCounterClockwise()
{
	if(!pieceTimer->isActive())
		return;

	currentPiece->rotateCounterClockwise();

	//cancel rotation if a collision happened
	if(checkCollisions(currentPiece->posX(), currentPiece->posY()))
	{
		currentPiece->rotateClockwise();
		currentPiece->resetPreviousProperties();
	}
	else
	{
		piecePaintingNeeded = true;
		repaint();
	}
}

void GameBoard::speedUp()
{
	if(pieceTimer->interval() > FAST_SPEED && fast == false && pieceTimer->isActive())
	{
		pieceTimer->setInterval(FAST_SPEED);
		fastFallHeight = calculateFallPosition(currentPiece->posX(), currentPiece->posY())
			- currentPiece->posY();
		fast = true;
	}
}

void GameBoard::restoreSpeed()
{
	if(fast == true && pieceTimer->interval() == FAST_SPEED)
	{
		pieceTimer->setInterval(timeInterval);
		fastFallHeight = 0;
	}
	fast = false;
}

void GameBoard::dropPiece()
{
	if(!pieceTimer->isActive())
		return;

	fastFallHeight = calculateFallPosition(currentPiece->posX(), currentPiece->posY())
		- currentPiece->posY();
	currentPiece->setPosY(calculateFallPosition(currentPiece->posX(), currentPiece->posY()));
	piecePaintingNeeded = true;
	repaint();
	gravity();
	fastFallHeight = 0;
}

/* Check if a colision exists between current piece and scenario */
bool GameBoard::checkCollisions(int x, int y)
{
	if(gameMode == tetris || gameMode == invertedTetris) //tetris normal or reversed with walls
	{
		if(x + currentPiece->firstLeft() < 0 || x + currentPiece->firstRight() > columns - 1)
			return true;
	}

	//if(currentPiece->posY() >= rows - currentPiece->firstDown())
	if(y >= rows - currentPiece->firstDown())
	   return true;

	int offset = 0;
	//if(currentPiece->posY() < 0)
		//offset = 0 - currentPiece->posY();
	if(y < 0)
		offset = 0 - y;

	for(int i = offset; i <= currentPiece->firstDown(); i++)
	{
		for(int j = currentPiece->firstLeft(); j <= currentPiece->firstRight(); j++)
		{
			if((x + j) > (columns - 1))
			{
				if(currentPiece->pieceMatrix[i][j] != 0 && scenario[(y + i) * columns + x + j - columns] != 0)
					return true;
			}
			else if((x + j) < 0)
			{
				if(currentPiece->pieceMatrix[i][j] != 0 && scenario[(y + i) * columns + columns - 1] != 0)
					return true;
			}
			else
			{
				if(currentPiece->pieceMatrix[i][j] != 0 && scenario[(y + i) * columns + x + j] != 0)
					return true;
			}
		}
	}
	
	return false;
}

/* Check if a colision exists between current piece's previous properties and scenario */
bool GameBoard::checkPreviousCollisions(int x, int y)
{
	if(gameMode == tetris || gameMode == invertedTetris) //tetris normal or reversed with walls
	{
		if(x + currentPiece->previousFirstLeft() < 0 || x + currentPiece->previousFirstRight() > columns - 1)
			return true;
	}

	//if(currentPiece->posY() >= rows - currentPiece->firstDown())
	if(y >= rows - currentPiece->previousFirstDown())
	   return true;

	int offset = 0;
	//if(currentPiece->posY() < 0)
		//offset = 0 - currentPiece->posY();
	if(y < 0)
		offset = 0 - y;

	for(int i = offset; i <= currentPiece->previousFirstDown(); i++)
	{
		for(int j = currentPiece->previousFirstLeft(); j <= currentPiece->previousFirstRight(); j++)
		{
			if((x + j) > (columns - 1))
			{
				if(currentPiece->previousPieceMatrix[i][j] != 0
					&& scenario[(y + i) * columns + x + j - columns] != 0)
					return true;
			}
			else if((x + j) < 0)
			{
				if(currentPiece->previousPieceMatrix[i][j] != 0
					&& scenario[(y + i) * columns + columns - 1] != 0)
					return true;
			}
			else
			{
				if(currentPiece->previousPieceMatrix[i][j] != 0
					&& scenario[(y + i) * columns + x + j] != 0)
					return true;
			}
		}
	}

	return false;
}

void GameBoard::checkLines()
{
	for(int i = rows - 1; i >= 0; i--)
	{
		for(int j = 0; j < columns; j++)
		{
			if(scenario[i * columns + j] == 0)
				break;
			if(j == columns - 1)
			{
				fullLines[fullLinesCounter] = i;
				fullLinesCounter++;
			}
		}
	}
}

void GameBoard::levelUp()
{
	currentLevel++;
	timeInterval = 1000 / (1 + currentLevel);// + FAST_SPEED;
}

void GameBoard::calculateScore(int lines, int fallHeight)
{
	currentScore += fallHeight;

	switch(lines)
	{
	case 1:
		currentScore += 40 * currentLevel;
		break;
	case 2:
		currentScore += 100 * currentLevel;
		break;
	case 3:
		currentScore += 300 * currentLevel;
		break;
	case 4:
		currentScore += 1200 * currentLevel;
		break;
	}
}

int GameBoard::calculateFallPosition(int x, int y)
{
	int pos = y;
	while(pos < rows - currentPiece->firstDown() && !checkCollisions(x, pos + 1))
		pos++;
	return pos;
}

int GameBoard::calculatePreviousFallPosition(int x, int y)
{
	int pos = y;
	while(pos < rows - currentPiece->previousFirstDown() && !checkPreviousCollisions(x, pos + 1))
		pos++;
	return pos;
}

int GameBoard::calculateEmptySquaresUnderDroppedPiece(int x, int y)
{
	int pos = calculateFallPosition(x, y);
	if(pos < 0)
		return -1;

	int emptySquares = 0;
	for(int j = currentPiece->firstLeft(); j <= currentPiece->firstRight(); j++)
	{
		int posAux = pos + currentPiece->firstDownAtColumn(j) + 1;
		for(int i = posAux; i < rows; i++)
		{
			if((x + j) > (columns - 1))
			{
				if(scenario[i * columns + x + j - columns] == 0)
					emptySquares++;
			}
			else if((x + j) < 0)
			{
				if(scenario[i * columns + columns - 1] == 0)
					emptySquares++;
			}
			else
			{
				if(scenario[i * columns + x + j] == 0)
					emptySquares++;
			}
		}
	}
	return emptySquares;
}


//public slots


void GameBoard::setRotationStyle(int style)
{
	rotationStyle = style;
	currentPiece->setRotationStyle(style);
	nextPiece->setRotationStyle(style);
}

void GameBoard::setGameType(int value)
{
	gameType = value;
	if(gameType == twoPlayers || gameType == networkServer || gameType == networkClient)
	{
		totalGames = 0;
		totalScore = 0;
	}
}

void GameBoard::setGameMode(int value)
{
	 gameMode = value;
	 //clearScenario();
	 //fullRepaint(true);
}

void GameBoard::setMouseControlType(QAction *action)
{
	if(action->objectName() == "actionNoMouseControl")
		mouseControlType = noMouseControl;
	else if(action->objectName() == "actionActivatePermanentMouseControl")
		mouseControlType = permanentMouseControl;
	else if(action->objectName() == "actionActivateMouseControl")
		mouseControlType = mouseControl;
	else if(action->objectName() == "actionActivateMouseWheelControl")
		mouseControlType = mouseWheelControl;
}

void GameBoard::setGameColors(GameColors *cols)
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
	fullRepaint(true);
	repaint();
}

void GameBoard::startGame(bool repaint)
{
	switch(gameMode)
	{
	case tubularix:
	case invertedTubularix:
	case tetrisWithoutWalls:
	case invertedTetrisWithoutWalls:
		currentPiece->setLimits(0, columns - 1);
		nextPiece->setLimits(0, columns - 1);
		break;
	case tetris:
	case invertedTetris:
		currentPiece->setLimits(-5, columns - 1);
		nextPiece->setLimits(-5, columns - 1);
		break;
	}

	//piece's start position
	switch(gameMode)
	{
	case tubularix:
	case invertedTubularix:
		currentPiece->setPosX(0 - currentPiece->firstLeft());
		break;
	case tetrisWithoutWalls:
	case invertedTetrisWithoutWalls:
	case tetris:
	case invertedTetris:
		currentPiece->setPosX(columns / 2 - 1 - currentPiece->firstLeft());
		break;
	}
	currentPiece->setPreviousPosX(currentPiece->posX());

	//mouseControlType = linealMouseControl;
	pieceGrabbed = false;
	piecePosClicked = 0;
	setGameState(1);
	piecePaintingNeeded = true;
	scenarioPaintingNeeded = true;
	scenarioChangesPaintingNeeded = true;
	for(int i = 0; i < 4; i++)
		fullLines[i] = -1;
	fullLinesCounter = 0;
	fast = false;
	destroyedLines = 0;
	destroyLinesStep = 0;
	linesToAdd = 0.0;
	currentLevel = startLevel - 1;
	levelUp();
	currentScore = 0;
	fastFallHeight = 0;
	timeElapsed = 0;
	if(!nextPieces.isEmpty())
	{
		nextPiece->selectPiece(nextPieces.first());
		nextPieces.removeFirst();
	}
	else
		nextPiece->selectPiece();
	emit nextPieceNeeded();

	clearScenario();

	if(repaint)
		fullRepaint(true);

	pieceTimer->start(timeInterval);
	generatePieces();

	emit gameStarted();
}

void GameBoard::stopGame()
{
	setGameState(0);

	if(pieceTimer->isActive())
		pieceTimer->stop();

	//piecePaintingNeeded = true;
	//scenarioPaintingNeeded = true;
	//scenarioChangesPaintingNeeded = true;
	//repaint();
	emit gameStopped();
}

void GameBoard::pauseGame(bool pause)
{
	if(pause)
	{
		if(timeElapsed + time.elapsed() < timeInterval - FAST_SPEED)
			timeElapsed += time.elapsed();
		else
			timeElapsed = timeInterval - FAST_SPEED - 1;
		pieceTimer->stop();

		setGameState(2);
		piecePaintingNeeded = true;
		scenarioPaintingNeeded = true;
		scenarioChangesPaintingNeeded = true;
		repaint();
		emit gamePaused(true);
	}
	else
	{
		setGameState(1);
		pieceTimer->start(timeInterval - timeElapsed);
		time.restart();

		piecePaintingNeeded = true;
		scenarioPaintingNeeded = true;
		scenarioChangesPaintingNeeded = true;
		repaint();
		emit gamePaused(false);
	}
}

void GameBoard::fullRepaint(bool createBackground)
{
	if(createBackground)// && !scenario.isEmpty())
		createScenarioBackground();

	piecePaintingNeeded = true;
	scenarioPaintingNeeded = true;
	scenarioChangesPaintingNeeded = true;
	//repaint();
}

void GameBoard::showMessage(const QString &text, const QString &subText)
{
	setGameState(3);
	message = text;
	subMessage = subText;
	//scenarioChangesPaintingNeeded = true;
	fullRepaint();
	repaint();
}

void GameBoard::setNextPiecesList(QList <int> pieces)
{
	nextPieces.clear();
	nextPieces = pieces;
}

void GameBoard::addNextPiece(int piece)
{
	nextPieces.append(piece);
}


//private


void GameBoard::destroyLines()
{
	//scenarioPaintingNeeded = true;
	scenarioChangesPaintingNeeded = true;

	if(destroyLinesStep == 0 || destroyLinesStep == 2) //turn the pieces darker
	{
		for(int c = 0; c < fullLinesCounter; c++)
		{
			for(int j=0; j < columns; j++)
			{
				scenario[fullLines[c] * columns + j] = scenario[fullLines[c] * columns + j] + 50;
				scenarioChanges[fullLines[c] * columns + j] = 1;
			}
		}
		repaint();
		destroyLinesStep++;
		return;
	}
	else if(destroyLinesStep == 1 || destroyLinesStep == 3) //return the pieces to their normal color
	{
		for(int c = 0; c < fullLinesCounter; c++)
		{
			for(int j=0; j < columns; j++)
			{
				scenario[fullLines[c] * columns + j] = scenario[fullLines[c] * columns + j] - 50;
				scenarioChanges[fullLines[c] * columns + j] = 1;
			}
		}
		repaint();
		destroyLinesStep++;
		return;
	}

	//delete full lines and move down the rest of the scenario (reversed i and j)
	for(int c = 0; c < fullLinesCounter; c++)
	{
		fullLines[c] += c;
		for(int i = fullLines[c]; i >= 0; i--)
		{
			for(int j = 0; j < columns; j++)
			{
				if(i > 0)
					scenario[i * columns + j] = scenario[(i - 1) * columns + j];
				else if(i == 0)
					scenario[i * columns + j] = 0;
				scenarioChanges[i * columns + j] = 1;
			}
		}
	}
	repaint();

	emit linesDestroyed(fullLinesCounter);

	//restore values
	for(int i = 0; i < 4; i++)
		fullLines[i] = -1;
	fullLinesCounter = 0;
	destroyLinesStep = 0;
}

void GameBoard::addLines()
{
	if(gameState() != started)
		return;

	//end game if with the addition of the line scenario overflows
	for(int j = 0; j < columns; j++)
	{
		if(scenario[j] != 0)
			stopGame();
	}

	//move up scenario one row (reversed i)
	for(int i = 0; i < rows - 1; i++)
	{
		for(int j = 0; j < columns; j++)
		{
			scenario[i * columns + j] = scenario[(i + 1) * columns + j];
			scenarioChanges[i * columns + j] = 1;
		}
	}

	//add a row with a randomly placed empty square at the bottom of the scenario
	for(int j = 0; j < columns; j++)
	{
		scenario[(rows - 1) * columns + j] = 1;
		scenarioChanges[(rows - 1) * columns + j] = 1;
	}
	scenario[(rows - 1) * columns + (qrand() % columns)] = 0;

	scenarioChangesPaintingNeeded = true;
	repaint();
	linesToAdd--;
}

/*void GameBoard::test(QPainter &painter)
{

}*/

void GameBoard::createScenarioBackground()
{
	//scenarioBackground = QImage(size(), QImage::Format_ARGB32_Premultiplied);
	//scenarioBackground.fill(0);
	scenarioBackground = QPicture();
	QPainter painter;
	if(!painter.begin(&scenarioBackground))
		return;

	QSize size;
	size.setHeight(initialHeight);
	size.setWidth(initialWidth);
	//if(gameMode == tubularix || gameMode == invertedTubularix)
		size.scale(width(), height(), Qt::KeepAspectRatio);
	//else
		//size.scale(width(), height(), Qt::IgnoreAspectRatio);
	painter.translate(qreal(width()) / 2.0 - qreal(size.width()) / 2.0,
		qreal(height()) / 2.0 - qreal(size.height()) / 2.0);
	painter.scale(qreal(size.width()) / qreal(initialWidth),
		qreal(size.height()) / qreal(initialHeight));

	if(antialias && (gameMode == tubularix || gameMode == invertedTubularix))
		painter.setRenderHint(QPainter::Antialiasing);

	qreal pHeight = 162.0 / qreal(rows);//9.0;
	qreal mult = 18.0 / qreal(rows) * 10.8 / qreal(rows);//0.6;
	qreal auxI = 0.0;

	int mapSquareSize = 6 * 18 / columns;
	if(rows >= columns)
		mapSquareSize = 6 * 18 / rows;
	int squareSize = 38 * 18 / columns;
	if(rows >= columns)
		squareSize = 38 * 18 / rows;
	int squareFirstX = initialWidth / 2 - squareSize * columns / 2;
	int squareFirstY = initialHeight / 2 - squareSize * rows / 2 + 10;
	int squareFirstYReversed = initialHeight / 2 + squareSize * rows / 2 - 20;

	//painter.setBrush(Qt::NoBrush);
	//painter.setBrush(gameColors->color(0));
	painter.setPen(gameColors->color(9));

	//paint scenario matrix contents
	for(int i = 0; i < rows; i++)
	{
		if(gameMode == invertedTubularix) //reversed
			auxI = qreal(i) + 1.0;
		else if (gameMode == tubularix) //normal
			auxI = qreal(rows) - qreal(i);

		qreal radius1 = auxI * (pHeight + auxI * mult) + 40.5;
		qreal radius2 = (auxI - 1.0) * (pHeight + (auxI - 1.0) * mult) + 40.5;

		for(int j = 0; j < columns; j++)
		{
			int color = 0;

			//small scenario map
			if(showMap)
			{
				paintMapSquare(painter, 776 - mapSquareSize * columns / 2,
					(initialHeight - 30 - rows * mapSquareSize), mapSquareSize, i, j, color);
			}

			if(gameMode == tubularix || gameMode == invertedTubularix)
			{
				painter.save();
				painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
				paintTubularixSquare(painter, radius1, radius2, i, j, color);
				painter.restore();
			}
			else if(gameMode == tetris || gameMode == tetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstY, squareSize, i, j, color);
			else if(gameMode == invertedTetris || gameMode == invertedTetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstYReversed, squareSize, -i, j, color);
		}
	}
	painter.end();
}

void GameBoard::paintScenario(QPainter &painter)
{
	painter.save();

	qreal pHeight = 162.0 / qreal(rows);//9.0;
	qreal mult = 18.0 / qreal(rows) * 10.8 / qreal(rows);//0.6;
	qreal auxI = 0.0;

	int mapSquareSize = 6 * 18 / columns;
	if(rows >= columns)
		mapSquareSize = 6 * 18 / rows;
	int squareSize = 38 * 18 / columns;
	if(rows >= columns)
		squareSize = 38 * 18 / rows;
	int squareFirstX = initialWidth / 2 - squareSize * columns / 2;
	int squareFirstY = initialHeight / 2 - squareSize * rows / 2 + 10;
	int squareFirstYReversed = initialHeight / 2 + squareSize * rows / 2 - 20;

	painter.setPen(gameColors->color(9));

	//paint scenario matrix contents
	for(int i = 0; i < rows; i++)
	{
		if(gameMode == invertedTubularix) //reversed
			auxI = qreal(i) + 1.0;
		else if (gameMode == tubularix) //normal
			auxI = qreal(rows) - qreal(i);

		qreal radius1 = auxI * (pHeight + auxI * mult) + 40.5;
		qreal radius2 = (auxI - 1.0) * (pHeight + (auxI - 1.0) * mult) + 40.5;

		for(int j = 0; j < columns; j++)
		{
			if(scenarioChangesPaintingNeeded && scenarioChanges[i * columns + j] == 1)
				scenarioChanges[i * columns + j] = 0;
			else if(!scenarioPaintingNeeded || scenario[i * columns + j] == 0)
				continue;

			int color = scenario[i * columns + j];

			//small scenario map
			if(showMap)
			{
				paintMapSquare(painter, 776 - mapSquareSize * columns / 2,
					(initialHeight - 30 - rows * mapSquareSize), mapSquareSize, i, j, color);
			}

			if(gameMode == tubularix || gameMode == invertedTubularix)
			{
				painter.save();
				painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
				paintTubularixSquare(painter, radius1, radius2, i, j, color);
				painter.restore();
			}
			else if(gameMode == tetris || gameMode == tetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstY, squareSize, i, j, color);
			else if(gameMode == invertedTetris || gameMode == invertedTetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstYReversed, squareSize, -i, j, color);
		}
	}
	painter.restore();
	scenarioPaintingNeeded = false;
	scenarioChangesPaintingNeeded = false;
}

void GameBoard::paintPiece(QPainter &painter)
{
	//if(!pieceTimer->isActive())
		//return;

	painter.save();

	qreal pHeight = 162.0 / qreal(rows);//9.0;
	qreal mult = 18.0 / qreal(rows) * 10.8 / qreal(rows);//0.6;
	qreal auxI = 0.0;

	int mapSquareSize = 6 * 18 / columns;
	if(rows >= columns)
		mapSquareSize = 6 * 18 / rows;
	int squareSize = 38 * 18 / columns;
	if(rows >= columns)
		squareSize = 38 * 18 / rows;
	int squareFirstX = initialWidth / 2 - squareSize * columns / 2;
	int squareFirstY = initialHeight / 2 - squareSize * rows / 2 + 10;
	int squareFirstYReversed = initialHeight / 2 + squareSize * rows / 2 - 20;

	painter.setPen(gameColors->color(9));

	int auxJ;

	//unpaint piece's previous position
	for(int i = currentPiece->previousPosY(); i < (currentPiece->previousPosY() + 4); i++)
	{
		if(gameMode == invertedTubularix) //reversed
			auxI = qreal(i) + 1.0;
		else if (gameMode == tubularix) //normal
			auxI = qreal(rows) - qreal(i);

		qreal radius1 = auxI * (pHeight + auxI * mult) + 40.5;
		qreal radius2 = (auxI - 1.0) * (pHeight + (auxI - 1.0) * mult) + 40.5;

		for(int j = currentPiece->previousPosX(); j < (currentPiece->previousPosX() + 4); j++)
		{
			if(j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			if(currentPiece->previousPieceMatrix[i - currentPiece->previousPosY()]
			   [j - currentPiece->previousPosX()] == 0 || i < 0)
				continue;

			int color = 0;

			//small scenario map
			if(showMap)
			{
				paintMapSquare(painter, 776 - mapSquareSize * columns / 2,
					(initialHeight - 30 - rows * mapSquareSize), mapSquareSize, i, auxJ, color);
			}

			if(gameMode == tubularix || gameMode == invertedTubularix)
			{
				painter.save();
				painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
				paintTubularixSquare(painter, radius1, radius2, i, auxJ, color);
				painter.restore();
			}
			else if(gameMode == tetris || gameMode == tetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstY, squareSize, i, auxJ, color);
			else if(gameMode == invertedTetris || gameMode == invertedTetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstYReversed, squareSize, -i, auxJ, color);
		}
	}

	//paint piece
	for(int i = currentPiece->posY(); i < (currentPiece->posY() + 4); i++)
	{
		if(gameMode == invertedTubularix) //reversed
			auxI = qreal(i) + 1.0;
		else if (gameMode == tubularix) //normal
			auxI = qreal(rows) - qreal(i);

		qreal radius1 = auxI * (pHeight + auxI * mult) + 40.5;
		qreal radius2 = (auxI - 1.0) * (pHeight + (auxI - 1.0) * mult) + 40.5;

		for(int j = currentPiece->posX(); j < (currentPiece->posX() + 4); j++)
		{
			if(j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			if(currentPiece->pieceMatrix[i - currentPiece->posY()][j - currentPiece->posX()] == 0 || i < 0)
				continue;

			int color = currentPiece->pieceMatrix[i - currentPiece->posY()][j - currentPiece->posX()];

			//small scenario map
			if(showMap)
			{
				paintMapSquare(painter, 776 - mapSquareSize * columns / 2,
					(initialHeight - 30 - rows * mapSquareSize), mapSquareSize, i, auxJ, color);
			}

			if(gameMode == tubularix || gameMode == invertedTubularix)
			{
				painter.save();
				painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
				paintTubularixSquare(painter, radius1, radius2, i, auxJ, color);
				painter.restore();
			}
			else if(gameMode == tetris || gameMode == tetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstY, squareSize, i, auxJ, color);
			else if(gameMode == invertedTetris || gameMode == invertedTetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstYReversed, squareSize, -i, auxJ, color);
		}
	}
	painter.restore();
	piecePaintingNeeded = false;
}

void GameBoard::paintGhostPiece(QPainter &painter)
{
	//if(!pieceTimer->isActive())
		//return;

	painter.save();

	qreal pHeight = 162.0 / qreal(rows);//9.0;
	qreal mult = 18.0 / qreal(rows) * 10.8 / qreal(rows);//0.6;
	qreal auxI = 0.0;

	int mapSquareSize = 6 * 18 / columns;
	if(rows >= columns)
		mapSquareSize = 6 * 18 / rows;
	int squareSize = 38 * 18 / columns;
	if(rows >= columns)
		squareSize = 38 * 18 / rows;
	int squareFirstX = initialWidth / 2 - squareSize * columns / 2;
	int squareFirstY = initialHeight / 2 - squareSize * rows / 2 + 10;
	int squareFirstYReversed = initialHeight / 2 + squareSize * rows / 2 - 20;

	painter.setPen(gameColors->color(9));

	int pos = calculatePreviousFallPosition(currentPiece->previousPosX(), currentPiece->previousPosY());
	int auxJ;

	//unpaint piece's previous position
	for(int i = pos; i < pos + 4; i++)
	{
		if(gameMode == invertedTubularix) //reversed
			auxI = qreal(i) + 1.0;
		else if (gameMode == tubularix) //normal
			auxI = qreal(rows) - qreal(i);

		qreal radius1 = auxI * (pHeight + auxI * mult) + 40.5;
		qreal radius2 = (auxI - 1.0) * (pHeight + (auxI - 1.0) * mult) + 40.5;

		for(int j = currentPiece->previousPosX(); j < (currentPiece->previousPosX() + 4); j++)
		{
			if(j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			if(currentPiece->previousPieceMatrix[i - pos][j - currentPiece->previousPosX()] == 0 || i < 0)
				continue;

			int color = 0;

			//small scenario map
			if(showMap)
			{
				paintMapSquare(painter, 776 - mapSquareSize * columns / 2,
					(initialHeight - 30 - rows * mapSquareSize), mapSquareSize, i, auxJ, color);
			}

			if(gameMode == tubularix || gameMode == invertedTubularix)
			{
				painter.save();
				painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
				paintTubularixSquare(painter, radius1, radius2, i, auxJ, color);
				painter.restore();
			}
			else if(gameMode == tetris || gameMode == tetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstY, squareSize, i, auxJ, color);
			else if(gameMode == invertedTetris || gameMode == invertedTetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstYReversed, squareSize, -i, auxJ, color);
		}
	}

	pos = calculateFallPosition(currentPiece->posX(), currentPiece->posY());

	//paint piece
	for(int i = pos; i < pos + 4; i++)
	{
		if(gameMode == invertedTubularix) //reversed
			auxI = qreal(i) + 1.0;
		else if (gameMode == tubularix) //normal
			auxI = qreal(rows) - qreal(i);

		qreal radius1 = auxI * (pHeight + auxI * mult) + 40.5;
		qreal radius2 = (auxI - 1.0) * (pHeight + (auxI - 1.0) * mult) + 40.5;

		for(int j = currentPiece->posX(); j < (currentPiece->posX() + 4); j++)
		{
			if(j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			if(currentPiece->pieceMatrix[i - pos][j - currentPiece->posX()] == 0 || i < 0)
				continue;

			int color = currentPiece->pieceMatrix[i - pos][j - currentPiece->posX()] + 100;

			//small scenario map
			if(showMap)
			{
				paintMapSquare(painter, 776 - mapSquareSize * columns / 2,
					(initialHeight - 30 - rows * mapSquareSize), mapSquareSize, i, auxJ, color);
			}

			if(gameMode == tubularix || gameMode == invertedTubularix)
			{
				painter.save();
				painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
				paintTubularixSquare(painter, radius1, radius2, i, auxJ, color);
				painter.restore();
			}
			else if(gameMode == tetris || gameMode == tetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstY, squareSize, i, auxJ, color);
			else if(gameMode == invertedTetris || gameMode == invertedTetrisWithoutWalls)
				paintTetrisSquare(painter, squareFirstX, squareFirstYReversed, squareSize, -i, auxJ, color);
		}
	}
	painter.restore();
}

void GameBoard::paintShadow(QPainter &painter)
{
	//if(!pieceTimer->isActive())
		//return;

	painter.save();

	//qreal pHeight = 162.0 / qreal(rows);//9.0;
	//qreal mult = 18.0 / qreal(rows) * 10.8 / qreal(rows);//0.6;
	//qreal auxI = 0.0;

	int mapSquareSize = 6 * 18 / columns;
	if(rows >= columns)
		mapSquareSize = 6 * 18 / rows;
	int squareSize = 38 * 18 / columns;
	if(rows >= columns)
		squareSize = 38 * 18 / rows;
	int squareFirstX = initialWidth / 2 - squareSize * columns / 2;
	int squareFirstY = initialHeight / 2 - squareSize * rows / 2 + 10;
	int squareFirstYReversed = initialHeight / 2 + squareSize * rows / 2 - 20;

	int auxJ;

	QPen shadowPen;

	//paint current piece's shadow in normal tubularix mode
	if(gameMode == tubularix)
	{
		//unpaint current piece's previous shadow
		shadowPen.setColor(gameColors->color(8));
		shadowPen.setWidth(3);
		painter.setPen(shadowPen);
		painter.setBrush(gameColors->color(8));
		for(int j = currentPiece->previousFirstLeft(); j <= currentPiece->previousFirstRight(); j++)
		{
			if(currentPiece->previousPosX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			painter.save();
			painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
			QPointF currentPieceShadowPoints[4] = {
				QPointF(-0.45 * ((2 * M_PI * 36.5) / qreal(columns)), 36.5),
				QPointF(0.45 * ((2 * M_PI * 36.5) / qreal(columns)), 36.5),
				QPointF(0.45 * ((2 * M_PI * 32.5) / qreal(columns)), 32.5),
				QPointF(-0.45 * ((2 * M_PI * 32.5) / qreal(columns)), 32.5)};

			painter.rotate((currentPiece->previousPosX() + auxJ) * (360.0 / qreal(columns)) + 180.0);
			painter.drawConvexPolygon(currentPieceShadowPoints, 4);
			painter.restore();
		}

		//paint current piece's shadow
		shadowPen.setColor(gameColors->color(9));
		shadowPen.setWidth(1);
		painter.setPen(shadowPen);
		painter.setBrush(gameColors->color(12));
		for(int j = currentPiece->firstLeft(); j <= currentPiece->firstRight(); j++)
		{
			if(currentPiece->posX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			painter.save();
			painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
			QPointF currentPieceShadowPoints[4] = {
				QPointF(-0.45 * ((2 * M_PI * 36.5) / qreal(columns)), 36.5),
				QPointF(0.45 * ((2 * M_PI * 36.5) / qreal(columns)), 36.5),
				QPointF(0.45 * ((2 * M_PI * 32.5) / qreal(columns)), 32.5),
				QPointF(-0.45 * ((2 * M_PI * 32.5) / qreal(columns)), 32.5)};

			painter.rotate((currentPiece->posX() + auxJ) * (360.0 / qreal(columns)) + 180.0);
			painter.drawConvexPolygon(currentPieceShadowPoints, 4);
			painter.restore();
		}
	}

	//paint current piece's upper shadow in reversed tubularix mode
	if(gameMode == invertedTubularix)
	{
		//unpaint current piece's previous shadow
		shadowPen.setColor(gameColors->color(8));
		painter.setPen(shadowPen);
		painter.setBrush(gameColors->color(8));
		for(int j = currentPiece->previousFirstLeft(); j <= currentPiece->previousFirstRight(); j++)
		{
			if(currentPiece->previousPosX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			painter.save();
			painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
			QPointF currentPieceShadowPoints[4] = {
				QPointF(-0.4 * ((2 * M_PI * 420.5) / qreal(columns)), 410.5),
				QPointF(0.4 * ((2 * M_PI * 420.5) / qreal(columns)), 410.5),
				QPointF(0.4 * ((2 * M_PI * 410.5) / qreal(columns)), 400.5),
				QPointF(-0.4 * ((2 * M_PI * 410.5) / qreal(columns)), 400.5)};

			painter.rotate((currentPiece->previousPosX() + auxJ) * (360.0 / qreal(columns)) + 180.0);
			painter.drawConvexPolygon(currentPieceShadowPoints, 4);
			painter.restore();
		}

		//paint current piece's shadow
		painter.setBrush(gameColors->color(13));
		for(int j = currentPiece->firstLeft(); j <= currentPiece->firstRight(); j++)
		{
			if(currentPiece->posX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			painter.save();
			painter.translate(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0);
			QPointF currentPieceShadowPoints[4] = {
				QPointF(-0.4 * ((2 * M_PI * 420.5) / qreal(columns)), 410.5),
				QPointF(0.4 * ((2 * M_PI * 420.5) / qreal(columns)), 410.5),
				QPointF(0.4 * ((2 * M_PI * 410.5) / qreal(columns)), 400.5),
				QPointF(-0.4 * ((2 * M_PI * 410.5) / qreal(columns)), 400.5)};

			painter.rotate((currentPiece->posX() + auxJ) * (360.0 / qreal(columns)) + 180.0);
			painter.drawConvexPolygon(currentPieceShadowPoints, 4);
			painter.restore();
		}
	}

	//paint current piece's upper shadow in tetris with and without walls
	if(gameMode == tetris || gameMode == tetrisWithoutWalls)
	{
		//unpaint current piece's previous shadow
		shadowPen.setColor(gameColors->color(8));
		painter.setPen(shadowPen);
		painter.setBrush(gameColors->color(8));
		for(int j = currentPiece->previousFirstLeft(); j <= currentPiece->previousFirstRight(); j++)
		{
			if(currentPiece->previousPosX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			//painter.save();
			painter.drawRect(squareFirstX + (currentPiece->previousPosX() + auxJ) * squareSize,
				squareFirstY + squareSize * rows + 10, squareSize, squareSize / 2);
			//painter.restore();
		}

		//paint current piece's shadow
		painter.setBrush(gameColors->color(13));
		for(int j = currentPiece->firstLeft(); j <= currentPiece->firstRight(); j++)
		{
			if(currentPiece->posX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			//painter.save();
			painter.drawRect(squareFirstX + (currentPiece->posX() + auxJ) * squareSize,
				squareFirstY + squareSize * rows + 10, squareSize, squareSize / 2);
			//painter.restore();
		}
	}

	//paint current piece's upper shadow in reversed tetris with and without walls
	if(gameMode == invertedTetris || gameMode == invertedTetrisWithoutWalls)
	{
		//unpaint current piece's previous shadow
		shadowPen.setColor(gameColors->color(8));
		painter.setPen(shadowPen);
		painter.setBrush(gameColors->color(8));
		for(int j = currentPiece->previousFirstLeft(); j <= currentPiece->previousFirstRight(); j++)
		{
			if(currentPiece->previousPosX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			//painter.save();
			painter.drawRect(squareFirstX + (currentPiece->previousPosX() + auxJ) * squareSize,
				squareFirstYReversed - squareSize * rows, squareSize, squareSize / 2);
			//painter.restore();
		}

		//paint current piece's shadow
		painter.setBrush(gameColors->color(13));
		for(int j = currentPiece->firstLeft(); j <= currentPiece->firstRight(); j++)
		{
			if(currentPiece->posX() + j > columns - 1)
				auxJ = j - columns;
			else
				auxJ = j;

			//painter.save();
			painter.drawRect(squareFirstX + (currentPiece->posX() + auxJ) * squareSize,
				squareFirstYReversed - squareSize * rows, squareSize, squareSize / 2);
			//painter.restore();
		}
	}
	painter.restore();
}

void GameBoard::paintNextPiece(QPainter &painter)
{
	painter.save();

	painter.setPen(Qt::NoPen);
	//painter.setBrush(gameColors->color(8));
	painter.drawRect(758, 36, 4 * 22, 4 * 22);

	painter.setPen(gameColors->color(9));
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(nextPiece->pieceMatrix[i][j])
			{
				painter.setBrush(gameColors->color(nextPiece->pieceMatrix[i][j]));
				painter.drawRect(760 + (j - nextPiece->firstLeft()) * 20, 38 + i * 20, 20, 20);
			}
		}
	}
	painter.restore();
}

void GameBoard::paintMapSquare(QPainter &painter, int squareFirstX, int squareFirstY,
	int squareSize, int row, int col, int color)
{
	//painter.setPen(Qt::NoPen);
	painter.setPen(gameColors->color(9, row));
	painter.setBrush(gameColors->color(color, row));
	//painter.setPen(gameColors->color(color, row));
	painter.drawRect(squareFirstX + col * squareSize, squareFirstY + row * squareSize,
		squareSize, squareSize);
	/*painter.setBrush(painter.brush().color().darker(130));
	painter.drawRect(squareFirstX + j * squareSize + squareSize * 0.1,
		squareFirstY + i * squareSize + squareSize * 0.1,
		squareSize * 0.9, squareSize * 0.9);*/
}

void GameBoard::paintTetrisSquare(QPainter &painter, int squareFirstX, int squareFirstY,
	int squareSize, int row, int col, int color)
{
	//painter.setPen(Qt::NoPen);
	if(gameMode == invertedTetrisWithoutWalls || gameMode == invertedTetris)
	{
		painter.setPen(gameColors->color(9, rows + row - 1));
		painter.setBrush(gameColors->color(color, rows + row - 1));
	}
	else
	{
		painter.setPen(gameColors->color(9, row));
		painter.setBrush(gameColors->color(color, row));
	}
	//painter.setPen(gameColors->color(color, row));
	painter.drawRect(squareFirstX + col * squareSize, squareFirstY + row * squareSize,
		squareSize, squareSize);
	/*painter.setBrush(painter.brush().color().darker(130));
	painter.drawRect(squareFirstX + j * squareSize + squareSize * 0.1,
		squareFirstY + i * squareSize + squareSize * 0.1,
		squareSize * 0.9, squareSize * 0.9);*/
}

void GameBoard::paintTubularixSquare(QPainter &painter, qreal r1, qreal r2, int row, int col, int color)
{
	if(gameMode == invertedTubularix)
		row = rows - row;

	painter.rotate(qreal(col) * (360.0 / qreal(columns)) + 180.0);

	/*QPointF piecePoints[] = {
		QPointF(-0.5 * ((2.0 * M_PI * r1) / qreal(columns)), r1),
		QPointF(0.5 * ((2.0 * M_PI * r1) / qreal(columns)), r1),
		QPointF(0.5 * ((2.0 * M_PI * r2) / qreal(columns)), r2),
		QPointF(-0.5 * ((2.0 * M_PI * r2) / qreal(columns)), r2)
	};*/
	/*r1 -= 1.0;
	r2 += 1.0;*/
	QPointF piecePoints[] = {
		QPointF(-0.51 * ((2.0 * M_PI * r1) / qreal(columns)), r1),
		QPointF(0.51 * ((2.0 * M_PI * r1) / qreal(columns)), r1),
		QPointF(0.51 * ((2.0 * M_PI * r2) / qreal(columns)), r2),
		QPointF(-0.51* ((2.0 * M_PI * r2) / qreal(columns)), r2)
	};
	painter.setPen(gameColors->color(9, row));
	painter.setBrush(gameColors->color(color, row));

	//if(color != 0)
	/*{
		//QRadialGradient gradient(-0.51 * ((2.0 * M_PI * r1) / qreal(columns)), r1,
			//(2.0 * M_PI * r1) / qreal(columns));
		//QRadialGradient gradient(0, r1, 0.1 * (2.0 * M_PI * r1) / qreal(columns));
		//gradient.setSpread(QGradient::ReflectSpread);
		QRadialGradient gradient(0, r1, (2.0 * M_PI * r1) / qreal(columns));
		gradient.setColorAt(0, gameColors->color(color, row));
		gradient.setColorAt(1, gameColors->color(color, row).darker(170));
		//gradient.setColorAt(1, gameColors->color(color, row).darker(50));
		painter.setBrush(gradient);
	}*/

	/*QPen p = painter.pen();
	p.setWidthF(0.5);
	painter.setPen(p);*/
	//painter.setPen(Qt::NoPen);
	//painter.setPen(gameColors->color(color, row));
	painter.drawConvexPolygon(piecePoints, 4);
	//painter.drawPolyline(piecePoints, 4);

	/*QPainterPath path;
	path.moveTo(QPointF(-0.51 * ((2.0 * M_PI * r1) / qreal(columns)), r1));
	path.lineTo(QPointF(0.51 * ((2.0 * M_PI * r1) / qreal(columns)), r1));
	path.lineTo(QPointF(0.51 * ((2.0 * M_PI * r2) / qreal(columns)), r2));
	path.lineTo(QPointF(-0.51* ((2.0 * M_PI * r2) / qreal(columns)), r2));
	painter.drawPath(path);*/

	/*QPointF piecePoints2[] = {
		QPointF(-0.45 * ((2.0 * M_PI * r1 * 0.99) / qreal(columns)), r1 * 0.98),
		QPointF(0.45 * ((2.0 * M_PI * r1 * 0.99) / qreal(columns)), r1 * 0.98),
		QPointF(0.45 * ((2.0 * M_PI * r2 * 1.01) / qreal(columns)), r2 * 1.02),
		QPointF(-0.45 * ((2.0 * M_PI * r2 * 1.01) / qreal(columns)), r2 * 1.02)
	};*/
	//painter.setBrush(gameColors->color(color, row).darker(96));
	//painter.setPen(Qt::NoPen);
	//painter.drawConvexPolygon(piecePoints2, 4);
}

void GameBoard::paintTextInfo(QPainter &painter)
{
	//unpaint previous text info
	painter.setPen(Qt::NoPen);
	painter.drawRect(10, 3, 234, 22);
	painter.drawRect(10, 33, 200, 22);
	painter.drawRect(10, 63, 105, 22);
	painter.drawRect(765, 3, 85, 22);
	if(gameType == twoPlayers || gameType == networkServer || gameType == networkClient)
	{
		painter.drawRect(10, initialHeight - 62, 175, 22);
		painter.drawRect(10, initialHeight - 32, 185, 22);
	}

	//show text info
	QFont font;
	QFontMetrics fontMetrics = QFontMetrics(font);
	font.setPointSize(14);
	painter.setFont(font);
	painter.setPen(gameColors->color(10));
	painter.drawText(10, 20, tr("lines destroyed: ") + QString::number(destroyedLines));
	painter.drawText(10, 50, tr("score: ") + QString::number(currentScore));
	painter.drawText(10, 80, tr("level: ") + QString::number(currentLevel));
	painter.drawText(765, 20, tr("next"));
	if(gameType == twoPlayers || gameType == networkServer || gameType == networkClient)
	{
		painter.setPen(gameColors->color(10));
		painter.drawText(10, initialHeight - 15, tr("total games won: ") + QString::number(totalGames));
		painter.drawText(10, initialHeight - 45, tr("total score: ") + QString::number(totalScore));
	}

	//show pause, game over, or wait screen as needed
	font.setPointSize(46);
	painter.setFont(font);
	switch(gameState())
	{
	/*
	case stopped: //Game over
		painter.save();
		painter.setBrush(gameColors->color(11));
		painter.drawRoundedRect(105, 300, 640, 200, 10, 10);
		painter.drawText(200, 380, tr("game over"));
		font.setPointSize(36);
		painter.setFont(font);
		painter.drawText(200, 460, tr("press r to restart"));
		painter.restore();
		break;
	*/
	case paused: //Pause
		//hide game board in tubularix and inverted tubularix modes
		if(gameMode == tubularix || gameMode == invertedTubularix)
			painter.drawEllipse(QPointF(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0), 415, 415);

		//hide game board in the other modes
		else
		{
			int squareSize = 38 * 18 / columns;
			if(rows >= columns)
				squareSize = 38 * 18 / rows;
			painter.drawRect(initialWidth / 2 - squareSize * columns / 2,
				initialHeight / 2 - squareSize * rows / 2,
				squareSize * columns, squareSize * rows + squareSize);
		}

		//hide map
		if(showMap)
		{
			int mapSquareSize = 6 * 18 / columns;
			if(rows >= columns)
				mapSquareSize = 6 * 18 / rows;
			painter.drawRect(776 - mapSquareSize * columns / 2,
				initialHeight - 30 - rows * mapSquareSize,
				mapSquareSize * columns, mapSquareSize * rows);
		}

		//hide next piece?

		painter.save();
		painter.setBrush(gameColors->color(11));
		painter.drawRoundedRect(100, 300, 630, 200, 10, 10);
		fontMetrics = QFontMetrics(font);
		painter.drawText(100 + 630 / 2 - fontMetrics.width(tr("game paused")) / 2, 380,
			tr("game paused"));
		font.setPointSize(36);
		painter.setFont(font);
		fontMetrics = QFontMetrics(font);
		painter.drawText(100 + 630 / 2 - fontMetrics.width(tr("press p to continue")) / 2, 460,
			tr("press p to continue"));
		painter.restore();
		break;
	case showingMessage: //Show final message
		painter.save();
		painter.setBrush(gameColors->color(11));
		painter.drawRoundedRect(15, 300, 805, 200, 10, 10);

		font.setPointSize(46);
		fontMetrics = QFontMetrics(font);
		while(font.pointSize() > 1 && fontMetrics.width(message) >= initialWidth - 45)
		{
			font.setPointSize(font.pointSize() - 1);
			fontMetrics = QFontMetrics(font);
		}

		painter.setFont(font);
		painter.drawText(15 + 805 / 2 - fontMetrics.width(message) / 2, 380, message);

		font.setPointSize(36);
		fontMetrics = QFontMetrics(font);
		while(font.pointSize() > 1 && fontMetrics.width(subMessage) >= initialWidth - 45)
		{
			font.setPointSize(font.pointSize() - 1);
			fontMetrics = QFontMetrics(font);
		}
		painter.setFont(font);
		painter.drawText(15 + 805 / 2 - fontMetrics.width(subMessage) / 2, 460, subMessage);

		painter.restore();
		//message.clear();
		//subMessage.clear();
		break;
	}
}

//return the column that corresponds to the position received
int GameBoard::cursorAtColumn(QPoint pos)
{
	int col = 0;

	//set col between 0 and columns - 1
	if(gameMode == tubularix || gameMode == invertedTubularix)
	{
		int x = pos.x() - (width() / 2);
		int y = (height() / 2) - pos.y();
		qreal angle = 0.0;
		if(x == 0)
		{
			if(y >= 0)
				angle = 90.0;
			else
				angle = 270.0;
		}
		else
			angle = atan2(qreal(y), qreal(x)) * 360.0 / (2.0 * M_PI); //angle in degrees (-180.0 to 180.0)
		if(angle < 0.0)
			angle += 360.0;

		//reverse and apply an offset to the column corresponding to the real gameboard
		angle = 360.0 - angle;
		col = columns * ((angle / 360.0) + (90.0 / 360.0) + (360.0 / columns / 360.0 / 2.0));

		if(col >= columns)
			col -= columns;
	}

	//set col between -4 and columns - 1 + 4
	else if(gameMode >= tetrisWithoutWalls && gameMode <= invertedTetris)
	{
		QSize size;
		size.setHeight(initialHeight);
		size.setWidth(initialWidth);
		size.scale(width(), height(), Qt::KeepAspectRatio);
		qreal scale = qreal(size.width()) / qreal(initialWidth);
		qreal left = qreal(width()) / 2.0 - qreal(size.width()) / 2.0;
		qreal w = initialWidth * scale;

		qreal squareSize = 38 * 18 / columns * scale;
		if(rows >= columns)
			squareSize = 38 * 18 / rows * scale;
		//int squareFirstX = left + w / 2 - squareSize * columns / 2;
		int squareFirstX = left + w / 2 - squareSize * (columns + 4 * 2) / 2;

		/*if(pos.x() < squareFirstX)
			col = 0;
		else if(pos.x() > squareFirstX + squareSize * columns)
			col = columns - 1;
		else
			col = (pos.x() - squareFirstX) / squareSize;*/
		if(pos.x() < squareFirstX)
			col = 0 - 4;
		else if(pos.x() > squareFirstX + squareSize * (columns + 4 * 2))
			col = columns - 1 + 4;
		else
		{
			col = qreal(pos.x() - squareFirstX) / squareSize;
			if(col >= 0)
				col -= 4;
			else
				col -= 4 - 1;
		}
		//showMessage(QString::number(pos.x()), QString::number(col));
	}

	return col;
}

void GameBoard::clearScenario()
{
	scenario.clear();
	scenarioChanges.clear();
	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < columns; j++)
		{
			scenario.append(0);
			scenarioChanges.append(0);
		}
	}
}


//private slots


void GameBoard::gravity()
{
	if(pieceTimer->interval() != timeInterval && pieceTimer->interval() != FAST_SPEED)// && fast == false)
	{
		pieceTimer->setInterval(timeInterval);
		fastFallHeight = 0;
	}

	if(currentPiece->posY() + currentPiece->firstDown() >= rows - 1
		|| checkCollisions(currentPiece->posX(), currentPiece->posY() + 1))
	{
		updateScenario();

		checkLines();
		destroyedLines += fullLinesCounter;
		calculateScore(fullLinesCounter, fastFallHeight);

		//level up every 10 destroyed lines
		for(int i = fullLinesCounter; i > 0; i--)
		{
			if((destroyedLines - i + 1) % 10 == 0)
				levelUp();
		}

		//restore speed if key_down is still being pressed
		if(gameState() == started)
			restoreSpeed();

		//stop gravity and start animations if needed
		if(fullLinesCounter || linesToAdd >= 1.0)
		{
			pieceTimer->stop();
			animTimer->start(0);
		}
		else if(currentPiece->posY() + currentPiece->firstUp() < 0)
		{
			stopGame();

			/*
			piecePaintingNeeded = true;
			//scenarioPaintingNeeded = true;
			scenarioChangesPaintingNeeded = true;
			repaint();
			*/
		}
		else
			generatePieces();
	}
	else if(pieceTimer->isActive())
	{
		currentPiece->setPosY(currentPiece->posY() + 1);
		piecePaintingNeeded = true;
		repaint();
	}

	timeElapsed = 0;
	time.restart();
}

void GameBoard::doAnimations()
{
	if(gameState() != started)
		return;

	if(animTimer->interval() < 170)
		animTimer->setInterval(170);

	//do first the destroying animation and when finished, do the add lines animation
	if(fullLinesCounter > 0)
		destroyLines();
	else if(linesToAdd >= 1.0)
		addLines();
	else //return to game
	//if(!fullLinesCounter && linesToAdd < 1.0)
	{
		animTimer->stop();
		generatePieces();
		pieceTimer->start(timeInterval);
	}
}

void GameBoard::hideCursor()
{
	setCursor(QCursor(Qt::BlankCursor));
	cursorTimer->stop();
}


//protected


void GameBoard::paintEvent(QPaintEvent *event)
{
	emit paintEventActivated();

	QPainter painter(this);

	painter.setBrush(gameColors->color(8));
	/*QRadialGradient gradient(width() / 2, height() / 2, width());
	gradient.setColorAt(0, gameColors->color(8));
	gradient.setColorAt(1, gameColors->color(8).darker(200));
	painter.setBrush(gradient);*/
	//painter.setPen(Qt::SolidLine);
	painter.setPen(gameColors->color(9));

	//fill background if needed
	if(scenarioPaintingNeeded)
	{
		painter.drawRect(0, 0, width(), height());
		painter.drawPicture(0, 0, scenarioBackground);
		//painter.drawImage(0, 0, scenarioBackground);
	}

	QSize size;
	size.setHeight(initialHeight);
	size.setWidth(initialWidth);
	//if(gameMode == tubularix || gameMode == invertedTubularix)
		size.scale(width(), height(), Qt::KeepAspectRatio);
	//else
		//size.scale(width(), height(), Qt::IgnoreAspectRatio);
	painter.translate(qreal(width()) / 2.0 - qreal(size.width()) / 2.0,
		qreal(height()) / 2.0 - qreal(size.height()) / 2.0);
	painter.scale(qreal(size.width()) / qreal(initialWidth),
		qreal(size.height()) / qreal(initialHeight));

	if(antialias && (gameMode == tubularix || gameMode == invertedTubularix))
		painter.setRenderHint(QPainter::Antialiasing);

	/*
	frameCount++;
	painter.drawRect(55, 780, 40, 30);
	painter.drawText(65, 800, QString::number(frameCount));
	*/

	if(!scenarioPaintingNeeded && !scenarioChangesPaintingNeeded)
	{
		if(showGhostPiece && piecePaintingNeeded)
			paintGhostPiece(painter);
		if(piecePaintingNeeded)
			paintPiece(painter);
		paintShadow(painter);
	}
	else
	{
		painter.save();
		
		if(gameMode == tubularix || gameMode == invertedTubularix)
			painter.drawEllipse(QPointF(qreal(initialWidth) / 2.0, qreal(initialHeight) / 2.0), 40, 40);

		paintScenario(painter);
		paintNextPiece(painter);
		if(showGhostPiece && piecePaintingNeeded)
			paintGhostPiece(painter);
		if(piecePaintingNeeded)
			paintPiece(painter);
		paintShadow(painter);
		paintTextInfo(painter);
		painter.restore();
	}

	QWidget::paintEvent(event);
}

void GameBoard::mouseDoubleClickEvent(QMouseEvent *event)
{
	//change full screen mode if no mouse control enabled
	if(mouseControlType == noMouseControl && event->button() == Qt::LeftButton)
		emit fullScreenChanged();

	QWidget::mouseDoubleClickEvent(event);
}

void GameBoard::mousePressEvent(QMouseEvent *event)
{
	if(!pieceTimer->isActive())
		return;

	if(mouseControlType == permanentMouseControl || mouseControlType == mouseWheelControl
		|| mouseControlType == linealMouseControl)
	{
		if(event->button() == Qt::LeftButton)
			dropPiece();
		else if(event->button() == Qt::RightButton)
			rotatePieceCounterClockwise();
	}
	else if(mouseControlType == mouseControl)
	{
		//grab the piece if clicked over a column that contains it and set the column of
		//the piece where it was clicked
		if(event->button() == Qt::LeftButton)
		{
			int col = cursorAtColumn(event->pos());
			piecePosClicked = -1;
			for(int i = currentPiece->firstLeft(); piecePosClicked == -1
				&& i <= currentPiece->firstRight(); i++)
			{
				int aux = currentPiece->posX() + i;
				if(aux >= columns)
					aux -= columns;
				if(col == aux)
				{
					pieceGrabbed = true;
					piecePosClicked = i;
					if(piecePosClicked >= columns)
						piecePosClicked -= columns;
				}
			}
		}

		else if(event->button() == Qt::RightButton)
			rotatePieceCounterClockwise();
		else if(event->button() == Qt::MidButton)
			dropPiece();
	}

	//QWidget::mousePressEvent(event);
}

void GameBoard::mouseReleaseEvent(QMouseEvent *event)
{
	if(mouseControlType == mouseControl && event->button() == Qt::LeftButton && pieceGrabbed)
		pieceGrabbed = false;
}

void GameBoard::mouseMoveEvent(QMouseEvent *event)
{
	//main toolbar
	if(mouseControlType == noMouseControl || mouseControlType == mouseWheelControl)
	{
		if(event->pos().y() <= 40)
			emit mouseMovedTop();
		else
			emit mouseMovedBottom();
	}

	//show or hide cursor depending on mouse control type
	if(mouseControlType == noMouseControl)
	{
		setCursor(QCursor(Qt::ArrowCursor));
		cursorTimer->start(500);
	}
	else if((mouseControlType == permanentMouseControl || mouseControlType == mouseControl)
		&& cursor().shape() != Qt::ArrowCursor)
		setCursor(QCursor(Qt::ArrowCursor));
	else if((mouseControlType == mouseWheelControl || mouseControlType == linealMouseControl)
		&& cursor().shape() != Qt::BlankCursor)
		hideCursor();

	if(!pieceTimer->isActive())
		return;

	if(mouseControlType == permanentMouseControl)
	{
		int currentPos = currentPiece->posX() + 1;
		if(currentPos >= columns)
			currentPos -= columns;
		int col = cursorAtColumn(event->pos());
		if(col != currentPos)
		{
			if(gameMode >= tubularix && gameMode <= invertedTetrisWithoutWalls)
			{
				if(col - 1 >= 0)
					moveToCol(col - 1);
				else
					moveToCol(col - 1 + columns);
			}
			else if(gameMode == tetris || gameMode == invertedTetris)
				moveToCol(col - 1);
		}
	}
	else if(mouseControlType == mouseControl && pieceGrabbed && event->buttons() & Qt::LeftButton)
	{
		int currentPos = currentPiece->posX() + piecePosClicked;
		if(currentPos >= columns)
			currentPos -= columns;
		int col = cursorAtColumn(event->pos());
		if(col != currentPos)
		{
			if(gameMode >= tubularix && gameMode <= invertedTetrisWithoutWalls)
			{
				if(col - piecePosClicked >= 0)
					moveToCol(col - piecePosClicked);
				else
					moveToCol(col - piecePosClicked + columns);
			}
			else if(gameMode == tetris || gameMode == invertedTetris)
				moveToCol(col - piecePosClicked);
		}
	}
	else if(mouseControlType == linealMouseControl)
	{
		//grabMouse();
		if(event->pos().x() > width() - 2)
			cursor().setPos(cursor().pos().x() - (width() - 2), cursor().pos().y());
		else if(event->pos().x() < 1)
			cursor().setPos(cursor().pos().x() + (width() - 2), cursor().pos().y());

		int currentPos = currentPiece->posX() + 1;
		if(currentPos >= columns)
			currentPos -= columns;

		int col = qreal(event->pos().x()) / qreal(width()) * columns;
		if(col >= columns)
			col -= columns;

		if(col != currentPos)
		{
			if(col - 1 >= 0)
				moveToCol(col - 1);
			else
				moveToCol(col - 1 + columns);
		}
	}

	QWidget::mouseMoveEvent(event);
}

void GameBoard::wheelEvent(QWheelEvent *event)
{
	if(!pieceTimer->isActive())
		return;

	if(mouseControlType == permanentMouseControl || mouseControlType == linealMouseControl)
	{
		if(event->delta() < 0)
			moveDown();
	}
	else if(mouseControlType == mouseWheelControl)
	{
		if(event->delta() > 0)
			moveLeft();
		else if(event->delta() < 0)
			moveRight();
	}

	QWidget::wheelEvent(event);
}

void GameBoard::resizeEvent(QResizeEvent *event)
{
	fullRepaint(true);

	QWidget::resizeEvent(event);
}
