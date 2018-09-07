#!/bin/bash

BENCH=$1
CEC=$2

if [ "$BENCH" == "c3540" -o "$BENCH" == "c5315" -o "$BENCH" == "c6288" -o "$BENCH" == "c7552" ]; then
  if [ "$CEC" == "-cec" ]; then
    ./abc -c "r benchmarks/nianze/$BENCH.bench; thgen2; th2blif results/nianze/$BENCH.blif;r benchmarks/nianze/$BENCH.bench; comb; cec -n results/nianze/$BENCH.blif"
  else
    ./abc -c "r benchmarks/nianze/$BENCH.bench; thgen2; th2blif results/nianze/$BENCH.blif"
  fi
else
  if [ "$CEC" == "-cec" ]; then
    ./abc -c "r benchmarks/nianze/$BENCH.blif; thgen2; th2blif results/nianze/$BENCH.blif;r benchmarks/nianze/$BENCH.blif; comb; cec -n results/nianze/$BENCH.blif"
  else
    ./abc -c "r benchmarks/nianze/$BENCH.blif; thgen2; th2blif results/nianze/$BENCH.blif"
  fi
fi
