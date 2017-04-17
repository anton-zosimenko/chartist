#include "reader.h"

#include <QFile>
#include <QByteArray>
#include <QList>

Reader::Reader()
{
}

Reader::~Reader()
{
}

// чтение данных из CSV файла
void Reader::readFromFile(
    const QString &fileName,
    DataSeries *data,
    uint16_t partSize
)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::logic_error("Can't open file with data");
    }
    uint16_t candles_size = 0;
    Candle *candles = (Candle *)malloc(partSize * sizeof(Candle));
    if (candles == nullptr) {
        throw std::runtime_error("Can't allocate memory for candles part");
    }
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        // срезать "\r\n" из строки
        line.chop(2);
        QList<QByteArray> parsedLine = line.split(',');
        if (parsedLine.length() != 7) {
            throw std::logic_error("Corrupted data in file");
        }
        candles[candles_size].date = (uint64_t)parsedLine.at(0).toULongLong();
        candles[candles_size].time = (uint64_t)parsedLine.at(1).toULongLong();
        candles[candles_size].open = parsedLine.at(2).toFloat();
        candles[candles_size].high = parsedLine.at(3).toFloat();
        candles[candles_size].low = parsedLine.at(4).toFloat();
        candles[candles_size].close = parsedLine.at(5).toFloat();
        candles[candles_size].volume = parsedLine.at(6).toFloat();
        candles_size++;
        if (candles_size == partSize) {
            // добавим накопленную часть данных к основным
            data->append(candles, candles_size);
            candles_size = 0;
        }
    }
    if (candles_size > 0) {
        // не забываем про "хвост" накопленных данных
        data->append(candles, candles_size);
        candles_size = 0;
    }
    free(candles);
    file.close();
}
