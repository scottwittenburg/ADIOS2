/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * helloBPWriter_nompi_pyext.cpp test involving import of python into C++ code
 *
 *  Created on: Sept 19, 2017
 *      Author: Scott Wittenburg <scott.wittenburg@kitware.com>
 *
 *  Note: Before this example will run correctly, you must first set the
 *.       PYTHONPATH environment variable to include the directory which
 *        contains the "dummyEngine.py" python module.  E.g.:
 *
 *        export PYTHONPATH=/.../adios2/build/lib/python2.7/site-packages
 */

#include <ios>       //std::ios_base::failure
#include <iostream>  //std::cout
#include <stdexcept> //std::invalid_argument std::exception
#include <vector>

#include <pybind11/embed.h>

#include <sstream>
#include <string>
#include <regex>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <adios2.h>

int main(int argc, char *argv[])
{
    pybind11::scoped_interpreter guard;

    /** ADIOS class factory of IO class objects, DebugON is recommended */
    adios2::ADIOS adios(adios2::DebugON);

    /*** IO class object: settings and factory of Settings: Variables,
     * Parameters, Transports, and Execution: Engines
     */
    adios2::IO &bpIO = adios.DeclareIO("BPFile_N2N");

    std::string moduleName("dummyEngine");
    std::string engineName("DummyEngine");

    // Try to import the 'dummyEngine' module
    pybind11::object engineModule = pybind11::module::import(moduleName.c_str());

    std::cout << "'dummyEngine' module: " << std::endl;
    std::cout << engineModule << std::endl;

    pybind11::object engineConstructor = engineModule.attr(engineName.c_str());

    std::cout << "contstructor attribute: " << std::endl;
    std::cout << engineConstructor << std::endl;

    // pybind11::object argObj = pybind11::cast(bpIO);
    // pybind11::object enginePyObject = engineConstructor(argObj, engineName, adios2::OpenMode::Write);

    // std::cout << "engine instance: " << std::endl;
    // std::cout << enginePyObject << std::endl;

    // EnginePy *dummyEngine = pyobj.cast<EnginePy *>();

    return 0;
}
