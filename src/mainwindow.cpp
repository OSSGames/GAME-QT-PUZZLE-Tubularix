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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gametypedialog.h"
#include "gameoptionsdialog.h"
#include <QDateTime>
#include <QColorDialog>
#include <QGridLayout>
#include <QLocale>
#include <QSettings>
#include <QTime>
#include <QTimer>
//#include <QLatin1String>

#include <iostream>
using namespace std;

#include <QBitArray>

//public


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindowClass)
{
	ui->setupUi(this);
	randSeed = QTime::currentTime().msec();
	qsrand(randSeed);
	currentGameType = singlePlayer;
	currentGameMode = tubularix;
	networkConnected = false;
	serverError = false;
	connectionInterrupted = false;
	nextClientBlockSize = 0;
	nextServerBlockSize = 0;
	startLevel = 1;
	linesToSend = 0;
	gameColors = new GameColors();

	gameBoard = new GameBoard(this);
	gameBoard2 = new GameBoard(this);
	//gameBoard->setMinimumWidth(width() / 8);
	//gameBoard2->setMinimumWidth(width() / 8);
	splitter = new QSplitter(Qt::Horizontal, this);
	//splitter->setFrameShape(QFrame::StyledPanel);
	splitter->setFrameShadow(QFrame::Sunken);
	//splitter->setChildrenCollapsible(false);
	splitter->setFocusPolicy(Qt::NoFocus);
	splitter->addWidget(gameBoard2);
	splitter->addWidget(gameBoard);
	QList <int> list1;
	list1<<100<<100;
	splitter->setSizes(list1);
	setCentralWidget(splitter);

	actionGroupMouseControl = new QActionGroup(this);
	actionGroupMouseControl->addAction(ui->actionNoMouseControl);
	actionGroupMouseControl->addAction(ui->actionActivatePermanentMouseControl);
	actionGroupMouseControl->addAction(ui->actionActivateMouseControl);
	actionGroupMouseControl->addAction(ui->actionActivateMouseWheelControl);
	connect(actionGroupMouseControl, SIGNAL(triggered(QAction*)), this, SLOT(setMouseControlType(QAction*)));

	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui->actionShowHighScores, SIGNAL(triggered()), this, SLOT(showHighScores()));
	connect(ui->actionSelectLanguage, SIGNAL(triggered()), this, SLOT(selectLanguage()));
	connect(ui->actionRestoreSizeSettings, SIGNAL(triggered()), this, SLOT(restoreSizeSettings()));
	connect(ui->actionAboutTubularix, SIGNAL(triggered()), this, SLOT(showAboutTubularix()));
	connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(showAboutQt()));
	connect(ui->actionControlHelp, SIGNAL(triggered()), this, SLOT(showControlHelp()));
	connect(ui->actionNetworkGameHelp, SIGNAL(triggered()), this, SLOT(showNetworkGameHelp()));
	connect(ui->actionNewGame, SIGNAL(triggered()), this, SLOT(startGame()));
	connect(ui->actionSelectGameType, SIGNAL(triggered()), this, SLOT(selectGameType()));
	connect(ui->actionGameOptions, SIGNAL(triggered()), this, SLOT(showGameOptions()));
	connect(ui->actionFullScreen, SIGNAL(triggered()), this, SLOT(changeFullScreen()));

	//Game board player 1
	connect(ui->actionActivateAntialiasing, SIGNAL(toggled(bool)),
		gameBoard, SLOT(setAntialiasing(bool)));
	connect(this, SIGNAL(gameTypeChanged(int)), gameBoard, SLOT(setGameType(int)));
	connect(this, SIGNAL(gameModeChanged(int)), gameBoard, SLOT(setGameMode(int)));
	connect(this, SIGNAL(startLevelChanged(int)), gameBoard, SLOT(setStartLevel(int)));
	connect(ui->actionShowMap, SIGNAL(toggled(bool)), gameBoard, SLOT(setShowMap(bool)));
	connect(ui->actionPiecesAppearingGradually, SIGNAL(toggled(bool)),
		gameBoard, SLOT(setPiecesAppearGradually(bool)));
	connect(ui->actionShowGhostPiece, SIGNAL(toggled(bool)), gameBoard, SLOT(setShowGhostPiece(bool)));
	connect(actionGroupMouseControl, SIGNAL(triggered(QAction*)),
		gameBoard, SLOT(setMouseControlType(QAction*)));
	connect(this, SIGNAL(rotationStyleChanged(int)), gameBoard, SLOT(setRotationStyle(int)));
	connect(this, SIGNAL(gameColorsChanged(GameColors*)), gameBoard, SLOT(setGameColors(GameColors*)));

	connect(gameBoard, SIGNAL(fullScreenChanged()), this, SLOT(changeFullScreen()));
	connect(gameBoard, SIGNAL(mouseMovedTop()), this, SLOT(showExtraElements()));
	connect(gameBoard, SIGNAL(mouseMovedBottom()), this, SLOT(hideExtraElements()));

	connect(gameBoard, SIGNAL(gameStopped()), this, SLOT(endGame()));
	connect(gameBoard, SIGNAL(nextPieceNeeded()), this, SLOT(addPiece()));

	//Game board player 2
	connect(ui->actionActivateAntialiasing, SIGNAL(toggled(bool)),
		gameBoard2, SLOT(setAntialiasing(bool)));
	connect(this, SIGNAL(gameTypeChanged(int)), gameBoard2, SLOT(setGameType(int)));
	connect(this, SIGNAL(gameModeChanged(int)), gameBoard2, SLOT(setGameMode(int)));
	connect(this, SIGNAL(startLevelChanged(int)), gameBoard2, SLOT(setStartLevel(int)));
	connect(ui->actionShowMap, SIGNAL(toggled(bool)), gameBoard2, SLOT(setShowMap(bool)));
	connect(ui->actionPiecesAppearingGradually, SIGNAL(toggled(bool)),
		gameBoard2, SLOT(setPiecesAppearGradually(bool)));
	connect(ui->actionShowGhostPiece, SIGNAL(toggled(bool)), gameBoard2, SLOT(setShowGhostPiece(bool)));
	connect(actionGroupMouseControl, SIGNAL(triggered(QAction*)),
		gameBoard2, SLOT(setMouseControlType(QAction*)));
	connect(this, SIGNAL(rotationStyleChanged(int)), gameBoard2, SLOT(setRotationStyle(int)));
	connect(this, SIGNAL(gameColorsChanged(GameColors*)), gameBoard2, SLOT(setGameColors(GameColors*)));

	connect(gameBoard2, SIGNAL(fullScreenChanged()), this, SLOT(changeFullScreen()));
	connect(gameBoard2, SIGNAL(mouseMovedTop()), this, SLOT(showExtraElements()));
	connect(gameBoard2, SIGNAL(mouseMovedBottom()), this, SLOT(hideExtraElements()));

	connect(gameBoard2, SIGNAL(gameStopped()), this, SLOT(endGame()));
	connect(gameBoard2, SIGNAL(nextPieceNeeded()), this, SLOT(addPiece()));

	readSettings();

	//key controllers
	keyFirstTimerPlayer1 = new QTimer(this);
	keyFirstTimerPlayer2 = new QTimer(this);
	connect(keyFirstTimerPlayer1, SIGNAL(timeout()), this, SLOT(setRepeatKeyPlayer1()));
	connect(keyFirstTimerPlayer2, SIGNAL(timeout()), this, SLOT(setRepeatKeyPlayer2()));
	keyFirstTimerPlayer1->setInterval(keyRepetitionDelay);
	keyFirstTimerPlayer2->setInterval(keyRepetitionDelay);
	keyRepeatTimerPlayer1 = new QTimer(this);
	keyRepeatTimerPlayer2 = new QTimer(this);
	connect(keyRepeatTimerPlayer1, SIGNAL(timeout()), this, SLOT(updateRepeatKeyPressPlayer1()));
	connect(keyRepeatTimerPlayer2, SIGNAL(timeout()), this, SLOT(updateRepeatKeyPressPlayer2()));
	keyRepeatTimerPlayer1->setInterval(1000 / keyRepetitionRate);
	keyRepeatTimerPlayer2->setInterval(1000 / keyRepetitionRate);

	//ai movement
	aiMovementTimer1 = new QTimer(this);
	connect(aiMovementTimer1, SIGNAL(timeout()), this, SLOT(aiMovement1()));
	aiMovementTimer1->setInterval(1000 / 8);
	aiMovementTimer2 = new QTimer(this);
	connect(aiMovementTimer2, SIGNAL(timeout()), this, SLOT(aiMovement2()));
	aiMovementTimer2->setInterval(1000 / 8);

	connect(ui->actionActivateAiPlayer1, SIGNAL(toggled(bool)), this, SLOT(setAiPlayer1(bool)));
	connect(ui->actionActivateAiPlayer2, SIGNAL(toggled(bool)), this, SLOT(setAiPlayer2(bool)));

	startGame();

	//setFocus();
	setNormalScreen();
}

MainWindow::~MainWindow()
{
	delete ui;
}


//private slots


void MainWindow::showAboutTubularix()
{
	pauseGame();
	QMessageBox::information(this, tr("About ") + " tubularix 0.5.1.7",
		tr("Tubularix is a game similar to Tetris seen from "
		"a tubular perspective. Enjoy it and send comments, bug reports, or whatever you want to "
		"miguelescudero@users.sourceforge.net.") + "\n\n" +
		tr("You can find more info about the game in the README file inside the game's dir, or you can go "
		"to the project page http://tubularix.sourceforge.net to find info, updates, etc.") + "\n\n" +
		tr("Credits:") + "\n" +
		tr("Developer:") + " Miguel Escudero" + "\n" +
		tr("Czech translation:") + " Pavel Fric http://fripohled.blogspot.com/" + "\n" +
		tr("Spanish translation:") + " Miguel Escudero" + "\n" +
		tr("Galician translation:") + " Miguel Escudero" + "\n");
}

void MainWindow::showAboutQt()
{
	pauseGame();
	qApp->aboutQt();
}

void MainWindow::showControlHelp()
{
	pauseGame();
	QMessageBox::information(this, tr("Control help"),
		tr("Keyboard:") + "\n\n" +
		tr("Main: P to pause, R to restart game, F to switch between normal mode/full screen "
		"and ESC to exit full screen mode.") + "\n\n" +
		tr("Player 1 (right player): left and right arrows to move, down arrow to move "
		"down faster, B to drop the piece, up arrow to rotate piece counter-clockwise and space to rotate "
		"clockwise.") + "\n\n" +
		tr("Player 2 (left player): A and D to move, S to move down faster, X to drop "
		"the piece, W to rotate piece counter-clockwise and Q to rotate clockwise.") + "\n\n"  +
		tr("Mouse:") + "\n\n" +
		tr("There are three types of mouse control."
		"They can be activated in Options->Activate mouse movement.") + "\n\n" +
		tr("Permanent mouse control: the piece is moved with the mouse cursor, dropped with the left button, "
		"moved down with the mouse wheel, and rotated with the right button.") + "\n\n" +
		tr("Mouse control: the piece is moved with the mouse cursor "
		"(but first it must be grabbed with left button), "
		"dropped with the middle button and rotated with the right button.") + "\n\n" +
		tr("Mouse wheel control: the piece is moved with the mouse wheel, "
		"dropped with the left button and rotated with the right button."));
}

void MainWindow::showNetworkGameHelp()
{
	pauseGame();
	QMessageBox::information(this, tr("Network game help"),
		tr("You can play a two player game over a local network or internet.") + "\n\n" +
		tr("First, the player who will act as the server will have to create the game opening the game "
		"type dialog, selecting create network game (server) and specifying the port that will be used "
		"for the game. That port should be opened in the firewall or nat of your system before you can "
		"start a game as a server.") + "\n\n" +
		tr("Once the server is ready, the second player should open the game type dialog, "
		"select connect to network game (client), enter the server's IP address and port, "
		"accept game type settings, wait a few seconds and hopefully the game will start."));
}

void MainWindow::selectGameType()
{
	if(isFullScreen())
		setNormalScreen();

	pauseGame();

	GameTypeDialog dialog(this, currentGameType, currentGameMode, customMode, rows, columns, startLevel,
		samePieces, sendDestroyedLines);
	if(dialog.exec())
	{
		if(dialog.customMode())
		{
			customMode = true;
			rows = dialog.rows();
			columns = dialog.columns();
		}
		else
		{
			customMode = false;
			switch(dialog.selectedGameMode())
			{
			case tubularix:
			case invertedTubularix:
				rows = 18;
				columns = 12;
				break;
			case tetrisWithoutWalls:
			case invertedTetrisWithoutWalls:
			case tetris:
			case invertedTetris:
				rows = 23;
				columns = 10;
				break;
			}
		}
		setRows(rows);
		setColumns(columns);
		setStartLevel(dialog.startLevel());
		int previousGameType = currentGameType;
		setGameType(dialog.selectedGameType(), dialog.newConnectionRequested());
		setGameMode(dialog.selectedGameMode());
		samePieces = dialog.samePieces();
		setSendDestroyedLines(dialog.sendDestroyedLines(), previousGameType);

		startGame();
	}
}

void MainWindow::showGameOptions()
{
	//pauseGame();

	GameOptionsDialog dialog(this);
	connect(&dialog, SIGNAL(changesApplied()), this, SLOT(applyGameOptions()));
	if(dialog.exec())
		applyGameOptions();
	disconnect(&dialog, SIGNAL(changesApplied()), this, SLOT(applyGameOptions()));
}

void MainWindow::applyGameOptions()
{
	readGameSettings();

	//Apply some key repetition settings
	keyFirstTimerPlayer1->setInterval(keyRepetitionDelay);
	keyFirstTimerPlayer2->setInterval(keyRepetitionDelay);
	keyRepeatTimerPlayer1->setInterval(1000 / keyRepetitionRate);
	keyRepeatTimerPlayer2->setInterval(1000 / keyRepetitionRate);
}

void MainWindow::setGameType(int newGameType, bool newConnectionRequested)
{
	if(currentGameType == singlePlayer)
		onePlayerWindowGeometry = geometry();
	else if(currentGameType == twoPlayers || currentGameType == networkServer ||
		currentGameType == networkClient)
		twoPlayersWindowGeometry = geometry();

	int previousGameType = currentGameType;
	currentGameType = newGameType;
	emit gameTypeChanged(newGameType);

	//close previous network connections if needed
	if(previousGameType == networkServer && (newGameType != networkServer || newConnectionRequested))
	{
		if(networkConnected)
			closeServerConnection();
		else
			closeServer();
	}
	if(previousGameType == networkClient && (newGameType != networkClient || newConnectionRequested))
		closeClient();

	//enable/disable ai activation for player 2
	if(newGameType == twoPlayers)
	{
		ui->actionActivateAiPlayer2->setEnabled(true);
	}
	else
	{
		ui->actionActivateAiPlayer2->setChecked(false);
		ui->actionActivateAiPlayer2->setEnabled(false);
	}

	if(newGameType == singlePlayer)
	{
		setGeometry(onePlayerWindowGeometry);
		gameBoard2->setNetworkedGameboard(false);
	}
	else if(newGameType == twoPlayers)
	{
		setGeometry(twoPlayersWindowGeometry);
		gameBoard2->setNetworkedGameboard(false);
	}
	else if(newGameType == networkServer)
	{
		setGeometry(twoPlayersWindowGeometry);
		gameBoard2->setNetworkedGameboard(true);
		if(previousGameType != networkServer || newConnectionRequested)
			readNetworkServerSettings();
	}
	else if(newGameType == networkClient)
	{
		setGeometry(twoPlayersWindowGeometry);
		gameBoard2->setNetworkedGameboard(true);
		if(previousGameType != networkClient || newConnectionRequested)
			readNetworkClientSettings();
	}
}

void MainWindow::setGameMode(int newGameMode)
{
	//if(currentGameMode == newGameMode)
		//return;

	currentGameMode = newGameMode;
	emit gameModeChanged(newGameMode);
}

void MainWindow::setRows(int n)
{
	gameBoard->setRows(n);
	gameBoard2->setRows(n);
}

void MainWindow::setColumns(int n)
{
	gameBoard->setColumns(n);
	gameBoard2->setColumns(n);
}

void MainWindow::setStartLevel(int newStartLevel)
{
	startLevel = newStartLevel;
	emit startLevelChanged(newStartLevel);
}

void MainWindow::readSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	ui->actionActivateAntialiasing->setChecked(settings.value("antialiasing", true).toBool());
	ui->actionShowMap->setChecked(settings.value("showMap", true).toBool());
	ui->actionPiecesAppearingGradually->setChecked(settings.value("piecesAppearingGradually", true).toBool());
	ui->actionShowGhostPiece->setChecked(settings.value("showGhostPiece", true).toBool());
	setGameType(settings.value("gameType", 1).toInt());
	setGameMode(settings.value("gameMode", tubularix).toInt());

	//Mouse control
	mouseControlType = settings.value("mouseControlType", 0).toInt();
	if(mouseControlType < 0 || mouseControlType > 3)
		mouseControlType = 0;
	actionGroupMouseControl->actions()[mouseControlType]->setChecked(true);
	gameBoard->setMouseControlType(actionGroupMouseControl->actions()[mouseControlType]);
	gameBoard2->setMouseControlType(actionGroupMouseControl->actions()[mouseControlType]);

	//Custom game settings
	customMode = settings.value("customMode", false).toBool();
	rows = settings.value("rows", 18).toInt();
	columns = settings.value("columns", 12).toInt();
	setRows(rows);
	setColumns(columns);

	samePieces = settings.value("samePieces", true).toBool();
	setSendDestroyedLines(settings.value("sendDestroyedLines", true).toBool());

	setStartLevel(settings.value("startLevel", 1).toInt());

	onePlayerWindowGeometry = settings.value("onePlayerWindowGeometry", QRect(40, 80, 530, 580)).toRect();
	twoPlayersWindowGeometry = settings.value("twoPlayersWindowGeometry", QRect(40, 80, 780, 460)).toRect();

	//Little fix to solve bad size settings in version 0.4.0.7 that caused problems at least on windows xp
	if(onePlayerWindowGeometry.x() == 0 && onePlayerWindowGeometry.y() == 0)
		onePlayerWindowGeometry.setRect(40, 80, 530, 580);
	if(twoPlayersWindowGeometry.x() == 0 && twoPlayersWindowGeometry.y() == 0)
		twoPlayersWindowGeometry.setRect(40, 80, 780, 460);

	if(currentGameType == singlePlayer)
		setGeometry(onePlayerWindowGeometry);
	else if(currentGameType == twoPlayers || currentGameType == networkServer ||
		currentGameType == networkClient)
		setGeometry(twoPlayersWindowGeometry);
	settings.endGroup();

	settings.beginGroup("General");
	language = settings.value("language", QLocale::system().name()).toString();
	settings.endGroup();

	readGameSettings();
}

void MainWindow::readGameSettings()
{
	QSettings settings;
	settings.beginGroup("GameOptions");

	//Controls
	pauseKey = settings.value("pauseCode", Qt::Key_P).toInt();
	restartKey = settings.value("restartCode", Qt::Key_R).toInt();
	left1Key = settings.value("left1Code", Qt::Key_Left).toInt();
	right1Key = settings.value("rigt1Code", Qt::Key_Right).toInt();
	down1Key = settings.value("down1Code", Qt::Key_Down).toInt();
	drop1Key = settings.value("drop1Code", Qt::Key_B).toInt();
	rotateCC1Key = settings.value("rotateCC1Code", Qt::Key_Up).toInt();
	rotateC1Key = settings.value("rotateC1Code", Qt::Key_Space).toInt();
	left2Key = settings.value("left2Code", Qt::Key_A).toInt();
	right2Key = settings.value("right2Code", Qt::Key_D).toInt();
	down2Key = settings.value("down2Code", Qt::Key_S).toInt();
	drop2Key = settings.value("drop2Code", Qt::Key_X).toInt();
	rotateCC2Key = settings.value("rotateCC2Code", Qt::Key_W).toInt();
	rotateC2Key = settings.value("rotateC2Code", Qt::Key_Q).toInt();

	//Rotation style
	setRotationStyle(settings.value("rotationStyle", 0).toInt());

	//Key repetition
	movementKeyRepetition = settings.value("activateMovementKeyRepetition", true).toBool();
	rotationKeyRepetition = settings.value("activateRotationKeyRepetition", false).toBool();
	keyRepetitionDelay = settings.value("repetitionDelay", 205).toInt();
	keyRepetitionRate = settings.value("repetitionRate", 25).toInt();
	settings.endGroup();

	settings.beginGroup("Colors");
	gameColors->setColor(0, QColor(settings.value("color0", QColor(90, 90, 90).name()).toString())); //empty square
	gameColors->setColor(1, QColor(settings.value("color1", QColor(250, 70, 70).name()).toString())); //piece
	gameColors->setColor(2, QColor(settings.value("color2", QColor(60, 60, 250).name()).toString())); //piece
	gameColors->setColor(3, QColor(settings.value("color3", QColor(230, 240, 40).name()).toString())); //piece
	gameColors->setColor(4, QColor(settings.value("color4", QColor(50, 180, 240).name()).toString())); //piece
	gameColors->setColor(5, QColor(settings.value("color5", QColor(130, 20, 250).name()).toString())); //piece
	gameColors->setColor(6, QColor(settings.value("color6", QColor(250, 180, 60).name()).toString())); //piece
	gameColors->setColor(7, QColor(settings.value("color7", QColor(140, 240, 40).name()).toString())); //piece
	gameColors->setColor(8, QColor(settings.value("color8", QColor(70, 70, 70).name()).toString())); //background
	gameColors->setColor(9, QColor(settings.value("color9", QColor(0, 0, 0).name()).toString())); //grid
	gameColors->setColor(10, QColor(settings.value("color10", QColor(250, 250, 250).name()).toString())); //text
	gameColors->setColor(11, QColor(settings.value("color11", QColor(10, 10, 10).name()).toString())); //text background
	gameColors->setColor(12, QColor(settings.value("color12", QColor(50, 50, 50).name()).toString())); //piece shadow 1
	gameColors->setColor(13, QColor(settings.value("color13", QColor(100, 100, 100).name()).toString())); //piece shadow 2

	gameColors->setBackgroundColorDegradation(settings.value("backgroundColorDegradation", 100).toInt());
	gameColors->setBackgroundColorDegradationMultiplier(
		settings.value("backgroundColorDegradationMultiplier", 0.0).toDouble());
	gameColors->setGridColorDegradation(settings.value("gridColorDegradation", 100).toInt());
	gameColors->setGridColorDegradationMultiplier(
		settings.value("gridColorDegradationMultiplier", 0.0).toDouble());
	gameColors->setPieceColorDegradation(settings.value("pieceColorDegradation", 100).toInt());
	gameColors->setPieceColorDegradationMultiplier(
		settings.value("pieceColorDegradationMultiplier", 0.0).toDouble());

	gameColors->setPreset(settings.value("preset", 1).toInt());
	settings.endGroup();

	emit gameColorsChanged(gameColors);
}

void MainWindow::readNetworkClientSettings()
{
	QSettings settings;
	settings.beginGroup("NetworkClient");
	ip = settings.value("ip", "127.0.0.1").toString();
	port = settings.value("port", 5124).toInt();
	settings.endGroup();

	connect(&tcpClient, SIGNAL(readyRead()), this, SLOT(processClientPendingStreams()));
	connect(&tcpClient, SIGNAL(connected()), this, SLOT(setClientConnected()));
	connect(&tcpClient, SIGNAL(disconnected()), this, SLOT(closeClient()));
	connect(gameBoard, SIGNAL(paintEventActivated()), this, SLOT(sendStream()));

	tcpClient.connectToHost(ip, port);
}

void MainWindow::readNetworkServerSettings()
{
	QSettings settings;
	settings.beginGroup("NetworkServer");
	//ip = settings.value("ip", "127.0.0.1").toString();
	port = settings.value("port", 5124).toInt();
	settings.endGroup();

	//if(tcpServer.listen(QHostAddress(ip), port))
	if(tcpServer.listen(QHostAddress::Any, port))
	{
		connectionInterrupted = false;
		cout<<"Server waiting for client at port "<<qPrintable(QString::number(port))<<endl;
		connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
		serverError = false;
	}
	else
	{
		cout<<"Error: server unable to connect"<<endl;
		serverError = true;
		//setGameType(networkClient);
	}
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("antialiasing", ui->actionActivateAntialiasing->isChecked());
	settings.setValue("showMap", ui->actionShowMap->isChecked());
	settings.setValue("piecesAppearingGradually", ui->actionPiecesAppearingGradually->isChecked());
	settings.setValue("showGhostPiece", ui->actionShowGhostPiece->isChecked());
	settings.setValue("gameType", currentGameType);
	settings.setValue("gameMode", currentGameMode);
	settings.setValue("mouseControlType", mouseControlType);

	//Custom game settings
	settings.setValue("customMode", customMode);
	settings.setValue("rows", rows);
	settings.setValue("columns", columns);

	settings.setValue("samePieces", samePieces);
	settings.setValue("sendDestroyedLines", sendDestroyedLines);

	settings.setValue("startLevel", startLevel);

	settings.setValue("onePlayerWindowGeometry", onePlayerWindowGeometry);
	settings.setValue("twoPlayersWindowGeometry", twoPlayersWindowGeometry);
	settings.endGroup();

	settings.beginGroup("General");
	settings.setValue("language", language);
	settings.endGroup();
}

void MainWindow::selectLanguage()
{
	pauseGame();

	bool ok;
	QStringList availableLanguages;
	availableLanguages<<tr("cs (czech)")<<tr("en (english)")<<tr("es (spanish)")<<tr("gl (galician)");

	QString selectedLanguage = QInputDialog::getItem(this, tr("Language selection"),
		tr("Select application's language (you must restart before changes will take effect)"),
		availableLanguages, 0, false, &ok);

	if(ok && !selectedLanguage.isEmpty())
	{
		language = selectedLanguage.left(2);
		QMessageBox::information(this, tr("Language selection"),
			tr("You must restart the application before changes will take effect"));
	}
}

void MainWindow::restoreSizeSettings()
{
	if(isFullScreen())
		setNormalScreen();

	if(currentGameType == singlePlayer)
		setGeometry(40, 80, 530, 580);
	else if(currentGameType == twoPlayers)
		setGeometry(40, 80, 780, 460);
}

void MainWindow::changeFullScreen()
{
	if(isFullScreen())
		setNormalScreen();
	else if(isMaximized())
	{
		showNormal();
		showFullScreen();
		splitter->setFrameShape(QFrame::NoFrame);
		hideExtraElements();
	}
	else
	{
		showFullScreen();
		splitter->setFrameShape(QFrame::NoFrame);
		hideExtraElements();
	}
}

void MainWindow::setNormalScreen()
{
	showNormal();
	splitter->setFrameShape(QFrame::StyledPanel);
	showExtraElements();
}

void MainWindow::showExtraElements()
{
	if(!ui->mainToolBar->isVisible() || !ui->menuBar->isVisible())
	{
		ui->mainToolBar->show();
		ui->menuBar->show();
	}
}

void MainWindow::hideExtraElements()
{
	if(isFullScreen() && (ui->mainToolBar->isVisible() && ui->menuBar->isVisible()))
	{
		ui->mainToolBar->hide();
		ui->menuBar->hide();
	}

}

void MainWindow::startGame(bool sendStreamIfNeeded)
{
	linesToSend = 0;

	pieces.clear();
	if(samePieces)
	{
		//qsrand(1);
		if(sendStreamIfNeeded)
		{
			randSeed = QTime::currentTime().msec();
			qsrand(randSeed);
		}

		for(int i = 0; i < 10; i++)
			pieces.append(qrand() % 7);
	}
	gameBoard->setNextPiecesList(pieces);
	if(currentGameType == twoPlayers || currentGameType == networkServer || currentGameType == networkClient)
		gameBoard2->setNextPiecesList(pieces);

	keyFirstEventList[0] = -1;
	keyRepeatEventList[0] = -1;
	keyRepeatTimerPlayer1->start();
	if(currentGameType == twoPlayers)
	{
		keyFirstEventList[1] = -1;
		keyRepeatEventList[1] = -1;
		keyRepeatTimerPlayer2->start();
	}

	if(currentGameType == singlePlayer)
	{
		gameBoard->startGame();
		gameBoard2->hide();
	}
	else if(currentGameType == twoPlayers)
	{
		gameBoard->startGame();
		gameBoard2->startGame();
		gameBoard2->show();
	}
	else if(currentGameType == networkServer || currentGameType == networkClient)
	{
		if(networkConnected && sendStreamIfNeeded)
			sendStream(startGameUpdate);

		gameBoard->startGame();
		gameBoard2->startGame();
		gameBoard2->show();

		if(!networkConnected)
		{
			if(currentGameType == networkServer)
			{
				if(serverError)
				{
					showMessage(tr("Error starting server"),
						tr("Server unable to start at port ") + QString::number(port));
				}
				else if(connectionInterrupted)
				{
					showMessage(tr("Game stopped"), tr("Lost connection with client"));
				}
				else
				{
					showMessage(tr("Server connected at port ") + QString::number(port),
						tr("Wait while a client connects"));
				}
			}
			else if(currentGameType == networkClient)
			{
				if(connectionInterrupted)
				{
					showMessage(tr("Game stopped"), tr("Lost connection with server"));
				}
				else
				{
					showMessage(tr("Client waiting for server response at port ") + QString::number(port),
						tr("Try again if you're not connected in a few seconds"));
				}
			}
		}
	}
}

void MainWindow::pauseGame(bool sendStreamIfNeeded)
{
	if((currentGameType == networkServer || currentGameType == networkClient) && networkConnected
		&& sendStreamIfNeeded && gameBoard->gameState() == started)
		sendStream(pauseGameUpdate);

	//right player
	if(gameBoard->gameState() == started)
		gameBoard->pauseGame(true);

	//left player
	if((currentGameType == twoPlayers || currentGameType == networkServer ||
		currentGameType == networkClient) && gameBoard2->gameState() == started)
		gameBoard2->pauseGame(true);
}

void MainWindow::pauseOrResumeGame(bool sendStreamIfNeeded)
{
	if((currentGameType == networkServer || currentGameType == networkClient) && networkConnected
		&& sendStreamIfNeeded)
		sendStream(pauseGameUpdate);

	//right player
	if(gameBoard->gameState() == started)
		gameBoard->pauseGame(true);
	else if(gameBoard->gameState() == paused)
		gameBoard->pauseGame(false);

	//left player
	if(currentGameType == twoPlayers || currentGameType == networkServer ||
		currentGameType == networkClient)
	{
		if(gameBoard2->gameState() == started)
			gameBoard2->pauseGame(true);
		else if(gameBoard2->gameState() == paused)
			gameBoard2->pauseGame(false);
	}
}

void MainWindow::showMessage(const QString &text, const QString &subText)
{
	if(gameBoard->gameState() != stopped)
		gameBoard->stopGame();
	gameBoard->showMessage(text, subText);
	if(gameBoard2->gameState() != stopped)
		gameBoard2->stopGame();
	gameBoard2->showMessage(text, subText);
}

void MainWindow::endGame(bool sendStreamIfNeeded)
{
	if(currentGameType == singlePlayer)
	{
		keyFirstTimerPlayer1->stop();
		keyRepeatTimerPlayer1->stop();

		gameBoard->showMessage(tr("game over"), tr("press r to restart"));

		int pos = checkHighScore();
		if(pos == 0)
		{
			if(ui->actionActivateAiPlayer1->isChecked())
				QTimer::singleShot(4000, this, SLOT(startGame()));
			return;
		}

		bool ok;
		playerName = QInputDialog::getText(this, tr("High scores"),
		   tr("Enter your name"), QLineEdit::Normal, playerName, &ok);
		playerName.left(10);
		if(ok && !playerName.isEmpty())
		{
			writeHighScores();
			showHighScores(pos);
		}
	}
	else if(currentGameType == twoPlayers) //just tell who wins, don't show high scores
	{
		if(sendDestroyedLines) //the one who reaches first the top of the scenario is the one who loses
		{
			keyFirstTimerPlayer1->stop();
			keyRepeatTimerPlayer1->stop();
			keyFirstTimerPlayer2->stop();
			keyRepeatTimerPlayer2->stop();

			if(gameBoard->gameState() == stopped && gameBoard2->gameState() == started)
			{
				gameBoard2->addTotalGames(1);
				gameBoard->addTotalScore(gameBoard->score());
				gameBoard2->addTotalScore(gameBoard2->score());

				gameBoard2->stopGame();

				gameBoard->showMessage(tr("you lose!"), tr("press r to restart"));
				gameBoard2->showMessage(tr("you win!"), tr("press r to restart"));
				if(ui->actionActivateAiPlayer1->isChecked() && ui->actionActivateAiPlayer2->isChecked())
					QTimer::singleShot(4000, this, SLOT(startGame()));
			}
			else if(gameBoard2->gameState() == stopped && gameBoard->gameState() == started)
			{
				gameBoard->addTotalGames(1);
				gameBoard->addTotalScore(gameBoard->score());
				gameBoard2->addTotalScore(gameBoard2->score());

				gameBoard->stopGame();

				gameBoard2->showMessage(tr("you lose!"), tr("press r to restart"));
				gameBoard->showMessage(tr("you win!"), tr("press r to restart"));
				if(ui->actionActivateAiPlayer1->isChecked() && ui->actionActivateAiPlayer2->isChecked())
					QTimer::singleShot(4000, this, SLOT(startGame()));
			}
			/*else
			{
				QMessageBox::information(this, tr("Two player game"), tr("Draw: no one wins"));
			}*/
		}

		//the one who reaches first the top of the scenario has to wait for the other one and
		//the one with the highest score wins
		else
		{
			if(gameBoard->gameState() == stopped && gameBoard2->gameState() == started)
			{
				gameBoard->showMessage(tr("game over"), tr("wait until the other player ends"));
				return;
			}
			if(gameBoard2->gameState() == stopped && gameBoard->gameState() == started)
			{
				gameBoard2->showMessage(tr("game over"), tr("wait until the other player ends"));
				return;
			}

			keyFirstTimerPlayer1->stop();
			keyRepeatTimerPlayer1->stop();
			keyFirstTimerPlayer2->stop();
			keyRepeatTimerPlayer2->stop();

			gameBoard->addTotalScore(gameBoard->score());
			gameBoard2->addTotalScore(gameBoard2->score());

			//compare scores (gameBoard number is the oposite of Player number)
			QString message;
			if(gameBoard->score() > gameBoard2->score())
			{
				gameBoard->addTotalGames(1);
				message = tr("Right player wins with a difference of ")
							 + QString::number(gameBoard->score() - gameBoard2->score()) + tr(" points");
			}
			else if(gameBoard->score() < gameBoard2->score())
			{
				gameBoard2->addTotalGames(1);
				message = tr("Left player wins with a difference of ")
							 + QString::number(gameBoard2->score() - gameBoard->score()) + tr(" points");
			}
			else
				message = tr("Draw: no one wins");

			gameBoard->showMessage(tr("game over"), tr("press r to restart"));
			gameBoard2->showMessage(tr("game over"), tr("press r to restart"));

			//show result and exit
			QMessageBox::information(this, tr("Two player game"), message);
		}
	}
	else if((currentGameType == networkServer || currentGameType == networkClient) && networkConnected
		&& sendStreamIfNeeded)
	{
		//if(sendDestroyedLines) //the one who reaches first the top of the scenario is the one who loses
		{
			keyFirstTimerPlayer1->stop();
			keyRepeatTimerPlayer1->stop();

			if(gameBoard->gameState() == stopped && gameBoard2->gameState() == started)
			{
				//gameBoard2->addTotalGames(1);
				gameBoard->addTotalScore(gameBoard->score());
				//gameBoard2->addTotalScore(gameBoard2->score());

				gameBoard2->stopGame();

				gameBoard->showMessage(tr("you lose!"), tr("press r to restart"));
				gameBoard2->showMessage(tr("you win!"), tr("press r to restart"));
				if(ui->actionActivateAiPlayer1->isChecked() && ui->actionActivateAiPlayer2->isChecked())
					QTimer::singleShot(4000, this, SLOT(startGame()));

				sendStream(stopGameUpdate);
			}
			else if(gameBoard2->gameState() == stopped && gameBoard->gameState() == started)
			{
				gameBoard->addTotalGames(1);
				gameBoard->addTotalScore(gameBoard->score());
				//gameBoard2->addTotalScore(gameBoard2->score());

				gameBoard->stopGame();

				gameBoard2->showMessage(tr("you lose!"), tr("press r to restart"));
				gameBoard->showMessage(tr("you win!"), tr("press r to restart"));
				if(ui->actionActivateAiPlayer1->isChecked() && ui->actionActivateAiPlayer2->isChecked())
					QTimer::singleShot(4000, this, SLOT(startGame()));

				sendStream(stopGameUpdate);
			}
			/*else
			{
				QMessageBox::information(this, tr("Two player game"), tr("Draw: no one wins"));
			}*/
		}

		//the one who reaches first the top of the scenario has to wait for the other one and
		//the one with the highest score wins
		/*else
		{
			if(gameBoard->gameState() == stopped && gameBoard2->gameState() == started)
			{
				gameBoard->showMessage(tr("game over"), tr("wait until the other player ends"));
				sendStream(stopGameUpdate);
				return;
			}
			if(gameBoard2->gameState() == stopped && gameBoard->gameState() == started)
			{
				gameBoard2->showMessage(tr("game over"), tr("wait until the other player ends"));
				sendStream(stopGameUpdate);
				return;
			}

			keyFirstTimerPlayer1->stop();
			keyRepeatTimerPlayer1->stop();

			gameBoard->addTotalScore(gameBoard->score());
			//gameBoard2->addTotalScore(gameBoard2->score());

			//compare scores (gameBoard number is the oposite of Player number)
			QString message;
			if(gameBoard->score() > gameBoard2->score())
			{
				gameBoard->addTotalGames(1);
				message = tr("Right player wins with a difference of ")
							 + QString::number(gameBoard->score() - gameBoard2->score()) + tr(" points");
			}
			else if(gameBoard->score() < gameBoard2->score())
			{
				gameBoard2->addTotalGames(1);
				message = tr("Left player wins with a difference of ")
							 + QString::number(gameBoard2->score() - gameBoard->score()) + tr(" points");
			}
			else
				message = tr("Draw: no one wins");

			gameBoard->showMessage(tr("game over"), tr("press r to restart"));
			gameBoard2->showMessage(tr("game over"), tr("press r to restart"));

			//show result and exit
			QMessageBox::information(this, tr("Two player game"), message);
		}*/
	}
}

void MainWindow::showHighScores(int winner)
{
	pauseGame();

	QList<QStringList> stringList = readHighScores();
	QString message;
	message.append("<font family=\"monospace\" size=3><b>");
	message.append("<pre STYLE=\"font-family:'monospace'\"><u>Pos Score           "
		"Level   Lines     Name            Hour      Date      </u></pre>");
	for(int i = 0; i < stringList.size(); i++)
	{
		if(i == winner - 1)
			message.append("<pre><font color=red>" + QString::number(winner).rightJustified(3, ' ')
				+ " " + stringList[i].join("  ") + "</font></pre>");
		else if(!stringList.at(i).isEmpty())
			message.append("<pre>" + QString::number(i + 1).rightJustified(3, ' ')
				+ " " + stringList[i].join("  ") + "\n" + "</pre>");
	}
	message.append("</b></font>");
	QMessageBox messageBox;
	messageBox.setTextFormat(Qt::RichText);
	messageBox.setWindowTitle(tr("Tubularix - High Scores"));
	messageBox.setText(message);
	messageBox.exec();
}

int MainWindow::checkHighScore()
{
	QList<QStringList> stringList = readHighScores();

	for(int i = 0; i < 10; i++)
	{
		if(stringList.at(i).isEmpty())
			return i + 1;
		if(gameBoard->score() > stringList.at(i).at(0).toInt())
			return i + 1;
	}

	return 0;
}

QList<QStringList> MainWindow::readHighScores()
{
	QList<QStringList> stringList;

	QSettings settings;
	if(customMode)
		settings.beginGroup("HighScoresCustom" + QString::number(currentGameMode));
	else
		settings.beginGroup("HighScores" + QString::number(currentGameMode));

	for(int i = 0; i < 10; i++)
	{
		stringList.append(settings.value("score" + QString::number(i + 1), QStringList()).toStringList());
	}
	settings.endGroup();

	return stringList;
}

void MainWindow::writeHighScores()
{
	QList<QStringList> stringList = readHighScores();

	//set currentScoreData values
	QStringList currentScoreData;
	currentScoreData<<QString::number(gameBoard->score()).leftJustified(14, ' ');
	currentScoreData<<QString::number(gameBoard->level()).leftJustified(6, ' ' );
	currentScoreData<<QString::number(gameBoard->lines()).leftJustified(8, ' ' );
	currentScoreData<<playerName.leftJustified(14, ' ');
	currentScoreData<<QTime::currentTime().toString("hh:mm:ss");
	currentScoreData<<QDate::currentDate().toString("yyyy-MM-dd");

	for(int i = 0; i < 10; i++)
	{
		if(stringList.at(i).isEmpty())
		{
			stringList.replace(i, currentScoreData);
			break;
		}
		else if(currentScoreData.at(0).toInt() > stringList.at(i).at(0).toInt())
		{
			stringList.insert(i, currentScoreData);
			break;
		}
	}

	int stringListSize = 10;
	if (stringList.size() < 10)
		stringListSize = stringList.size();


	QSettings settings;
	if(customMode)
		settings.beginGroup("HighScoresCustom" + QString::number(currentGameMode));
	else
		settings.beginGroup("HighScores" + QString::number(currentGameMode));

	//that may solve some QSetting problems in OS/2
	/*
	for(int i = 0; i < stringListSize; i++)
	{
		for(int j = 0; j < stringList[i].size(); j++)
			stringList[i][j] = stringList[i][j].toLatin1();
	}
	*/

	for(int i = 0; i < stringListSize; i++)
		settings.setValue("score" + QString::number(i + 1), stringList[i]);
	settings.endGroup();
}

void MainWindow::updateFirstKeyPressPlayer1()
{
	if(gameBoard->gameState() != started || keyFirstEventList[0] == -1)
		return;

	if(keyFirstEventList[0] == left1Key)
	{
		gameBoard->moveLeft();
		if(movementKeyRepetition)
			keyFirstTimerPlayer1->start();
	}
	if(keyFirstEventList[0] == right1Key)
	{
		gameBoard->moveRight();
		if(movementKeyRepetition)
			keyFirstTimerPlayer1->start();
	}
	if(keyFirstEventList[0] == rotateCC1Key)
	{
		gameBoard->rotatePieceCounterClockwise();
		if(rotationKeyRepetition)
			keyFirstTimerPlayer1->start();
	}
	if(keyFirstEventList[0] == rotateC1Key)
	{
		gameBoard->rotatePieceClockwise();
		if(rotationKeyRepetition)
			keyFirstTimerPlayer1->start();
	}
}

void MainWindow::updateFirstKeyPressPlayer2()
{
	if(currentGameType == singlePlayer ||
		gameBoard2->gameState() != started || keyFirstEventList[1] == -1)
		return;

	if(keyFirstEventList[1] == left2Key)
	{
		gameBoard2->moveLeft();
		if(movementKeyRepetition)
			keyFirstTimerPlayer2->start();
	}
	if(keyFirstEventList[1] == right2Key)
	{
		gameBoard2->moveRight();
		if(movementKeyRepetition)
			keyFirstTimerPlayer2->start();
	}
	if(keyFirstEventList[1] == rotateCC2Key)
	{
		gameBoard2->rotatePieceCounterClockwise();
		if(rotationKeyRepetition)
			keyFirstTimerPlayer2->start();
	}
	if(keyFirstEventList[1] == rotateC2Key)
	{
		gameBoard2->rotatePieceClockwise();
		if(rotationKeyRepetition)
			keyFirstTimerPlayer2->start();
	}
}

void MainWindow::updateRepeatKeyPressPlayer1()
{
	if(gameBoard->gameState() != started ||
		keyRepeatEventList[0] == -1 || keyRepeatEventList[0] != keyFirstEventList[0])
		return;

	if(keyRepeatEventList[0] == left1Key)
	{
		if(movementKeyRepetition)
			gameBoard->moveLeft();
	}
	if(keyRepeatEventList[0] == right1Key)
	{
		if(movementKeyRepetition)
			gameBoard->moveRight();
	}
	if(keyRepeatEventList[0] == rotateCC1Key)
	{
		if(rotationKeyRepetition)
			gameBoard->rotatePieceCounterClockwise();
	}
	if(keyRepeatEventList[0] == rotateC1Key)
	{
		if(rotationKeyRepetition)
			gameBoard->rotatePieceClockwise();
	}
	
	keyFirstTimerPlayer1->stop();
}

void MainWindow::updateRepeatKeyPressPlayer2()
{
	if(currentGameType == singlePlayer || gameBoard2->gameState() != started ||
		keyRepeatEventList[1] == -1 || keyRepeatEventList[1] != keyFirstEventList[1])
		return;

	if(keyRepeatEventList[1] == left2Key)
	{
		if(movementKeyRepetition)
			gameBoard2->moveLeft();
	}
	if(keyRepeatEventList[1] == right2Key)
	{
		if(movementKeyRepetition)
			gameBoard2->moveRight();
	}
	if(keyRepeatEventList[1] == rotateCC2Key)
	{
		if(rotationKeyRepetition)
			gameBoard2->rotatePieceCounterClockwise();
	}
	if(keyRepeatEventList[1] == rotateC2Key)
	{
		if(rotationKeyRepetition)
			gameBoard2->rotatePieceClockwise();
	}
	
	keyFirstTimerPlayer2->stop();
}

void MainWindow::setRepeatKeyPlayer1()
{
	keyRepeatEventList[0] = keyFirstEventList[0];
}

void MainWindow::setRepeatKeyPlayer2()
{
	keyRepeatEventList[1] = keyFirstEventList[1];
}

void MainWindow::setAiPlayer1(bool value)
{
	if(value == true)
		aiMovementTimer1->start();
	else
		aiMovementTimer1->stop();
}

void MainWindow::setAiPlayer2(bool value)
{
	if(value == true)
		aiMovementTimer2->start();
	else
		aiMovementTimer2->stop();
}

//very bad ai movement, and absolutely messy code, but useful for test purposes
void MainWindow::aiMovement1()
{
	if(gameBoard->gameState() != started)
		return;

	//crea una lista con las posiciones más bajas y otra con las que menos cuadrados dejan libres,
	//busca coincidencias y selecciona aleatoriamente una de las posiciones si hay varias
	//igual de buenas o si no hay coincidencias
	/*
	QMap <int, int> lowPositionsDownside;
	QMap <int, int> lowPositionsUpside;
	QMap <int, int> lessEmptySquaresInmediately;
	QMap <int, int> lessEmptySquares;
	*/

	//Criterios:
	//llega más abajo
	//gameBoard->calculateFallPosition(gameBoard->piece()->posX(), gameBoard->piece()->posY())
	//	+ gameBoard->piece()->firstDown();

	//queda menos elevada
	//gameBoard->calculateFallPosition(gameBoard->piece()->posX(), gameBoard->piece()->posY())
	//  + gameBoard->piece()->firstDown();// - gameBoard->piece()->height();

	//deja menos huecos inmediantamente debajo o tal vez mejor hasta encontrar una superficie
	//crea una función similar a la otra

	//deja menos huecos debajo de ella
	//gameBoard->calculateEmptySquaresUnderDroppedPiece(gameBoard->piece()->posX(),
	//	gameBoard->piece()->posY());

	//calculate the best position
	int pos = gameBoard->calculateFallPosition(gameBoard->piece()->posX(), gameBoard->piece()->posY())
			  + gameBoard->piece()->firstDown();// - gameBoard->piece()->height();
	int col = gameBoard->piece()->posX();
	int rot = 0;
	static int emptySquares = -1;
	static int pieceFirstUp = -1;

	if(emptySquares == -1)
	{
		emptySquares = gameBoard->calculateEmptySquaresUnderDroppedPiece(gameBoard->piece()->posX(),
			gameBoard->piece()->posY());
	}
	if(pieceFirstUp == -1)
		pieceFirstUp = gameBoard->piece()->firstUp();
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < gameBoard->totalColumns(); j++)
		{
			int eS = gameBoard->calculateEmptySquaresUnderDroppedPiece(j, gameBoard->piece()->posY());
			int p = gameBoard->calculateFallPosition(j, gameBoard->piece()->posY())
				+ gameBoard->piece()->firstDown();// - gameBoard->piece()->height();
			/*if(eS < emptySquares && eS >= 0)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}
			else if(eS == emptySquares && eS >= 0 && p > pos)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}*/
			if(p > pos)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}
			if(p == pos && eS < emptySquares && eS >= 0)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}
			/*else if(p == pos && eS == emptySquares && eS >= 0 && gameBoard->piece()->firstUp() > pieceFirstUp)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}*/
		}
		gameBoard->piece()->rotateCounterClockwise();
	}

	//move the piece to the previously selected as best position
	for(int i = 0; i < rot; i++)
		gameBoard->rotatePieceCounterClockwise();

	if(gameBoard->piece()->posX() > col)
		gameBoard->moveLeft();
	else if(gameBoard->piece()->posX() < col)
		gameBoard->moveRight();
	else if(!rot)
		gameBoard->speedUp();

	if(!rot)
	{
		emptySquares = -1;
		pieceFirstUp = -1;
	}
}

//very bad ai movement, and absolutely messy code, but useful for test purposes
void MainWindow::aiMovement2()
{
	if(gameBoard2->gameState() != started)
		return;

	//crea una lista con las posiciones más bajas y otra con las que menos cuadrados dejan libres,
	//busca coincidencias y selecciona aleatoriamente una de las posiciones si hay varias
	//igual de buenas o si no hay coincidencias
	/*
	QMap <int, int> lowPositionsDownside;
	QMap <int, int> lowPositionsUpside;
	QMap <int, int> lessEmptySquaresInmediately;
	QMap <int, int> lessEmptySquares;
	*/

	//Criterios:
	//llega más abajo
	//gameBoard->calculateFallPosition(gameBoard->piece()->posX(), gameBoard->piece()->posY())
	//	+ gameBoard->piece()->firstDown();

	//queda menos elevada
	//gameBoard->calculateFallPosition(gameBoard->piece()->posX(), gameBoard->piece()->posY())
	//  + gameBoard->piece()->firstDown();// - gameBoard->piece()->height();

	//deja menos huecos inmediantamente debajo o tal vez mejor hasta encontrar una superficie
	//crea una función similar a la otra

	//deja menos huecos debajo de ella
	//gameBoard->calculateEmptySquaresUnderDroppedPiece(gameBoard->piece()->posX(),
	//	gameBoard->piece()->posY());

	//calculate the best position
	int pos = gameBoard2->calculateFallPosition(gameBoard2->piece()->posX(), gameBoard2->piece()->posY())
			  + gameBoard2->piece()->firstDown();// - gameBoard->piece()->height();
	int col = gameBoard2->piece()->posX();
	int rot = 0;
	static int emptySquares = -1;
	static int pieceFirstUp = -1;

	if(emptySquares == -1)
	{
		emptySquares = gameBoard2->calculateEmptySquaresUnderDroppedPiece(gameBoard2->piece()->posX(),
			gameBoard2->piece()->posY());
	}
	if(pieceFirstUp == -1)
		pieceFirstUp = gameBoard2->piece()->firstUp();
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < gameBoard2->totalColumns(); j++)
		{
			int eS = gameBoard2->calculateEmptySquaresUnderDroppedPiece(j, gameBoard2->piece()->posY());
			int p = gameBoard2->calculateFallPosition(j, gameBoard2->piece()->posY())
				+ gameBoard2->piece()->firstDown();// - gameBoard2->piece()->height();
			/*if(eS < emptySquares && eS >= 0)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}
			else if(eS == emptySquares && eS >= 0 && p > pos)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}*/
			if(p > pos)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}
			if(p == pos && eS < emptySquares && eS >= 0)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}
			/*else if(p == pos && eS == emptySquares && eS >= 0 && gameBoard->piece()->firstUp() > pieceFirstUp)
			{
				pos = p;
				col = j;
				rot = i;
				emptySquares = eS;
			}*/
		}
		gameBoard2->piece()->rotateCounterClockwise();
	}

	//move the piece to the previously selected as best position
	for(int i = 0; i < rot; i++)
		gameBoard2->rotatePieceCounterClockwise();

	if(gameBoard2->piece()->posX() > col)
		gameBoard2->moveLeft();
	else if(gameBoard2->piece()->posX() < col)
		gameBoard2->moveRight();
	else if(!rot)
		gameBoard2->speedUp();

	if(!rot)
	{
		emptySquares = -1;
		pieceFirstUp = -1;
	}
}

void MainWindow::acceptConnection()
{
	tcpServerConnection = tcpServer.nextPendingConnection();
	connect(tcpServerConnection, SIGNAL(readyRead()), this, SLOT(processServerPendingStreams()));
	//connect(tcpServerConnection, SIGNAL(connected()), this, SLOT(setServerConnected()));
	connect(tcpServerConnection, SIGNAL(disconnected()), this, SLOT(closeServerConnection()));
	//connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
		 //this, SLOT(displayError(QAbstractSocket::SocketError)));
	connect(gameBoard, SIGNAL(paintEventActivated()), this, SLOT(sendStream()));

	setServerConnected();
	closeServer();

	startGame();
}

void MainWindow::setClientConnected()
{
	connectionInterrupted = false;
	networkConnected = true;
	cout<<"Client connected at port "<<qPrintable(QString::number(port))<<endl;
}

void MainWindow::setServerConnected()
{
	connectionInterrupted = false;
	networkConnected = true;
	cout<<"Server connected at port "<<qPrintable(QString::number(port))<<endl;
}

void MainWindow::closeClient()
{
	disconnect(&tcpClient, SIGNAL(readyRead()), this, SLOT(processClientPendingStreams()));
	disconnect(&tcpClient, SIGNAL(connected()), this, SLOT(setClientConnected()));
	disconnect(&tcpClient, SIGNAL(disconnected()), this, SLOT(closeClient()));
	disconnect(gameBoard, SIGNAL(paintEventActivated()), this, SLOT(sendStream()));

	networkConnected = false;
	cout<<"Client disconnected"<<endl;
	tcpClient.close();

	showMessage(tr("Game stopped"), tr("Lost connection with server"));
	connectionInterrupted = true;
}

void MainWindow::closeServerConnection()
{
	disconnect(tcpServerConnection, SIGNAL(readyRead()), this, SLOT(processServerPendingStreams()));
	disconnect(tcpServerConnection, SIGNAL(connected()), this, SLOT(setServerConnected()));
	disconnect(tcpServerConnection, SIGNAL(disconnected()), this, SLOT(closeServerConnection()));
	disconnect(gameBoard, SIGNAL(paintEventActivated()), this, SLOT(sendStream()));

	networkConnected = false;
	cout<<"Server disconnected"<<endl;
	tcpServerConnection->close();

	showMessage(tr("Game stopped"), tr("Lost connection with client"));
	connectionInterrupted = true;
}

void MainWindow::closeServer()
{
	disconnect(&tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

	//networkConnected = false;
	cout<<"Server stopped wating for client"<<endl;
	tcpServer.close();
}

void MainWindow::configureStream(QByteArray &data, networkUpdateType updateType)
{
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out<<quint16(0);

	//update type
	out<<qint16(updateType);

	if(updateType == gameUpdate)
	{
		//needed paintings
		out<<gameBoard->isPiecePaintingNeeded();
		out<<gameBoard->isScenarioChangesPaintingNeeded();
		out<<gameBoard->isScenarioPaintingNeeded();

		quint16 length;

		if(gameBoard->isPiecePaintingNeeded() || gameBoard->isScenarioChangesPaintingNeeded()
			|| gameBoard->isScenarioPaintingNeeded())
		{
			//piece
			out<<qint32(gameBoard->piece()->getRotationStyle());
			out<<qint32(gameBoard->piece()->selectedPiece());
			out<<qint32(gameBoard->piece()->posX());
			out<<qint32(gameBoard->piece()->posY());
			out<<qint32(gameBoard->piece()->rotationPos());
			out<<qint32(gameBoard->piece()->previousPosX());
			out<<qint32(gameBoard->piece()->previousPosY());

			for(int i = 0; i < 4; i++)
				for(int j = 0; j < 4; j++)
					out<<qint8(gameBoard->piece()->pieceMatrix[i][j]);

			for(int i = 0; i < 4; i++)
				for(int j = 0; j < 4; j++)
					out<<qint8(gameBoard->piece()->previousPieceMatrix[i][j]);

			out<<qint32(gameBoard->getNextPiece()->selectedPiece());
		}

		if(gameBoard->isScenarioChangesPaintingNeeded() || gameBoard->isScenarioPaintingNeeded())
		{
			//scenario changes
			length = gameBoard->getScenarioChanges().count();
			out<<length;
			//send scenarioChanges as an array of bits to save bandwidth
			QBitArray scenarioChanges(length);
			for(int i = 0; i < length; i++)
				scenarioChanges.setBit(i, gameBoard->getScenarioChanges()[i]);
				//out<<qint8(gameBoard->getScenarioChanges()[i]);
			out<<scenarioChanges;

			//scenario
			length = gameBoard->getScenario().count();
			out<<length;
			for(int i = 0; i < length; i++)
				out<<qint8(gameBoard->getScenario()[i]);
		}

		//scores and such
		out<<qint32(gameBoard->getCurrentLevel());
		out<<qint32(gameBoard->getCurrentScore());
		out<<qint32(gameBoard->getTotalGames());
		out<<qint32(gameBoard->getTotalScore());

		//game states and such
		//out<<qint32(gameBoard->gameState());
	}
	else if(updateType == startGameUpdate)
	{
		out<<qint32(rows);
		out<<qint32(columns);
		out<<qint32(startLevel);
		//out<<qint32(currentGameType);
		out<<qint32(currentGameMode);
		out<<qint32(randSeed);
	}
	else if(updateType == stopGameUpdate)
	{

	}
	else if(updateType == pauseGameUpdate)
	{

	}
	else if(updateType == sendLinesUpdate)
	{
		out<<linesToSend;
		linesToSend = 0;
	}

	out.device()->seek(0);
	out<<quint16(data.size() - (int)sizeof(quint16));
}

void MainWindow::processStream(QDataStream &in/*, quint16 nextBlockSize*/)
{
	//update type
	qint16 updateType;
	in>>updateType;

	if(updateType == gameUpdate)
	{
		//needed paintings
		bool piecePaintigNeeded, scenarioChangesPaintingNeeded, scenarioPaintingNeeded;
		in>>piecePaintigNeeded;
		in>>scenarioChangesPaintingNeeded;
		in>>scenarioPaintingNeeded;

		qint8 c;
		qint32 n;
		quint16 length;

		if(piecePaintigNeeded || scenarioChangesPaintingNeeded || scenarioPaintingNeeded)
		{
			//piece
			in>>n;
			//gameBoard2->piece()->setRotationStyle(int(n));
			gameBoard2->setRotationStyle(int(n));
			in>>n;
			gameBoard2->piece()->selectPiece(int(n));
			in>>n;
			gameBoard2->piece()->setPosX(int(n));
			in>>n;
			gameBoard2->piece()->setPosY(int(n));
			in>>n;
			gameBoard2->piece()->setRotationPos(int(n));
			in>>n;
			gameBoard2->piece()->setPreviousPosX(int(n));
			in>>n;
			gameBoard2->piece()->setPreviousPosY(int(n));

			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					in>>c;
					gameBoard2->piece()->pieceMatrix[i][j] = char(c);
				}
			}

			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					in>>c;
					gameBoard2->piece()->previousPieceMatrix[i][j] = char(c);
				}
			}

			in>>n;
			gameBoard2->getNextPiece()->selectPiece(int(n));
		}

		if(scenarioChangesPaintingNeeded || scenarioPaintingNeeded)
		{
			//scenario changes
			in>>length;
			QList <char> scenarioChanges;
			QBitArray scenarioChangesAux(length);
			in>>scenarioChangesAux;
			for(int i = 0; i < length; i++)
			{
				//in>>c;
				//scenarioChanges.append(char(c));
				scenarioChanges.append(char(scenarioChangesAux[i]));
			}
			gameBoard2->setScenarioChanges(scenarioChanges);

			//scenario
			in>>length;
			QList <char> scenario;
			for(int i = 0; i < length; i++)
			{
				in>>c;
				scenario.append(char(c));
			}
			gameBoard2->setScenario(scenario);
		}

		//scores and such
		in>>n;
		gameBoard2->setCurrentLevel(int(n));
		in>>n;
		gameBoard2->setCurrentScore(long(n));
		in>>n;
		gameBoard2->setTotalGames(long(n));
		in>>n;
		gameBoard2->setTotalScore(long(n));

		//game states and such
		//in>>n;
		//gameBoard2->setGameState(n);

		gameBoard2->setPiecePaintingNeeded(piecePaintigNeeded);
		gameBoard2->setScenarioChangesPaintingNeeded(scenarioChangesPaintingNeeded);
		gameBoard2->setScenarioPaintingNeeded(scenarioPaintingNeeded);
		if(gameBoard2->gameState() == started || scenarioPaintingNeeded)
			gameBoard2->repaint();
	}
	else if(updateType == startGameUpdate)
	{
		qint32 n;
		in>>n;
		setRows(int(n));
		in>>n;
		setColumns(int(n));
		in>>n;
		setStartLevel(int(n));
		//in>>n;
		//setGameType(int(n));
		in>>n;
		setGameMode(int(n));

		in>>n;
		randSeed = int(n);
		if(samePieces)
			qsrand(randSeed);

		//gameBoard2->startGame();
		startGame(false);

	}
	else if(updateType == stopGameUpdate)
	{
		gameBoard2->stopGame();
		//stopGame(false);
	}
	else if(updateType == pauseGameUpdate)
	{
		//gameBoard2->pauseGame();
		pauseOrResumeGame(false);
	}
	else if(updateType == sendLinesUpdate)
	{
		qint32 n;
		in>>n;
		gameBoard->increaseLinesToAdd(int(n));
	}
}

void MainWindow::sendStream(networkUpdateType updateType)
{
	if(networkConnected)
	{
		if(currentGameType == networkServer)
			sendServerStream(networkUpdateType(updateType));
		else if(currentGameType == networkClient)
			sendClientStream(networkUpdateType(updateType));
	}
}

void MainWindow::sendClientStream(networkUpdateType updateType)
{
	static quint32 n = 0;
	QByteArray data;
	configureStream(data, updateType);
	tcpClient.write(data);
	cout<<"Client sends packet "<<++n<<" (updateType = "<<updateType<<"), size: ";
	cout<<quint16(data.size() - (int)sizeof(quint16))<<endl;
}

void MainWindow::sendServerStream(networkUpdateType updateType)
{
	static quint32 n = 0;
	QByteArray data;
	configureStream(data, updateType);
	tcpServerConnection->write(data);

	cout<<"Server sends packet "<<++n<<" (updateType = "<<updateType<<"), size: ";
	cout<<quint16(data.size() - (int)sizeof(quint16))<<endl;
}

void MainWindow::processClientPendingStreams()
{
	static quint32 n = 0;
	QDataStream in(&tcpClient);
	in.setVersion(QDataStream::Qt_4_4);

	while(1)
	{
		if(nextClientBlockSize == 0)
		{
			if(tcpClient.bytesAvailable() < (int)sizeof(quint16))
				break;
			in>>nextClientBlockSize;
		}

		if(tcpClient.bytesAvailable() < nextClientBlockSize)
			break;

		processStream(in/*, nextClientBlockSize*/);
		nextClientBlockSize = 0;
		cout<<"Data stream "<<++n<<" received"<<endl;
	}
}

void MainWindow::processServerPendingStreams()
{
	static quint32 n = 0;
	QDataStream in(tcpServerConnection);
	in.setVersion(QDataStream::Qt_4_4);

	while(1)
	{
		if(nextServerBlockSize == 0)
		{
			if(tcpServerConnection->bytesAvailable() < (int)sizeof(quint16))
				break;
			in>>nextServerBlockSize;
		}

		if(tcpServerConnection->bytesAvailable() < nextServerBlockSize)
			break;

		processStream(in/*, nextServerBlockSize*/);
		nextServerBlockSize = 0;
		cout<<"Data stream "<<++n<<" received"<<endl;
	}
}

void MainWindow::addPiece()
{
	if(samePieces)
	{
		pieces.append(qrand() % 7);
		gameBoard->addNextPiece(pieces.last());
		gameBoard2->addNextPiece(pieces.last());
	}
}

void MainWindow::setSendDestroyedLines(bool value, int previousGameType)
{
	//disconnect previous connections if needed
	if(previousGameType != -1 && sendDestroyedLines == true
		&& (!value || previousGameType != currentGameType))
	{
		if(previousGameType == twoPlayers)
		{
			disconnect(gameBoard, SIGNAL(linesDestroyed(int)), gameBoard2, SLOT(increaseLinesToAdd(int)));
			disconnect(gameBoard2, SIGNAL(linesDestroyed(int)), gameBoard, SLOT(increaseLinesToAdd(int)));
		}
		else if(previousGameType == networkServer || previousGameType == networkClient)
		{
			disconnect(gameBoard, SIGNAL(linesDestroyed(int)), this, SLOT(sendLinesToAdd(int)));
		}
	}

	if(value && previousGameType != currentGameType)
	{
		if(currentGameType == twoPlayers)
		{
			connect(gameBoard, SIGNAL(linesDestroyed(int)), gameBoard2, SLOT(increaseLinesToAdd(int)));
			connect(gameBoard2, SIGNAL(linesDestroyed(int)), gameBoard, SLOT(increaseLinesToAdd(int)));
		}
		else if(currentGameType == networkServer || currentGameType == networkClient)
		{
			connect(gameBoard, SIGNAL(linesDestroyed(int)), this, SLOT(sendLinesToAdd(int)));
		}
	}
	sendDestroyedLines = value;
}

void MainWindow::setRotationStyle(int style)
{
	rotationStyle = style;
	emit rotationStyleChanged(style);
}

void MainWindow::setMouseControlType(QAction *action)
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


//protected


void MainWindow::closeEvent(QCloseEvent *event)
{
	if(isFullScreen())
		setNormalScreen();
	writeSettings();

	/*if(currentGameType == networkServer)
		closeServerConnection();
	else if(currentGameType == networkClient)
		closeClient();*/

	//qApp->quit();
	event->accept();

	QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	//main controls
	//pause/continue game
	if(event->key() == pauseKey && !event->isAutoRepeat())
		pauseOrResumeGame();

	//restart game
	if(event->key() == restartKey)
		startGame();

	//switch between normal mode/fullScreen
	if(event->key() == Qt::Key_F)
		changeFullScreen();

	//escape fullScreen/maximized and show normal mode
	if(event->key() == Qt::Key_Escape)
		setNormalScreen();

	/*
	//show/hide ghost piece
	if(event->key() == Qt::Key_G)
		ui->actionShowGhostPiece->toggle();
	*/

	/*
	//ai movement for player 1 enable/disable
	if(event->key() == Qt::Key_M)
	{
		if(aiMovementTimer1->isActive())
			aiMovementTimer1->stop();
		else
			aiMovementTimer1->start();
	}
	*/

	/*
	//ai movement for player 2 enable/disable
	if(event->key() == Qt::Key_N)
	{
		if(aiMovementTimer2->isActive())
			aiMovementTimer2->stop();
		else
			aiMovementTimer2->start();
	}
	*/

	//player 1
	if(gameBoard->gameState() == started && !event->isAutoRepeat())
	{
		if(event->key() == left1Key || event->key() == right1Key || event->key() == rotateCC1Key ||
		   event->key() == rotateC1Key)
		{
			keyFirstEventList[0] = event->key();
			keyRepeatEventList[0] = -1;
			updateFirstKeyPressPlayer1();
		}
		if(event->key() == down1Key)
			gameBoard->speedUp();
		if(event->key() == drop1Key)
			gameBoard->dropPiece();

		//trick to send a line to the other player
		/*if(event->key() == Qt::Key_L && sendDestroyedLines)
			gameBoard2->increaseLinesToAdd(2);*/
	}

	//player 2
	if(currentGameType == twoPlayers && gameBoard2->gameState() == started && !event->isAutoRepeat())
	{
		if(event->key() == left2Key || event->key() == right2Key || event->key() == rotateCC2Key ||
		   event->key() == rotateC2Key)
		{
			keyFirstEventList[1] = event->key();
			keyRepeatEventList[1] = -1;
			updateFirstKeyPressPlayer2();
		}
		if(event->key() == down2Key)
			gameBoard2->speedUp();
		if(event->key() == drop2Key)
			gameBoard2->dropPiece();
	}

	QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	//player 1
	//if(gameBoard->gameState() == started)
	{
		if(event->key() == down1Key)
		{
			if(!event->isAutoRepeat())
				gameBoard->restoreSpeed();
		}
		if(event->key() == left1Key || event->key() == right1Key || event->key() == rotateCC1Key ||
		   event->key() == rotateC1Key)
		{
			if(!event->isAutoRepeat() && event->key() == keyFirstEventList[0])
			{
				keyRepeatEventList[0] = -1;
				keyFirstEventList[0] = -1;
			}
		}
	}

	//player 2
	//if(gameBoard2->gameState() == started)
	if(currentGameType == twoPlayers)
	{
		if(event->key() == down2Key)
		{
			if(!event->isAutoRepeat())
				gameBoard2->restoreSpeed();
		}
		if(event->key() == left2Key || event->key() == right2Key || event->key() == rotateCC2Key ||
		   event->key() == rotateC2Key)
		{
			if(!event->isAutoRepeat() && event->key() == keyFirstEventList[1])
			{
				keyRepeatEventList[1] = -1;
				keyFirstEventList[1] = -1;
			}
		}
	}

	QMainWindow::keyReleaseEvent(event);
}

void MainWindow::moveEvent(QMoveEvent *event)
{
	if(!isFullScreen() && !isMaximized())
	{
		if(currentGameType == singlePlayer)
			onePlayerWindowGeometry = geometry();
		else if(currentGameType == twoPlayers)
			twoPlayersWindowGeometry = geometry();
		else if(currentGameType == networkServer)
			twoPlayersWindowGeometry = geometry();
		else if(currentGameType == networkClient)
			twoPlayersWindowGeometry = geometry();
	}

	QMainWindow::moveEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	if(!isFullScreen() && !isMaximized())
	{
		if(currentGameType == singlePlayer)
			onePlayerWindowGeometry = geometry();
		else if(currentGameType == twoPlayers)
			twoPlayersWindowGeometry = geometry();
		else if(currentGameType == networkServer)
			twoPlayersWindowGeometry = geometry();
		else if(currentGameType == networkClient)
			twoPlayersWindowGeometry = geometry();
	}

	QMainWindow::resizeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
	gameBoard->fullRepaint(true);
	gameBoard->repaint();
	if(currentGameType == twoPlayers || currentGameType == networkServer || currentGameType == networkClient)
	{
		gameBoard2->fullRepaint(true);
		gameBoard2->repaint();
	}

	QMainWindow::showEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
	switch (event->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
