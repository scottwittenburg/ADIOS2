/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * InlineReader.tcc
 *
 *  Created on: Nov 16, 2018
 *      Author: Aron Helser aron.helser@kitware.com
 */

#ifndef ADIOS2_ENGINE_INLINEREADER_TCC_
#define ADIOS2_ENGINE_INLINEREADER_TCC_

#include "InlineReader.h"

#include <iostream>

namespace adios2
{
namespace core
{
namespace engine
{

template <>
inline void InlineReader::GetSyncCommon(Variable<std::string> &variable,
                                          std::string *data)
{
    variable.m_Data = data;
    auto blockInfo = variable.m_BlocksInfo.back();
    if (blockInfo.IsValue) {
        *data = blockInfo.Value;
    } else {
        *data = blockInfo.Buffer.Ptr()[0];
    }
    if (m_Verbosity == 5)
    {
        std::cout << "Inline Reader " << m_ReaderRank << "     GetSync("
                  << variable.m_Name << ")\n";
    }
}

template <class T>
inline void InlineReader::GetSyncCommon(Variable<T> &variable, T *data)
{
    variable.m_Data = data;
    auto blockInfo = variable.m_BlocksInfo.back();
    if (blockInfo.IsValue) {
        *data = blockInfo.Value;
    }
    if (m_Verbosity == 5)
    {
        std::cout << "Inline Reader " << m_ReaderRank << "     GetSync("
                  << variable.m_Name << ")\n";
    }
}

template <class T>
void InlineReader::GetDeferredCommon(Variable<T> &variable, T *data)
{
    // returns immediately
    if (m_Verbosity == 5)
    {
        std::cout << "Inline Reader " << m_ReaderRank << "     GetDeferred("
                  << variable.m_Name << ")\n";
    }
    m_NeedPerformGets = true;
}

template <class T>
inline void InlineReader::GetBlockSyncCommon(Variable<T> &variable)
{
    // variable.m_Data = data;
    auto blockInfo = variable.m_BlocksInfo.back();
    if (blockInfo.Buffer.Ptr()) {
        // *data = blockInfo.Data.Ptr();
    }
    if (m_Verbosity == 5)
    {
        std::cout << "Inline Reader " << m_ReaderRank << "     GetBlockSync("
                  << variable.m_Name << ")\n";
    }
}


} // end namespace engine
} // end namespace core
} // end namespace adios2

#endif // ADIOS2_ENGINE_INLINEREADER_TCC_
