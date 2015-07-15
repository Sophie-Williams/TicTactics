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
IterativeDepthSearcher<G>&
IterativeDepthSearcher<G>::operator=(IterativeDepthSearcher&& o)
{
	m_worker = std::move(o.m_worker);
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
	return m_worker.bestMove();
}

template<typename G>
void IterativeDepthSearcher<G>::applyMove(const move& move)
{
	m_worker.applyMove(move);
}

template<typename G>
SearchState IterativeDepthSearcher<G>::currentState() const
{
	// FIXME: correct results here (not that important)
	return SearchState::PAUSED;
}

template<typename G>
SearchState IterativeDepthSearcher<G>::setState(SearchState state)
{
	switch (state) {
		case SearchState::RUNNING:
			this->m_worker.resume();
			return SearchState::RUNNING;
		case SearchState::PAUSED:
			this->m_worker.pause();
			return SearchState::PAUSED;
		case SearchState::STOPPED:
			this->m_worker.terminate();
			return SearchState::STOPPED;
	}
	return SearchState::STOPPED;
}

template<typename G>
void IterativeDepthSearcher<G>::waitState(SearchState state)
{
	setState(state);
}

template<typename G>
IterativeWorker<G>::IterativeWorker(board b)
	: m_board(std::move(b)), m_wait(new std::mutex{}), m_pause(true),
	  m_bestMoves(), m_worker(&IterativeWorker::doWork, this, 1),
	  interrupt_sig(false), terminate_sig(false)
{
	m_wait->lock();
}

template<typename G>
IterativeWorker<G>::IterativeWorker(IterativeWorker&& o)
	: m_board(std::move(o.m_board)), m_wait(std::move(o.m_wait)),
	  m_pause(o.m_pause), m_bestMoves(std::move(o.m_bestMoves)),
	  m_worker(std::move(o.m_worker)), interrupt_sig(o.interrupt_sig),
	  terminate_sig(o.terminate_sig)
{
}

template<typename G>
IterativeWorker<G>&
IterativeWorker<G>::operator =(IterativeWorker&& o)
{
	this->terminate();
	m_board = std::move(o.m_board);
	m_wait = std::move(o.m_wait);
	m_pause = o.m_pause;
	m_bestMoves = std::move(o.m_bestMoves);
	m_worker = std::move(o.m_worker);
	interrupt_sig = o.interrupt_sig;
	return *this;
}

template<typename G>
IterativeWorker<G>::~IterativeWorker()
{
	terminate();
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
	ENVIRONMENT env {
#ifdef LOOKUPTABLE
		{}
#endif
	};
	auto start = Clock::now();
	do {
		try {
			SEARCHRESULT res = startSearch(m_board, n, env);
			if(m_pause)
				doPause();
			m_bestMoves = std::move(res.moves);

			auto timediff = std::chrono::duration_cast<seconds_floating>(
					res.stats.end - res.stats.start);
			auto timediffSum = std::chrono::duration_cast<seconds_floating>(
					res.stats.end - start) - pause_time;
			std::clog << ":Iterative Worker Report:" << std::endl
					<< ":# Depth: " << res.stats.maxdepth << std::endl
					<< ":# Time (this run): " << timediff.count() << std::endl
					<< ":# Time (total): " << timediffSum.count() << std::endl
					<< ":# Nodes: " << res.stats.encountered_nodes << std::endl
					<< ":# N/t: " << res.stats.encountered_nodes
							/ timediff.count() << std::endl
					<< ":# Pruned: " << res.stats.times_pruned << std::endl
#ifdef LOOKUPTABLE
					<< ":# Lookup-Hits (this run): " << env.table.getHits() << std::endl
					<< ":# Lookup-Used: " << env.table.getUsed() << std::endl
#endif
					<< ":# Rating: " << res.perfectScore << std::endl
					<< ":# Sequence: ";
			std::ostream_iterator<move> cbound(std::clog, " ");
			std::copy(m_bestMoves.rbegin(), m_bestMoves.rend(), cbound);
			if(!(res.perfectScore < G::infinity)) {
				std::clog << "\n:WIN!!!!" << std::endl;
				break;
			}
			if(!(res.perfectScore > -G::infinity)) {
				std::clog << "\n:LOSS!!!!" << std::endl;
				break;
			}
			std::clog << std::endl;
#ifdef LOOKUPTABLE
			env.table.resetHits();
#endif
		} catch (interrupt& i) {
			std::clog << ":interrupted, repeating run" << std::endl;
			n--;
		}
	} while(n++ < 40);
} catch (typename IterativeWorker::terminate& i) {
	std::clog << ":terminated" << std::endl;
	return; // No-op
}

template<typename G>
auto IterativeWorker<G>::bestMove() const
-> move
{
	std::lock_guard<std::mutex> _g{ m_wait };
	if(m_bestMoves.size() > 0)
		return m_bestMoves.back();
	return G::illegal_move;
}

template<typename G>
void IterativeWorker<G>::applyMove(const move& mv)
{
	// TODO: logic here
}

template<typename G>
auto IterativeWorker<G>::startSearch(board b, Depth maxDepth, ENVIRONMENT& env)
-> SEARCHRESULT
{
	SEARCHRESULT result = {};
	result.stats.maxdepth = maxDepth;
	result.stats.start = Clock::now();

	if(maxDepth == 0) {
		result.stats.end = result.stats.start;
		return result;
	}
	score best = alpha_beta(b, maxDepth, -G::infinity, G::infinity,
			result.moves, env, result.stats);

	result.perfectScore = best;
	result.stats.end = Clock::now();
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
	if(terminate_sig)
		throw typename IterativeWorker::terminate{};
	if(interrupt_sig)
		throw typename IterativeWorker::interrupt{};
	if(m_pause)
		doPause();
	MoveHistory& tempMoves = getHistory(depth);
	score bestRating = -G::infinity; // If we can't make moves, we have lost
#ifdef LOOKUPTABLE
	move best, overallbest = G::illegal_move;
	hash boardHash = G::hash(board);
	bool initialized = env.table.load(boardHash, best);
	if(initialized)
	{
		do{
		overallbest = best;
		_detail::temporary_move<G> apply(board, best);
		switch(apply.getWinner())
		{
		case G::player1:
			history.clear();
			history.push_back(best);
			return G::infinity;
		case G::player2:
			continue;
		case G::both:
			bestRating = 0;
			break;
		default:
			bestRating = beta_alpha(board, depth - 1, alpha, beta,
					tempMoves, env, stats);
		}
		std::swap(history, tempMoves);
		history.push_back(best);
		if(!(bestRating < G::infinity))
		{
			return G::infinity;
		}
		if(bestRating > alpha)
		{
			alpha = bestRating;
		}
		if(!(alpha < beta)) { // Prune!
			stats.times_pruned++;
			return bestRating; // Maximum weight, so nobody takes this
		}
		} while(false);
	}
#endif
	for(move_iterator it = G::getMoves(board); it != G::end_of_moves;
			++it)
	{
		move m = *it;
#ifdef LOOKUPTABLE
		if(initialized && m == best)
			continue;
#endif
		_detail::temporary_move<G> apply(board, m);
		score currentRating;
		switch(apply.getWinner())
		{
		case G::player1:
			history.clear();
			history.push_back(m);
#ifdef LOOKUPTABLE
			env.table.store(boardHash, m);
#endif
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
#ifdef LOOKUPTABLE
			overallbest = m;
#endif
		}
		if(bestRating > alpha) {
			alpha = bestRating; // Maximize
		}
		if(!(alpha < beta)) { // Prune!
			stats.times_pruned++;
			return bestRating; // Maximum weight, so nobody takes this
		}
		if(!(currentRating < G::infinity)) {
			break;
		}
	}
#ifdef LOOKUPTABLE
	if(!(overallbest == G::illegal_move))
		env.table.store(boardHash, overallbest);
#endif
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
	if(terminate_sig)
		throw typename IterativeWorker::terminate{};
	if(interrupt_sig)
		throw typename IterativeWorker::interrupt{};
	if(m_pause)
		doPause();
	MoveHistory& tempMoves = getHistory(depth);
	score bestRating = G::infinity; // If we can't make moves, we have lost
#ifdef LOOKUPTABLE
	move best, overallbest = G::illegal_move;
	hash boardHash = G::hash(board);
	bool initialized = env.table.load(boardHash, best);
	if(initialized)
	{
		do{
		overallbest = best;
		_detail::temporary_move<G> apply(board, best);
		switch(apply.getWinner())
		{
		case G::player1:
			continue;
		case G::player2:
			history.clear();
			history.push_back(best);
			return -G::infinity; // Found a win
		case G::both:
			bestRating = 0;
			break;
		default:
			bestRating = alpha_beta(board, depth - 1, alpha, beta,
					tempMoves, env, stats);
		}

		std::swap(history, tempMoves);
		history.push_back(best);

		if(!(bestRating > -G::infinity))
		{
			return -G::infinity;
		}
		if(bestRating < beta)
		{
			beta = bestRating;
		}
		if(!(alpha < beta)) { // Prune!
			stats.times_pruned++;
			return bestRating; // Maximum weight, so nobody takes this
		}
		}while(false); // For continue;
	}
#endif
	for(move_iterator it = G::getMoves(board); it != G::end_of_moves;
			++it)
	{
		move m = *it;
#ifdef LOOKUPTABLE
		if(initialized && m == best)
			continue;
#endif
		_detail::temporary_move<G> apply(board, m);
		score currentRating;
		switch(apply.getWinner())
		{
		case G::player1:
			continue;
		case G::player2:
			history.clear();
			history.push_back(m);
#ifdef LOOKUPTABLE
			env.table.store(boardHash, m);
#endif
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
#ifdef LOOKUPTABLE
			overallbest = m;
#endif
		}
		if(bestRating < beta) {
			beta = bestRating; // Minimize
		}
		if(!(beta > alpha)) { // Prune!
			stats.times_pruned++;
			return bestRating; // Maximum weight, so nobody takes this
		}
		if(!(currentRating > -G::infinity)) {
			break;
		}
	}
#ifdef LOOKUPTABLE
	if(!(overallbest == G::illegal_move))
		env.table.store(boardHash, overallbest);
#endif
	return bestRating;
}

