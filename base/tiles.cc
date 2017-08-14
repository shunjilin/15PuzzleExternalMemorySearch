// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "tiles.hpp"

Tiles::Tiles(FILE *in) {
	unsigned int w, h;

	if (fscanf(in, " %u %u", &w, &h) != 2)
		throw Fatal("Failed to read width and height");

	if (w != Width && h != Height)
		throw Fatal("Width and height instance/compiler option mismatch");

	if (fscanf(in, " starting positions for each tile:") != 0)
		throw Fatal("Failed to read the starting position label");

	for (int t = 0; t < Ntiles; t++) {
		int p;
		int r = fscanf(in, " %u", &p);
		if (r != 1)
			throw Fatal("Failed to read the starting positions: r=%d", r);
		init[p] = t;
	}

	if (fscanf(in, " goal positions:") != 0)
		throw Fatal("Failed to read the goal position label");

	for (int t = 0; t < Ntiles; t++) {
		int p;
		if (fscanf(in, " %u", &p) != 1)
			throw Fatal("Failed to read the goal position");
		if (p != t)
			throw Fatal("Non-canonical goal positions");
	}
}
