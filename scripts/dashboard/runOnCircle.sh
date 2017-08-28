#!/usr/bin/bash

# Make sure we can find cmake
. /home/adios2/.bashrc

echo "runOnCircle.sh (node index = ${CIRCLE_NODE_INDEX}, total nodes = ${CIRCLE_NODE_TOTAL})"

# Initialize modules system
. /etc/profile.d/lmod.sh >/dev/null

BASEDIR=$(readlink -f $(dirname ${BASH_SOURCE}))
cd ${BASEDIR}

ARGUMENTS="$@"

IFS=' '
read -ra DASHSCRIPTS <<< "$ARGUMENTS"
for i in "${DASHSCRIPTS[@]}"; do
    if [[ "$i" =~ scripts/dashboard/circle_([^\.]+) ]];
    then
        CONFIG="${BASH_REMATCH[1]}"

        echo "Running build: ${CONFIG}"

        DASHBOARDDIR=${BASEDIR}/../../../${CONFIG}
        LOGSDIR=${DASHBOARDDIR}/Logs
        mkdir -p ${LOGSDIR}

        export CTEST_DASHBOARD_ROOT="${DASHBOARDDIR}"

        LOG=${LOGSDIR}/${CONFIG}
		ctest -S ${BASEDIR}/circle_${CONFIG}.cmake -VV 1>${LOG}.out 2>${LOG}.err
    else
        echo "Unable to find build name in ${i}"
    fi
done
