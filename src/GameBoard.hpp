/*
 * TicTacticsBot
 * /TicTacticsBot/src/GameBoard.hpp
 *
 *  Created on: 12.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include "tttdefs.hpp"
#include "Board.hpp"

class GameBoard
{
public:
	/**
	 * Creates a new, empty GameBoard
	 */
	GameBoard() = default;
	GameBoard(const GameBoard&) = default;
	/**
	 * Applies and undos a move. The move is not checked
	 * for validity. The player making the move is the player
	 * who's turn it is.
	 *
	 * @return Color - the color of the player who won. NEITHER
	 *                 if nobody won so far and BOTH on draw
	 */
	Color apply(const Move& m);
	void undo();
	inline void setPlayer(Color c) // For debugging, mostly
	{
		m_player = c;
	}

	Score getScore() const noexcept;

	MoveIterator begin() const noexcept;
	MoveIterator end() const noexcept;

	MoveIterator possibleMoves() const noexcept;

	inline BoardHash getHash() const noexcept
	{
		BoardHash h;
		for(const Board& b : m_boards)
		{
			h |= b.getHash();
			h <<= 18;
		}
		h |= m_bigboard.getHash();
		return h;
	}
private:
	MoveHistory history = {{}, 0};
	Board m_boards[SQUARES_NUMBER];
	Board m_bigboard;
	Square m_lastTurn = Square::SQUARE_NONE;
	Color m_player = Color::CROSS;

	template< typename charT, typename Traits >
	friend
	std::basic_ostream<charT, Traits>& operator<<(
			std::basic_ostream<charT, Traits>& os, const GameBoard& c);
};

#include "GameBoard.tpp"
