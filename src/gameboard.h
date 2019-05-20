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

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "gamecolors.h"
#include "piece.h"
#include <QList>
#include <QPicture>
//#include <QImage>
#include <QTime>
#include <QWidget>

class GameBoard : public QWidget
{
Q_OBJECT
public:
	GameBoard(QWidget *parent = 0);

	int gameState() const {return gState;}
	void setGameState(int state = 0) {gState = state;}

	bool getNetworkedGameboard() {return networkedGameboard;}
	void setNetworkedGameboard(bool value);

	void generatePieces();
	void updateScenario(bool deletePiece = false);

	bool moveLeft();
	bool moveRight();
	bool moveToCol(int col);
	bool moveDown();
	void rotatePieceClockwise();
	void rotatePieceCounterClockwise();
	void speedUp();
	void restoreSpeed();
	void dropPiece();

	bool checkCollisions(int x, int y);
	bool checkPreviousCollisions(int x, int y);
	void checkLines();
	void levelUp();
	void calculateScore(int lines, int fallHeight);

	int calculateFallPosition(int x, int y);
	int calculatePreviousFallPosition(int x, int y);
	int calculateEmptySquaresUnderDroppedPiece(int x, int y);

	int score() const {return currentScore;}
	int level() const {return currentLevel;}
	int lines() const {return destroyedLines;}
	int totalRows() const {return rows;}
	int totalColumns() const {return columns;}

	//the rule about not naming get[Property] to the getters is broken here
	//it would be better to rename all properties and methods as they are elsewhere
	Piece *piece() {return currentPiece;}
	void setPiece(Piece *newPiece) {currentPiece = newPiece;}
	Piece *getNextPiece() {return nextPiece;}
	void setNextPiece(Piece *newPiece) {nextPiece = newPiece;}
	QList <char> getScenario() const {return scenario;}
	void setScenario(QList <char> newScenario) {scenario = newScenario;}
	QList <char> getScenarioChanges() const {return scenarioChanges;}
	void setScenarioChanges(QList <char> newScenarioChanges) {scenarioChanges = newScenarioChanges;}
	GameColors *getColors() {return gameColors;}
	void setColors(GameColors *newColors) {gameColors = newColors;}
	int getCurrentLevel() {return currentLevel;}
	void setCurrentLevel(int level) {currentLevel = level;}
	long getCurrentScore() {return currentScore;}
	void setCurrentScore(long score) {currentScore = score;}
	long getTotalGames() {return totalGames;}
	void setTotalGames(long games) {totalGames = games;}
	long getTotalScore() {return totalScore;}
	void setTotalScore(long score) {totalScore = score;}
	bool isScenarioPaintingNeeded() {return scenarioPaintingNeeded;}
	void setScenarioPaintingNeeded(bool value = true) {scenarioPaintingNeeded = value;}
	bool isScenarioChangesPaintingNeeded() {return scenarioChangesPaintingNeeded;}
	void setScenarioChangesPaintingNeeded(bool value = true) {scenarioChangesPaintingNeeded = value;}
	bool isPiecePaintingNeeded() {return piecePaintingNeeded;}
	void setPiecePaintingNeeded(bool value = true) {piecePaintingNeeded = value;}

public slots:
	void setAntialiasing(bool value = false) {antialias = value; fullRepaint(true);}
	void setGameType(int value = 1);
	void setGameMode(int value = 1);
	void setStartLevel(int value = 1) {startLevel = value;}
	void setShowMap(bool value = false) {showMap = value; fullRepaint(true);}
	void setPiecesAppearGradually(bool value = false) {piecesAppearGradually = value; fullRepaint();}
	void setShowGhostPiece(bool value = false) {showGhostPiece = value; fullRepaint();}
	void setMouseControlType(QAction *action);
	void setGameColors(GameColors *cols);

	void setRows(int value = 18) {rows = value; fullRepaint(true);}
	void setColumns(int value = 12) {columns = value; fullRepaint(true);}

	void addTotalGames(int g = 1) {totalGames += g; fullRepaint();}
	void addTotalScore(int s = 0) {totalScore += s; fullRepaint();}

	void startGame(bool repaint = true);
	void stopGame();
	void pauseGame(bool pause = true);

	void fullRepaint(bool createBackground = false);
	void showMessage(const QString &text, const QString &subText = "");

	void increaseLinesToAdd(int lines) {linesToAdd += float(lines / 2.0);}

	QList <int> nextPiecesList() {return nextPieces;}
	void setNextPiecesList(QList <int> pieces);
	void addNextPiece(int);

	void setRotationStyle(int style = 0);

private:
	enum {initialWidth = /*1016*/835, initialHeight = 835, FAST_SPEED = 25};
	enum {noMouseControl = 0, permanentMouseControl, mouseControl, mouseWheelControl,
		linealMouseControl}; //mouse control
	enum {singlePlayer = 1, twoPlayers, networkServer,	networkClient}; //game type
	enum {tubularix = 1, invertedTubularix, tetrisWithoutWalls, invertedTetrisWithoutWalls,
		tetris, invertedTetris}; //game mode
	enum {stopped = 0, started, paused, showingMessage}; //game state

	//int frameCount;

	int rows;
	int columns;

	bool antialias;
	bool showMap;
	bool piecesAppearGradually;
	bool showGhostPiece;
	int gameType;
	int gameMode;
	int startLevel;
	GameColors *gameColors;

	int gState; //0: stopped, 1: started, 2: paused, 3: showing message
	int timeInterval;
	int timeElapsed;
	int destroyedLines;
	int destroyLinesStep;
	float linesToAdd;
	int currentLevel;
	long currentScore;
	long totalGames;
	long totalScore;
	int fastFallHeight;
	bool fast;
	QList <char> scenario;
	QList <char> scenarioChanges;
	QPicture scenarioBackground;
	//QImage scenarioBackground;
	Piece *currentPiece;
	Piece *nextPiece;
	int fullLinesCounter;
	int fullLines[4];
	QTimer *pieceTimer;
	QTimer *animTimer;
	QTimer *cursorTimer;
	QTime time;

	bool piecePaintingNeeded;
	bool scenarioPaintingNeeded;
	bool scenarioChangesPaintingNeeded;

	bool networkedGameboard;

	QString message;
	QString subMessage;

	void destroyLines();
	void addLines();

	//void test(QPainter &painter);
	void createScenarioBackground();
	void paintScenario(QPainter &painter);
	void paintPiece(QPainter &painter);
	void paintGhostPiece(QPainter &painter);
	void paintShadow(QPainter &painter);
	void paintNextPiece(QPainter &painter);
	void paintMapSquare(QPainter &painter, int squareFirstX, int squareFirstY,
		int squareSize, int row, int col, int color);
	void paintTetrisSquare(QPainter &painter, int squareFirstX, int squareFirstY,
		int squareSize, int row, int col, int color);
	void paintTubularixSquare(QPainter &painter, qreal r1, qreal r2, int row, int col, int color = 0);
	void paintTextInfo(QPainter &painter);

	QList <int> nextPieces;

	int rotationStyle;
	int mouseControlType;
	bool pieceGrabbed;
	int piecePosClicked;
	int cursorAtColumn(QPoint pos);

	void clearScenario();

private slots:
	void gravity();
	void doAnimations();
	void hideCursor();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void resizeEvent(QResizeEvent *event);

signals:
	void gameStarted();
	void gameStopped();
	void gamePaused(bool paused);

	void linesDestroyed(int lines);

	void fullScreenChanged();
	void mouseMovedTop();
	void mouseMovedBottom();
	void paintEventActivated();

	void nextPieceNeeded();
};

#endif // GAMEBOARD_H
