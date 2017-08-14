#!/bin/bash

for d in base incrmd optab closed inplace templates poolalloc packed intrusive closedopt array1d array2d; do
	echo $d
	cd $d
	make || exit 1
	cd ..
done