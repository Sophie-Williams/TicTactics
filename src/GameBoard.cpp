/*
 * TicTacticsBot
 * /TicTacticsBot/src/GameBoard.cpp
 *
 *  Created on: 13.07.2015
 *      Author: WorldSEnder
 */

#include <sstream>
#include "GameBoard.hpp"
#include "RatingTable.h"

Color
GameBoard::apply (const Move& m)
{
  AppliedMove& hmove = history.history[history.moves++];
  Board& played = m_boards[toIndex (m.bigboard)];
  hmove.move = m;

  Color won = played.set (m.smallboard, m_player);
  hmove.wonsmall = won;
  m_player = m_player == Color::CROSS ? Color::CIRCLE : Color::CROSS;

  hmove.wasFreemove = m_lastTurn == Square::SQUARE_NONE;
  m_lastTurn = m.smallboard;
  if (m_boards[toIndex (m_lastTurn)].isFull ())
    m_lastTurn = Square::SQUARE_NONE;

  if (won == Color::NONE)
    {
      hmove.wonbig = Color::NONE;
      return Color::NONE; // Can't win the big board without a small board
    } /* else */
  won = m_bigboard.set (m.bigboard, won);
  hmove.wonbig = won;
  return won;
}

void
GameBoard::undo ()
{
  AppliedMove& hmove = history.history[--history.moves];
  Board& played = m_boards[toIndex (hmove.move.bigboard)];
  played.set (hmove.move.smallboard, Color::NONE);

  if (hmove.wonsmall != Color::NONE)
    {
      m_bigboard.set (hmove.move.bigboard, Color::NONE);
      played.setWon (Color::NONE);
    }
  if (hmove.wonbig != Color::NONE)
    {
      m_bigboard.setWon (Color::NONE);
    }

  if (hmove.wasFreemove)
    m_lastTurn = Square::SQUARE_NONE;
  else
    m_lastTurn = hmove.move.bigboard;
  m_player = m_player == Color::CROSS ? Color::CIRCLE : Color::CROSS;
}

Score
GameBoard::getScore () const noexcept
{
  switch (m_bigboard.getWon ())
    {
    case Color::CROSS:
      return maxScore;
    case Color::CIRCLE:
      return -maxScore;
    case Color::BOTH:
      return 0;
    case Color::NONE:
      break;
    }
  Score score = 0;
  for (const Board& b : m_boards)
    {
      score += b.rate ();
    }
  score += m_bigboard.rate () * 5;
  return score;
}

MoveIterator
GameBoard::possibleMoves () const noexcept
{
  MoveIterator it;
  if (m_bigboard.getWon () != Color::NONE)
    return it;
  if (m_lastTurn != Square::SQUARE_NONE)
    {
      const Board& board = m_boards[toIndex (m_lastTurn)];
      Board::playerset set = board.getFreeFields ();
      for (BoardIndex field = 0; set.any () && field < 9; field++, set >>= 1)
	{
	  if (!set.test (0))
	    continue;
	  Square fieldS = fromIndex (field);
	  if (history.history[history.moves - 1].move.bigboard == fieldS)
	    continue;
	  it.moves[it.moveCount++] =
	    { m_lastTurn, fieldS};
	}
    }
  else
    {
      for(BoardIndex boardidx = 0; boardidx < 9; boardidx++)
	{
	  const Board& board = m_boards[boardidx];
	  Board::playerset set = board.getFreeFields();
	  for(BoardIndex field = 0; set.any() && field < 9; field++, set >>= 1)
	    {
	      if(!set.test(0))
	      continue;
	      Square fieldS = fromIndex(field);
	      if(history.moves > 0 && // Can't send someone back to his square
		  history.history[history.moves - 1].move.bigboard == fieldS)
	      continue;
	      it.moves[it.moveCount++] =
		{ fromIndex(boardidx), fieldS};
	    }
	}
    }
  return it;
}

MoveIterator
GameBoard::begin () const noexcept
{
  return possibleMoves ();
}

MoveIterator
GameBoard::end () const noexcept
{
  return MoveIterator
    { };
}

void
GameBoard::startOfGame () noexcept
{
  history.moves = 0;
  m_lastTurn = Square::SQUARE_NONE;
}
std::string
GameBoard::repr (Square s) const noexcept
{
  if (s == Square::SQUARE_NONE)
    {
      std::stringstream fill;
      fill << *this;
      return fill.str ();
    }
  else
    {
      const Board& b = m_boards[toIndex (s)];
      return "Printing single boards not supported";
    }
}
