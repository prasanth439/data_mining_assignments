#/bin/bash
time_file1="time_file1.txt"
time_file2="time_file2.txt"
apriEXE="apriori"
fpTREE="fptree"
folder=$1

if [[ $1 = "-plot" ]]; then
    folder = $2
    for number in 1 5 10 25 50 90
    do
        # ./$fpTREE $number $folder $filename $time_file1 
        echo $number $folder $time_file1 0
    done
    for number in 1 5 10 25 50 90
    do
        # ./$apriEXE $number $folder $filename $time_file2 
        echo $number $folder $time_file2 0
    done
    python plot.py
elif [[ $3 = "-apriori" ]]; then
    filename=$4 
    # ./$apriEXE $number $folder $filename 
    echo $2 $folder "$filename.txt" 1
else 
    filename=$4
    # ./$fpTREE $number $folder $filename $output
    echo $2 $folder "$filename.txt" 1
fi
