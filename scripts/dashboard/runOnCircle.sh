#!/usr/bin/bash

# Make sure we can find cmake
. /home/adios2/.bashrc

echo "Hello from runOnCircle.sh (node index = ${CIRCLE_NODE_INDEX}, total nodes = ${CIRCLE_NODE_TOTAL})"

# Initialize modules system
. /etc/profile.d/lmod.sh >/dev/null

BASEDIR=$(readlink -f $(dirname ${BASH_SOURCE}))
cd ${BASEDIR}



ARGUMENTS="$@"

IFS=' '
read -ra DASHSCRIPTS <<< "$ARGUMENTS"
for i in "${DASHSCRIPTS[@]}"; do
    # fileName=basename $i
    # echo "Running dashboard script: ${fileName}"
    if [[ "$i" =~ scripts/dashboard/circle_([^\.]+) ]];
    then
        CONFIG="${BASH_REMATCH[1]}"
        echo "Running build: ${CONFIG}"
        # mkdir -p ${BASEDIR}/../../../${CONFIG}/Logs
    else
        echo "Unable to find build name in ${i}"
    fi
done



# Post a comment on the PR linking to the CDash results
#. postComment.sh >/dev/null || true

# Configure a set of dashboards to run
# DASHBOARD_CONFIGS="GNU4_NoMPI GNU7_OpenMPI"

# for CONFIG in ${DASHBOARD_CONFIGS}
# do
#   echo ${CONFIG}
#   LOG=${BASEDIR}/../../../Logs/${CONFIG}
#   ctest -S ${BASEDIR}/circle_${CONFIG}.cmake -VV 1>${LOG}.out 2>${LOG}.err
# done
