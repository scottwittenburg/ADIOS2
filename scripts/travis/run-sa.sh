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
export CCC_ANALYZER=$(find /usr/share/clang | grep ccc-analyzer$)
export CXX_ANALYZER=$(find /usr/share/clang | grep c++-analyzer$)

echo "CCC_ANALYZER=${CCC_ANALYZER}"
echo "CXX_ANALYZER=${CXX_ANALYZER}"

CUSTOM_BUILD_NAME="${TRAVIS_PULL_REQUEST_BRANCH}_${TRAVIS_BUILD_NUMBER}_clang-analyzer"
CTEST_SCRIPT="${SOURCE_DIR}/scripts/travis/travis_clang-analyzer.cmake"

extra_checkers=(
  alpha.security.ArrayBoundV2
  alpha.security.MallocOverflow
  alpha.security.ReturnPtrRange
  alpha.security.taint.TaintPropagation
  alpha.unix.Chroot
  alpha.unix.PthreadLock
  alpha.unix.SimpleStream
  alpha.unix.Stream
  alpha.unix.cstring.BufferOverlap
  alpha.unix.cstring.NotNullTerminated
  alpha.unix.cstring.OutOfBounds
)

ANALYSES=""

for checker in "${extra_checkers[@]}"; do
  ANALYSES="${ANALYSES} -analyzer-checker ${checker}"
done

echo "Extra checkers environment variable: ${ANALYSES}"

export CCC_ANALYZER_ANALYSIS=${ANALYSES}

steps=( update configure build )
for step in "${steps[@]}"; do
    echo "Running $step step"
    ctest -VV -S ${CTEST_SCRIPT} -Ddashboard_full=OFF -Ddashboard_do_${step}=TRUE -DCTEST_BUILD_NAME=${CUSTOM_BUILD_NAME}
done

