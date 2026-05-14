#!/bin/bash
make clean
make init
make -j4 OPT=-p RELEASE=1
./build/aji samples/test-loop.aji
gprof ./build/aji gmon.out > gprof.log

