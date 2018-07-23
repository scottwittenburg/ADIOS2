/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * DataManDeserializer.cpp Deserializer class for DataMan streaming format
 *
 *  Created on: May 11, 2018
 *      Author: Jason Wang
 */

#include "DataManDeserializer.tcc"

#include <cstring>
#include <iostream>

namespace adios2
{
namespace format
{

DataManDeserializer::DataManDeserializer(bool isRowMajor, bool isLittleEndian)
{
    m_IsRowMajor = isRowMajor;
    m_IsLittleEndian = isLittleEndian;
}

void DataManDeserializer::Put(std::shared_ptr<std::vector<char>> data)
{
    int key = rand();
    m_MutexBuffer.lock();
    while (m_BufferMap.count(key) > 0)
    {
        key = rand();
    }
    m_BufferMap[key] = data;
    m_MutexBuffer.unlock();
    size_t position = 0;
    while (position < data->capacity())
    {
        uint32_t metasize;
        std::memcpy(&metasize, data->data() + position, sizeof(metasize));
        position += sizeof(metasize);
        if (position + metasize > data->size())
        {
            break;
        }
        DataManVar var;
        try
        {
            nlohmann::json metaj =
                nlohmann::json::parse(data->data() + position);
            position += metasize;
            var.isRowMajor = metaj["M"].get<bool>();
            var.isLittleEndian = metaj["E"].get<bool>();
            var.name = metaj["N"].get<std::string>();
            var.type = metaj["Y"].get<std::string>();
            var.shape = metaj["S"].get<Dims>();
            var.count = metaj["C"].get<Dims>();
            var.start = metaj["O"].get<Dims>();
            var.step = metaj["T"].get<size_t>();
            var.size = metaj["I"].get<size_t>();
            var.rank = metaj["R"].get<int>();
            var.doid = metaj["D"].get<std::string>();
            var.position = position;
            var.index = key;
            auto it = metaj.find("Z");
            if (it != metaj.end())
            {
                var.compression = it->get<std::string>();
            }
            it = metaj.find("ZR");
            if (it != metaj.end())
            {
                var.compressionRate = it->get<float>();
            }
            if (position + var.size > data->capacity())
            {
                break;
            }
            m_MutexMetaData.lock();
            if (m_MetaDataMap[var.step] == nullptr)
            {
                m_MetaDataMap[var.step] =
                    std::make_shared<std::vector<DataManVar>>();
            }
            m_MetaDataMap[var.step]->push_back(std::move(var));
            m_MutexMetaData.unlock();
            position += var.size;
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
        m_MutexMaxMin.lock();
        if (m_MaxStep < var.step)
        {
            m_MaxStep = var.step;
        }
        if (m_MinStep > var.step)
        {
            m_MinStep = var.step;
        }
        m_MutexMaxMin.unlock();
    }
}

void DataManDeserializer::Erase(size_t step)
{
    m_MutexMetaData.lock();
    const auto &i = m_MetaDataMap.find(step);
    if (i != m_MetaDataMap.end())
    {
        for (const auto &k : *i->second)
        {
            if (BufferContainsSteps(k.index, step + 1, MaxStep()) == false)
            {
                m_MutexBuffer.lock();
                m_BufferMap.erase(k.index);
                m_MutexBuffer.unlock();
            }
        }
    }
    m_MetaDataMap.erase(step);
    m_MutexMetaData.unlock();

    m_MutexMaxMin.lock();
    m_MinStep = step + 1;
    m_MutexMaxMin.unlock();
}

size_t DataManDeserializer::MaxStep()
{
    std::lock_guard<std::mutex> l(m_MutexMaxMin);
    return m_MaxStep;
}

size_t DataManDeserializer::MinStep()
{
    std::lock_guard<std::mutex> l(m_MutexMaxMin);
    return m_MinStep;
}

const std::shared_ptr<std::vector<DataManDeserializer::DataManVar>>
DataManDeserializer::GetMetaData(size_t step)
{
    std::lock_guard<std::mutex> l(m_MutexMetaData);
    const auto &i = m_MetaDataMap.find(step);
    if (i != m_MetaDataMap.end())
    {
        return m_MetaDataMap[step];
    }
    else
    {
        return nullptr;
    }
}

bool DataManDeserializer::BufferContainsSteps(int index, size_t begin,
                                              size_t end)
{
    // This is a private function and is always called after m_MutexMetaData is
    // locked, so there is no need to lock again here.
    for (size_t i = begin; i <= end; ++i)
    {
        const auto &j = m_MetaDataMap.find(i);
        if (j != m_MetaDataMap.end())
        {
            for (const auto &k : *j->second)
            {
                if (k.index == index)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool DataManDeserializer::HasOverlap(Dims in_start, Dims in_count,
                                     Dims out_start, Dims out_count)
{
    for (size_t i = 0; i < in_start.size(); ++i)
    {
        if (in_start[i] > out_start[i] + out_count[i] ||
            out_start[i] > in_start[i] + in_count[i])
        {
            return false;
        }
    }
    return true;
}

bool DataManDeserializer::GetVarList(size_t step,
                                     std::vector<DataManVar> &varList)
{
    m_MutexMetaData.lock();
    auto metaDataStep = m_MetaDataMap.find(step);
    if (metaDataStep == m_MetaDataMap.end())
    {
        return false;
    }
    for (auto &i : *metaDataStep->second)
    {
        bool hasVar = false;
        for (DataManVar &j : varList)
        {
            if (j.name == i.name)
            {
                hasVar = true;
            }
        }
        if (hasVar == false)
        {
            DataManVar var;
            var.name = i.name;
            var.shape = i.shape;
            var.type = i.type;
            var.doid = i.doid;
            varList.push_back(std::move(var));
        }
    }
    m_MutexMetaData.unlock();
    return true;
}

} // namespace format
} // namespace adios2