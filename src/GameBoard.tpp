/*
 * TicTacticsBot
 * /TicTacticsBot/src/GameBoard.tpp
 *
 *  Created on: 14.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include "GameBoard.hpp"
#include <iostream>

template<typename charT, typename Traits>
  std::basic_ostream<charT, Traits>&
  operator<< (std::basic_ostream<charT, Traits>& os, const GameBoard& c)
  {
    os << "+-----+-----+-----+\n";
    for (BoardIndex y = 0; y < 9; y++)
      {
	os << "|";
	for (BoardIndex x = 0; x < 9; x++)
	  {
	    BoardIndex board = (x / 3) + (y / 3) * 3;
	    BoardIndex field = (x % 3) + (y % 3) * 3;
	    Color player = c.m_boards[board].forField (fromIndex (field));
	    switch (player)
	      {
	      case Color::CIRCLE:
		os << "O";
		break;
	      case Color::CROSS:
		os << "X";
		break;
	      case Color::BOTH:
		os << "*";
		break;
	      default:
		if (fromIndex (board) == c.m_lastTurn)
		  os << "~";
		else
		  os << ".";
		break;
	      }
	    if (x != 8 && (x % 3) == 2)
	      os << "|";
	    else if (x != 8)
	      os << " ";
	  }
	os << "|\n";
	if (y != 8 && (y % 3) == 2)
	  {
	    os << "+-----+-----+-----+\n";
	  }
      }
    os << "+-----+-----+-----+\n";
    os << "+-----+\n";
    for (BoardIndex i = 0; i < 9; i++)
      {
	if ((i % 3) == 0)
	  os << "|";
	if ((i % 3) == 1)
	  os << " ";
	switch (c.m_bigboard.forField (fromIndex (i)))
	  {
	  case Color::CIRCLE:
	    os << "O";
	    break;
	  case Color::CROSS:
	    os << "X";
	    break;
	  case Color::BOTH:
	    os << "*";
	    break;
	  default:
	    os << ".";
	    break;
	  }
	if ((i % 3) == 1)
	  os << " ";
	if ((i % 3) == 2)
	  os << "|\n";
      }
    os << "+-----+\n";
    os << "Won: " << c.m_bigboard.getWon ();
    return os;
  }
