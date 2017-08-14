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
		int f, g, pop, openind;
		Node *parent;
		SearchState *state;

		virtual bool pred(HeapElm *_o) {
			Node *o = static_cast<Node*>(_o);
			if (f == o->f)
				return g > o->g;
			return f < o->f;
		}

 		virtual void setindex(int i) {
			openind = i;
		}
	};

public:
	Astar(SearchDomain &d) : SearchAlg(d), closed(512927357) {}

	std::vector<SearchState*> search(SearchState *init) {
		Node *initnode = wrap(init, 0, 0, -1);
		open.push(initnode);
		closed.add(init, initnode);

		while (!open.isempty() && path.size() == 0) {
			Node *n = static_cast<Node*>(open.pop());
			if (dom.isgoal(n->state)) {
				for (Node *p = n; p; p = p->parent)
					path.push_back(p->state);
				break;
			}

			std::vector<Edge> kids = dom.expand(n->state);
			this->expd++;
			this->gend += kids.size();

			for (unsigned int i = 0; i < kids.size(); i++) {
				if (kids[i].op == n->pop) {
					dom.release(kids[i].kid);
					continue;
				}

				unsigned long hash = kids[i].kid->hash();
				Node *dup = static_cast<Node*>(closed.find(hash, kids[i].kid));
				if (dup) {
					int g = n->g + kids[i].cost;
					dom.release(kids[i].kid);
					if (dup->g <= g)
						continue;
					assert (dup->openind >= 0);
					dup->f = dup->f - dup->g + g;
					dup->g = g;
					dup->pop = kids[i].pop;
					dup->parent = n;
					open.update(dup->openind);
				} else {
					Node *k = wrap(kids[i].kid, n, kids[i].cost, kids[i].pop);
					open.push(k);
					closed.add(hash, k->state, k);
				}
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
		n->openind = -1;
		n->parent = p;
		n->state = s;
		return n;
	}
};