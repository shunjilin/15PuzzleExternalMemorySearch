#!/bin/bash

OPTIMAL=array2d

echo $OPTIMAL
cd $OPTIMAL
make || exit 1
cd ..

