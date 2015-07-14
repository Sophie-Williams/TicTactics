/*
 * TicTacticsBot
 * /TicTacticsBot/src/Searcher.tpp
 *
 *  Created on: 12.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include "Searcher.hpp"
#include <utility>
#include <chrono>
#include <iostream>
#include <iterator>

namespace _detail {

template<typename Game>
struct temporary_move {
	typedef typename Game::board board;
	typedef typename Game::move move;
	typedef typename Game::winner winner;

	temporary_move(board& b, const move& mv)
		: m_board(b), m_win(Game::applyMove(b, mv))
	{
	}
	inline winner getWinner() const
	{
		return m_win;
	}
	~temporary_move()
	{
		Game::undoLastMove(m_board);
	}

	board& m_board;
	winner m_win;
};

}  // namespace _detail

template<typename G, template<typename> class S>
auto Builder<G, S>::setBoard(board b)
-> Builder&
{
	this->m_board = std::move(b);
	return *this;
}

template<typename G, template<typename> class S>
S<G> Builder<G, S>::build()
{
	return S<G>{ this->m_board };
}

template<typename G>
IterativeDepthSearcher<G>::IterativeDepthSearcher(board b)
	: m_worker(std::move(b))
{
}

template<typename G>
auto IterativeDepthSearcher<G>::bestMove() const
-> move
{
}

template<typename G>
void IterativeDepthSearcher<G>::applyMove(const move& move)
{

}

template<typename G>
SearchState IterativeDepthSearcher<G>::currentState() const
{
	return SearchState::PAUSED;
}

template<typename G>
SearchState IterativeDepthSearcher<G>::setState(SearchState state)
{
	if(state == SearchState::RUNNING) {
		this->m_worker.resume();
		return SearchState::RUNNING;
	}
	this->m_worker.pause();
	return SearchState::PAUSED;
}

template<typename G>
void IterativeDepthSearcher<G>::waitState(SearchState state)
{

}

template<typename G>
IterativeWorker<G>::IterativeWorker(board b)
	: m_board(std::move(b)), m_wait(new std::mutex{}), m_bestMoves(),
	  m_worker(doWork, this, 1), interrupt(false)
{
	m_wait->lock();
}

template<typename G>
IterativeWorker<G>::IterativeWorker(IterativeWorker&& o)
	: m_board(std::move(o.m_board)), m_wait(std::move(o.m_wait)),
	  m_bestMoves(std::move(o.m_bestMoves)), m_worker(std::move(o.m_worker)),
	  interrupt(o.interrupt)
{
}

template<typename G>
IterativeWorker<G>::~IterativeWorker()
{
	interrupt = true;
	m_worker.join();
}

template<typename G>
auto IterativeWorker<G>::makeHistory()
-> std::vector<MoveHistory>
{
	MoveHistory _original = {};
	_original.reserve(G::expectedDepth());
	return std::vector<MoveHistory>{G::expectedDepth(), _original};
}

template<typename G>
auto IterativeWorker<G>::getHistory(Depth d)
-> MoveHistory&
{

	static std::vector<MoveHistory> allHistories = makeHistory();
	return allHistories[d];
}

template<typename G>
void IterativeWorker<G>::doWork(Depth n)
try {
	typedef std::chrono::duration<float> seconds_floating;
	ENVIRONMENT env { *m_wait, interrupt, {} };
	do {
		SEARCHRESULT res = startSearch(m_board, n, env);
		{
			std::lock_guard<std::mutex> _g{ *m_wait };
			m_bestMoves = std::move(res.moves);
		}
		auto timediff = std::chrono::duration_cast<seconds_floating>(
				res.stats.end - res.stats.start);
		std::cout << "Iterative Worker Report:" << std::endl
				<< "# Depth: " << res.stats.maxdepth << std::endl
				<< "# Time: " << timediff.count() << std::endl
				<< "# Nodes: " << res.stats.encountered_nodes << std::endl
				<< "# N/t: " << res.stats.encountered_nodes
						/ timediff.count() << std::endl
				<< "# Pruned: " << res.stats.times_pruned << std::endl
				<< "# Sequence: ";
		std::ostream_iterator<move> cbound(std::cout, " ");
		std::copy(m_bestMoves.rbegin(), m_bestMoves.rend(), cbound);
		if(!(res.perfectScore < G::infinity)) {
			std::cout << "\nWIN!!!!" << std::endl;
			break;
		}
		if(!(res.perfectScore > -G::infinity)) {
			std::cout << "\nLOSS!!!!" << std::endl;
			break;
		}
		std::cout << std::endl;
	} while(n++ < 40);
} catch (typename IterativeWorker::interrupt& i) {
	std::cout << "interrupted" << std::endl;
	return; // No-op
}

template<typename G>
auto IterativeWorker<G>::bestMove() const
-> move
{
	std::lock_guard<std::mutex> _g{ m_wait };
	return m_bestMoves.back();
}

template<typename G>
auto IterativeWorker<G>::startSearch(board b, Depth maxDepth, ENVIRONMENT& env)
-> SEARCHRESULT
{
	SEARCHRESULT result = {};
	result.stats.maxdepth = maxDepth;
	result.stats.start = std::chrono::high_resolution_clock::now();

	if(maxDepth == 0) {
		result.stats.end = result.stats.start;
		return result;
	}
	score best = alpha_beta(b, maxDepth, -G::infinity, G::infinity,
			result.moves, env, result.stats);

	result.perfectScore = best;
	result.stats.end = std::chrono::high_resolution_clock::now();
	return result;
}

template<typename G>
auto IterativeWorker<G>::alpha_beta(board& board, Depth depth, Alpha alpha,
		Beta beta, MoveHistory& history, ENVIRONMENT& env, SEARCHSTATS& stats)
-> score
{
	// Maximizes
	stats.encountered_nodes++;
	if(depth == 0) {
		history.clear();
		return G::rateBoard(board);
	}
	{
		std::lock_guard<std::mutex> _g{env.fence};
		if(env.interrupt)
			throw typename IterativeWorker::interrupt{};
	}
	MoveHistory& tempMoves = getHistory(depth);
	score bestRating = -G::infinity; // If we can't make moves, we have lost
	for(move_iterator it = G::getMoves(board); it != G::end_of_moves;
			++it)
	{
		move m = *it;
		_detail::temporary_move<G> apply(board, m);
		score currentRating;
		switch(apply.getWinner())
		{
		case G::player1:
			history.clear();
			history.push_back(m);
			return G::infinity;
		case G::player2:
			continue;
		case G::both:
			currentRating = 0;
			break;
		default:
			currentRating = beta_alpha(board, depth - 1, alpha, beta,
					tempMoves, env, stats);
		}
		if(currentRating > bestRating) {
			bestRating = currentRating;
			std::swap(history, tempMoves);
			history.push_back(m);
		}
		if(bestRating > alpha) {
			alpha = bestRating; // Maximize
		}
		if(!(alpha < beta)) { // Prune!
			stats.times_pruned++;
			return bestRating; // Maximum weight, so nobody takes this
		}
		if(!(bestRating < G::infinity))
			return bestRating;
	}
	return bestRating;
}
// Minimizes the score
template<typename G>
auto IterativeWorker<G>::beta_alpha(board& board, Depth depth, Alpha alpha,
		Beta beta, MoveHistory& history, ENVIRONMENT& env, SEARCHSTATS& stats)
-> score
{
	// Maximizes
	stats.encountered_nodes++;
	if(depth == 0) {
		history.clear();
		return G::rateBoard(board);
	}
	{
		std::lock_guard<std::mutex> _g{env.fence};
		if(env.interrupt)
			throw typename IterativeWorker::interrupt{};
	}
	MoveHistory& tempMoves = getHistory(depth);
	score bestRating = G::infinity; // If we can't make moves, we have lost
	for(move_iterator it = G::getMoves(board); it != G::end_of_moves;
			++it)
	{
		move m = *it;
		_detail::temporary_move<G> apply(board, m);
		score currentRating;
		switch(apply.getWinner())
		{
		case G::player1:
			continue;
		case G::player2:
			history.clear();
			history.push_back(m);
			return -G::infinity; // Found a win
		case G::both:
			currentRating = 0;
			break;
		default:
			currentRating = alpha_beta(board, depth - 1, alpha, beta,
					tempMoves, env, stats);
		}
		if(currentRating < bestRating) {
			bestRating = currentRating;
			std::swap(history, tempMoves);
			history.push_back(m);
		}
		if(bestRating < beta) {
			beta = bestRating; // Minimize
		}
		if(!(beta > alpha)) { // Prune!
			stats.times_pruned++;
			return bestRating; // Maximum weight, so nobody takes this
		}
		if(!(bestRating > -G::infinity))
			return bestRating;
	}
	return bestRating;
}

