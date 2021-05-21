#! /bin/bash

benchmarks=(avrora luindex lusearch sunflow xalan)
# benchmarks=(luindex sunflow xalan)
iters=$1
# tools=(FT2 "FT2S -samplingrate=50" "FT2S -samplingrate=10" "FT2S -samplingscheme=adaptive")
tools=("FT2S -samplingscheme=adaptive")
xalan_exception=""

cd benchmarks

for benchmark in ${benchmarks[@]}
do
    cd $benchmark

    if [ $benchmark == "xalan" ]; then
        xalan_exception=" 4"
    fi
    
    echo "$benchmark Starting..."

    for tool in "${tools[@]}"
    do
        echo -e "\n$tool Starting...\n"

        for iter in `seq 1 $iters`;
        do
            echo "Starting Iteration $iter"
            ./TEST -tool="$tool" -array=FINE -field=FINE -noTidGC -availableProcessors=4 -benchmark=1 -warmup=0 RRBench"$xalan_exception" 2> /dev/null | grep "Data Race" >> "$tool"_"$benchmark"_racepairs.txt
        done

        python3 ../../remove_dup_racepairs.py "$tool"_"$benchmark"_racepairs.txt > "$tool"_"$benchmark"_unique_racepairs.txt

        mv "$tool"_"$benchmark"_racepairs.txt ../../output
        mv "$tool"_"$benchmark"_unique_racepairs.txt ../../output

    done

    echo -e "$benchmark Done...\n"

    cd ..
done