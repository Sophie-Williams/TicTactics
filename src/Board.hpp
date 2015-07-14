/*
 * TicTacticsBot
 * /TicTacticsBot/src/Board.hpp
 *
 *  Created on: 13.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include "tttdefs.hpp"
#include <bitset>

class Board
{
public:
	typedef std::bitset<9> playerset;

	Board() = default;
	Board(const Board&) = default;
	~Board() = default;

	/**
	 * Set a field to a color. Returns the color that has won,
	 * if any (or both when in a draw).
	 */
	Color set(Square, Color);
	/**
	 * Set the color that has won (if any). We don't have to
	 * keep our own turn history this way.
	 * @returns the Color set (for utility)
	 */
	Color setWon(Color);
	Color forField(Square) const;
	/**
	 * Rate the Board
	 */
	Score rate() const;

	inline bool isFull() const
	{
		return (player1.to_ulong() | player2.to_ulong()) == 0x1FF;
	}

	inline Color getWon() const
	{
		return winner;
	}

	inline playerset getFreeFields() const
	{
		return ~(player1 | player2);
	}

	inline BoardHash getHash() const noexcept
	{
		return BoardHash {(player1.to_ulong() << 9) | player2.to_ulong()};
	}

private:
	playerset player1;
	playerset player2;

	Color winner = Color::NONE;
};


