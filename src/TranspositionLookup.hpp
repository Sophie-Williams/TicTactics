/*
 * TicTacticsBot
 * /TicTacticsBot/src/TranspositionLookup.hpp
 *
 *  Created on: 14.07.2015
 *      Author: WorldSEnder
 */

#pragma once
#include <inttypes.h>
#include <memory>

template<typename K, typename V, typename M>
struct TranspositionNode
{
	struct {
		K exact_key;
		V score; // score of that
		M move; // best move
	} data;
	bool initialized; // a member we can write on in unitialized context
};

typedef unsigned long long MemSize;

template<typename A>
struct alloc_base
{
protected:
	typedef A Allocator;
	alloc_base(Allocator allocator) : alloc(allocator) {};
	Allocator alloc;

	inline static MemSize toLowerPot2(MemSize arg)
	{
		return 1 << (63 - __builtin_clzll(arg));
	}
};

template<typename H>
struct hasher_base
{
protected:
	typedef H Hasher;
	hasher_base(Hasher hash) : hash_fn(hash) {};
	Hasher hash_fn;
};

template<typename hashable, typename score,
		typename move,
		typename H=std::hash<hashable>,
		typename A=std::allocator<hashable>>
class TranspositionTable :
		alloc_base<typename A::template rebind<TranspositionNode<hashable, score, move>>::other>,
		hasher_base<H>
{
	typedef H Hasher;
	typedef TranspositionNode<hashable, score, move> Node;
	typedef typename A::template rebind<Node>::other Allocator;
	typedef alloc_base<Allocator> Alloc_B;
	typedef hasher_base<H> Hasher_B;
public:
	typedef hashable key;
	typedef score value;
	typedef move action;

	TranspositionTable(MemSize = 0x8000000, const Hasher& = Hasher{},
			const Allocator& = Allocator{});
	TranspositionTable(const TranspositionTable&) = delete;
	~TranspositionTable();
	/**
	 * Stores a key along with it's value.
	 * If there is already an entry with that key (or the same
	 * hash) it is overridden.
	 */
	void store(const key&, const value&, const action&);
	/**
	 * Loads a value from the hashtable, if available.
	 * If not available, value is left unchanged.
	 *
	 * @return true if key was found.
	 */
	bool load(const key&, value&, action&);
	/**
	 * Clears the hashtable.
	 */
	void clear();
private:
	MemSize memory;
	Node* nodes;
};

#include "TranspositionLookup.tpp"

