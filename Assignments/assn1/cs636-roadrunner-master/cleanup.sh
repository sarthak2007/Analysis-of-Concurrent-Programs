#! /bin/bash

benchmarks=(avrora luindex lusearch sunflow xalan)

cd benchmarks

for benchmark in ${benchmarks[@]}
do
    cd $benchmark
    rm -f *.txt
    cd ..
done