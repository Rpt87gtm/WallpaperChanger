#pragma once

#include <vector>

template<typename T>
class IFileData
{
public:
    virtual ~IFileData() = default;
    virtual std::vector<T> Data() = 0;
};