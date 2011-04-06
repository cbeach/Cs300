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
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QFont>
#include <QGridLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QPixmap>

#include "Board.h"
#include "GuiBoard.h"
#include "GuiButton.h"

using namespace std;


GuiBoard::GuiBoard(){
	
	ButtonBoard = new GuiButton**[3];
	GameStatusInteger = 0;

	Player= true;
	for (int i = 0; i < 3; i++){
		ButtonBoard[i] = new GuiButton*[3];
	}	
	
	init();
}

GuiBoard::~GuiBoard()
{
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			delete ButtonBoard[i][j];
		}
		delete[] ButtonBoard[i];
	}
	delete[] ButtonBoard;
	
}

//I use init functions to handle the initialization of more complex
//data types that take a little more processing.
void
GuiBoard::init()
{
	TicTacToe = new Board();

	QGridLayout *grid = new QGridLayout;
	
	//these are the instructional icons at the top of the
	//window
	QPixmap *mouseL = new QPixmap(".qrc/XmouseL.png");
	QPixmap *mouseR = new QPixmap(".qrc/OmouseR.png");
	QPixmap *mouseM = new QPixmap(".qrc/RWmouseM.png");
	
	//these are the icons at the bottom that show who's turn it is
	OPixmap  = new QPixmap(".qrc/O.png");	
	XPixmap  = new QPixmap(".qrc/X.png");	

	//this wrap the above images in a QWidget type class
	//so that they can be put into the grid layout
	QLabel *leftLabel = new QLabel();
	QLabel *rightLabel = new QLabel();
	QLabel *middleLabel = new QLabel();
	
	OsTurnLabel = new QLabel();
	XsTurnLabel = new QLabel();
	GameStatusLabel = new QLabel();

	//this actually puts the above images in the Qlabel wrappers
	leftLabel->setPixmap(*mouseL);
	rightLabel->setPixmap(*mouseR);
	middleLabel->setPixmap(*mouseM);

	XsTurnLabel->setPixmap(*XPixmap);
	OsTurnLabel->setPixmap(NULL);
	
	//this block adds all of the labels to the grid.
	grid->addWidget(leftLabel, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	grid->addWidget(middleLabel, 0, 1, Qt::AlignHCenter | Qt::AlignVCenter);
	grid->addWidget(rightLabel, 0, 2, Qt::AlignHCenter | Qt::AlignVCenter);
	grid->addWidget(XsTurnLabel, 5, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	grid->addWidget(OsTurnLabel, 5, 2, Qt::AlignHCenter | Qt::AlignVCenter);
	grid->addWidget(GameStatusLabel, 5, 1, Qt::AlignHCenter | Qt::AlignVCenter);
 	

	//create 9 new GuiButtons, connect all of the signals to the apropriate slots,
	//set their size and font, show them the pointer to the tictactoe engine and 
	//put them in the grid layout.
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ButtonBoard[j][i] = new GuiButton(*(new QIcon()),"", this, j, i);
			ButtonBoard[j][i]->setBoard(TicTacToe);
			ButtonBoard[j][i]->setLocation(j,i);
			ButtonBoard[j][i]->setFont(QFont("Times", 18, QFont::Bold));
			ButtonBoard[j][i]->setMaximumSize(75,75);
			ButtonBoard[j][i]->setMinimumSize(75,75);
			connect(ButtonBoard[j][i], SIGNAL(click(int,int,GuiButton*)),
				this, SLOT(changeButton(int,int)));
			connect(this, SIGNAL(newGameSignal()),
				ButtonBoard[j][i], SLOT(eraseButton()));
			grid->addWidget(ButtonBoard[j][i],j+1,i);			
		}
	}

	//add the gridLayout to a BoxLayout so that Qt will look at it as one widget.
	QVBoxLayout *box = new QVBoxLayout;
	box->addLayout(grid);
	setLayout(box);
}

//this slot gets called everytime a player pushes a button
void
GuiBoard::changeButton(int player, int flag){
	
	//if GameStatusInteger is greater than one some one won,or there was a draw
	//last turn.  This means that some one clicked another button, so we'll clean
	//things up and let them play another game
	if (GameStatusInteger > 0)	
	{
		GameStatusInteger = 0;
		emit newGameSignal();
		GameStatusLabel->setPixmap(NULL);
		OsTurnLabel->setPixmap(NULL);
		XsTurnLabel->setPixmap(*XPixmap);
		TicTacToe->eraseBoard();
		return;
	}

	//this
	switch (flag) {
		case -1:
			break;

		case inProgress:
			switch (player){
				case -1://error. some one was already in that square
					break;
				case 0://X just went, switch the pictures to let O know it's their turn.
					XsTurnLabel->setPixmap(NULL);
					OsTurnLabel->setPixmap(*OPixmap);

					break;
				case 1://O just went, switch the pictures like above
					XsTurnLabel->setPixmap(*XPixmap);
					OsTurnLabel->setPixmap(NULL);

					break;
			}
			break;
		case XWins:
			GameStatusInteger = XWins;
			GameStatusLabel->setPixmap(QPixmap(".qrc/XWins.png"));
			XsTurnLabel->setPixmap(*XPixmap);
			OsTurnLabel->setPixmap(*XPixmap);
			break;
		
		case OWins:
			GameStatusInteger = OWins;
			GameStatusLabel->setPixmap(QPixmap(".qrc/OWins.png"));
			XsTurnLabel->setPixmap(*OPixmap);
			OsTurnLabel->setPixmap(*OPixmap);
			break;
		
		case Draw:
			GameStatusInteger = Draw;
			XsTurnLabel->setPixmap(NULL);
			OsTurnLabel->setPixmap(NULL);

			GameStatusLabel->setPixmap(QPixmap(".qrc/Draw.png"));

	}		
}

//some setters and getters
void
GuiBoard::setBoard(Board *newBoard){
	TicTacToe = newBoard;
}

Board const*
GuiBoard::getBoard(){
	return TicTacToe;

}
