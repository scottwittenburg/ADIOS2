#!/usr/bin/env bash

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

ANALYSIS_OUTPUT_DIR=$HOME/cppcheck-output
CHECK_EXE=cppcheck
mkdir ${ANALYSIS_OUTPUT_DIR}
OUTPUT_FILE=${ANALYSIS_OUTPUT_DIR}/cppcheck-project.txt

cd ${BUILD_DIR}

${CHECK_EXE} --version
${CHECK_EXE} --help
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${SOURCE_DIR}
${CHECK_EXE} --enable=style --project=compile_commands.json 2> ${OUTPUT_FILE}

if [ -s ${OUTPUT_FILE} ]; then
    cat ${OUTPUT_FILE}
    exit 1
fi

exit 0
