// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "fatal.hpp"
#include "hashtbl.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>

struct Tiles {
	enum {
		Width = 4,
		Height = 4,
		Ntiles = Width*Height,
	};

	struct State : public HashKey {
		int tiles[Ntiles];
		int blank;
		int h;

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

	State initial() const {
		State s;
		s.blank = -1;
		for (int i = 0; i < Ntiles; i++) {
			if (init[i] == 0)
				s.blank = i;
			s.tiles[i] = init[i];
		}
		if (s.blank < 0)
			throw Fatal("No blank tile");
		s.h = mdist(s.blank, s.tiles);
		return s;
	}

	int h(const State &s) const {
		return s.h;
	}

	bool isgoal(const State &s) const {
		return s.h == 0;
	}

	int nops(const State &s) const {
		return optab[(int) s.blank].n;
	}

	int nthop(const State &s, int n) const {
		return optab[(int) s.blank].ops[n];
	}

	struct Undo { int h, blank; };

	Edge<Tiles> apply(State &s, int newb) const {
		Edge<Tiles> e(1, newb, s.blank);
		e.undo.h = s.h;
		e.undo.blank = s.blank;

		int tile = s.tiles[newb];
		s.tiles[(int) s.blank] = tile;
		s.h += mdincr[tile][newb][(int) s.blank];
		s.blank = newb;

		return e;
	}

	void undo(State &s, const Edge<Tiles> &e) const {
		s.h = e.undo.h;
		s.tiles[(int) s.blank] = s.tiles[(int) e.undo.blank];
		s.blank = e.undo.blank;
	}

private:

	// mdist returns the Manhattan distance of the given tile array.
	int mdist(int blank, int tiles[]) const {
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
