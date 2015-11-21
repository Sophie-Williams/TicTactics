/*
 * TicTacticsBot
 * /TicTacticsBot/src/TicTacIO.cpp
 *
 *  Created on: 15.07.2015
 *      Author: WorldSEnder
 */

#include "TicTacIO.hpp"
#include <string>
#include <cctype>
#include <cstdlib>

namespace _detail
{

#define DEFINE_SYMBL(name, _value) const std::string name = #_value;

  DEFINE_SYMBL(DEBUG, TTTP:Debug)
  DEFINE_SYMBL(META, TTTP:Meta)
  DEFINE_SYMBL(CHAT, TTTP:Chat)
  DEFINE_SYMBL(ENGINE, TTTP:Engine)

  DEFINE_SYMBL(VERSION1, TTTP v1)

  DEFINE_SYMBL(START, Start game)
  DEFINE_SYMBL(INVALID_MV, Invalid move)
  DEFINE_SYMBL(INVALID_IN, Wrong input)
  DEFINE_SYMBL(END, End TTTP)

  DEFINE_SYMBL(P1_INIT, Player One init)
  DEFINE_SYMBL(P2_INIT, Player Two init)

  DEFINE_SYMBL(PLAYER1, Player One)
  DEFINE_SYMBL(PLAYER2, Player Two)

  DEFINE_SYMBL(ENGINE_SETTINGS, Engine settings)
  DEFINE_SYMBL(GAME_SETTINGS, Game settings)
  DEFINE_SYMBL(SETTINGS_END, End settings)

  DEFINE_SYMBL(START_TURNS, Start turns)

  DEFINE_SYMBL(OP_PRINT_FULL, Print full)
  DEFINE_SYMBL(OP_PRINT_BOARD_INC, Print B)

  DEFINE_SYMBL(OP_GO_INC, go)
  DEFINE_SYMBL(OP_PAUSE, pause)
  DEFINE_SYMBL(OP_STOP, stop)

  const std::string LOST_MESSAGE = " lost himself the game"; // Can't stringify

}

static bool
isInputString (const std::string& input, const std::string& expected)
{
  if (input.at (input.size () - 1) == '\r')
    {
      return input.substr (0, input.size () - 1) == expected;
    }
  return input == expected;
}

EngineInstance::EngineInstance (istream& input, ostream& output) :
    board (), is (input), os (output), search (), state (SearchState::PAUSED)
{
}

EngineInstance::~EngineInstance ()
{
  if (search)
    search->setState (SearchState::STOPPED);
}

void
EngineInstance::fail (std::string message)
{
  std::cerr << message << std::endl;
  exit (EXIT_FAILURE);
}

std::string
EngineInstance::getline ()
{
  std::string result;
  std::getline (is, result);
  return result;
}

bool
EngineInstance::isMove (std::string& buffer)
{
  if (buffer.size () < 4 || buffer[0] != 'B' || buffer[2] != 'F'
      || !isdigit (buffer[1]) || !isdigit (buffer[3]) || buffer[1] == '0'
      || buffer[3] == '0')
    return false;
  return true;
}

void
EngineInstance::run ()
{
  using namespace _detail;

  std::string buffer = getline ();
  if (!isInputString (buffer, VERSION1))
    {
      fail ("|" + buffer + "|");
    }
  sendline ("Ultimate destruction!");

  while (!isInputString (buffer = getline (), START))
    {
      fail ("Expected " + START);
    }
  sendline (START);

  buffer = getline ();
  if (!isInputString (buffer, P1_INIT))
    {
      fail ("Must begin with Player One init");
    }
  while (!isInputString (buffer = getline (), P2_INIT))
    {
      if (!isMove (buffer))
	fail (INVALID_IN);
      Move m =
	{ fromIndex (buffer[1] - '1'), fromIndex (buffer[3] - '1') };
      // To lazy to check the move...
      // if(!valid(m)) fail(INVALID_MV)
      board.apply (m);
      board.setPlayer (Color::CROSS);
    }
  board.setPlayer (Color::CIRCLE);
  while (!isInputString (buffer = getline (), START_TURNS))
    {
      if (!isMove (buffer))
	fail (INVALID_IN);
      Move m =
	{ fromIndex (buffer[1] - '1'), fromIndex (buffer[3] - '1') };
      // To lazy to check the move...
      // if(!valid(m)) fail(INVALID_MV)
      board.apply (m);
      board.setPlayer (Color::CIRCLE);
    }
  board.setPlayer (Color::CROSS);
  board.startOfGame ();
  search = Builder<TicTacToe, IterativeDepthSearcher>
    { }.setBoard (board).build ();
  Color winner;
  while (buffer = getline (), true)
    {
      if (isMove (buffer))
	{
	  winner = board.apply (
	    { fromIndex (buffer[1] - '1'), fromIndex (buffer[3] - '1') });
	  if (winner != Color::NONE)
	    break;
	}
      else if (buffer.find (OP_GO_INC) == 0)
	{
	  if (state == SearchState::PAUSED)
	    state = search->setState (SearchState::RUNNING);
	  else if (state == SearchState::STOPPED)
	    fail ("Already stopped, can't restart");
	}
      else if (isInputString (buffer, OP_PAUSE))
	{
	  if (state == SearchState::RUNNING)
	    state = search->setState (SearchState::PAUSED);
	  else if (state == SearchState::STOPPED)
	    fail ("Already stopped, can't pause");
	}
      else if (isInputString (buffer, OP_STOP))
	{
	  if (state != SearchState::STOPPED)
	    state = search->setState (SearchState::STOPPED);
	}
      else if (isInputString (buffer, OP_PRINT_FULL))
	{
	  sendline (board.repr ());
	}
      else if (buffer.find (OP_PRINT_BOARD_INC) == 0)
	{
	  Square s = fromIndex (buffer[7]);
	  sendline (board.repr (s));
	}
      else
	{
	  fail (INVALID_IN);
	}
    }
  switch (winner)
    {
    case Color::CROSS:
      sendline (PLAYER1, " has won");
      break;
    case Color::CIRCLE:
      sendline (PLAYER2, " has won");
      break;
    case Color::BOTH:
      fail ("Didn't expect that kind of spanish draw");
      break;
    case Color::NONE:
      break;
    }
  sendline (board.repr ());
  sendline (END);
}
