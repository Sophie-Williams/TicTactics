/*
 * TicTacticsBot
 * /TicTacticsBot/src/TranspositionLookup.tpp
 *
 *  Created on: 14.07.2015
 *      Author: WorldSEnder
 */

#pragma once

#include <utility>
#include <iostream>
#include "TranspositionLookup.hpp"

template<typename h, typename m, typename H, typename A>
  TranspositionTable<h, m, H, A>::TranspositionTable (MemSize memSize,
						      const Hasher& hash_fn,
						      const Allocator& alloc_fn) :
      alloc_base<Allocator> (alloc_fn), hasher_base<H> (hash_fn), memory (
	  Alloc_B::toLowerPot2 (memSize / sizeof(Node)) - 1), nodes (
	  this->alloc.allocate (memory + 1)), initialized (0), hits (0)
  {
    std::cout << "Using memSize: " << memSize << " with " << memory << " data"
	<< std::endl;
    std::cout << sizeof(Node) << std::endl;
  }

template<typename h, typename m, typename H, typename A>
  TranspositionTable<h, m, H, A>::~TranspositionTable ()
  {
    clear ();
    this->alloc.deallocate (nodes, memory + 1);
  }

template<typename h, typename m, typename H, typename A>
  void
  TranspositionTable<h, m, H, A>::store (const key& k, const action& a)
  {
    size_t hash = this->hash_fn (k) & (memory);
    Node* node = nodes + hash;
    if (!node->initialized)
      {
	this->alloc.construct (node, k, a);
	initialized++;
      }
    else
      {
	node->data.exact_key = k;
	node->data.move = a;
      }
  }

template<typename h, typename m, typename H, typename A>
  bool
  TranspositionTable<h, m, H, A>::load (const key& k, action& a)
  {
    size_t hash = this->hash_fn (k) & (memory);
    Node* node = nodes + hash;
    if (!node->initialized)
      {
	return false;
      }
    if (!(node->data.exact_key == k))
      return false;
    hits++;
    a = node->data.move;
    return true;
  }

template<typename h, typename m, typename H, typename A>
  void
  TranspositionTable<h, m, H, A>::clear ()
  {
    initialized = 0;
    hits = 0;
    for (Node *node = nodes; node < (nodes + memory + 1); node++)
      {
	if (node->initialized)
	  this->alloc.destroy (node);
      }
  }
