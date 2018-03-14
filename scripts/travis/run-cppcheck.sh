#!/usr/bin/env bash

if [ -z "${SOURCE_DIR}" ]
then
  echo "Error: SOURCE_DIR is empty or undefined"
  exit 1
fi

# First download CMake binaries (at least 3.10)
cd ${HOME}
wget https://cmake.org/files/v3.10/cmake-3.10.2-Linux-x86_64.tar.gz
tar -zxf cmake-3.10.2-Linux-x86_64.tar.gz
CMAKE_EXE="${HOME}/cmake-3.10.2-Linux-x86_64/bin/cmake"
CTEST_EXE="${HOME}/cmake-3.10.2-Linux-x86_64/bin/ctest"

# Now get cppcheck source pkg and build it
cd ${HOME}
wget https://github.com/danmar/cppcheck/archive/1.82.tar.gz
mkdir -p /opt/cppcheck/build /opt/cppcheck/install
cd /opt/cppcheck
tar -xzf ${HOME}/1.82.tar.gz
cd build

CC=$(which clang-3.8)
CXX=$(which clang++-3.8)

echo "Found clang C compiler: ${CC}"
echo "Found clang C++ compiler: ${CXX}"

${CMAKE_EXE} \
  -DCMAKE_INSTALL_PREFIX:PATH=/opt/cppcheck/install \
  -DCMAKE_C_COMPILER:FILEPATH=${CC} \
  -DCMAKE_CXX_COMPILER:FILEPATH=${CXX} \
  ../cppcheck-1.82

make -j8
make install

CPPCHECK_EXE=/opt/cppcheck/install/bin/cppcheck
${CPPCHECK_EXE} --version
${CPPCHECK_EXE} --help

# Now run dashboard script
cd ${BUILD_DIR}

export CMAKE_CPPCHECK_COMMAND="${CPPCHECK_EXE};--enable=style;--template=\"CPPCHECK - POTENTIAL ERROR - {file}:{line}: ({severity}) => {id} {message}\""
export CPPCHECK_EXE=${CPPCHECK_EXE}

CUSTOM_BUILD_NAME="${TRAVIS_PULL_REQUEST_BRANCH}_${TRAVIS_BUILD_NUMBER}_cppcheck"
CTEST_SCRIPT="${SOURCE_DIR}/scripts/travis/travis_cppcheck.cmake"

steps=( update configure build )
for step in "${steps[@]}"; do
    echo "Running $step step"
    ${CTEST_EXE} -VV -S ${CTEST_SCRIPT} -Ddashboard_full=OFF -Ddashboard_do_${step}=TRUE -DCTEST_BUILD_NAME=${CUSTOM_BUILD_NAME}
done
