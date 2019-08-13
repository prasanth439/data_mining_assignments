# data_mining_assignments
COL761 Assignments

# Commands to Run
./2016CS10358.sh webdocs.dat X -apriori result

# Mount Commands from linux
ssh cs1160358@hpc.iitd.ac.in 
[password]

sshfs cs1160358@hpc.iitd.ac.in:/home/cse/btech/cs1160358 /absolute/path/to/mount/folder

qsub -P cse  -I -l select=1:ncpus=1 -l walltime=6:00:00

