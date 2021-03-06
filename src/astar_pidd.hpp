// Copyright 2012 Ethan Burns. All rights reserved.
// Modified, Copyright 2017 Shunji Lin. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

/*******************//**
 * WORK IN PROGRESS
 ***********************/

#include "search.hpp"
#include "utils.hpp"
#include "node.hpp"

#include "compress/compress_open_list.hpp"
#include "compress/compress_closed_list_async.hpp"
#include "utils/compunits.hpp"
//#include "utils/scoped_thread.hpp"

#include <set>
#include <future>
#include <algorithm>
#include <thread>

using namespace compunits;
using namespace compress;

const std::size_t N_THREADS = 32;//std::thread::hardware_concurrency() - 1;
using FoundReopened = std::pair<bool, bool>;

namespace astar_pidd {

    template<class Domain>
    struct NodeStats {
        Node<Domain> node;
        bool lookup;
        bool found;
        bool reopen;
        bool first_probe;
        size_t pointer;
        NodeStats() : lookup(true), found(false), reopen(false),
                      first_probe(true), pointer(0) {}
        bool operator==(const NodeStats<Domain>& other) const {
            return node == other.node;
        }
    };

    template<class Domain> class AStarPIDD : public SearchAlg<Domain> {

        CompressClosedListAsync<Node<Domain> > closed;
        CompressOpenList<Node<Domain> > open;
    
        std::vector<typename Domain::State> path;

        std::size_t duplicates = 0;

        std::vector<Node<Domain> > collect_unique_nodes(std::size_t n_nodes) {
            // inefficient, could use a set here

            //TODO: NEED TO ASSERT THAT LOWER G IS SELECTED
            std::vector<Node<Domain> > nodes;
            while (!open.isempty() &&
                   nodes.size() < n_nodes) {
                Node<Domain> node = open.pop();
                auto it = std::find(nodes.begin(), nodes.end(), node);
                if (it != nodes.end()) {
                    if (it->g > node.g) it->g = node.g;
                    ++duplicates;
                } else {
                    nodes.push_back(node);
                }   
            }
            return nodes;
        }

    public:
        AStarPIDD(Domain &d) : SearchAlg<Domain>(d),
            closed(true, true, true, 950_MiB), // TODO: move to user option
            open() {
            std::cout << N_THREADS << " concurrent threads are supported.\n";
        }

        std::vector<typename Domain::State>
        search(typename Domain::State &init) {
            open.push(wrap(init, nullptr, 0, -1));

            while (!open.isempty() && path.size() == 0) {
                // temporary placeholder for intermediate reopenings
                std::vector<Node<Domain> > reopened_nodes;

                auto nodes = collect_unique_nodes(N_THREADS);
                
                closed.batch_duplicate_detection(nodes);
                               
                for (std::size_t i = 0; i < nodes.size(); ++i) {
                    auto n = nodes[i];
                    
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
                        std::cout << "duplicates " << duplicates << std::endl;
                        return path;
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
