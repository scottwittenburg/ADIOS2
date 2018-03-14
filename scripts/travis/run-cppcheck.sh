#!/usr/bin/env bash

set -ex

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

CPPCHECK_EXE=/opt/cppcheck/install/bin/cppcheck

ANALYSIS_OUTPUT_DIR=$HOME/cppcheck-output
mkdir ${ANALYSIS_OUTPUT_DIR}
OUTPUT_FILE=${ANALYSIS_OUTPUT_DIR}/cppcheck-project.txt

export WRAPPED_CC="${SOURCE_DIR}/scripts/travis/cppcheck_wrap_gcc.sh"
export WRAPPED_CXX="${SOURCE_DIR}/scripts/travis/cppcheck_wrap_gplusplus.sh"
export CPPCHECK_EXE=${CPPCHECK_EXE}

cd ${BUILD_DIR}

CUSTOM_BUILD_NAME="${TRAVIS_PULL_REQUEST_BRANCH}_${TRAVIS_BUILD_NUMBER}_cppcheck"
CTEST_SCRIPT="${SOURCE_DIR}/scripts/travis/travis_cppcheck.cmake"

${CPPCHECK_EXE} --version
${CPPCHECK_EXE} --help
# cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${SOURCE_DIR}
# ${CPPCHECK_EXE} --enable=style --project=compile_commands.json 2> ${OUTPUT_FILE}

# if [ -s ${OUTPUT_FILE} ]; then
#     cat ${OUTPUT_FILE}
#     exit 1
# fi

steps=( update configure build )
for step in "${steps[@]}"; do
    echo "Running $step step"
    CPPCHECK=""
    if [ "$step" == "build" ]
    then
        CPPCHECK="TRUE"
    fi
    export DOCPPCHECK=${CPPCHECK}
    ctest -VV -S ${CTEST_SCRIPT} -Ddashboard_full=OFF -Ddashboard_do_${step}=TRUE -DCTEST_BUILD_NAME=${CUSTOM_BUILD_NAME}
done

# exit 0
