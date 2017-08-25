#!/usr/bin/bash

# Make sure we can find cmake
. /home/adios2/.bashrc

# Initialize modules system
. /etc/profile.d/lmod.sh >/dev/null

BASEDIR=$(readlink -f $(dirname ${BASH_SOURCE}))
cd ${BASEDIR}

mkdir -p ${BASEDIR}/../../../Logs

# Post a comment on the PR linking to the CDash results
#. postComment.sh >/dev/null || true

# Configure a set of dashboards to run
DASHBOARD_CONFIGS="GNU4_NoMPI GNU7_OpenMPI"

for CONFIG in ${DASHBOARD_CONFIGS}
do
  echo ${CONFIG}
  LOG=${BASEDIR}/../../../Logs/${CONFIG}
  ctest -S ${BASEDIR}/circle_${CONFIG}.cmake -VV 1>${LOG}.out 2>${LOG}.err
done
