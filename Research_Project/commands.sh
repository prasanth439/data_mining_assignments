#!/bin/bash

qsub -P cse  -I -l select=1:ncpus=1:ngpus=1 -l walltime=3:00:00
cd ~/datamining
source install.sh
cd ~/datamining/graphrnn/Datamining_GraphRNN/check_proj
