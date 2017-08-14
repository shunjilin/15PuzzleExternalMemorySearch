// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "fatal.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>

struct Tiles : public SearchDomain {
	enum {
		Width = 4,
		Height = 4,
		Ntiles = Width*Height,
	};

	struct State : public SearchState {
		char tiles[Ntiles];
		char blank;
		char h;

		// Rich's hash function.
		virtual unsigned long hash() {
			tiles[(int) blank] = 0;
			unsigned long h = tiles[0];
			for (int i = 1; i < Ntiles; i++)
				h = h * 3 + tiles[i];
			return h;
		}

		virtual bool eq(HashKey *h) {
			State *o = static_cast<State*>(h);
			if (blank != o->blank)
				return false;
			for (int i = 0; i < Ntiles; i++) {
				if (i != blank && tiles[i] != o->tiles[i])
					return false;
			}
			return true;
		}
	};

	// Tiles constructs a new instance by reading
	// the initial state from the given file which is
	// expected to be in Wheeler's tiles instance
	// format.
	Tiles(FILE*);

	virtual SearchState *initial() {
		State *s = new State();
		s->blank = -1;
		for (int i = 0; i < Ntiles; i++) {
			if (init[i] == 0)
				s->blank = i;
			s->tiles[i] = init[i];
		}
		if (s->blank < 0)
			throw Fatal("No blank tile");
		s->h = mdist(s->blank, s->tiles);
		return s;
	}

	virtual int h(SearchState *ss) {
		State *s = static_cast<State*>(ss);
		return s->h;
	}

	virtual bool isgoal(SearchState *ss) {
		State *s = static_cast<State*>(ss);
		return s->h == 0;
	}

	virtual std::vector<Edge> expand(SearchState *ss) {
		State *s = static_cast<State*>(ss);
		std::vector<Edge> kids;
		int n = optab[(int) s->blank].n;
		assert (n <= 4);
		for (int i = 0; i < n; i++)
			kids.push_back(kid(*s, optab[(int) s->blank].ops[i]));

		return kids;
	}

	virtual void release(SearchState *s) {
		delete s;
	}

private:

	// kid returns a new Edge for the child of s with
	// the blank moved to newb.
	Edge kid(const State &s, int newb) {
		int tile = s.tiles[newb];

		State *kid = new State();
		for (int i = 0; i < Ntiles; i++)
			kid->tiles[i] = s.tiles[i];
		kid->tiles[newb] = 0;
		kid->tiles[(int) s.blank] = s.tiles[newb];
		kid->blank = newb;

		assert(mdincr[tile][newb][s.blank] != -100); 	// uninitialized increment
		assert (s.blank != newb);
		kid->h = s.h + mdincr[tile][newb][(int) s.blank];

		return Edge(1, newb, s.blank, static_cast<SearchState*>(kid));
	}

	// mdist returns the Manhattan distance of the given tile array.
	int mdist(int blank, char tiles[]) const {
		int sum = 0;
		for (int i = 0; i < Ntiles; i++) {
			if (i == blank)
				continue;
			int row = i / Width, col = i % Width;
			int grow = tiles[i] / Width, gcol = tiles[i] % Width;
			sum += abs(gcol - col) + abs(grow - row);
		}
		return sum;
	}

	// initmd initializes the md and mdincr tables.
	void initmd();

	// initoptob initializes the operator table, optab.
	void initoptab();

	// init is the initial tile positions.
	int init[Ntiles];

	// md is indexed by tile and location.  Each entry
	int md[Ntiles][Ntiles];

	// mdincr is indexed by tile, source location, and
	// destination location.  Each entry is the change
	// in Manhattan distance for that particular move.
	int mdincr[Ntiles][Ntiles][Ntiles];

	// optab is indexed by the blank position.  Each
	// entry is a description of the possible next
	// blank positions.
	struct { int n, ops[4]; } optab[Ntiles];
};
