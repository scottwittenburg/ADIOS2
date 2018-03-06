#!/usr/bin/env bash

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

ANALYSIS_OUTPUT_DIR=$HOME/scan-build-output
SCAN_BUILD_EXE=scan-build-3.8
mkdir ${ANALYSIS_OUTPUT_DIR}

cd ${BUILD_DIR}

${SCAN_BUILD_EXE} -h
${SCAN_BUILD_EXE} cmake ${SOURCE_DIR}
${SCAN_BUILD_EXE} -plist-html --status-bugs -o ${ANALYSIS_OUTPUT_DIR} \
  -enable-checker "alpha.security.ArrayBoundV2" \
  -enable-checker "alpha.security.MallocOverflow" \
  -enable-checker "alpha.security.ReturnPtrRange" \
  -enable-checker "alpha.security.taint.TaintPropagation" \
  -enable-checker "alpha.unix.Chroot" \
  -enable-checker "alpha.unix.PthreadLock" \
  -enable-checker "alpha.unix.SimpleStream" \
  -enable-checker "alpha.unix.Stream" \
  -enable-checker "alpha.unix.cstring.BufferOverlap" \
  -enable-checker "alpha.unix.cstring.NotNullTerminated" \
  -enable-checker "alpha.unix.cstring.OutOfBounds" \
  make -j8

if [ $? -eq 0 ]
then
  exit 0
fi

exit 1
