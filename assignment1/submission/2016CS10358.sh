#/bin/bash
time_file1="time_file1.txt"
time_file2="time_file2.txt"
apriEXE="apriori"
fpTREE="fptree"
folder=$1

if [[ $1 = "-plot" ]]; then
    folder=$2
    for number in 1 5 10 25 50 90
    do
        # ./$fpTREE $number $folder $filename $time_file1 
        ./$fpTREE $number $folder $time_file1 0
    done
    for number in 1 5 10 25 50 90
    do
        # ./$apriEXE $number $folder $filename $time_file2 
        ./$apriEXE $number $folder $time_file2 0
    done
    python3 plot.py
elif [[ $3 = "-apriori" ]]; then
    filename=$4 
    # ./$apriEXE $number $folder $filename 
    ./$apriEXE $2 $folder "$filename.txt" 1
else 
    filename=$4
    # ./$fpTREE $number $folder $filename $output
    ./$fpTREE $2 $folder "$filename.txt" 1
fi
