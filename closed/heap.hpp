// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef _HEAP_HPP_
#define _HEAP_HPP_

#include "fatal.hpp"
#include <vector>
#include <limits>
#include <cassert>

struct HeapElm {
	virtual bool pred(HeapElm*) = 0;
 	virtual void setindex(int) = 0;
};

// Heap implements a simple binary heap.
class Heap {
	std::vector<HeapElm*> heap;

	int parent(int i) { return (i - 1) / 2; }

	int left(int i) { return 2 * i + 1; }

	int right(int i) { return 2 * i + 2; }

	void swap(int i, int j) {
		HeapElm *e = heap[i];
		heap[i] = heap[j];
		heap[j] = e;
		heap[i]->setindex(i);
		heap[j]->setindex(j);
	}

	int pullup(int i) {
		if (i == 0)
			return i;
		unsigned int p = parent(i);
		if (heap[i]->pred(heap[p])) {
			swap(i, p);
			return pullup(p);
		}
		return i;
	}

	int pushdown(int i) {
		int l = left(i), r = right(i);
		int s = i;
		if ((unsigned int) l < heap.size() && heap[l]->pred(heap[s]))
			s = l;
		if ((unsigned int) r < heap.size() && heap[r]->pred(heap[s]))
			s = r;
		if (s != i) {
			swap(s, i);
			return pushdown(s);
		}
		return i;
	}

public:

	// push adds the given element to the heap.
	void push(HeapElm *e) {
		if (heap.size() == (unsigned int) std::numeric_limits<int>::max() - 1)
			throw Fatal("The heap is too big");
		heap.push_back(e);
		e->setindex(heap.size()-1);
		pullup(heap.size()-1);
	}

	// pop removes the given element from the heap.
	HeapElm *pop() {
		if (heap.size() == 0)
			return 0;
		HeapElm *e = heap[0];
		if (heap.size() > 1) {
			heap[0] = heap.back();
			heap.pop_back();
			pushdown(0);
		} else {
			heap.pop_back();
		}
		e->setindex(-1);
		return e;
	}

	// update re-assesses the position of a node in the heap.
	void update(int i) {
		assert (i >= 0);
		assert ((unsigned int) i < heap.size());
		i = pullup(i);
		pushdown(i);
	}

	// isempty returns true if the heap is empty.
	bool isempty() {
		return heap.size() == 0;
	}

};

#endif	// _HEAP_HPP_
