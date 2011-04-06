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

#ifndef GUIBUTTON_H
#define GUIBUTTON_H

#include <QPushButton>

class QWidget;
class QPixmap;
class Board;

class GuiButton: public QPushButton
{
	Q_OBJECT;

	public:
		enum {InProgress, XWins, OWins, Draw};
		
		//various constructors
		GuiButton();
		~GuiButton();
		GuiButton(QWidget *parent = 0);
		GuiButton(const QString&, QWidget *parent = 0);
		GuiButton(const QIcon&, const QString&, QWidget *parent = 0);
		GuiButton(const QIcon&, const QString&, QWidget *parent = 0, int x = 0, int y = 0);

		//set the pointer to the tictactoe engine
		void setBoard(Board*);
		//set this button's location in the 3x3 board
		void setLocation(int, int);

		//get the location of this button in the 3x3 board
		int const * getLocation();
		//get the pointer to the tictactoe engine
		Board const * getBoard();
		
	public
	slots:
		//clear this button.  called when the game is over
		void eraseButton();	
	signals:
		//emited when some one clicks on this button
		void click(int, int, GuiButton*);

	

	protected:
			
		//this keeps track of the game state.  i.e. x or o win/draw
		static int GameStatusInteger;
		//this is this buttons spot in the 3x3 grid
		int Location[2];
		
		//the X/O pictures that show up on the face of the buttons
		QPixmap *PlayerIcon;		
		QIcon *XPlayer;
		QIcon *OPlayer;
					
		//a pointer to the tictactoe engine
		Board *TicTacToe;
	
		//this handles what happens when a mouse button is pressed
		void mousePressEvent(QMouseEvent *);
		void undo();
		
		void xMoved();
		void yMoved();

};

#endif
