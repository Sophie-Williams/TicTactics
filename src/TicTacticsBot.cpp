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

	board.apply(fromInput("B7F1"));
	board.apply(fromInput("B1F4"));
	board.apply(fromInput("B7F2"));
	board.apply(fromInput("B2F2"));
	board.apply(fromInput("B8F3"));
	board.apply(fromInput("B2F9"));
	board.apply(fromInput("B8F6"));
	board.apply(fromInput("B3F5"));
	board.apply(fromInput("B9F4"));
	board.apply(fromInput("B5F6"));
	board.apply(fromInput("B9F7"));
	board.apply(fromInput("B5F8"));
	board.apply(fromInput("B3F8"));
	board.apply(fromInput("B6F3"));
	board.apply(fromInput("B5F9"));
	board.apply(fromInput("B6F7"));
	board.apply(fromInput("B6F5"));
	board.apply(fromInput("B8F1"));
	board.apply(fromInput("B5F2"));
	board.apply(fromInput("B2F4"));

	std::cout << board << std::endl;

	IterativeDepthSearcher<TicTacToe> search =
			(Builder<TicTacToe, IterativeDepthSearcher>{}).setBoard(board).build();

	std::cin.ignore();
	search.setState(SearchState::RUNNING);
	std::cin.ignore();
	return 0;
}

