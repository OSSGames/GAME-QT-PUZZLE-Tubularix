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

#include "keyselectionbutton.h"
#include <QKeyEvent>


//public


KeySelectionButton::KeySelectionButton(QWidget *parent)
	: QPushButton(parent)
{
	previousKey = this->text();
	selecting = false;
	connect(this, SIGNAL(clicked()), this, SLOT(selectKey()));
}


//private slots


void KeySelectionButton::selectKey()
{
	previousKey = this->text();
	this->setText(tr("Press a key"));
	selecting = true;
	emit introducingKey();
}


//protected


void KeySelectionButton::keyPressEvent(QKeyEvent *event)
{
	if(!selecting)
		return;

	switch(event->key())
	{
	case Qt::Key_Left:
		this->setText("Left");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Right:
		this->setText("Right");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Down:
		this->setText("Down");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Up:
		this->setText("Up");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Space:
		this->setText("Space");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Enter:
		this->setText("Enter");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Return:
		this->setText("Return");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Backspace:
		this->setText("Backspace");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Home:
		this->setText("Home");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_End:
		this->setText("End");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_PageUp:
		this->setText("PageUp");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_PageDown:
		this->setText("PageDown");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Insert:
		this->setText("Insert");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Delete:
		this->setText("Delete");
		keyCode = event->key();
		emit keyIntroduced(this->objectName(), event->key());
		break;
	case Qt::Key_Escape:
		this->setText(previousKey);
		break;
	case Qt::Key_Shift:
	case Qt::Key_Control:
	case Qt::Key_Alt:
	case Qt::Key_AltGr:
	case Qt::Key_Meta:
	case Qt::Key_CapsLock:
	case Qt::Key_ScrollLock:
	case Qt::Key_NumLock:
		break;
	default:
		if(!event->text().isEmpty())
		{
			this->setText(event->text().toUpper());
			keyCode = event->key();
			emit keyIntroduced(this->objectName(), event->key());
		}
		else
			this->setText(previousKey);
		break;
	}
}

void KeySelectionButton::keyReleaseEvent(QKeyEvent *event)
{
	switch(event->key())
	{
	case Qt::Key_Shift:
	case Qt::Key_Control:
	case Qt::Key_Alt:
	case Qt::Key_AltGr:
	case Qt::Key_Meta:
	case Qt::Key_CapsLock:
	case Qt::Key_ScrollLock:
	case Qt::Key_NumLock:
		break;
	default:
		selecting = false;
		break;
	}
}

void KeySelectionButton::focusOutEvent(QFocusEvent */*event*/)
{
	if(selecting)
		this->setText(previousKey);

	selecting = false;
	emit keyIntroductionRejected(false);
}
