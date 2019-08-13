#/bin/bash
dataset=$1
tag=$3
number=$2
filename=$4
apriEXE="apriori"
fpTREE="fptree"
if [[ $tag = "-apriori" ]]; then
    ./$apriEXE $number $dataset $filename
# else
#     ./$fpTREE $number $dataset $filename
fi
