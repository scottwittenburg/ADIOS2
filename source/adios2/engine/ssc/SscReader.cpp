/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * SscReader.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: Jason Wang
 */

#include "SscReader.tcc"
#include "adios2/helper/adiosComm.h"
#include "adios2/helper/adiosFunctions.h"
#include "adios2/helper/adiosJSONcomplex.h"
#include "nlohmann/json.hpp"

namespace adios2
{
namespace core
{
namespace engine
{

SscReader::SscReader(IO &io, const std::string &name, const Mode mode,
                     helper::Comm comm)
: Engine("SscReader", io, name, mode, std::move(comm))
{
    TAU_SCOPED_TIMER_FUNC();
    MPI_Comm_rank(MPI_COMM_WORLD, &m_WorldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &m_WorldSize);
    m_ReaderRank = m_Comm.Rank();
    m_ReaderSize = m_Comm.Size();
    m_WriterSize = m_WorldSize - m_ReaderSize;

    m_GlobalWritePattern.resize(m_WriterSize);

    SyncMpiPattern();
    SyncWritePattern();
}

SscReader::~SscReader() { TAU_SCOPED_TIMER_FUNC(); }

StepStatus SscReader::BeginStep(const StepMode stepMode,
                                const float timeoutSeconds)
{
    TAU_SCOPED_TIMER_FUNC();

    if (m_Verbosity >= 5)
    {
        std::cout << "SscReader::BeginStep, World Rank " << m_WorldRank
                  << ", Reader Rank " << m_ReaderRank << std::endl;
    }

    if (m_InitialStep)
    {
        m_InitialStep = false;
        SyncReadPattern();
        MPI_Win_create(m_Buffer.data(), m_Buffer.size(), sizeof(char),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &m_MpiWin);
    }
    else
    {
        ++m_CurrentStep;
    }

    MPI_Win_fence(0, m_MpiWin);
    MPI_Win_fence(0, m_MpiWin);

    if (m_Buffer[0] == 1)
    {
        return StepStatus::EndOfStream;
    }

    return StepStatus::OK;
}

void SscReader::PerformGets() { TAU_SCOPED_TIMER_FUNC(); }

size_t SscReader::CurrentStep() const
{
    TAU_SCOPED_TIMER_FUNC();
    return m_CurrentStep;
}

void SscReader::EndStep()
{
    TAU_SCOPED_TIMER_FUNC();
    if (m_Verbosity >= 5)
    {
        std::cout << "SscReader::EndStep, World Rank " << m_WorldRank
                  << ", Reader Rank " << m_ReaderRank << std::endl;
    }
}

// PRIVATE

void SscReader::SyncMpiPattern()
{
    int readerMasterWorldRank = 0;
    int writerMasterWorldRank = 0;
    if (m_ReaderRank == 0)
    {
        readerMasterWorldRank = m_WorldRank;
    }
    MPI_Allreduce(&readerMasterWorldRank, &m_ReaderMasterWorldRank, 1, MPI_INT,
                  MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&writerMasterWorldRank, &m_WriterMasterWorldRank, 1, MPI_INT,
                  MPI_MAX, MPI_COMM_WORLD);

    if (m_WorldSize == m_ReaderSize)
    {
        throw(std::runtime_error("no writers are found"));
    }

    if (m_Verbosity >= 5)
    {
        std::cout << "SscReader::SyncMpiPattern, World Rank " << m_WorldRank
                  << ", Reader Rank " << m_ReaderRank << std::endl;
    }
}

void SscReader::SyncWritePattern()
{
    if (m_Verbosity >= 5)
    {
        std::cout << "SscReader::SyncWritePattern, World Rank " << m_WorldRank
                  << ", Reader Rank " << m_ReaderRank << std::endl;
    }

    // sync with writers
    size_t globalSizeDst = 0;
    size_t globalSizeSrc = 0;
    MPI_Allreduce(&globalSizeSrc, &globalSizeDst, 1, MPI_UNSIGNED_LONG_LONG,
                  MPI_MAX, MPI_COMM_WORLD);
    std::vector<char> globalVec(globalSizeDst);

    MPI_Win win;
    MPI_Win_create(NULL, 0, sizeof(char), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
    MPI_Get(globalVec.data(), globalVec.size(), MPI_CHAR, 0, 0,
            globalVec.size(), MPI_CHAR, win);
    MPI_Win_fence(0, win);
    MPI_Win_free(&win);

    // deserialize

    m_GlobalWritePattern.resize(m_WriterSize);
    ssc::JsonToBlockVecVec(globalVec, m_GlobalWritePattern);

    for (const auto &blockVec : m_GlobalWritePattern)
    {
        for (const auto &b : blockVec)
        {
            if (b.type.empty())
            {
                throw(std::runtime_error("unknown data type"));
            }
#define declare_type(T)                                                        \
    else if (b.type == helper::GetType<T>())                                   \
    {                                                                          \
        auto v = m_IO.InquireVariable<T>(b.name);                              \
        if (not v)                                                             \
        {                                                                      \
            m_IO.DefineVariable<T>(b.name, b.shape, b.start, b.shape);         \
        }                                                                      \
    }
            ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type
            else { throw(std::runtime_error("unknown data type")); }
        }
    }

    nlohmann::json j;
    try
    {
        j = nlohmann::json::parse(globalVec);
    }
    catch (...)
    {
        throw(std::runtime_error("corrupted json string"));
    }

    for (const auto &attributeJson : j[m_WriterSize])
    {
        const std::string type(attributeJson["T"].get<std::string>());
        if (type.empty())
        {
        }
#define declare_type(T)                                                        \
    else if (type == helper::GetType<T>())                                     \
    {                                                                          \
        const auto &attributesDataMap = m_IO.GetAttributesDataMap();           \
        auto it =                                                              \
            attributesDataMap.find(attributeJson["N"].get<std::string>());     \
        if (it == attributesDataMap.end())                                     \
        {                                                                      \
            if (attributeJson["S"].get<bool>())                                \
            {                                                                  \
                m_IO.DefineAttribute<T>(attributeJson["N"].get<std::string>(), \
                                        attributeJson["V"].get<T>());          \
            }                                                                  \
            else                                                               \
            {                                                                  \
                m_IO.DefineAttribute<T>(                                       \
                    attributeJson["N"].get<std::string>(),                     \
                    attributeJson["V"].get<std::vector<T>>().data(),           \
                    attributeJson["V"].get<std::vector<T>>().size());          \
            }                                                                  \
        }                                                                      \
    }
        ADIOS2_FOREACH_ATTRIBUTE_STDTYPE_1ARG(declare_type)
#undef declare_type
    }
}

void SscReader::SyncReadPattern()
{
    if (m_Verbosity >= 5)
    {
        std::cout << "SscReader::SyncReadPattern, World Rank " << m_WorldRank
                  << ", Reader Rank " << m_ReaderRank << std::endl;
    }

    nlohmann::json j;

    // serialize
    auto variables = m_IO.GetAvailableVariables();
    for (auto &var : variables)
    {
        m_LocalReadPattern.emplace_back();
        auto &b = m_LocalReadPattern.back();
        std::string type = var.second["Type"];
        if (type.empty())
        {
            throw(std::runtime_error("unknown data type"));
        }
#define declare_type(T)                                                        \
    else if (type == helper::GetType<T>())                                     \
    {                                                                          \
        auto v = m_IO.InquireVariable<T>(var.first);                           \
        b.name = var.first;                                                    \
        b.count = v->m_Count;                                                  \
        b.start = v->m_Start;                                                  \
        b.shape = v->m_Shape;                                                  \
        b.type = type;                                                         \
    }
        ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type
        else { throw(std::runtime_error("unknown data type")); }

        j.emplace_back();
        auto &jref = j.back();
        jref["N"] = var.first;
        jref["T"] = type;
        jref["O"] = b.start;
        jref["C"] = b.count;
        jref["S"] = b.shape;
        jref["X"] = 0;
        jref["Y"] = 0;
    }

    std::string localStr = j.dump();

    // aggregate global read pattern across all readers
    size_t localSize = localStr.size();
    size_t maxLocalSize;
    m_Comm.Allreduce(&localSize, &maxLocalSize, 1, helper::Comm::Op::Max);
    std::vector<char> localVec(maxLocalSize, '\0');
    std::memcpy(localVec.data(), localStr.c_str(), localStr.size());
    std::vector<char> globalVec(maxLocalSize * m_ReaderSize);
    m_Comm.GatherArrays(localVec.data(), maxLocalSize, globalVec.data(), 0);

    std::string globalStr;
    if (m_ReaderRank == 0)
    {
        nlohmann::json globalJson;
        try
        {
            for (size_t i = 0; i < m_ReaderSize; ++i)
            {
                globalJson[i] = nlohmann::json::parse(
                    globalVec.begin() + i * maxLocalSize,
                    globalVec.begin() + (i + 1) * maxLocalSize);
            }
        }
        catch (...)
        {
            throw(std::runtime_error(
                "reader received corrupted aggregated read pattern"));
        }
        globalStr = globalJson.dump();
    }

    size_t globalSizeSrc = globalStr.size();
    size_t globalSizeDst = 0;
    MPI_Allreduce(&globalSizeSrc, &globalSizeDst, 1, MPI_UNSIGNED_LONG_LONG,
                  MPI_MAX, MPI_COMM_WORLD);

    if (globalStr.size() < globalSizeDst)
    {
        globalStr.resize(globalSizeDst);
    }

    globalVec.resize(globalSizeDst);
    std::memcpy(globalVec.data(), globalStr.data(), globalStr.size());

    if (m_Verbosity >= 5)
    {
        std::cout << "SscReader::SyncReadPattern, World Rank " << m_WorldRank
                  << ", Reader Rank " << m_ReaderRank
                  << ", serialized global read pattern: " << globalStr
                  << std::endl;
    }

    // sync with writers
    MPI_Win win;
    MPI_Win_create(globalVec.data(), globalVec.size(), sizeof(char),
                   MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
    MPI_Win_fence(0, win);
    MPI_Win_free(&win);

    ssc::CalculateOverlap(m_GlobalWritePattern, m_LocalReadPattern);
    m_AllReceivingWriterRanks = ssc::AllOverlapRanks(m_GlobalWritePattern);
    CalculatePosition(m_GlobalWritePattern, m_AllReceivingWriterRanks);

    size_t totalDataSize = 0;
    for (auto i : m_AllReceivingWriterRanks)
    {
        totalDataSize += i.second.second;
    }

    m_Buffer.resize(totalDataSize + 1);

    if (m_Verbosity >= 10)
    {
        ssc::PrintBlockVec(m_LocalReadPattern, "Local Read Pattern");
    }
}

void SscReader::CalculatePosition(ssc::BlockVecVec &bvv,
                                  ssc::RankPosMap &allRanks)
{
    size_t bufferPosition = 0;
    for (size_t rank = 0; rank < bvv.size(); ++rank)
    {
        bool hasOverlap = false;
        for (const auto r : allRanks)
        {
            if (r.first == rank)
            {
                hasOverlap = true;
                break;
            }
        }
        if (hasOverlap)
        {
            allRanks[rank].first = bufferPosition;
            auto &bv = bvv[rank];
            for (auto &b : bv)
            {
                b.bufferStart += bufferPosition;
            }
            size_t currentRankTotalSize = TotalDataSize(bv);
            allRanks[rank].second = currentRankTotalSize;
            bufferPosition += currentRankTotalSize;
        }
    }
}

#define declare_type(T)                                                        \
    void SscReader::DoGetSync(Variable<T> &variable, T *data)                  \
    {                                                                          \
        GetSyncCommon(variable, data);                                         \
    }                                                                          \
    void SscReader::DoGetDeferred(Variable<T> &variable, T *data)              \
    {                                                                          \
        GetDeferredCommon(variable, data);                                     \
    }                                                                          \
    std::map<size_t, std::vector<typename Variable<T>::Info>>                  \
    SscReader::DoAllStepsBlocksInfo(const Variable<T> &variable) const         \
    {                                                                          \
        return AllStepsBlocksInfoCommon(variable);                             \
    }                                                                          \
    std::vector<typename Variable<T>::Info> SscReader::DoBlocksInfo(           \
        const Variable<T> &variable, const size_t step) const                  \
    {                                                                          \
        return BlocksInfoCommon(variable, step);                               \
    }
ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type

void SscReader::DoClose(const int transportIndex)
{
    TAU_SCOPED_TIMER_FUNC();
    if (m_Verbosity >= 5)
    {
        std::cout << "SscReader::DoClose, World Rank " << m_WorldRank
                  << ", Reader Rank " << m_ReaderRank << std::endl;
    }
    MPI_Win_free(&m_MpiWin);
}

} // end namespace engine
} // end namespace core
} // end namespace adios2
