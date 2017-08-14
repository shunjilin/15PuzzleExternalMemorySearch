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
			path.push_back(n);
			return true;
		}

		if (f > bound) {
			if (minoob < 0 || f < minoob)
				minoob = f;
			return false;
		}

		std::vector<Edge> kids = dom.expand(n);
		this->expd++;
		this->gend += kids.size();
		for (unsigned int i = 0; i < kids.size(); i++) {
			if (kids[i].op == pop) {
				dom.release(kids[i].kid);
				continue;
			}
			if (dfs(kids[i].kid, kids[i].cost + cost, kids[i].pop)) {
				path.push_back(n);
				for (i += 1; i < kids.size(); i++)
					dom.release(kids[i].kid);
				return true;
			}
			dom.release(kids[i].kid);
		}

		return false;
	}
};