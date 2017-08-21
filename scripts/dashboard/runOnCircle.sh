#!/bin/bash

BASEDIR=$(readlink -f $(dirname ${BASH_SOURCE}))
cd ${BASEDIR}

echo "Updating scripts..."
git pull --ff-only
cd ${HOME}
mkdir -p ${BASEDIR}/../../../Logs

DASHBOARD_CONFIGS="GNU7_NoMPI"

for CONFIG in ${DASHBOARD_CONFIGS}
do
  echo ${CONFIG}
  LOG=${BASEDIR}/../../../Logs/${CONFIG}
  ctest -S ${BASEDIR}/circle_${CONFIG}.cmake -VV 1>${LOG}.out 2>${LOG}.err
done
