#!/usr/bin/env bash

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

### Would like a more recent version of cppcheck so that we can use
### the --project option, but we will have to fetch and build that
### ourselves.  It doesn't seem like all three builds needs to incur
### this cost either, so I'm considering doing it in here.
# CHECK_TARBALL_URL=
# CHECK_EXE_ROOT_DIR=$HOME/cppcheck
# cd $CHECK_EXE_ROOT_DIR
# mkdir build install
# curl -OL

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
