/*
 * TicTacticsBot
 * /TicTacticsBot/src/TranspositionLookup.tpp
 *
 *  Created on: 14.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include <utility>
#include "TranspositionLookup.hpp"


template<typename h,typename s,typename m, typename H, typename A>
TranspositionTable<h, s, m, H, A>::TranspositionTable(
		MemSize memSize, const Hasher& hash_fn,
		const Allocator& alloc_fn)
	: alloc_base<Allocator>(alloc_fn), hasher_base<H>(hash_fn),
	  memory(Alloc_B::toLowerPot2(memSize / sizeof(Node))),
	  nodes(this->alloc.allocate(memory))
{
}

template<typename h,typename s,typename m, typename H, typename A>
TranspositionTable<h, s, m, H, A>::~TranspositionTable()
{
	clear();
	this->alloc.deallocate(nodes, memory);
}

template<typename h,typename s,typename m, typename H, typename A>
void TranspositionTable<h, s, m, H, A>::store(const key& k, const value& v, const action& a)
{
	size_t hash = this->hash_fn(k) & (memory - 1);
	Node* node = nodes + hash;
	if(!node->initialized)
	{
		this->alloc.construct(node, {k, v, a}, true);
	}
	else
	{
		node->data.key = k;
		node->data.value = v;
		node->data.move = a;
	}
}

template<typename h,typename s,typename m, typename H, typename A>
bool TranspositionTable<h, s, m, H, A>::load(const key& k, value& v, action& a)
{
	size_t hash = this->hash_fn(k) & (memory - 1);
	Node* node = nodes + hash;
	if(!node->initialized)
	{
		return false;
	}
	if(!(node->data.key == k))
		return false;
	v = node->data.value;
	a = node->data.move;
	return true;
}

template<typename h,typename s,typename m, typename H, typename A>
void TranspositionTable<h, s, m, H, A>::clear()
{
	for(Node *node = nodes; node < (nodes + memory); node++)
	{
		if(node->initialized)
			this->alloc.destroy(node);
	}
}
