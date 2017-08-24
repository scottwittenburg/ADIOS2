# Client maintainer: chuck.atkins@kitware.com
set(CTEST_SITE "CircleCI PR Build")
set(CTEST_BUILD_NAME "$ENV{CIRCLE_BRANCH}_GNU7_OpenMPI")
set(CTEST_BUILD_CONFIGURATION Release)
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_FLAGS "-k")
set(CTEST_TEST_ARGS PARALLEL_LEVEL 1)
set(dashboard_model Experimental)
set(dashboard_root_name "Builds/GCC-7.1.0_OpenMPI")

set(ENV{CC}  gcc)
set(ENV{CXX} g++)
set(ENV{FC}  gfortran)

set(dashboard_cache "
ADIOS_USE_MPI:BOOL=ON
ADIOS_USE_BZip2:BOOL=ON
ADIOS_USE_HDF5:BOOL=ON
ADIOS_USE_DataMan_ZeroMQ:BOOL=ON
")

set(dashboard_binary_name "GNU4_NoMPI")             # Keep build trees separate for each build
# set(CTEST_SOURCE_DIRECTORY "/home/adios2/adios2")   # Avoid checking out the source before building
# set(CTEST_GIT_COMMAND "/usr/bin/true")              # Avoid the red update failures on the dashboard
set(dashboard_git_branch $ENV{CIRCLE_BRANCH})       # Let dashboard know what branch CircleCI is testing

include(${CMAKE_CURRENT_LIST_DIR}/adios_common.cmake)