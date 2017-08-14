// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef _SEARCH_HPP_
#define _SEARCH_HPP_

#include <vector>
#include "hashtbl.hpp"

struct SearchState : public HashKey { };

// A Edge holds information about an edge in the
// search graph, i.e., the cost, operator and successor.
struct Edge {
	Edge(int c, int o,  int po, SearchState*k) : op(o), pop(po), cost(c), kid(k) { }

	int op;	// The operator that generated this node.
	int pop;	// The operator that would re-generate the parent.
	int cost;	// The cost of the edge.

	SearchState *kid;
};

// SearchDomain defines the common interface to all
// search domains.
struct SearchDomain {
	// initial returns the initial search state.
	virtual SearchState* initial() = 0;

	// h returns a heuristic cost-to-go estimate for the given state.
	virtual int h(SearchState*) = 0;

	// isgoal returns true if the state is a goal state.
	virtual bool isgoal(SearchState*) = 0;

	// expand returns the successors of the given state.
	virtual std::vector<Edge> expand(SearchState*) = 0;

	// release frees up any memory associated with the
	// given SearchState.
	virtual void release(SearchState*) = 0;
};


// SearchAlg defines the common interface to all search
// algorithms.
struct SearchAlg {

	// SearchAlg constructs a new search algorithm that
	// searches in the given domain.
	SearchAlg(SearchDomain &d) : dom(d), expd(0), gend(0) { }

	// search searches for a goal from the initial state
	// The return value is the path to the goal which can
	// be empty if the goal was no found.
	virtual std::vector<SearchState*> search(SearchState*) = 0;

	// dom is the domain over which this search is defined.
	SearchDomain &dom;

	// expd and gend are the number of states that have been
	// expanded and generated respectively.
	unsigned long expd, gend;
};

#endif	// _SEARCH_HPP_