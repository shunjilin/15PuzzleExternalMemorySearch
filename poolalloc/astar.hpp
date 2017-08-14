// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "utils.hpp"
#include "hashtbl.hpp"
#include "heap.hpp"
#include "pool.hpp"

template<class D> class Astar : public SearchAlg<D> {
	HashTable closed;
	Heap open;
	std::vector<typename D::State> path;

	struct Node : public HeapElm {
		char f, g, pop;
		Node *parent;
		typename D::State state;

		virtual bool pred(HeapElm *_o) {
			Node *o = static_cast<Node*>(_o);
			if (f == o->f)
				return g > o->g;
			return f < o->f;
		}

 		virtual void setindex(int i) { }
	};

	Pool<Node> nodes;

public:
	Astar(D &d) : SearchAlg<D>(d), closed(512927357) {}

	std::vector<typename D::State> search(typename D::State &init) {
		open.push(wrap(init, 0, 0, -1));

		while (!open.isempty() && path.size() == 0) {
			Node *n = static_cast<Node*>(open.pop());
			if (closed.find(&n->state)) {
				nodes.destruct(n);
				continue;
			}

			if (this->dom.isgoal(n->state)) {
				for (Node *p = n; p; p = p->parent)
					path.push_back(p->state);
				break;
			}

			closed.add(&n->state, static_cast<void*>(n));

			this->expd++;
			for (int i = 0; i < this->dom.nops(n->state); i++) {
				int op = this->dom.nthop(n->state, i);
				if (op == n->pop)
					continue;
				this->gend++;
				Edge<D> e = this->dom.apply(n->state, op);
				open.push(wrap(n->state, n, e.cost, e.pop));
				this->dom.undo(n->state, e);
			}
		}
		return path;
	}

	Node *wrap(const typename D::State &s, Node *p, int c, int pop) {
		Node *n = nodes.construct();
		n->g = c;
		if (p)
			n->g += p->g;
		n->f = n->g + this->dom.h(s);
		n->pop = pop;
		n->parent = p;
		n->state = s;
		return n;
	}
};
