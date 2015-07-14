/*
 * TicTacticsBot
 * /TicTacticsBot/src/defines.hpp
 *
 *  Created on: 12.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include <chrono>
#include <mutex>
#include <vector>
#include <inttypes.h>
#include "TranspositionLookup.hpp"

template<typename move>
using move_history = std::vector<move>;

struct SEARCHSTATS {
	std::chrono::system_clock::time_point start, end;

	unsigned maxdepth;
	uint64_t encountered_nodes;
	uint64_t times_pruned;
};

template<typename move, typename score>
struct SEARCHRESULTS {
	score perfectScore;
	move_history<move> moves;
	SEARCHSTATS stats;
};

template<typename hash, typename score, typename move>
struct ENVIRONMENT {
	std::mutex& fence; // For pausing
	volatile bool& interrupt; // For canceling

	TranspositionTable<hash, score, move> table;
};

enum class SearchState {
	RUNNING, PAUSED
};
