/*
 * TicTacticsBot
 * /TicTacticsBot/src/Boad.cpp
 *
 *  Created on: 13.07.2015
 *      Author: WorldSEnder
 */

#include "Board.hpp"
#include "RatingTable.h"
#include "WinnerTable.h"

Color Board::set(Square s, Color c)
{
	bool won = false;
	switch(c) {
	case Color::CROSS:
		player1.set(toIndex(s));
		won = winnerTable[player1.to_ulong()];
		break;
	case Color::CIRCLE:
		player2.set(toIndex(s));
		won = winnerTable[player2.to_ulong()];
		break;
	case Color::NONE:
		player1.set(toIndex(s), false);
		player2.set(toIndex(s), false);
		break;
	case Color::BOTH:
		player1.set(toIndex(s));
		player2.set(toIndex(s));
		if(winner != Color::NONE)
			return Color::NONE;
		bool won1 = winnerTable[player1.to_ulong()];
		bool won2 = winnerTable[player1.to_ulong()];
		if(won1) {
			if(won2)
				return setWon(Color::BOTH);
			return setWon(Color::CROSS);
		}
		if(won2)
			return setWon(Color::CIRCLE);
		if(isFull())
			return setWon(Color::BOTH);
		return Color::NONE;
	}
	if(winner != Color::NONE)
		return Color::NONE;
	if(won)
		return setWon(c);
	if(isFull())
		return setWon(Color::BOTH);
	return Color::NONE;
}

Color Board::setWon(Color c)
{
	return winner = c;
}

Score Board::rate() const
{
	switch (winner) {
		case Color::CROSS:
			return maxBoardScore;
		case Color::CIRCLE:
			return -maxBoardScore;
		case Color::BOTH:
			return 0;
		default:
			break;
	}
	return ratingTable[(player1.to_ulong() << 9) | player2.to_ulong()];
}

Color Board::forField(Square s) const
{
	if(player1.test(toIndex(s)))
	{
		if(player2.test(toIndex(s)))
			return Color::BOTH;
		return Color::CROSS;
	}
	if(player2.test(toIndex(s)))
		return Color::CIRCLE;
	return Color::NONE;
}
