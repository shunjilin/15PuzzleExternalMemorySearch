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
#include "compress/compress_closed_list.hpp"
#include "utils/compunits.hpp"
//#include "utils/scoped_thread.hpp"

#include <set>
#include <future>
#include <algorithm>
#include <thread>

using namespace compunits;
using namespace compress;

const std::size_t N_THREADS = std::thread::hardware_concurrency();
using FoundReopened = std::pair<bool, bool>;

namespace astar_pidd {

    template<class Domain> class AStarPIDD : public SearchAlg<Domain> {

        CompressClosedList<Node<Domain> > closed;
        CompressOpenList<Node<Domain> > open;
    
        std::vector<typename Domain::State> path;

        std::size_t duplicates = 0;

        std::vector<Node<Domain> > collect_unique_nodes(std::size_t n_nodes) {
            // inefficient, could use a set here
            std::vector<Node<Domain> > nodes;
            while (!open.isempty() &&
                   nodes.size() < n_nodes) {
                Node<Domain> node = open.pop();
                if (std::find(nodes.begin(), nodes.end(), node) == nodes.end()) {
                    nodes.push_back(node); // only inserts unique nodes
                } else {
                    ++duplicates;
                }
                    
            }
            return nodes;
        }

        std::vector<std::future<FoundReopened> >
        batch_duplicate_detection(std::vector<Node<Domain> > &nodes) {
         
            std::vector<std::future<FoundReopened> > futures;
            for (auto& node : nodes) {
                futures.push_back
                    (std::async(std::launch::async,
                                &CompressClosedList<Node<Domain> >::find_in_closed,
                                &this->closed, node));
            }
            return futures;
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

                // check for duplicates in buffer
                for (auto &node : nodes) {
                    bool found, reopened;
                    tie(found, reopened) = closed.find_in_buffers(node);
                    if (found && reopened) {
                        this->reopd++;
                        reopened_nodes.push_back(node);
                        node.g = -1; // remove node TODO: fix hack
                    }
                    if (found && !reopened) {
                        node.g = -1; // remove node
                    }
                }
                
                
                nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
                               [](Node<Domain>& node) {
                                               return node.g == -1;
                                           }), nodes.end());

                 auto duplicate_detection_futures =
                     batch_duplicate_detection(nodes);

               
                nodes.insert(nodes.end(), reopened_nodes.begin(), reopened_nodes.end());
                
                for (std::size_t i = 0; i < nodes.size(); ++i) {
                    if (i < duplicate_detection_futures.size()) {
                        bool found, reopened;
                        tie(found, reopened) = duplicate_detection_futures[i].get();
                        if (found && reopened) this->reopd++;
                        if (found && !reopened) continue;
                    }
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
                for (std::size_t i = 0; i < duplicate_detection_futures.size(); ++i) {
                    closed.insert_in_buffer(nodes[i]);
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
