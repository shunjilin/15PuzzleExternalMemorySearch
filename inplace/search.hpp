// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef _SEARCH_HPP_
#define _SEARCH_HPP_

#include <vector>
#include "hashtbl.hpp"

struct SearchState : public HashKey {
	// clone makes a duplicate of the given state.
	virtual SearchState *clone() = 0;
};

// An UndoToken contains information used by the search
// domain to undo the application of an operator.
struct UndoToken { };

// A Edge holds information about an edge in the
// search graph, i.e., the cost, operator and successor.
struct Edge {
	Edge(int c, int o,  int po) : op(o), pop(po), cost(c) { }

	int op;	// The operator that generated this node.
	int pop;	// The operator that would re-generate the parent.
	int cost;	// The cost of the edge.

	union {
		SearchState *kid;
		UndoToken *undo;
	} as;
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
	//
	// The kid state will be freed by the search algorithm
	// by calling the release method when it is no longer
	// needed.
	//
	// The as field of the returned edges contains a pointer
	// to the SearchState, i.e., the kid field should be used
	// in the union instead of the undo field.
	virtual std::vector<Edge> expand(SearchState*) = 0;

	// release frees up any memory associated with the
	// given SearchState.
	virtual void release(SearchState*) = 0;

	// nops returns the number of operators applicable
	// in the current state.
	virtual int nops(SearchState*) = 0;

	// nthop returns the nth operator for the given state.
	virtual int nthop(SearchState*, int) = 0;

	// apply applies the oprerator to the given state
	// and returns the resulting edge.
	//
	// The undo field of the as union in the edge is valid
	// and can be used to undo the application of the
	// operator.
	virtual Edge apply(SearchState*, int) = 0;

	// undo reverts the application of the operator that
	// created the given edge.  Any memory allocated
	// for the undo field of the edge must be freed by
	// the search domain.
	virtual void undo(SearchState*, Edge&) = 0;
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