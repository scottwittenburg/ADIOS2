/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * PythonPluginEngine.cpp
 *
 *  Created on: Sept 21, 2017
 *      Author: Scott Wittenburg <scott.wittenburg@kitware.com>
 */

#include "PythonPluginEngine.h"

#include "PythonInterpreter.h"

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace adios2
{

/******************************************************************************/

struct PythonPluginEngine::Impl
{
    std::string m_PluginName;
    pybind11::object enginePyObject;
    // PluginEngineInterface *m_Plugin = nullptr;
};


/******************************************************************************/

PythonPluginEngine::PythonPluginEngine(IO &io, const std::string &name,
                           const OpenMode openMode, MPI_Comm mpiComm)
: Engine("Plugin", io, name, openMode, mpiComm), m_Impl(new Impl)
{
    Init();
    // m_Impl->m_Plugin =
    //     m_Impl->m_HandleCreate(io, m_Impl->m_PluginName, openMode, mpiComm);
    std::cout << "PythonPluginEngine::PythonPluginEngine" << std::endl;
}

PythonPluginEngine::~PythonPluginEngine() {
    // m_Impl->m_HandleDestroy(m_Impl->m_Plugin);
    std::cout << "PythonPluginEngine::~PythonPluginEngine" << std::endl;
}

void PythonPluginEngine::PerformReads(ReadMode mode)
{
    // m_Impl->m_Plugin->PerformReads(mode);
    std::cout << "PythonPluginEngine::PerformReads" << std::endl;
}

void PythonPluginEngine::Release() {
    // m_Impl->m_Plugin->Release();
    std::cout << "PythonPluginEngine::Release" << std::endl;
}

void PythonPluginEngine::Advance(const float timeoutSeconds)
{
    // m_Impl->m_Plugin->Advance(timeoutSeconds);
    std::cout << "PythonPluginEngine::Advance(timeoutSeconds)" << std::endl;
}

void PythonPluginEngine::Advance(const AdvanceMode mode, const float timeoutSeconds)
{
    // m_Impl->m_Plugin->Advance(mode, timeoutSeconds);
    std::cout << "PythonPluginEngine::Advance(mode, timeoutSeconds)" << std::endl;
}

void PythonPluginEngine::AdvanceAsync(const AdvanceMode mode,
                                AdvanceAsyncCallback callback)
{
    // m_Impl->m_Plugin->AdvanceAsync(mode, callback);
    std::cout << "PythonPluginEngine::AdvanceAsync" << std::endl;
}

void PythonPluginEngine::SetCallBack(
    std::function<void(const void *, std::string, std::string, std::string,
                       std::vector<size_t>)>
        callback)
{
    // m_Impl->m_Plugin->SetCallBack(callback);
    std::cout << "PythonPluginEngine::SetCallBack" << std::endl;
}

void PythonPluginEngine::Close(const int transportIndex)
{
    // m_Impl->m_Plugin->Close(transportIndex);
    std::cout << "PythonPluginEngine::Close" << std::endl;
}

void PythonPluginEngine::Init()
{
    std::cout << "PythonPluginEngine::Init" << std::endl;

    adios2::PythonInterpreter::instance().initialize();

    auto paramPluginNameIt = m_IO.m_Parameters.find("PluginName");
    if (paramPluginNameIt == m_IO.m_Parameters.end())
    {
        throw std::invalid_argument("PythonPluginEngine: PluginName must be "
                                    "specified in engine parameters");
    }
    m_Impl->m_PluginName = paramPluginNameIt->second;

    // Get the python engine plugin module name
    auto paramPluginModuleIt = m_IO.m_Parameters.find("PluginModule");
    if (paramPluginModuleIt == m_IO.m_Parameters.end())
    {
        throw std::invalid_argument("PythonPluginEngine: PluginModule must be "
                                    "specified in engine parameters");
    }
    std::string pluginModuleName = paramPluginModuleIt->second;

    // Get the python engine plugin class name
    auto paramPluginClassIt = m_IO.m_Parameters.find("PluginClass");
    if (paramPluginClassIt == m_IO.m_Parameters.end())
    {
        throw std::invalid_argument("PythonPluginEngine: PluginClass must be "
                                    "specified in engine parameters");
    }
    std::string pluginClassName = paramPluginClassIt->second;

    // Load the module and instantiate the class
    // pybind11::scoped_interpreter guard;

    // Try to import the 'dummyEngine' module
    pybind11::object engineModule = pybind11::module::import(pluginModuleName.c_str());

    std::cout << "'" << pluginModuleName << "' module: " << std::endl;
    std::cout << engineModule << std::endl;

    pybind11::object engineConstructor = engineModule.attr(pluginClassName.c_str());

    std::cout << "contstructor attribute: " << std::endl;
    std::cout << engineConstructor << std::endl;

    // m_Impl->enginePyObject = engineConstructor(m_Impl->m_PluginName, adios2::OpenMode::Write);
    m_Impl->enginePyObject = engineConstructor();
    // m_Impl->enginePyObject.inc_ref();

    std::cout << "engine instance: " << std::endl;
    std::cout << m_Impl->enginePyObject << std::endl;
}

#define define(T)                                                              \
    void PythonPluginEngine::DoWrite(Variable<T> &variable, const T *values)   \
    {                                                                          \
        std::cout << "PythonPluginEngine::DoWrite(var, vals), "                \
                  << "type: " << typeid(T).name() << std::endl;                \
        m_Impl->enginePyObject.attr("Writef")();                               \
    }                                                                          \
    void PythonPluginEngine::DoScheduleRead(Variable<T> &variable,             \
                                            const T *values)                   \
    {                                                                          \
        std::cout << "PythonPluginEngine::DoScheduledRead(var, vals), "        \
                  << "type: " << typeid(T).name() << std::endl;                \
    }                                                                          \
    void PythonPluginEngine::DoScheduleRead(const std::string &variableName,   \
                                            const T *values)                   \
    {                                                                          \
        std::cout << "PythonPluginEngine::DoScheduledRead(varName, vals), "    \
                  << "type: " << typeid(T).name() << std::endl;                \
    }
ADIOS2_FOREACH_TYPE_1ARG(define)
#undef define

void PythonPluginEngine::DoWrite(VariableCompound &variable,
                                 const void *values)
{
    // m_Impl->m_Plugin->DoWrite(variable, values);
    std::cout << "PythonPluginEngine::DoWrite" << std::endl;
}

#define define(T, L)                                                           \
    Variable<T> *PythonPluginEngine::InquireVariable##L(                       \
        const std::string &name, const bool readIn)                            \
    {                                                                          \
        std::cout << "PythonPluginEngine::InquireVariable##L" << std::endl;    \
        return InquireVariable##L(name, readIn);                               \
    }
ADIOS2_FOREACH_TYPE_2ARGS(define)
#undef define

VariableBase *PythonPluginEngine::InquireVariableUnknown(
    const std::string &name, const bool readIn)
{
    std::cout << "PythonPluginEngine::InquireVariableUnknown" << std::endl;
    return InquireVariableUnknown(name, readIn);
}

} // end namespace adios
