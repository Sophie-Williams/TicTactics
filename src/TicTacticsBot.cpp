//============================================================================
// Name        : TicTacticsBot.cpp
// Author      : WorldsEnder
// Version     :
// Copyright   :
// Description : TicTacticsBot
//============================================================================

#include <iostream>
#include <string>
#include "Searcher.hpp"
#include "TicTacToe.hpp"

Move fromInput(std::string line)
{
	if(line.size() < 4)
	{
		return {Square::SQUARE_NONE, Square::SQUARE_NONE};
	}
	BoardIndex boardI = line.c_str()[1] - '1';
	BoardIndex fieldI = line.c_str()[3] - '1';
	if(boardI < 0 || boardI > 8 || fieldI < 0 || fieldI > 8)
	{
		return {Square::SQUARE_NONE, Square::SQUARE_NONE};
	}
	return {fromIndex(boardI), fromIndex(fieldI)};
}

int main()
{
	GameBoard board;

#define MOVE(m) board.apply(fromInput(#m))

	MOVE(B1F3);
	MOVE(B1F6);
	MOVE(B1F1);
	MOVE(B2F4);
	MOVE(B2F5);
	MOVE(B3F1);
	MOVE(B3F4);
	MOVE(B4F2);
	MOVE(B5F6);
	MOVE(B5F3);
	MOVE(B7F7);
	MOVE(B6F7);
	MOVE(B7F8);
	MOVE(B7F5);
	MOVE(B8F2);
	MOVE(B8F8);
// B9F9
//	B9F9
	MOVE(B1F5);
	MOVE(B5F4);
	MOVE(B4F1);
	MOVE(B1F9);
	MOVE(B9F2);
	MOVE(B2F1);
	MOVE(B1F7);
	MOVE(B7F2);
	MOVE(B2F6);
	MOVE(B6F6);
	MOVE(B6F2);
	MOVE(B2F2);
	MOVE(B2F8);
	MOVE(B8F6);
	MOVE(B6F4);
	MOVE(B4F4);
	MOVE(B4F6);
	MOVE(B6F8);
	MOVE(B8F4);
	MOVE(B4F7);
	MOVE(B7F6);
	MOVE(B6F1);
	MOVE(B1F2);
	MOVE(B2F7);
	MOVE(B7F4);

	std::cout << board << std::endl;

	IterativeDepthSearcher<TicTacToe> search =
			(Builder<TicTacToe, IterativeDepthSearcher>{}).setBoard(board).build();

	std::cin.ignore();
	search.setState(SearchState::RUNNING);
	std::cin.ignore();
	return 0;
}

