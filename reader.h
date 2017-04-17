#ifndef READER_H
#define READER_H

#include "core.h"

#include <QString>

class Reader
{
public:
    Reader();
    ~Reader();

    // чтение данных из CSV файла
    static void readFromFile(
        const QString &fileName,
        DataSeries *data,
        uint16_t partSize = 256
    );
};

#endif // READER_H
