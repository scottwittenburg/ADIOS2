#!/usr/bin/env bash

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

CPPCHECK_EXE=/opt/cppcheck/install/bin/cppcheck

ANALYSIS_OUTPUT_DIR=$HOME/cppcheck-output
mkdir ${ANALYSIS_OUTPUT_DIR}
OUTPUT_FILE=${ANALYSIS_OUTPUT_DIR}/cppcheck-project.txt

cd ${BUILD_DIR}

${CPPCHECK_EXE} --version
${CPPCHECK_EXE} --help
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${SOURCE_DIR}
${CPPCHECK_EXE} --enable=style --project=compile_commands.json 2> ${OUTPUT_FILE}

if [ -s ${OUTPUT_FILE} ]; then
    cat ${OUTPUT_FILE}
    exit 1
fi

exit 0
