#!/bin/bash

if [ -d "build" ]; then
rm -rf build
fi

mkdir build
cmake -B build
(cd build && make) 
