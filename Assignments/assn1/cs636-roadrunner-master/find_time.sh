#! /bin/bash

benchmarks=(avrora luindex lusearch sunflow xalan)
# benchmarks=(lusearch)
iters=$1
# tools=(N FT2 "FT2S -samplingrate=50" "FT2S -samplingrate=10" "FT2S -samplingscheme=adaptive")
tools=("FT2S -samplingscheme=adaptive")
xalan_exception=""

cd benchmarks

for benchmark in ${benchmarks[@]}
do
    cd $benchmark

    if [ $benchmark == "xalan" ]; then
        xalan_exception=" 4"
    fi
    
    echo -e "$benchmark Starting..."

    for tool in "${tools[@]}"
    do
        echo -e "\n$tool Starting...\n"

        echo "$tool" >> "$tool"_"$benchmark"_time.txt

        for iter in `seq 1 $iters`;
        do
            echo "Starting Iteration $iter"

            time=`./TEST -tool="$tool" -array=FINE -field=FINE -noTidGC -availableProcessors=4 -benchmark=1 -warmup=0 RRBench"$xalan_exception" 2> /dev/null | grep \<time\> | tail -n1 | tr -s " " | cut -d" " -f3`

            echo $time >> "$tool"_"$benchmark"_time.txt

        done

        cat "$tool"_"$benchmark"_time.txt | awk '{sum+=$0} END {print "Avg=",sum/(NR-1)}' >> "$tool"_"$benchmark"_time.txt

        mv "$tool"_"$benchmark"_time.txt ../../output

    done

    echo -e "\n$benchmark Done...\n"

    cd ..
done