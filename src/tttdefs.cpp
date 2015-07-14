/*
 * TicTacticsBot
 * /TicTacticsBot/src/tttdefs.cpp
 *
 *  Created on: 13.07.2015
 *      Author: WorldSEnder
 */

#include "tttdefs.hpp"

#include <algorithm>

MoveIterator::MoveIterator(const MoveIterator& other)
	: moveCount(other.moveCount)
{
	std::copy(other.moves, other.moves + 81, moves);
}

bool MoveIterator::operator!=(const MoveIterator& other) const
{
	// Weak test only tests for the "end of moves" iterator
	if(moveCount == 0 && other.moveCount == 0)
		return false;
	return true;
}

Move MoveIterator::operator*() const
{
	return moves[moveCount - 1];
}

MoveIterator& MoveIterator::operator++()
{
	moveCount--;
	return *this;
}


