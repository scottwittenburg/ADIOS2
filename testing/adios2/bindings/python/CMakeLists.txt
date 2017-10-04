#------------------------------------------------------------------------------#
# Distributed under the OSI-approved Apache License, Version 2.0.  See
# accompanying file Copyright.txt for details.
#------------------------------------------------------------------------------#

# Build list of tests which will need to be able to import modules, and thus,
# will need a PYTHONPATH environment variable set in order to work
set(pyPathNeededList "")

if(NOT ADIOS2_HAVE_MPI)
    python_add_test(NAME PythonBPWrite SCRIPT TestBPWriteTypes_nompi.py)
    python_add_test(NAME TestOpenPythonEngineFromPython SCRIPT TestOpenPythonEngineFromPython_nompi.py)
endif()

if(ADIOS2_HAVE_MPI)
    set(test_parameters
      EXEC_WRAPPER
      ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS})
    python_add_test(NAME PythonBPWrite ${test_parameters} SCRIPT TestBPWriteTypes.py)
    python_add_test(NAME TestOpenPythonEngineFromPython ${test_parameters} SCRIPT TestOpenPythonEngineFromPython.py)
    python_add_test(NAME TestOpenInlinePythonEngineFromPython ${test_parameters} SCRIPT TestOpenInlinePythonEngineFromPython.py)

    set(extra_test_args EXEC_WRAPPER ${MPIEXEC_COMMAND})
endif()

list(APPEND pyPathNeededList TestOpenPythonEngineFromPython)

add_executable(TestPythonEngineFromCpp TestPythonEngineFromCpp.cpp)
target_link_libraries(TestPythonEngineFromCpp adios2 gtest gtest_main)

gtest_add_tests(TARGET TestPythonEngineFromCpp ${extra_test_args} TEST_LIST pyEngineTests)
list(APPEND pyPathNeededList ${pyEngineTests})

set_property(TEST ${pyPathNeededList} PROPERTY
	ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:${ADIOS2_BINARY_DIR}/${CMAKE_INSTALL_PYTHONDIR}:$ENV{PYTHONPATH}"
)
