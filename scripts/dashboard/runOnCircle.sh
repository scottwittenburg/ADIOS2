#!/usr/bin/bash

# Initialize modules system
. /opt/ohpc/admin/lmod/lmod/init/bash >/dev/null

BASEDIR=$(readlink -f $(dirname ${BASH_SOURCE}))
cd ${BASEDIR}

mkdir -p ${BASEDIR}/../../../Logs

DASHBOARD_CONFIGS="GNU7_NoMPI"

# First do the system tools build without mpi
CONFIG="GNU4_NoMPI"

# unload any modules just in case
module purge

echo "Running configuration ${CONFIG}"
LOG=${BASEDIR}/../../../Logs/${CONFIG}
ctest -S ${BASEDIR}/circle_${CONFIG}.cmake -VV 1>${LOG}.out 2>${LOG}.err

# Next do the gnu7 parallel build
### Not yet implemented