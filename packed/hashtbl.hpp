// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef _HASHTBL_HPP_
#define _HASHTBL_HPP_

#include <vector>
#include "pool.hpp"

struct HashKey {
	virtual unsigned long hash() = 0;
	virtual bool eq(HashKey*) = 0;
};


// HashTable implements a simple single-sized hash table.
class HashTable {

	struct Node {
		unsigned long hash;
		HashKey *key;
		void *data;
		Node *next;
	};

	std::vector<Node*> buckets;
	Pool<Node> nodes;

public:

	HashTable(unsigned int sz) : buckets(sz, 0) { }

	// find looks up the given key in the hash table and returns
	// the data value if it is found or else it returns 0.
	void *find(HashKey *key) {
		unsigned long h = key->hash();
		unsigned int ind = h % buckets.size();

		Node *p;
		for (p = buckets[ind]; p && !p->key->eq(key); p = p->next)
			;
		if (p)
			return p->data;
		return 0;
	}

	// add adds a value to the hash table.
	void add(HashKey *key, void *data) {
		Node *n = nodes.construct();
		n->key = key;
		n->data = data;
		n->hash = key->hash();
		unsigned int ind = n->hash % buckets.size();
		n->next = buckets[ind];
		buckets[ind] = n;
	}

};

#endif	// _HASHTBL_HPP_
