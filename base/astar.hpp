// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "utils.hpp"
#include "hashtbl.hpp"
#include "heap.hpp"

class Astar : public SearchAlg {
	HashTable closed;
	Heap open;
	std::vector<SearchState*> path;

	struct Node : public HeapElm {
		char f, g, pop;
		Node *parent;
		SearchState *state;

		virtual bool pred(HeapElm *_o) {
			Node *o = static_cast<Node*>(_o);
			if (f == o->f)
				return g > o->g;
			return f < o->f;
		}

 		virtual void setindex(int i) { }
	};

public:
	Astar(SearchDomain &d) : SearchAlg(d), closed(512927357) {}

	std::vector<SearchState*> search(SearchState *init) {
		open.push(wrap(init, 0, 0, -1));

		while (!open.isempty() && path.size() == 0) {
			Node *n = static_cast<Node*>(open.pop());
			if (closed.find(n->state)) {
				dom.release(n->state);
				delete n;
				continue;
			}

			if (dom.isgoal(n->state)) {
				for (Node *p = n; p; p = p->parent)
					path.push_back(p->state);
				break;
			}

			closed.add(n->state, static_cast<void*>(n));

			std::vector<Edge> kids = dom.expand(n->state);
			this->expd++;
			this->gend += kids.size();
			for (unsigned int i = 0; i < kids.size(); i++) {
				if (kids[i].op == n->pop) {
					dom.release(kids[i].kid);
					continue;
				}
				open.push(wrap(kids[i].kid, n, kids[i].cost, kids[i].pop));
			}
		}
		return path;
	}

	Node *wrap(SearchState *s, Node *p, int c, int pop) {
		Node *n = new Node();
		n->g = c;
		if (p)
			n->g += p->g;
		n->f = n->g + dom.h(s);
		n->pop = pop;
		n->parent = p;
		n->state = s;
		return n;
	}
};
