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

#ifndef BOARD_H
#define BOARD_H


class Board {
	protected:
		

		struct PlayerMove{
			int	x, y;
			int	player;
		};

		int 		**BoardLayout;		//tic-tac-toe board represented as a 2D array
		PlayerMove	**History;		//this is an array of moves implemented as a stack
		int		HistoryStackPosition;	//position on the move stack
		int		NumSquaresFull;
		void 		push(PlayerMove*);
		PlayerMove* 	pop();
		bool 		checkForDraw();

		//returns 0 for no win
		//returns 1 if X wins
		//returns 2 if O wins
		int 	checkForWin();
	public:
		
		Board();
		~Board();
		
		enum 	{blank, X, O};
		
		//When SetCell is called the board is automatically checked to see if
		//if there is a draw, of if either player has won.
		//if neither play has won and there is no draw SetCell returns 0
		//if X has won SetCell returns 1
		//if O has won SetCell returns 2
		//if there is a draw SetCell returns 3
		//if there is an error, -1
	
		int** 	getBoard();		//returns the board array
		void 	undo(int, int);
		int 	move(int x, int y, int player);	//player X = 1 O = 2
		void 	eraseBoard();
		void 	displayBoard();
};
#endif
