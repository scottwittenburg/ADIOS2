/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * PythonInterpreter.cpp
 *
 *  Created on: Sept 27, 2017
 *      Author: Scott Wittenburg <scott.wittenburg@kitware.com>
 */

#include "PythonInterpreter.h"

#include <iostream>
#include <pybind11/embed.h>
#include <sstream>
#include <string>

#include "adios2sys/SystemTools.hxx"

#if WIN32
#include <Python.h>
extern __declspec(dllimport) int Py_NoSiteFlag;
#endif

#if defined(__linux)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#elif defined(__unix)
#include <dlfcn.h>
#endif

namespace adios2
{

PythonInterpreter PythonInterpreter::m_instance;

PythonInterpreter &PythonInterpreter::instance()
{
    return PythonInterpreter::m_instance;
}

PythonInterpreter::PythonInterpreter() : m_embedded(false)
{
    this->initialize();
}

PythonInterpreter::~PythonInterpreter() { this->finalize(); }

bool PythonInterpreter::isInitialized() const
{
    return Py_IsInitialized() != 0;
}

void PythonInterpreter::initialize()
{
    if (this->isInitialized())
    {
        return;
    }

    if (adios2sys::SystemTools::HasEnv("PYTHONHOME"))
    {
        static char *pyHomeVar =
            const_cast<char *>(adios2sys::SystemTools::GetEnv("PYTHONHOME"));
        std::cout << "Will use PYTHONHOME (" << pyHomeVar << ") as argument to "
                  << "Py_SetPythonHome" << std::endl;
        Py_SetPythonHome(pyHomeVar);
    }
#if defined(__linux) || defined(__unix)
    else
    {
        // Find the actual library file where the symbol below is defined,
        // then use that path to help find Python run-time libraries.
        void *handle = dlsym(RTLD_NEXT, "Py_SetProgramName");
        if (handle)
        {
            Dl_info di;
            int ret = dladdr(handle, &di);
            if (ret != 0 && di.dli_saddr && di.dli_fname)
            {
                static char *pyProgramLocation =
                    const_cast<char *>(di.dli_fname);
                std::cout << "Will use location of 'Py_SetProgramName' ("
                          << pyProgramLocation << ") as the argument to "
                          << "Py_SetProgramName" << std::endl;
                Py_SetProgramName(pyProgramLocation);
            }
        }
    }
#endif

    m_embedded = true;
    Py_NoSiteFlag = 1;
    pybind11::initialize_interpreter();

    pybind11::module sys = pybind11::module::import("sys");
    std::cout << "Just initialized the interpreter, sys.path = " << std::endl;
    pybind11::print(sys.attr("path"));
}

void PythonInterpreter::finalize()
{
    if (this->isInitialized())
    {
        pybind11::finalize_interpreter();
        m_embedded = false;
    }
}
}
