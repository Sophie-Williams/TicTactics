/*
 * TicTacticsBot
 * /TicTacticsBot/src/TicTacIO.hpp
 *
 *  Created on: 15.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include "TicTacToe.hpp"
#include "Searcher.hpp"

class EngineInstance
{
  typedef std::istream istream;
  typedef std::ostream ostream;

  typedef IterativeDepthSearcher<TicTacToe> Searcher;
public:
  /**
   * Input and Output have to outlive the engine in means of
   * lifetime.
   *
   * @param input the input the engine sources messages from
   * @param output the output the engine writes messages to
   */
  EngineInstance (istream& input, ostream& output);
  virtual
  ~EngineInstance ();
  /**
   * Runs the engine on the calling thread. Blocks until the
   * engine decides to turn off.
   *
   */
  void
  run ();
private:
  GameBoard board;
  istream& is;
  ostream& os;

  std::unique_ptr<Searcher> search;
  SearchState state;

  void
  fail (std::string = "BIG FAIL!");
  std::string
  getline ();
  bool
  isMove (std::string&);
  template<typename ... Args>
    void
    sendline (Args&& ... args)
    {
      using void_t = int[];
      void_t pipe =
	{ 0, ((void) (os << args), 0)... };
      (void) pipe;
      os << std::endl;
    }
};

