#/bin/bash
time_file1="time_file1.txt"
time_file2="time_file2.txt"
apriEXE="apriori"
fpTREE="fptree"
folder=$1
timeout_time=1800
if [[ $2 = "-plot" ]]; then
    for number in 1 5 10 25 50 90
    do
        # ./$fpTREE $number $folder $filename $time_file1 
        timeout $timeout_time ./$fpTREE $number $folder $time_file1 1
        status=$?
        if [[ $status -neq 0 ]]; then
            echo $timeout_time >> $time_file1
        fi
    done
    for number in 1 5 10 25 50 90
    do
        # echo $number $folder $filename $time_file2 
        timeout ./$apriEXE $number $folder $time_file2 1
        status=$?
        if [[ $status -neq 0 ]]; then
            echo $timeout_time >> $time_file2
        fi
    done
    python plot.py
elif [[ $3 = "-apriori" ]]; then
    filename=$4 
    # ./$apriEXE $number $folder $filename 
    timeout ./$apriEXE $2 $folder "$filename.txt" 0
else 
    filename=$4
    # ./$fpTREE $number $folder $filename $output
    ./$fpTREE $2 $folder "$filename.txt" 0
fi
