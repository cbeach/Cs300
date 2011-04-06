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
#include <assert.h>

#include "Board.h"

using namespace std;


//Constructors and Destructors************************************************
Board::Board():HistoryStackPosition(-1), NumSquaresFull(0){

	BoardLayout = new int*[3];
	for(int i = 0; i < 3; i++)
	{
		BoardLayout[i] = new int[3];
	}

	for (int i = 0; i < 3; i++){
		for ( int j = 0; j < 3; j++){
			BoardLayout[i][j] = 0;
		}
	}

	History = new PlayerMove*[9];
}


Board::~Board(){

	for(int i = 0; i < 3; i++)
	{
		delete[] BoardLayout[i];
	}
	delete[] BoardLayout;
	delete[] History;

}



//Public Functions*************************************************************
//The Move function is called when a player makes a move. x,y is the position
//on the 3X3 board. as for Player, the class enum containing blank, x, and y
//represent the values.

int
Board::move(int x, int y, int player){

	if(BoardLayout[x][y] != blank){	//is some one already moved here
		return -1;		//return a error code
	
	} else {
		BoardLayout[x][y] = player;
		PlayerMove *temp_move = new PlayerMove;
		temp_move->x = x;
		temp_move->y = y;
		temp_move->player = player;
		push(temp_move);
	}
	

	//check to see if there is a draw, return draw value
	if (checkForDraw() && !checkForWin()){
		return 3;
	}
	//no draw, see if some one won
	return checkForWin();

}


int **
Board::getBoard(){//return a copy of this board

	int **temp_int_array = new int*[3];
	for (int i = 0; i < 3; i++)
	{
		temp_int_array[i] = new int[3];
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			temp_int_array[i][j] = BoardLayout[i][j];
		}
	}
	return temp_int_array;
}

int
Board::checkForWin(){

	using namespace std;
	int potential_winner = 0;
	
	//check verticle rows for win
	for (int i = 0; i < 3; i++){
		potential_winner = 0;
		for (int j = 0; j < 3; j++){
			if (BoardLayout[i][j] == blank){
				//if there is a blank spot no one could 
				//have won, exit loop.
				break;

			} else if (potential_winner == blank && j == 0){
				//save the identity of the potential winner
				potential_winner = BoardLayout[i][j];

			} else if (potential_winner != BoardLayout[i][j]){
				//if the other player is on this row no one wins
				potential_winner = 0;
				break;

			} else if (potential_winner != 0 && j == 2){
				return potential_winner;
			}
		}
	}	
	
	//*****************************************************
	//all following if blocks follow the same patter as the first if block
	//*****************************************************
	//check the horizontal rows
	for (int j = 0; j < 3; j++){
		potential_winner = 0;
		for (int i = 0; i < 3; i++){
			if (BoardLayout[i][j] == 0){
				potential_winner = 0;
				break;

			} else if (potential_winner == 0 && i == 0){
				potential_winner = BoardLayout[i][j];

			} else if (potential_winner != BoardLayout[i][j]){
				potential_winner = 0;
				break;

			} else if (potential_winner != 0 && i == 2){
				return potential_winner;

			}
		}
	}


	potential_winner = 0;
	//check diagonal starting at top left
	for (int i = 0; i < 3; i++){
		if (BoardLayout[i][i] == 0){
			potential_winner = 0;
			break;

		} else if (potential_winner == 0 && i == 0){
			potential_winner = BoardLayout[i][i];

		} else if (potential_winner != BoardLayout[i][i]){
			potential_winner = 0;
			break;

		} else if (potential_winner != 0 && i == 2){
			return potential_winner;

		}
	}

	potential_winner = 0;
	//check diagonal starting at top right
	for (int i = 0; i < 3; i++){
		if (BoardLayout[2-i][i] == 0){
			potential_winner = 0;
			break;

		} else if (potential_winner == 0 && i == 0){
			potential_winner = BoardLayout[2-i][i];

		} else if (potential_winner != BoardLayout[2-i][i]){
			potential_winner = 0;
			break;

		} else if (potential_winner != 0 && i == 2){
			return potential_winner;

		}
	}

	return potential_winner;
}

bool
Board::checkForDraw(){

	using namespace std;
	int blocked_lines = 0;
	int potential_winner = 0;

	//the following for loop blocks check to see if both players
	//have a token in each winning row/column/diagonal
	//check columns for win
	for (int i = 0; i < 3; i++){
		potential_winner = 0;
		for (int j = 0; j < 3; j++){
			if (BoardLayout[i][j] == 0){

			} else if (potential_winner == 0 && j == 0){
				potential_winner = BoardLayout[i][j];

			} else if (potential_winner != 0 && potential_winner != BoardLayout[i][j] && BoardLayout[i][j] != 0){
				blocked_lines++;	
				break;

			} else if (potential_winner != 0 && j == 2){

			}
		}
	}	
	
	potential_winner = 0;
	//check the rows
	for (int j = 0; j < 3; j++){
		potential_winner = 0;
		for (int i = 0; i < 3; i++){
			if (BoardLayout[i][j] == 0){

			} else if (potential_winner == 0 && i == 0){
				potential_winner = BoardLayout[i][j];

			} else if (potential_winner != 0 && potential_winner != BoardLayout[i][j] && BoardLayout[i][j] != 0){
				blocked_lines++;
				break;

			} else if (potential_winner != 0 && i == 2){

			}
		}
	}


	potential_winner = 0;
	//check diagonal starting at top left
	for (int i = 0; i < 3; i++){
		if (BoardLayout[i][i] == 0){

		} else if (potential_winner == 0 && i == 0){
			potential_winner = BoardLayout[i][i];

		} else if (potential_winner != 0 && potential_winner != BoardLayout[i][i] && BoardLayout[i][i] != 0){
			blocked_lines++;
			break;

		} else if (potential_winner != 0 && i == 2){

		}
	}

	potential_winner = 0;
	//check diagonal starting at top right
	for (int i = 0; i < 3; i++){
		if (BoardLayout[2-i][i] == 0){

		} else if (potential_winner == 0 && i == 0){
			potential_winner = BoardLayout[2-i][i];

		} else if (potential_winner && potential_winner != BoardLayout[2-i][i] && BoardLayout[2-i][i] != 0){
			blocked_lines++;
			break;

		} else if (potential_winner != 0 && i == 2){

		}
	}
	if(blocked_lines == 8){
		return true;
	}
	else
		return false;


}


void
Board::undo(int x, int y){	//all this does is pop the top move off the history and remove that
				//piece from the board
	BoardLayout[y][x] = 0;
}


void
Board::push(PlayerMove *pushed_move){	//this stack keeps track of all the moves

	if(NumSquaresFull != 9)
	{
		HistoryStackPosition++;
		History[HistoryStackPosition] = pushed_move;
	}
}

Board::PlayerMove*
Board::pop(){

	if(HistoryStackPosition == -1)
		return NULL;

	PlayerMove *temp_move = History[HistoryStackPosition];
	History[HistoryStackPosition] = NULL;
	HistoryStackPosition--;

	return temp_move;
}

void Board::displayBoard()
{
	
	cout << BoardLayout[0][0] << '|' << BoardLayout[1][0] << '|' << BoardLayout[2][0] << endl;
	cout << "-----" << endl;
	cout << BoardLayout[0][1] << '|' << BoardLayout[1][1] << '|' << BoardLayout[2][1] << endl;
	cout << "-----" << endl;
	cout << BoardLayout[0][2] << '|' << BoardLayout[1][2] << '|' << BoardLayout[2][2] << endl << endl; 
 
}

void
Board::eraseBoard(){//clears the board for a new game.

	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			BoardLayout[i][j] = 0;
		}
	}

	for (int i = HistoryStackPosition; i <= 0; i--){
		if (History[i])
			delete History[i];
	}

	HistoryStackPosition = -1;
	NumSquaresFull = 0;
}




















