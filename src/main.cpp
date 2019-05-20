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
#include <QtGui/QApplication>
#include <QLocale>
#include <QSettings>
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("tubularix");
	QCoreApplication::setOrganizationDomain("tubularix.sourceforge.net");
	QCoreApplication::setApplicationName("tubularix");

	//Read settings to establish game's language
	QSettings settings;
	settings.beginGroup("General");

	QTranslator translator;
	if(!translator.load("tubularix_" + settings.value("language", QLocale::system().name()).toString()))
	{
		if(!translator.load(QCoreApplication::applicationDirPath() + "/translations/tubularix_"
			+ settings.value("language", QLocale::system().name()).toString()))
		{
			translator.load("/usr/share/games/tubularix/tubularix_"
				+ settings.value("language", QLocale::system().name()).toString());
		}
	}
	app.installTranslator(&translator);
	settings.endGroup();

	//Configure the codec used by tr()
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	MainWindow win;
	//win.setGeometry(20, 80, 1024, 900);
	win.show();

	return app.exec();
}
