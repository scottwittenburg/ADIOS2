#!/usr/bin/env bash

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

### To let ctest drive this process so we can have results on CDash
### I figured out how to avoid needing the scan-build driver.  Then
### I just need a dashboard script that I can trigger from this
### script.
# find /usr/share/clang | grep ccc-analyzer$
# find /usr/share/clang | grep c++-analyzer$
# cmake -DCMAKE_CXX_COMPILER:FILEPATH=/usr/share/clang/scan-build-3.9/libexec/c++-analyzer -DCMAKE_C_COMPILER:FILEPATH=/usr/share/clang/scan-build-3.9/libexec/ccc-analyzer ../adios2Src/
# export CCC_ANALYZER_ANALYSIS="-analyzer-checker alpha.security.ArrayBoundV2 -analyzer-checker alpha.security.MallocOverflow -analyzer-checker alpha.security.ReturnPtrRange -analyzer-checker alpha.security.taint.TaintPropagation -analyzer-checker alpha.unix.Chroot -analyzer-checker alpha.unix.PthreadLock -analyzer-checker alpha.unix.SimpleStream -analyzer-checker alpha.unix.Stream -analyzer-checker alpha.unix.cstring.BufferOverlap -analyzer-checker alpha.unix.cstring.NotNullTerminated -analyzer-checker alpha.unix.cstring.OutOfBounds"
# make -j8

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
