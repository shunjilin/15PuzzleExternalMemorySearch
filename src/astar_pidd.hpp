// Copyright 2012 Ethan Burns. All rights reserved.
// Modified, Copyright 2017 Shunji Lin. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "utils.hpp"
#include "node.hpp"

#include "compress/compress_open_list.hpp"
#include "compress/compress_closed_list.hpp"
#include "utils/compunits.hpp"
#include "utils/scoped_thread.hpp"

#include <set>

using namespace compunits;
using namespace compress;

constexpr int N_THREADS = 5;

namespace astar_pidd {

    template<class Domain> class AStarPIDD : public SearchAlg<Domain> {

        CompressClosedList<Node<Domain> > closed;
        CompressOpenList<Node<Domain> > open;
    
        std::vector<typename Domain::State> path;

        std::set<Node<Domain> > collect_unique_nodes(int n_nodes) {
            int current_h = open.get_curent_h_value();
            std::set<Node<Domain> > nodes;
            while (!open.isempty() &&
                   open.get_current_h_value() == current_h &&
                   nodes.size() < n_nodes) {
                Node<Domain> node = open.pop();
                nodes.insert(node); // only inserts unique nodes 
            }
            return nodes;
        }

        void detect_remove_duplicates(std::set<Node<Domain> > &nodes) {
            
            // TODO: complete implementation
        }

    public:
        AStarPIDD(Domain &d) : SearchAlg<Domain>(d),
            closed(true, true, true, 950_MiB), // TODO: move to user option
            open() { }

        std::vector<typename Domain::State>
        search(typename Domain::State &init) {
            open.push(wrap(init, nullptr, 0, -1));

            while (!open.isempty() && path.size() == 0) {

                auto nodes = collect_unique_nodes(N_THREADS);

                detect_remove_duplicates(nodes);

                // TODO: vector that stores return values for all threads?

                bool found, reopened;
                tie(found, reopened) = closed.find_insert(n);
                if (found && reopened) this->reopd++;
                if (found && !reopened) continue;

                typename Domain::State state;
                this->dom.unpack(state, n.packed);

                if (this->dom.isgoal(state)) {
                    // trace path here
                    path.push_back(state);
                    while(n.packed != n.parent_packed) {
                        Node<Domain> parent = closed.trace_parent(n);
                        typename Domain::State parent_state;
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
                    Edge<Domain> e = this->dom.apply(state, op);
                    open.push(wrap(state, &n, e.cost, e.pop));
                    this->dom.undo(state, e);
                }
            }
            return path;
        }

        Node<Domain> wrap(typename Domain::State &s,
                          Node<Domain>* p, int c, int pop) {
            Node<Domain> n;
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
