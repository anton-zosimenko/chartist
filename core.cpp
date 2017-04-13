#include "core.h"

#include <cstdlib>
#include <stdexcept>

DataSeries::DataSeries()
{
    mSize = 0;
    mName = L"";
    mData = nullptr;
}

DataSeries::~DataSeries()
{
    free(mData);
    mData = nullptr;
}

uint64_t DataSeries::size() const
{
    return mSize;
}

std::wstring DataSeries::name() const
{
    return mName;
}

const Candle * DataSeries::data() const
{
    return mData;
}

void DataSeries::append(const Candle *data, uint64_t size)
{
    if (mSize == 0) {
        mData = (Candle *)malloc(size * sizeof(Candle));
    } else {
        mData = (Candle *)realloc((void *)mData, (mSize + size) * sizeof(Candle));
    }
    if (mData == nullptr) {
        throw std::out_of_range("Can't append data to DataSeries");
    }
    for (uint64_t i = 0; i < size; ++i) {
        mData[mSize + i] = data[i];
    }
    mSize += size;
}
