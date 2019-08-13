#/bin/bash

repository="https://github.com/prasanth439/data_mining_assignments.git"

git clone "$repository"

# module load compiler/python/2.7.13/ucs4/gnu/447
module load compiler/python/3.6.0/ucs4/gnu/447
# module load compiler/java/8.112/precompiled 
module load lib/gnu/710/boost/1.64.0/gnu
module load pythonpackages/3.6.0/numpy/1.16.1/gnu
module load pythonpackages/3.6.0/matplotlib/3.0.2/gnu
# module load pythonpackages/2.7.13/ucs4/gnu/447/numpy/1.12.1/gnu
# module load pythonpackages/2.7.13/ucs4/gnu/447/matplotlib/2.0.0/gnu