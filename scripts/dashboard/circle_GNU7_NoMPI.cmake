# Client maintainer: chuck.atkins@kitware.com
set(CTEST_SITE "CircleCI PR Build")
set(CTEST_BUILD_NAME "Linux-CentOS-7_GCC-7.1.0_NoMPI")
set(CTEST_BUILD_CONFIGURATION Release)
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_FLAGS "-k")
set(CTEST_TEST_ARGS PARALLEL_LEVEL 1)
#set(dashboard_model Nightly)
set(dashboard_root_name "Builds/GCC-7.1.0_NoMPI")

include(${CMAKE_CURRENT_LIST_DIR}/EnvironmentModules.cmake)
#module(purge)
module(load gnu7)
#module(load hdf5)

set(ENV{CC}  gcc)
set(ENV{CXX} g++)
# set(ENV{FC}  gfortran)

# set(dashboard_cache "
# ADIOS_USE_MPI:BOOL=OFF
# ADIOS_USE_BZip2:BOOL=ON
# ADIOS_USE_HDF5:BOOL=ON
# ADIOS_USE_DataMan_ZeroMQ:BOOL=ON
# ")

set(dashboard_cache "
ADIOS_USE_MPI:BOOL=OFF
ADIOS_USE_BZip2:BOOL=ON
ADIOS_USE_DataMan_ZeroMQ:BOOL=ON
")

set(CTEST_SOURCE_DIRECTORY "/home/adios2/adios2")

message(status "CirclCI is testing branch $ENV{CIRCLE_BRANCH}")
set(dashboard_git_branch $ENV{CIRCLE_BRANCH})

include(${CMAKE_CURRENT_LIST_DIR}/adios_common.cmake)
