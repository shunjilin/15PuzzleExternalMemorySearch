// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "tiles.hpp"
#include "idastar.hpp"
#include "astar.hpp"
#include "compress_astar.hpp"
#include "external_astar.hpp"
#include "astar_ddd.hpp"
#include "utils/wall_timer.hpp"
#include <cstring>

using namespace astar_ddd;
using namespace external_astar;
using namespace compress;

using namespace std;

int main(int argc, const char *argv[]) {
	try {
		if (argc != 2)
			throw Fatal("Usage: tiles <algorithm>");
	
		Tiles tiles(stdin);
	
		SearchAlg<Tiles> *search = NULL;
		if (strcmp(argv[1], "idastar") == 0)
			search = new Idastar<Tiles>(tiles);
		else if (strcmp(argv[1], "astar") == 0)
			search = new Astar<Tiles>(tiles);
                else if (strcmp(argv[1], "astar_idd") == 0)
                        search = new CompressAstar<Tiles>(tiles);
                else if (strcmp(argv[1], "external_astar") == 0)
                        search = new ExternalAstar<Tiles>(tiles);
                else if (strcmp(argv[1], "astar_ddd") == 0)
                        search = new AstarDDD<Tiles>(tiles);
		else
			throw Fatal("Unknown algorithm: %s", argv[1]);

                dfpair(stdout, "search algorithm", "%s", argv[1]);
	
		Tiles::State init = tiles.initial();
		dfheader(stdout);
		dfpair(stdout, "initial heuristic", "%d", tiles.h(init));
                utils::WallTimer timer = utils::WallTimer();
	
		std::vector<Tiles::State> path = search->search(init);
                
                timer.stop();
                
                cout << "#pair  \"search wall time (s)\"   "
                     << "\"" << timer << "\"" << endl;
                
		dfpair(stdout, "total nodes expanded", "%lu", search->expd);
		dfpair(stdout, "total nodes generated", "%lu", search->gend);
		dfpair(stdout, "solution length", "%u", (unsigned int) path.size());
		dffooter(stdout);
	} catch (const Fatal &f) {
		fputs(f.msg, stderr);
		fputc('\n', stderr);
		return 1;
	}
	return 0;
}
