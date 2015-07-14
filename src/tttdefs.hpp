/*
 * TicTacticsBot
 * /TicTacticsBot/src/tttdefs.hpp
 *
 *  Created on: 12.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include <bitset>

class TicTacToe; // Forward declare, for friending?
class GameBoard;

typedef signed Score;
typedef unsigned char BoardIndex;
typedef std::bitset<18*10> BoardHash;

enum class Color : unsigned char {
	NONE = 0,
	CROSS,
	CIRCLE,
	BOTH
};

template< typename charT, typename Traits >
std::basic_ostream<charT, Traits>& operator<<(
		std::basic_ostream<charT, Traits>& os, const Color& c ) {
	switch (c) {
	case Color::BOTH:
		os << "BOTH"; break;
	case Color::CIRCLE:
		os << "CIRCLE"; break;
	case Color::CROSS:
		os << "CROSS"; break;
	case Color::NONE:
		os << "NONE"; break;
	}
	return os;
}


enum class Square : unsigned char {
	SQUARE_0 = 0,
	SQUARE_1,
	SQUARE_2,
	SQUARE_3,
	SQUARE_4,
	SQUARE_5,
	SQUARE_6,
	SQUARE_7,
	SQUARE_8,
	SQUARE_NONE
};

template< typename charT, typename Traits >
std::basic_ostream<charT, Traits>& operator<<(
		std::basic_ostream<charT, Traits>& os, const Square& s ) {
	switch (s) {
	case Square::SQUARE_0:
		os << "1"; break;
	case Square::SQUARE_1:
		os << "2"; break;
	case Square::SQUARE_2:
		os << "3"; break;
	case Square::SQUARE_3:
		os << "4"; break;
	case Square::SQUARE_4:
		os << "5"; break;
	case Square::SQUARE_5:
		os << "6"; break;
	case Square::SQUARE_6:
		os << "7"; break;
	case Square::SQUARE_7:
		os << "8"; break;
	case Square::SQUARE_8:
		os << "9"; break;
	case Square::SQUARE_NONE:
		os << "INVALID_SQUARE"; break;
	}
	return os;
}

inline BoardIndex toIndex(Square s)
{
	return static_cast<BoardIndex>(s);
}

inline Square fromIndex(BoardIndex c)
{
	return static_cast<Square>(c);
}

constexpr BoardIndex SQUARES_NUMBER = 9;

#ifdef USEBITFIELDS
# define BITFIELD(n) : n
#else
# define BITFIELD(n)
#endif

struct Move {
	Square bigboard BITFIELD(4);
	Square smallboard BITFIELD(4);

	template< typename charT, typename Traits >
	friend
	std::basic_ostream<charT, Traits>& operator<<(
			std::basic_ostream<charT, Traits>& os, const Move& mc ) {
		os << "B" << mc.bigboard << "F" << mc.smallboard;
		return os;
	}
};

struct AppliedMove {
	Move move;
	Color wonsmall BITFIELD(2);
	Color wonbig BITFIELD(2);
	bool wasFreemove BITFIELD(1);
};

struct MoveHistory {
	AppliedMove history[SQUARES_NUMBER * SQUARES_NUMBER];
	unsigned moves;
};

constexpr Score maxScore = 2048;
constexpr Score maxBoardScore = 100;

class MoveIterator { // A (restricted) input iterator
	constexpr MoveIterator()
		: moves{}, moveCount(0) {}

public:
	MoveIterator(const MoveIterator& other);
	~MoveIterator() = default;
	typedef Move type;

	bool operator!=(const MoveIterator& other) const;
	Move operator*() const;
	MoveIterator& operator++();
private:
	// TODO: define members
	Move moves[SQUARES_NUMBER * SQUARES_NUMBER];
	unsigned moveCount;

	friend GameBoard;
	friend TicTacToe;
};

