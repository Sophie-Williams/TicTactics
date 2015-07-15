/*
 * TicTacticsBot
 * /TicTacticsBot/src/TicTacToe.hpp
 *
 *  Created on: 12.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include "tttdefs.hpp"
#include "GameBoard.hpp"

// Represents TicTacToe as a general game
struct TicTacToe {
	typedef Color winner;
	typedef Move move;
	typedef GameBoard board;
	typedef Score score;
	typedef MoveIterator move_iterator;
	typedef BoardHash hashable;

	static constexpr winner player1 = Color::CROSS;
	static constexpr winner player2 = Color::CIRCLE;
	static constexpr winner neither = Color::NONE;
	static constexpr winner both = Color::BOTH;

	static constexpr score infinity = maxScore;

	static constexpr move illegal_move = {Square::SQUARE_NONE, Square::SQUARE_NONE};
	static constexpr move_iterator end_of_moves = {};

	static constexpr inline unsigned expectedDepth() {
		return SQUARES_NUMBER * (unsigned) SQUARES_NUMBER;
	}

	static inline winner applyMove(board& b, const move& m) noexcept {
		return b.apply(m);
	}

	static inline void undoLastMove(board& b) noexcept {
		b.undo();
	}

	static inline board newBoard() noexcept {
		return GameBoard{};
	}

	static inline score rateBoard(const board& b) noexcept {
		return b.getScore();
	}

	static inline move_iterator getMoves(const board& b) noexcept {
		return b.possibleMoves();
	}

	static inline hashable hash(const board& b) noexcept {
		return b.getHash();
	}
};
