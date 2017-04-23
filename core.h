#ifndef CORE_H
#define CORE_H

#include <inttypes.h>
#include <string>

struct Candle {
    uint64_t date;
    uint64_t time;
    float open;
    float high;
    float low;
    float close;
    float volume;
};

class DataSeries {
public:
    DataSeries();
    ~DataSeries();
    void append(const Candle *data, uint64_t size);
    uint64_t size() const;
    const Candle *data() const;
    float globalHigh() const;
    float globalLow() const;
private:
    uint64_t mSize;
    Candle *mData;
    float mGlobalHigh;
    float mGlobalLow;
};

#endif // CORE_H
