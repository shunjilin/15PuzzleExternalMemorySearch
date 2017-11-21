// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef _HASHTBL_HPP_
#define _HASHTBL_HPP_

#include <vector>
#include "pool.hpp"
#include <cstddef>

template <class Node> struct HashEntry {
	unsigned long hash;
	Node *next;
};

// HashTable implements a simple single-sized hash table.
template <class Key, class Node> class HashTable {

	std::vector<Node*> buckets;
    std::size_t n_entries;

public:

	HashTable(unsigned int sz) : buckets(sz, 0) { }

	// find looks up the given key in the hash table and returns
	// the data value if it is found or else it returns 0.
	Node *find(Key &key) {
		unsigned long h = key.hash();
		unsigned int ind = h % buckets.size();

		Node *p;
		for (p = buckets[ind]; p && !p->key().eq(key); p = p->hashentry().next)
			;
		return p;
	}

	// add adds a value to the hash table.
	void add(Node *n) {
		unsigned long hash = n->key().hash();
		unsigned int ind = hash % buckets.size();
		n->hashentry().hash = hash;
		n->hashentry().next = buckets[ind];
		buckets[ind] = n;
                ++n_entries;
	}

    std::size_t get_n_entries() const {
        return n_entries;
    }

};

#endif	// _HASHTBL_HPP_
