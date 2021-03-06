/*
 * TicTacticsBot
 * /TicTacticsBot/src/Searcher.hpp
 *
 *  Created on: 12.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include "defines.hpp"

//concept Game {
//	concept winner {
//		bool operator==(const winner&);
//	}
//	concept move {
//		move(const move&);
//		void operator=(const move&);
//		bool operator==(const move&);
//	}
//	concept board {
//		board();
//		board(const board&);
//		void operator=(const board&);
//	}
//	concept score { // Comparible and invertible
//		bool operator<(const score&);
//		bool operator>(const score&);
//		score operator-();
//		void operator=(const score&);
//		axiom {
//			score a, b, c;
//			if(a < b && b < c)
//				assert(a < c);
//			if(a > b && b > c)
//				assert(a > c);
//		}
//	}
//	concept move_iterator {
//		move_iterator(const move_iterator& other);
//		bool operator!=(const move_iterator& other) const;
//		move operator*() const;
//		move_iterator& operator++();
//	}
//	concept hashable {
//		axiom {
//			using std::hash<hashable>;
//			uint_64 h = hash{}(hashable);
//		}
//		hashable(const hashable&);
//		void operator=(const hashable&);
//		bool operator==(const hashable&);
//	}
//
//	static const winner player1;
//	static const winner player2;
//	static const winner neither;
//	static const winner both;
//
//	static const score infinity;
//	// static const score minusinfinity = -infinity;
//
//	static const move illegal_move;
//
//	static const move_iterator end_of_moves;
//
//	static player applyMove(board& b, const move& m) noexcept;
//
//	static void undoLastMove(board& b) noexcept;
//
//	static score rateBoard(const board& b) noexcept;
//
//	static move_iterator getMoves(const board& b) noexcept;
//
//	static hashable hash(const board&) noexcept;
//}

//template<Game>
//concept Searcher {
//	Searcher(Searcher&&) {
//		axiom {
//			assert(currentState() == SearchState::PAUSED);
//		}
//	}
//	// Outputs the currently best move
//	Game::move bestMove() const;
//	// Applies a move, other possible, parallel moves are not
//	// searched any longer
//	void applyMove(const Game::move&);
//	SearchState currentState() const;
//	SearchState setState(SearchState);
//	void waitState(SearchState);
//}

template<typename Game, template<typename > class Searcher>
  class Builder
  {
  public:
    typedef typename Game::board board;
    Builder&
    setBoard (board);

    std::unique_ptr<Searcher<Game>>
    build ();
  private:
    board m_board;
  };

#define SEARCHER_IMPORT_NAMES(Game)\
	typedef typename Game::winner winner;\
	typedef typename Game::move move;\
	typedef typename Game::board board;\
	typedef typename Game::score score;\
	typedef typename Game::move_iterator move_iterator;\
	typedef typename Game::hashable hash;

/**
 * IterativeDepthSearcher analyzes any game that fulfills the
 * concept of a Game with alpha-beta-pruning, optimized for
 * fast low-depth-calculations with an iterative-depth-approach.
 */
template<typename Game>
  class IterativeWorker
  {
    SEARCHER_IMPORT_NAMES(Game)public:
    IterativeWorker() = default;
    IterativeWorker(board b);
    IterativeWorker(const IterativeWorker&) = delete;
    IterativeWorker(IterativeWorker&&);

    IterativeWorker& operator=(const IterativeWorker&) = delete;
    IterativeWorker& operator=(IterativeWorker&&);
    ~IterativeWorker();

    move bestMove() const;
    void applyMove(const move&);

    inline void resume()
      {
	m_pause = false;
	m_wait->unlock();
      }
    inline void pause()
      {
	m_wait->lock();
	m_pause = true;
      }
    inline void terminate()
      {
	if(!m_worker.joinable())
	return;
	if(m_pause)
	resume();
	terminate_sig = true;
	m_worker.join();
      }
  private:
    class interrupt
      {};
    class terminate
      {};

    typedef unsigned Depth;
    typedef score Alpha;
    typedef score Beta;
    typedef SEARCHRESULTS<move, score> SEARCHRESULT;
    typedef ENVIRONMENT<hash, move> ENVIRONMENT;
    typedef move_history<move> MoveHistory;
    typedef std::chrono::duration<float> seconds_floating;
    typedef std::chrono::system_clock Clock;

    board m_board;
    std::unique_ptr<std::mutex> m_wait;
    volatile bool m_pause;
    MoveHistory m_bestMoves;
    std::thread m_worker;
    volatile bool interrupt_sig;
    volatile bool terminate_sig;
    seconds_floating pause_time;

    void doWork(Depth start = 1);

    inline void doPause()
      {
	auto start = Clock::now();
	  {
	    std::lock_guard<std::mutex> _g
	      { *m_wait};
	  }
	auto end = Clock::now();
	pause_time += (end - start);
      }

    std::vector<MoveHistory> makeHistory(); // Initializer
    MoveHistory& getHistory(Depth);

    SEARCHRESULT startSearch(board, Depth, ENVIRONMENT& env);
    // Maximizes the score, returned in alpha
    score alpha_beta(board&, Depth, Alpha, Beta, MoveHistory&,
	ENVIRONMENT&, SEARCHSTATS&);
    // Minimizes the score
    score beta_alpha(board&, Depth, Alpha, Beta, MoveHistory&,
	ENVIRONMENT&, SEARCHSTATS&);
  };

template<typename Game>
  class IterativeDepthSearcher
  {
    SEARCHER_IMPORT_NAMES(Game)public:
    IterativeDepthSearcher() = default;
    IterativeDepthSearcher(board);
    IterativeDepthSearcher(const IterativeDepthSearcher&) = delete;
    IterativeDepthSearcher(IterativeDepthSearcher&&) = default;

    IterativeDepthSearcher& operator=(const IterativeDepthSearcher&) = delete;
    IterativeDepthSearcher& operator=(IterativeDepthSearcher&&);

    move bestMove() const;
    void applyMove(const move&);
    SearchState currentState() const;
    SearchState setState(SearchState);
    void waitState(SearchState);
  private:

    IterativeWorker<Game> m_worker;
    friend Builder<Game, IterativeDepthSearcher>;
  };

#include "Searcher.tpp"
