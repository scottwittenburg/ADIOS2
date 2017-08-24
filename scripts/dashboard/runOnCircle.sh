#!/usr/bin/bash

# Initialize modules system
. /etc/profile.d/lmod.sh >/dev/null

BASEDIR=$(readlink -f $(dirname ${BASH_SOURCE}))
cd ${BASEDIR}

mkdir -p ${BASEDIR}/../../../Logs

# Post a comment on the PR linking to the CDash results
. postComment.sh >/dev/null || true

# First do the system tools build without mpi
CONFIG="GNU4_NoMPI"

# Unload any loaded modules, just in case
module purge

echo "Running configuration ${CONFIG}"
LOG=${BASEDIR}/../../../Logs/${CONFIG}
ctest -S ${BASEDIR}/circle_${CONFIG}.cmake -VV 1>${LOG}.out 2>${LOG}.err

# Next do the gnu7 parallel build
CONFIG="GNU7_OpenMPI"

# Unload any loaded modules
module purge

module load gnu7
module load openmpi
module load phdf5
module load netcdf

echo "Running configuration ${CONFIG}"
LOG=${BASEDIR}/../../../Logs/${CONFIG}
ctest -S ${BASEDIR}/circle_${CONFIG}.cmake -VV 1>${LOG}.out 2>${LOG}.err
