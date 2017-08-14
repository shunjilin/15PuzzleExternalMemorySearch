// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "utils.hpp"

class Idastar : public SearchAlg {
	std::vector<SearchState*> path;
	int bound, minoob;

public:

	Idastar(SearchDomain &d) : SearchAlg(d) { }

	virtual std::vector<SearchState*> search(SearchState *root) {
		bound = dom.h(root);


		dfrowhdr(stdout, "iteration", 4, "number", "bound",
			"nodes expanded", "nodes generated");
		unsigned int n = 0;
		do {
			minoob = -1;
			dfs(root, 0, -1);
			n++;
			dfrow(stdout, "iteration", "uduu", (unsigned long) n, (long) bound,
				this->expd, this->gend);
			bound = minoob;
		} while (path.size() == 0);

		return path;
	}

private:

	bool dfs(SearchState *n,  int cost, int pop) {
		int f = cost + dom.h(n);

		if (f <= bound && dom.isgoal(n)) {
			path.push_back(n->clone());
			return true;
		}

		if (f > bound) {
			if (minoob < 0 || f < minoob)
				minoob = f;
			return false;
		}

		this->expd++;
		int nops = dom.nops(n);
		for (int i = 0; i < nops; i++) {
			int op = dom.nthop(n, i);
			if (op == pop)
				continue;

			this->gend++;
			Edge e = dom.apply(n, op);
			bool goal = dfs(n, e.cost + cost, e.pop);
			dom.undo(n, e);
			if (goal) {
				path.push_back(n->clone());
				return true;
			}
		}

		return false;
	}
};