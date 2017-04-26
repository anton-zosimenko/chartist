#include "widget.h"
#include "reader.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>

Widget::Widget(QWidget *parent, const QString &fileName)
    : QWidget(parent)
{
    optShowLabelsWithMouse = true;
    optSelectAreaWithMouse = true;
    optShowVolumeGraph = true;
    optShowScrollArea = true;

    setMinimumSize(640, 480);

    mDataXBounds = QPointF(-1000, 1000);
    mDataYBounds = QPointF(0, 1);
    mVolumeBounds = QPointF(0, 1);

    mMousePos = QPoint(-1, -1);
    setMouseTracking(true);
    mMousePressPos = QPoint(-1, -1);
    mMouseReleasePos = QPoint(-1, -1);
    mIsMouseEnter = false;
    mIsLmbMousePressed = false;
    mIsResize = false;
    mIsCandleWidthChanged = false;
    mIsNeedClearArea = false;

    mBackgroundBrush = QBrush(Qt::white);
    mAxisPen = QPen(Qt::black, 1);
    mMouseAxisPen = QPen(Qt::blue, 1);
    mMouseAxisPen.setStyle(Qt::DashLine);
    mMouseAxisVolumePen = QPen(Qt::red, 1);
    mMouseAxisVolumePen.setStyle(Qt::DashLine);
    mMouseLabelPen = QPen(Qt::blue, 1);
    mMouseVolumeLabelPen = QPen(Qt::red, 1);
    mMouseSelectAreaPen = QPen(Qt::darkBlue, 1);
    mMouseSelectAreaPen.setStyle(Qt::DashLine);
    mMouseSelectAreaLabelsPen = QPen(mMouseSelectAreaPen.color(), 1);
    mMouseSelectAreaBrushAlpha = 80;
    mCandlePen = QPen(Qt::black, 1);
    mCandleUpBrush = QBrush(Qt::green);
    mCandleDownBrush = QBrush(Qt::red);
    mCandleBrushAlpha = 80;

    mAxisXLeftBorderLength = 0;
    mAxisXRightBorderLength = 52;
    mAxisYTopBorderLength = 0;
    mAxisYBottomBorderLength = 20;
    mAxisXDashCount = 10;
    mAxisYDashCount = 10;
    mAxisYVolumeDashCount = 4;
    mAxisXDashLen = 4;
    mAxisYDashLen = 4;
    mAxisXDashSpace = 2;
    mAxisYDashSpace = 4;
    mAxisLabelHalfWidth = 20;
    mAxisLabelHalfHeight = 5;
    mMaxAxisLabelLength = 6;
    mAxisLabelXAdditionalLength = 1;
    mAxisLabelYAdditionalLength = 1;
    mCandleWidth = 15;
    mBetweenCandlesWidth = 2;
    mViewedCandleCount = 0;
    mCandleOffsetFromEnd = 0;
    mCandleMinWidth = 3;
    mCandleMaxWidth = 50;
    mAxisYVolumeHeight = 100;
    mAxisYScrollBarHeight = 30;

    mDataSeries = DataSeries();
    // читаем данные из файла
    Reader::readFromFile(fileName, &mDataSeries);
}

bool Widget::showLabelsWithMouse() const
{
    return optShowLabelsWithMouse;
}

void Widget::setShowLabelsWithMouse(bool newValue)
{
    if (optShowLabelsWithMouse != newValue) {
        optShowLabelsWithMouse = newValue;
    }
}

bool Widget::selectAreaWithMouse() const
{
    return optSelectAreaWithMouse;
}

void Widget::setSelectAreaWithMouse(bool newValue)
{
    if (optSelectAreaWithMouse != newValue) {
        optSelectAreaWithMouse = newValue;
    }
}

bool Widget::showVolumeGraph() const
{
    return optShowVolumeGraph;
}

void Widget::setShowVolumeGraph(bool newValue)
{
    if (optShowVolumeGraph != newValue) {
        optShowVolumeGraph = newValue;
    }
}

bool Widget::showScrollArea() const
{
    return optShowScrollArea;
}

void Widget::setShowScrollArea(bool newValue)
{
    if (optShowScrollArea != newValue) {
        optShowScrollArea = newValue;
    }
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    paint(&painter, event);
    painter.end();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    mMousePos = event->pos();
    if (optShowLabelsWithMouse) {
        update();
    }
}

void Widget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    mIsMouseEnter = false;
    if (optShowLabelsWithMouse) {
        // перерисовка, чтоб стереть оси мышки с метками
        update();
    }
}

void Widget::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    mIsMouseEnter = true;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    mMousePressPos = event->pos();
    if (optSelectAreaWithMouse) {
        if (event->button() == Qt::LeftButton) {
            mIsLmbMousePressed = true;
            update();
        } else if (event->button() == Qt::RightButton) {
            mIsNeedClearArea = true;
            update();
        }
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    mIsLmbMousePressed = false;
    mMouseReleasePos = event->pos();
}

void Widget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    mIsResize = true;
    update();
}

void Widget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        if (mCandleWidth < mCandleMaxWidth / 2) {
            mCandleWidth *= 2;
            mIsCandleWidthChanged = true;
        } else if (mCandleWidth < mCandleMaxWidth) {
            mCandleWidth = mCandleMaxWidth;
            mIsCandleWidthChanged = true;
        }
    } else {
        if (mCandleWidth > mCandleMinWidth * 2) {
            mCandleWidth /= 2;
            mIsCandleWidthChanged = true;
        } else if (mCandleWidth > mCandleMinWidth) {
            mCandleWidth = mCandleMinWidth;
            mIsCandleWidthChanged = true;
        }
    }
    if (mIsCandleWidthChanged) {
        update();
    }
}

void Widget::paint(QPainter *painter, QPaintEvent *event)
{
    // сотрем все предыдущее залив область фоном
    painter->fillRect(event->rect(), mBackgroundBrush);

    int minX = 0;
    int minY = 0;
    int maxX = event->rect().width();
    int maxY = event->rect().height();
    int axisMinX = minX + mAxisXLeftBorderLength;
    int axisMinY = minY + mAxisYTopBorderLength;
    int axisMaxX = maxX - mAxisXRightBorderLength;
    int axisMaxY = maxY - mAxisYBottomBorderLength;
    if (optShowVolumeGraph) {
        // если включено отображение графика объема, то
        // сократим область графика по высоте
        axisMaxY -= mAxisYVolumeHeight;
    }
    int candleWidth = mCandleWidth + mBetweenCandlesWidth;

    // пересчитаем диапазоны значений на осях
    // ((при ресайзе окна или изменении ширины свечи) и наличии данных)
    if ((mIsResize || mIsCandleWidthChanged) && mDataSeries.size() > 0) {
        if (mIsResize) {
            mIsResize = false;
        } else {
            mIsCandleWidthChanged = false;
        }
        // место крайней правой свечи не занимаем
        mViewedCandleCount = (axisMaxX - axisMinX - candleWidth) / candleWidth;
        if (mViewedCandleCount > (int)mDataSeries.size()) {
            mViewedCandleCount = mDataSeries.size();
        }
        float ymin = INFINITY, ymax = 0, volmax = 0;
        for (int i = 0; i < mViewedCandleCount; ++i) {
            Candle currCandle = mDataSeries.data()[mDataSeries.size() - 1 - i];
            if (currCandle.low < ymin) {
                ymin = currCandle.low;
            }
            if (currCandle.high > ymax) {
                ymax = currCandle.high;
            }
            if (optShowVolumeGraph) {
                if (currCandle.volume > volmax) {
                    volmax = currCandle.volume;
                }
            }
        }
        mDataYBounds = QPointF(ymin, ymax);
        mDataXBounds = QPointF(-mViewedCandleCount, 0);
        if (optShowVolumeGraph) {
            mVolumeBounds = QPointF(0, volmax);
        }
    }

    // если отображается область скролла и кол-во видимых свечей меньше общего
    // кол-ва свечей, то сократим область графика по высоте
    if (optShowScrollArea && mViewedCandleCount < (int)mDataSeries.size()) {
        // если отображается область скролла,
        // то сократим область графика по высоте
        axisMaxY -= mAxisYScrollBarHeight;
    }

    // нарисуем оси
    painter->setPen(mAxisPen);
    // ось Х рисуем под графиком объема, если он задан
    int offset = optShowVolumeGraph ? mAxisYVolumeHeight : 0;
    painter->drawLine(
        QPoint(axisMinX, axisMaxY + offset),
        QPoint(axisMaxX, axisMaxY + offset)
    );
    painter->drawLine(
        QPoint(axisMaxX, axisMinY),
        QPoint(axisMaxX, axisMaxY + offset)
    );

    // нарисуем риски и данные на осях координат
    // (не забываем про смещение оси вниз, если рисуется объем)
    float deltaX = 1.0 * (axisMaxX - axisMinX) / mAxisXDashCount;
    float dataDeltaX = (mDataXBounds.y() - mDataXBounds.x()) / mAxisXDashCount;
    for (int i = 1; i < mAxisXDashCount; ++i) {
        float x = axisMinX + i*deltaX;
        painter->drawLine(QPointF(x, axisMaxY + offset), QPointF(x, axisMaxY + offset + mAxisXDashLen));
        painter->drawText(
            QRect(
                QPoint(
                    x - mAxisLabelHalfWidth,
                    axisMaxY + offset + mAxisXDashLen + mAxisXDashSpace
                ),
                QPoint(
                    x + mAxisLabelHalfWidth,
                    axisMaxY + offset + mAxisXDashLen + 2*mAxisLabelHalfHeight + mAxisXDashSpace
                )
            ),
            Qt::AlignCenter,
            makeAxisLabel(mDataXBounds.x() + i*dataDeltaX)
        );
    }
    float deltaY = 1.0 * (axisMaxY - axisMinY) / mAxisYDashCount;
    float dataDeltaY = (mDataYBounds.y() - mDataYBounds.x()) / mAxisYDashCount;
    for (int i = 1; i < mAxisYDashCount; ++i) {
        float y = axisMaxY - (axisMinY + i*deltaY);
        painter->drawLine(QPointF(axisMaxX, y), QPointF(axisMaxX + mAxisYDashLen, y));
        painter->drawText(
            QRect(
                QPoint(
                    axisMaxX + mAxisYDashLen + mAxisYDashSpace,
                    y - mAxisLabelHalfHeight
                ),
                QPoint(
                    axisMaxX + mAxisYDashLen + 2*mAxisLabelHalfWidth + mAxisYDashSpace,
                    y + mAxisLabelHalfHeight
                )
            ),
            Qt::AlignLeft,
            makeAxisLabel(mDataYBounds.x() + i*dataDeltaY)
        );
    }
    // риски графика объема
    if (optShowVolumeGraph) {
        float deltaY = 1.0 * mAxisYVolumeHeight / mAxisYVolumeDashCount;
        float dataDeltaY = (mVolumeBounds.y() - mVolumeBounds.x()) / mAxisYVolumeDashCount;
        for (int i = 1; i < mAxisYVolumeDashCount; ++i) {
            float y = axisMaxY + mAxisYVolumeHeight - i*deltaY;
            painter->drawLine(QPointF(axisMaxX, y), QPointF(axisMaxX + mAxisYDashLen, y));
            painter->drawText(
                QRect(
                    QPoint(
                        axisMaxX + mAxisYDashLen + mAxisYDashSpace,
                        y - mAxisLabelHalfHeight
                    ),
                    QPoint(
                        axisMaxX + mAxisYDashLen + 2*mAxisLabelHalfWidth + mAxisYDashSpace,
                        y + mAxisLabelHalfHeight
                    )
                ),
                Qt::AlignLeft,
                makeAxisLabel(mVolumeBounds.x() + i*dataDeltaY)
            );
        }
    }

    // нарисуем график, если задана опция
    for (int i = 0; i < mViewedCandleCount; ++i) {
        Candle currCandle = mDataSeries.data()[mDataSeries.size() - 1 - i];
        // место крайней правой свечи не занимаем
        int xmax = axisMaxX - (i + 1) * candleWidth;
        int xmin = xmax - mCandleWidth;
        painter->setPen(mCandlePen);
        // так как ось Y расположена сверху вниз, а рисуем мы ее снизу вверх
        // значения надо отображать "зеркально"
        float xavg = (xmin + xmax) / 2;
        // объем свечи
        if (optShowVolumeGraph) {
            // если включено график объемов, нужно помнить,
            // что axisMaxY скорректирована, используем "реальный" axisMaxY
            int axisMaxYReal = axisMaxY + mAxisYVolumeHeight;
            float yvol = getCurrentAxisValue(
                QPoint(0, mAxisYVolumeHeight),
                mVolumeBounds,
                currCandle.volume
            );
            QRectF volumeRect = QRectF(
                QPointF(xmin, axisMaxYReal - yvol),
                QPointF(xmax, axisMaxYReal)
            );
            QColor volumeColor = (
                currCandle.close > currCandle.open ?
                    mCandleUpBrush :
                    mCandleDownBrush
                ).color();
            volumeColor.setAlpha(mCandleBrushAlpha);
            QBrush volumeBrush = QBrush(volumeColor, Qt::SolidPattern);
            painter->fillRect(
                volumeRect,
                volumeBrush
            );
        }
        // тень свечи
        QPoint yScale = QPoint(axisMinY, axisMaxY);
        float ymax = getCurrentAxisValue(yScale, mDataYBounds, currCandle.high);
        float ymin = getCurrentAxisValue(yScale, mDataYBounds, currCandle.low);
        float yopn = getCurrentAxisValue(yScale, mDataYBounds, currCandle.open);
        float ycls = getCurrentAxisValue(yScale, mDataYBounds, currCandle.close);
        painter->drawLine(
            QPointF(xavg, yScale.y() - (ymin - yScale.x())),
            QPointF(xavg, yScale.y() - (ymax - yScale.x()))
        );
        QRectF candleRect = QRectF(
            QPointF(xmin, yScale.y() - (yopn - yScale.x())),
            QPointF(xmax, yScale.y() - (ycls - yScale.x()))
        );
        // цветоное тело свечи
        painter->fillRect(
            candleRect,
            currCandle.close > currCandle.open ? mCandleUpBrush : mCandleDownBrush
        );
        // контур свечи
        painter->drawRect(candleRect);
    }

    // нарисуем выделение области на графике
    if (optSelectAreaWithMouse) {
        // обработаем команду стирания области
        if (mIsNeedClearArea) {
            mMousePressPos = QPoint(-1, -1);
            mMouseReleasePos = QPoint(-1, -1);
            mIsNeedClearArea = false;
        }
        int mx1 = -1;
        int my1 = -1;
        if (mMousePressPos.x() >= axisMinX &&
            mMousePressPos.x() < axisMaxX &&
            mMousePressPos.y() >= axisMinY &&
            mMousePressPos.y() < axisMaxY
        ) {
            mx1 = mMousePressPos.x();
            my1 = mMousePressPos.y();
        }
        // оси выделенной области не должны выходить за основные оси
        if (mx1 != -1 && my1 != -1) {
            drawAxisLines(
                painter,
                mMouseSelectAreaLabelsPen,
                QPoint(mx1, my1),
                QPoint(axisMinX, axisMaxX),
                QPoint(axisMinY, axisMaxY),
                offset,
                true
            );
            // метки на осях координат
            drawAxisLabels(
                painter,
                QPoint(mx1, my1),
                QPoint(axisMinX, axisMaxX),
                QPoint(axisMinY, axisMaxY),
                mDataYBounds,
                offset
            );
            int mx2 = mIsLmbMousePressed ? mMousePos.x() : mMouseReleasePos.x();
            int my2 = mIsLmbMousePressed ? mMousePos.y() : mMouseReleasePos.y();
            if ((mx2 != -1 && my2 != -1) || mIsLmbMousePressed) {
                // оси выделенной области не должны выходить за основные оси
                if (mx2 < axisMinX) {
                    mx2 = axisMinX;
                }
                if (mx2 > axisMaxX - 1) {
                    mx2 = axisMaxX - 1;
                }
                if (my2 < axisMinY) {
                    my2 = axisMinY;
                }
                if (my2 > axisMaxY - 1) {
                    my2 = axisMaxY - 1;
                }
                // оси и риски
                // риски рисуем только если клик был внутри области графика
                bool isDrawDashs = (mIsLmbMousePressed ? mMousePos.x() : mMouseReleasePos.x()) >= axisMinX &&
                    (mIsLmbMousePressed ? mMousePos.x() : mMouseReleasePos.x()) < axisMaxX &&
                    (mIsLmbMousePressed ? mMousePos.y() : mMouseReleasePos.y()) >= axisMinY &&
                    (mIsLmbMousePressed ? mMousePos.y() : mMouseReleasePos.y()) < axisMaxY;
                drawAxisLines(
                    painter,
                    mMouseSelectAreaLabelsPen,
                    QPoint(mx2, my2),
                    QPoint(axisMinX, axisMaxX),
                    QPoint(axisMinY, axisMaxY),
                    offset,
                    isDrawDashs
                );
                // метки на осях координат
                drawAxisLabels(
                    painter,
                    QPoint(mx2, my2),
                    QPoint(axisMinX, axisMaxX),
                    QPoint(axisMinY, axisMaxY),
                    mDataYBounds,
                    offset
                );
                // вывести метки на графике
                // область вывода правее и ниже пересечения осей
                QPoint lefttop2 = QPoint(
                    mx2 + 1,
                    my2
                );
                QPoint rightbottom2 = QPoint(
                    mx2 + 4*mAxisLabelHalfWidth + 1,
                    my2 + 2*mAxisLabelHalfHeight
                );
                // надпись не выходит за область графика
                if (rightbottom2.x() >= axisMaxX - 1) {
                    rightbottom2.setX(axisMaxX - 1);
                    lefttop2.setX(rightbottom2.x() - 4*mAxisLabelHalfWidth);
                }
                if (rightbottom2.y() >= axisMaxY - 1) {
                    rightbottom2.setY(axisMaxY - 1);
                    lefttop2.setY(rightbottom2.y() - 2*mAxisLabelHalfHeight);
                }
                // найти пройденное расстояние, для отображения на графике
                float xVal1 = getCurrentDataValue(
                    QPoint(axisMinX, axisMaxX),
                    mDataXBounds,
                    mx1
                );
                float yVal1 = getCurrentDataValue(
                    QPoint(axisMinY, axisMaxY),
                    mDataYBounds,
                    my1
                );
                float xVal2 = getCurrentDataValue(
                    QPoint(axisMinX, axisMaxX),
                    mDataXBounds,
                    mx2
                );
                float yVal2 = getCurrentDataValue(
                    QPoint(axisMinY, axisMaxY),
                    mDataYBounds,
                    my2
                );
                // рисуем значения в точке отпускания кнопки мыши
                painter->drawText(
                    QRect(lefttop2, rightbottom2),
                    Qt::AlignCenter,
                    makeAxisLabel(qAbs(xVal2 - xVal1)) +
                        QString(";") +
                        makeAxisLabel(qAbs(yVal2 - yVal1))
                );
                // зальем область между метками
                QColor mouseSelectAreaBrushColor = mMouseSelectAreaPen.color();
                mouseSelectAreaBrushColor.setAlpha(mMouseSelectAreaBrushAlpha);
                QBrush mouseSelectAreaBrush = QBrush(
                    mouseSelectAreaBrushColor,
                    Qt::SolidPattern
                );
                painter->fillRect(
                    QRect(QPoint(mx1, my1), QPoint(mx2, my2)),
                    mouseSelectAreaBrush
                );
            }
        }
    }

    // нарисуем оси курсора мыши с метками текущих значений
    if (optShowLabelsWithMouse) {
        int mx = mMousePos.x();
        int my = mMousePos.y();
        if (mx >= axisMinX &&
            mx < axisMaxX &&
            my >= axisMinY &&
            my < axisMaxY &&
            mIsMouseEnter
        ) {
            setCursor(Qt::CrossCursor);
            // если включено выделение области мышкой, и нажата кнопка мыши
            // не будем рисовать оси и риски, потому что они затрутся
            if (optSelectAreaWithMouse && mIsLmbMousePressed) {
                painter->setPen(mMouseSelectAreaLabelsPen);
            } else {
                // оси и риски
                drawAxisLines(
                    painter,
                    mMouseLabelPen,
                    QPoint(mx, my),
                    QPoint(axisMinX, axisMaxX),
                    QPoint(axisMinY, axisMaxY),
                    offset
                );
                // метки на осях координат
                drawAxisLabels(
                    painter,
                    QPoint(mx, my),
                    QPoint(axisMinX, axisMaxX),
                    QPoint(axisMinY, axisMaxY),
                    mDataYBounds,
                    offset
                );
            }
        } else if (optShowVolumeGraph &&
            mx >= axisMinX &&
            mx < axisMaxX &&
            my >= axisMaxY &&
            my < axisMaxY + mAxisYVolumeHeight &&
            mIsMouseEnter
        ) {
            // если отображаем график объема и находимся в его области
            setCursor(Qt::CrossCursor);
            // оси и риски
            drawAxisLines(
                painter,
                mMouseVolumeLabelPen,
                QPoint(mx, my),
                QPoint(axisMinX, axisMaxX),
                QPoint(axisMinY, axisMaxY),
                offset
            );
            // метки на осях координат
            drawAxisLabels(
                painter,
                QPoint(mx, my),
                QPoint(axisMinX, axisMaxX),
                QPoint(axisMaxY, axisMaxY + mAxisYVolumeHeight),
                mVolumeBounds,
                0
            );
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }

    // нарисуем скроллбар, если нужно
    if (optShowScrollArea && mViewedCandleCount < (int)mDataSeries.size()) {
        float scaledCandleWidth = 1.0 * (axisMaxX - axisMinX) / mDataSeries.size();
        int mergedCounter = 1;
        if (scaledCandleWidth < 1) {
            mergedCounter = ceil(1 / scaledCandleWidth);
            scaledCandleWidth *= mergedCounter;
        }
        // рисуем с конца графика
        float startX = axisMaxX;
        int startIndex = mDataSeries.size() - 1;
        int windowsCount = ceil(1.0 * mDataSeries.size() / mergedCounter);
        QPointF dataBounds = QPointF(
            mDataSeries.globalLow(),
            mDataSeries.globalHigh()
        );
        for (int i = 0; i < windowsCount; ++i) {
            // мержим свечи, для того чтоб получить упрощенную свечу окна
            float high = 0, low = INFINITY;
            float open = mDataSeries.data()[startIndex].open;
            for (int j = 0; j < mergedCounter; ++j) {
                if (mDataSeries.data()[startIndex].high > high) {
                    high = mDataSeries.data()[startIndex].high;
                }
                if (mDataSeries.data()[startIndex].low < low) {
                    low = mDataSeries.data()[startIndex].low;
                }
                startIndex--;
                // количевство свечей в окнах округлено, поэтому последнее окно
                // может быть неполным, проверяем чтоб не выйти за границы
                if (startIndex < 0) {
                    break;
                }
            }
            float close = mDataSeries.data()[startIndex].close;
            // определим цвет свечи по разнице открытия и закрытия
            QColor color = (close > open ? mCandleUpBrush : mCandleDownBrush).color();
            // скроллбар расположен в самом низу виджета, поэтому область для
            // рисования определяем от самого низа
            QPoint yScale = QPoint(
                maxY - mAxisYScrollBarHeight,
                maxY
            );
            float ymax = getCurrentAxisValue(yScale, dataBounds, high);
            float ymin = getCurrentAxisValue(yScale, dataBounds, low);
            QRectF candleRect = QRectF(
                QPointF(startX, yScale.y() - (ymax - yScale.x())),
                QPointF(startX - scaledCandleWidth, yScale.y() - (ymin - yScale.x()))
            );
            // цветоное тело свечи
            painter->fillRect(
                candleRect,
                QBrush(color)
            );
            // контур свечи
            painter->setPen(QPen(color));
            painter->drawRect(candleRect);
            // скорректируем текущую координату для рисования
            startX -= scaledCandleWidth;
        }
        // нарисуем текущее отображаемое окно на скроллбаре
        float areaWidth = 1.0 * mViewedCandleCount / mDataSeries.size() * (axisMaxX - axisMinX);
        float areaStart = 1.0 * mCandleOffsetFromEnd /  mDataSeries.size() * (axisMaxX - axisMinX);
        // рисуем с правого края, поэтому координаты по Х инвертим
        painter->drawRect(
            QRectF(
                QPointF(axisMaxX - areaStart, maxY - 1),
                QPointF(axisMaxX - areaWidth, maxY - mAxisYScrollBarHeight)
            )
        );
    }
}

QString Widget::makeAxisLabel(const float value) const
{
    QString label = QString::number(value, 'f');
    if (label.length() > mMaxAxisLabelLength) {
        int dotPos = label.indexOf('.');
        if (dotPos != -1) {
            while (label.length() > mMaxAxisLabelLength && dotPos < label.length() - 2) {
                label.remove(label.length() - 1, 1);
            }
            while (label.at(label.length() - 1) == '0' && dotPos < label.length() - 2) {
                label.remove(label.length() - 1, 1);
            }
        }
    } else {
        while (label.length() <= mMaxAxisLabelLength) {
            label.append(' ');
        }
    }
    return label;
}

float Widget::getCurrentDataValue(
    const QPoint &axisBounds,
    const QPointF &dataBounds,
    const int currentAxisValue
) const
{
    if (currentAxisValue != axisBounds.x()) {
        return (dataBounds.y() - dataBounds.x()) *
            (currentAxisValue - axisBounds.x()) /
            (axisBounds.y() - axisBounds.x()) +
            dataBounds.x();
    } else {
        return dataBounds.x();
    }
}

float Widget::getCurrentAxisValue(
    const QPoint &axisBounds,
    const QPointF &dataBounds,
    const float currentDataValue
) const
{
    return 1.0 * (currentDataValue - dataBounds.x()) /
        (dataBounds.y() - dataBounds.x()) *
        (axisBounds.y() - axisBounds.x()) +
        axisBounds.x();
}

QRect Widget::getRectForAxisLabel(
    int val,
    const QPoint &axisXBounds,
    const QPoint &axisYBounds,
    bool isAxisLabelX
) const
{
    QPoint lefttop, rightbottom;
    if (isAxisLabelX) {
        lefttop = QPoint(
            val - mAxisLabelHalfWidth,
            axisYBounds.y() + mAxisXDashLen + mAxisXDashSpace
        );
        rightbottom = QPoint(
            val + mAxisLabelHalfWidth,
            axisYBounds.y() + mAxisXDashLen + 2*mAxisLabelHalfHeight + mAxisXDashSpace
        );
        // область вывода не выводим за границы графика
        if (lefttop.x() <= axisXBounds.x() + 1) {
            lefttop.setX(axisXBounds.x() + 1);
            rightbottom.setX(lefttop.x() + 2*mAxisLabelHalfWidth);
        }
        if (rightbottom.x() >= axisXBounds.y() - 1) {
            rightbottom.setX(axisXBounds.y() - 1);
            lefttop.setX(rightbottom.x() - 2*mAxisLabelHalfWidth);
        }
    } else {
        lefttop = QPoint(
            axisXBounds.y() + mAxisYDashLen + mAxisYDashSpace,
            val - mAxisLabelHalfHeight
        );
        rightbottom = QPoint(
            axisXBounds.y() + mAxisYDashLen + 2*mAxisLabelHalfWidth + mAxisYDashSpace,
            val + mAxisLabelHalfHeight
        );
        // область вывода не выводим за границы графика
        if (lefttop.y() <= axisYBounds.x() + 1) {
            lefttop.setY(axisYBounds.x() + 1);
            rightbottom.setY(lefttop.y() + 2*mAxisLabelHalfHeight);
        }
        if (rightbottom.y() >= axisYBounds.y() - 1) {
            rightbottom.setY(axisYBounds.y() - 1);
            lefttop.setY(rightbottom.y() - 2*mAxisLabelHalfHeight);
        }
    }
    return QRect(lefttop, rightbottom);
}

QRect Widget::getOuterRectForAxisLabel(
    const QRect &labelRect
) const
{
    return QRect(
        QPoint(
            labelRect.topLeft().x() - mAxisLabelXAdditionalLength,
            labelRect.topLeft().y() - mAxisLabelYAdditionalLength
        ),
        QPoint(
            labelRect.bottomRight().x() + mAxisLabelXAdditionalLength,
            labelRect.bottomRight().y() + mAxisLabelYAdditionalLength
        )
    );
}

void Widget::drawAxisLabels(
    QPainter *painter,
    const QPoint &pos,
    const QPoint &axisXBounds,
    const QPoint &axisYBounds,
    const QPointF dataYBounds,
    int offset
) const
{
    // нарисуем метку на оси Х
    QRect labelRect = getRectForAxisLabel(
        pos.x(),
        axisXBounds,
        QPoint(axisYBounds.x(), axisYBounds.y() + offset),
        true
    );
    QRect biggerRect = getOuterRectForAxisLabel(labelRect);
    painter->fillRect(biggerRect, mBackgroundBrush);
    painter->drawRect(biggerRect);
    float valueX = getCurrentDataValue(
        axisXBounds,
        mDataXBounds,
        pos.x()
    );
    painter->drawText(
        labelRect,
        Qt::AlignCenter,
        makeAxisLabel(valueX)
    );
    // нарисуем метку на оси Y
    labelRect = getRectForAxisLabel(
        pos.y(),
        axisXBounds,
        axisYBounds,
        false
    );
    biggerRect = getOuterRectForAxisLabel(labelRect);
    painter->fillRect(biggerRect, mBackgroundBrush);
    painter->drawRect(biggerRect);
    // так как ось Y расположена сверху вниз, а рисуем мы ее снизу вверх
    // значения надо отображать "зеркально"
    float valueY = getCurrentDataValue(
        axisYBounds,
        dataYBounds,
        axisYBounds.y() - (pos.y() - axisYBounds.x())
    );
    painter->drawText(
        labelRect,
        Qt::AlignCenter,
        makeAxisLabel(valueY)
    );
}

void Widget::drawAxisLines(
    QPainter *painter,
    const QPen &axisLabelsPen,
    const QPoint &pos,
    const QPoint &axisXBounds,
    const QPoint &axisYBounds,
    int offset,
    bool isDrawDashs
) const
{
    QPen axisPen = QPen(axisLabelsPen);
    axisPen.setStyle(Qt::DashLine);
    painter->setPen(axisPen);
    // оси
    painter->drawLine(
        QPoint(pos.x(), axisYBounds.x()),
        QPoint(pos.x(), axisYBounds.y() + offset + mAxisXDashLen)
    );
    painter->drawLine(
        QPoint(axisXBounds.x(), pos.y()),
        QPoint(axisXBounds.y() + mAxisYDashLen, pos.y())
    );
    painter->setPen(axisLabelsPen);
    // риски
    if (isDrawDashs) {
        painter->drawLine(
            QPoint(pos.x(), axisYBounds.y() + offset),
            QPoint(pos.x(), axisYBounds.y() + offset + mAxisXDashLen)
        );
        painter->drawLine(
            QPoint(axisXBounds.y(), pos.y()),
            QPoint(axisXBounds.y() + mAxisYDashLen, pos.y())
        );
    }
}
