#!/bin/bash

BENCH=$1

#./abc -c "r benchmarks/nianze/${BENCH}; thLu; r benchmarks/nianze/${BENCH}; synth2"
./abc -c "r benchmarks/nianze/${BENCH}; thexp1; th2blif temp.blif; r benchmarks/nianze/${BENCH}; thgen; thExt; pt;th2blif test.blif; r temp.blif; synth2; r test.blif; synth2"
