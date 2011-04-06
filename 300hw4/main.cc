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

#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItem>
#include <QtGui/QLabel>


#include "Board.h"
#include <GuiBoard.h>

using namespace std;

//#define ASCII 0
#define GUI

#ifdef GUI

int
main(int argc, char** argv)
{
	QApplication app(argc,argv);	
	GuiBoard board;
	board.show();
	return app.exec();
}


#endif






#ifdef ASCII
Board game;
void
DisplayBoard()
{
	int** ttt_board = game.GetBoard();
	
	cout << ttt_board[0][0] << '|' << ttt_board[1][0] << '|' << ttt_board[2][0] << endl;
	cout << "-----" << endl;
	cout << ttt_board[0][1] << '|' << ttt_board[1][1] << '|' << ttt_board[2][1] << endl;
	cout << "-----" << endl;
	cout << ttt_board[0][2] << '|' << ttt_board[1][2] << '|' << ttt_board[2][2] << endl << endl; 
 
}
int 
main(int argc, char** argv)
{

	game.Move(0,0,Board::X);
	game.Move(1,0,Board::O);
	game.Move(2,0,Board::O);
	game.Move(0,1,Board::O);
	game.Move(1,1,Board::X);
	game.Move(2,1,Board::X);
//	game.Move(0,2,Board::O);
	game.Move(1,2,Board::X);
	game.Move(2,2,Board::O);
		
	DisplayBoard();
	game.Undo();
	DisplayBoard();
	game.Undo();
	DisplayBoard();
	game.Undo();
	DisplayBoard();
	game.Undo();
	DisplayBoard();

}
#endif

