// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef _HEAP_HPP_
#define _HEAP_HPP_

#include "fatal.hpp"
#include <vector>
#include <limits>
#include <cassert>

template <class HeapElm> class Heap {
	std::vector< std::vector<HeapElm*> > bins;
 	int fill, min;

public:

	Heap(unsigned int sz) : bins(sz), fill(0), min(0) { }

	// push adds the given element to the heap.
	void push(HeapElm *e) {
		assert (bins.size() > e->f);

		if (e->f < min)
			min = e->f;

		e->openind = bins[e->f].size();
		bins[e->f].push_back(e);
		fill++;
	}

	// pop removes the given element from the heap.
	HeapElm *pop() {
		assert (fill > 0);

		for ( ; bins[min].empty() && (unsigned int) min < bins.size(); min++)
			;
		assert ((unsigned int) min < bins.size());

		HeapElm *e = bins[min].back();
		bins[min].pop_back();
		e->openind = -1;
		fill--;
		return e;
	
	}

	// isempty returns true if the heap is empty.
	bool isempty() {
		return fill == 0;
	}

};

#endif	// _HEAP_HPP_
