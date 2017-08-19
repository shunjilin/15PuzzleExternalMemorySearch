// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "utils.hpp"
#include "node.hpp"

#include "compress/compress_open_list.hpp"
#include "compress/compress_closed_list.hpp"

using namespace std;

namespace compress {

    template<class D> class CompressAstar : public SearchAlg<D> {

        CompressClosedList<Node<D> > closed;
        CompressOpenList<Node<D> > open;
    
        std::vector<typename D::State> path;

    public:
        CompressAstar(D &d) : SearchAlg<D>(d),
            closed(true, true, true, 0.5 * pow(1024, 3)),
            open() { }

        std::vector<typename D::State> search(typename D::State &init) {
            open.push(wrap(init, nullptr, 0, -1));

            while (!open.isempty() && path.size() == 0) {
                Node<D> n = open.pop();

                bool found, reopened;
                tie(found, reopened) = closed.find_insert(n);
                if (found && reopened) this->reopd++;
                if (found && !reopened) continue;

                typename D::State state;
                this->dom.unpack(state, n.packed);

                if (this->dom.isgoal(state)) {
                    // trace path here
                    path.push_back(state);
                    while(n.packed != n.parent_packed) {
                        Node<D> parent = closed.trace_parent(n);
                        typename D::State parent_state;
                        this->dom.unpack(parent_state, parent.packed);
                        path.push_back(parent_state);
                        n = parent;
                    }
                    closed.print_statistics();
                    open.clear();
                    closed.clear();
                    break;
                }

                this->expd++;
                for (int i = 0; i < this->dom.nops(state); i++) {
                    int op = this->dom.nthop(state, i);
                    if (op == n.pop)
                        continue;
                    this->gend++;
                    Edge<D> e = this->dom.apply(state, op);
                    open.push(wrap(state, &n, e.cost, e.pop));
                    this->dom.undo(state, e);
                }
            }
            return path;
        }

        Node<D> wrap(typename D::State &s, Node<D>* p, int c, int pop) {
            Node<D> n;
            n.g = c;
            if (p)
                n.g += p->g;
            n.f = n.g + this->dom.h(s);
            n.pop = pop;
            this->dom.pack(n.packed, s);
            if (p) {
                n.parent_packed = p->packed;
            } else {
                n.parent_packed = n.packed; // point to itself
            }
            return n;
        }
    };
}
