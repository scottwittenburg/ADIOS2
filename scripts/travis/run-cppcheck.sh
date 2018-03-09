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

# steps=( update configure build )
# for step in "${steps[@]}"; do
#     echo "Running $step step"
#     ctest -VV -S ${CTEST_SCRIPT} -Ddashboard_full=OFF -Ddashboard_do_${step}=TRUE -DCTEST_BUILD_NAME=${CUSTOM_BUILD_NAME}
# done

exit 0
