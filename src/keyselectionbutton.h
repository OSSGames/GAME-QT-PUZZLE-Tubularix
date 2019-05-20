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

#ifndef KEYSELECTIONBUTTON_H
#define KEYSELECTIONBUTTON_H

#include <QPushButton>

class KeySelectionButton : public QPushButton
{
Q_OBJECT
public:
	KeySelectionButton(QWidget *parent = 0);
	int key() {return keyCode;}

private slots:
	void selectKey();

private:
	bool selecting;
	QString previousKey;
	int keyCode;

protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void focusOutEvent(QFocusEvent *event);

signals:
	void keyIntroduced(QString name, int key);
	void keyIntroductionRejected(bool value);
	void introducingKey();
};

#endif // KEYSELECTIONBUTTON_H
