/*****************************************************************************
The MIT License

Copyright (c) 2010 Casey Beach

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*****************************************************************************/
#include <iostream>

#include <QPushButton>
#include <QMouseEvent>
#include <QPixmap>

#include "GuiButton.h"
#include "Board.h"

using namespace std;

int GuiButton::GameStatusInteger = 0;

GuiButton::GuiButton(){}
GuiButton::~GuiButton(){}


GuiButton::GuiButton(QWidget *parent):QPushButton(parent),
PlayerIcon(new QPixmap()){

	XPlayer = new QIcon(".qrc/X.png");
	OPlayer = new QIcon(".qrc/O.png");

}
GuiButton::GuiButton(const QString& name, QWidget *parent):
QPushButton(name,parent), PlayerIcon(new QPixmap()){

	XPlayer = new QIcon(QPixmap(".qrc/X.png"));
	OPlayer = new QIcon(QPixmap(".qrc/O.png"));


}
GuiButton::GuiButton(const QIcon &icon, 
const QString& name, QWidget *parent):
QPushButton(icon,name,parent), PlayerIcon(new QPixmap()){

	XPlayer = new QIcon(".qrc/X.png");
	OPlayer = new QIcon(".qrc/O.png");


}

GuiButton::GuiButton(const QIcon &icon, 
const QString& name, QWidget *parent, int x, int y):
QPushButton(icon,name,parent), PlayerIcon(new QPixmap()){
	XPlayer = new QIcon(".qrc/X.png");
	OPlayer = new QIcon(".qrc/O.png");
	Location[0] = x;
	Location[1] = y;
}

//many thanks to hatulflezet at
//http://www.qtforum.org/article/4276/qpushbutton-reacting-to-right-button-click.html
//for sharing this function.

void
GuiButton::mousePressEvent(QMouseEvent *event){
	if (GameStatusInteger != 0){
		emit click(-1, -1, this);
		return;
	}
	switch (event->button()){//which button was pressed?
		case Qt::RightButton:
			xMoved();
			break;	
	
		case Qt::LeftButton://following switch is virtually identical to above.
			yMoved();
			break;	
			
		case Qt::MidButton:
			emit click(-1,0,this);
			undo();
			break;
		default:
			break;
	}

}
//setters and getters
void
GuiButton::setBoard(Board *newBoard){
	TicTacToe = newBoard;	
}

void
GuiButton::setLocation(int x, int y){
	Location[0] = x;
	Location[1] = y;
}



Board const*
GuiButton::getBoard(){
	return TicTacToe;
}

int const* 
GuiButton::getLocation(){
	return Location;

}
//erase this button and start over again
void
GuiButton::eraseButton(){
	this->setIcon(QPixmap(""));
	GameStatusInteger = 0;
}

void 
GuiButton::undo(){
	this->setIcon(QPixmap(""));
	TicTacToe->undo(Location[0], Location[1]);	
}

void
GuiButton::xMoved(){
	switch (TicTacToe->move(Location[1],Location[0],Board::O)){
	//move returns play status
		case -1: //error, probably tried to move into an occupied square
			emit click(1, -1, this);
			break;
		case InProgress://play continues
			this->setIcon(*OPlayer);
			emit click(1, 0, this);
			break;	
		case XWins:
			GameStatusInteger = 1;
			this->setIcon(*OPlayer);
			emit click(1, 1, this);
			break;
		case OWins:
			
			GameStatusInteger = 2;
			this->setIcon(*OPlayer);
			emit click(1, 2, this);
			break;

		case Draw:
			GameStatusInteger = 3;
			this->setIcon(*OPlayer);
			emit click(1, 3, this);
			break;
		default:
			break;

	}
}

void
GuiButton::yMoved(){
	switch (TicTacToe->move(Location[1],Location[0],Board::X)){
		case -1://error, probably tried to move into an occupied square
			emit click(0, -1, this);
			break;
		case InProgress://play contiues
			this->setIcon(*XPlayer);
			emit click(0, 0, this);
			break;	
		case XWins:
			GameStatusInteger = 1;
			this->setIcon(*XPlayer);
			emit click(0, 1, this);
			break;
		case OWins:
			GameStatusInteger = 2;
			this->setIcon(*XPlayer);
			emit click(0, 2, this);
			break;

		case Draw:
			GameStatusInteger = 3;
			this->setIcon(*XPlayer);
			emit click(0, 3, this);
			break;
		default:
			break;
	
	}


}
