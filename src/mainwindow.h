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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gameboard.h"
#include "gamecolors.h"
#include "gametypedialog.h"
#include <QtGui/QMainWindow>
#include <QActionGroup>
#include <QInputDialog>
#include <QList>
#include <QMessageBox>
#include <QSplitter>
#include <QTcpServer>
#include <QTcpSocket>


//forward declarations
class QCloseEvent;
class QTimer;


namespace Ui
{
	class MainWindowClass;
}


class MainWindow : public QMainWindow
{
Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	GameBoard *gameBoard;
	GameBoard *gameBoard2;

private:
	enum {noMouseControl = 0, permanentMouseControl, mouseControl, mouseWheelControl,
		linealMouseControl}; //mouse control
	enum {singlePlayer = 1, twoPlayers, networkServer,	networkClient}; //game type
	enum {tubularix = 1, invertedTubularix, tetrisWithoutWalls, invertedTetrisWithoutWalls,
		tetris, invertedTetris}; //game mode
	enum {stopped = 0, started, paused, showingMessage}; //game state
	enum networkUpdateType {gameUpdate = 0, startGameUpdate, stopGameUpdate,
		pauseGameUpdate, sendLinesUpdate};

	Ui::MainWindowClass *ui;

	QActionGroup *actionGroupMouseControl;
	int mouseControlType;

	QString playerName;
	QString language;
	int currentGameType;
	int currentGameMode;
	int startLevel;
	GameColors *gameColors;
	QRect onePlayerWindowGeometry;
	QRect twoPlayersWindowGeometry;

	int keyFirstEventList[2];
	int keyRepeatEventList[2];
	QTimer *keyFirstTimerPlayer1;
	QTimer *keyFirstTimerPlayer2;
	QTimer *keyRepeatTimerPlayer1;
	QTimer *keyRepeatTimerPlayer2;

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

	int rotationStyle;

	bool movementKeyRepetition;
	bool rotationKeyRepetition;
	int keyRepetitionDelay;
	int keyRepetitionRate;

	bool customMode;
	int rows;
	int columns;

	QTimer *aiMovementTimer1;
	QTimer *aiMovementTimer2;

	//network
	QTcpSocket tcpClient;
	QTcpServer tcpServer;
	QTcpSocket *tcpServerConnection;
	quint16 nextClientBlockSize;
	quint16 nextServerBlockSize;
	QString ip;
	int port;
	bool networkConnected;
	bool serverError;
	bool connectionInterrupted;

	QList <int> pieces;
	bool samePieces;
	bool sendDestroyedLines;

	QSplitter *splitter;

	int randSeed;

	int linesToSend;

private slots:
	void showAboutTubularix();
	void showAboutQt();
	void showControlHelp();
	void showNetworkGameHelp();
	void selectGameType();
	void showGameOptions();
	void applyGameOptions();
	void setGameType(int newGameType = 1, bool newConnectionRequested = false);
	void setGameMode(int newGameMode = 1);
	void setRows(int n = 18);
	void setColumns(int n = 12);
	void setStartLevel(int newStartLevel = 1);
	void readSettings();
	void readGameSettings();
	void readNetworkClientSettings();
	void readNetworkServerSettings();
	void writeSettings();
	void selectLanguage();
	void restoreSizeSettings();

	void changeFullScreen();
	void setNormalScreen();
	void showExtraElements();
	void hideExtraElements();

	void startGame(bool sendStreamIfNeeded = true);
	void pauseGame(bool sendStreamIfNeeded = true);
	void pauseOrResumeGame(bool sendStreamIfNeeded = true);
	void endGame(bool sendStreamIfNeeded = true);
	void showMessage(const QString &text, const QString &subText = "");
	void showHighScores(int winner = -1);
	int checkHighScore();
	QList <QStringList> readHighScores();
	void writeHighScores();

	void updateFirstKeyPressPlayer1();
	void updateFirstKeyPressPlayer2();
	void updateRepeatKeyPressPlayer1();
	void updateRepeatKeyPressPlayer2();
	void setRepeatKeyPlayer1();
	void setRepeatKeyPlayer2();

	void aiMovement1();
	void aiMovement2();
	void setAiPlayer1(bool value = false);
	void setAiPlayer2(bool value = false);

	//network
	void acceptConnection();
	void setClientConnected();
	void setServerConnected();
	void closeClient();
	void closeServer();
	void closeServerConnection();
	void configureStream(QByteArray &data, networkUpdateType updateType);
	void processStream(QDataStream &in/*, quint16 nextBlockSize*/);
	void sendStream(networkUpdateType updateType = gameUpdate);
	void sendClientStream(networkUpdateType updateType);
	void sendServerStream(networkUpdateType updateType);
	void processClientPendingStreams();
	void processServerPendingStreams();

	void addPiece();

	void setSendDestroyedLines(bool value = true, int previousGameType = -1);
	void sendLinesToAdd(int lines) {linesToSend = lines; sendStream(sendLinesUpdate);}

	void setRotationStyle(int style = 0);
	void setMouseControlType(QAction *action);

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void moveEvent (QMoveEvent *event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void changeEvent(QEvent *event);

signals:
	void gameTypeChanged(int value);
	void gameModeChanged(int value);
	void rotationStyleChanged(int value);
	void startLevelChanged(int value);
	void gameColorsChanged(GameColors *cols);
	void screenShot(QPixmap pixmap);
};

#endif // MAINWINDOW_H
