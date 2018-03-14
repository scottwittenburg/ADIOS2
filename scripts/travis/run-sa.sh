#!/usr/bin/env bash

set -ex

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

# First download recent clang+llvm binaries
mkdir -p ${HOME}/clang_llvm/scan_output
pushd ${HOME}/clang_llvm
wget http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
tar xvf clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz

export CCC_ANALYZER=$(find ${HOME}/clang_llvm | grep ccc-analyzer$)
export CXX_ANALYZER=$(find ${HOME}/clang_llvm | grep c++-analyzer$)
export WRAPPED_CC="${SOURCE_DIR}/scripts/travis/cl_analyze_wrap_gcc.sh"
export WRAPPED_CXX="${SOURCE_DIR}/scripts/travis/cl_analyze_wrap_gplusplus.sh"

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

# Environment variables used by analyzers
export CCC_ANALYZER_ANALYSIS=${ANALYSES}
export OUTPUT_DIR=${HOME}/clang_llvm/scan_output

popd

steps=( update configure build )
for step in "${steps[@]}"; do
    echo "Running $step step"
    CL_ANAlYZE=""
    if [ "$step" == "build" ]
    then
        CL_ANAlYZE="TRUE"
    fi
    export DO_CL_ANAlYZE=${CL_ANAlYZE}
    ctest -VV -S ${CTEST_SCRIPT} -Ddashboard_full=OFF -Ddashboard_do_${step}=TRUE -DCTEST_BUILD_NAME=${CUSTOM_BUILD_NAME}
done

