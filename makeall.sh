#!/bin/bash

ALL=src

echo $ALL
cd $ALL
make || exit 1
cd ..

