#include "core.h"

#include <cstdlib>
#include <stdexcept>
#include <math.h>

DataSeries::DataSeries()
{
    mSize = 0;
    mData = nullptr;
    mGlobalHigh = 0;
    mGlobalLow = INFINITY;
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
        throw std::runtime_error("Can't append data to DataSeries");
    }
    for (uint64_t i = 0; i < size; ++i) {
        mData[mSize + i] = data[i];
        if (data[i].high > mGlobalHigh) {
            mGlobalHigh = data[i].high;
        }
        if (data[i].low < mGlobalLow) {
            mGlobalLow = data[i].low;
        }
    }
    mSize += size;
}

float DataSeries::globalHigh() const
{
    return mGlobalHigh;
}

float DataSeries::globalLow() const
{
    return mGlobalLow;
}
