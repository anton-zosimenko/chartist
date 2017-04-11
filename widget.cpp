#include "widget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    optShowLabelsWithMouse = true;
    optSelectAreaWithMouse = true;

    setMinimumSize(640, 480);

    mDataXBounds = QPointF(-11111, 11111);
    mDataYBounds = QPointF(0, 0.777);

    mMousePos = QPoint(-1, -1);
    setMouseTracking(true);
    mMousePressPos = QPoint(-1, -1);
    mMouseReleasePos = QPoint(-1, -1);
    mIsMouseEnter = false;
    mIsMousePressed = false;

    mBackgroundBrush = QBrush(Qt::white);
    mAxisPen = QPen(Qt::black, 1);
    mMouseAxisPen = QPen(Qt::blue, 1);
    mMouseAxisPen.setStyle(Qt::DashLine);
    mMouseLabelPen = QPen(Qt::blue, 1);
    mMouseSelectAreaPen = QPen(Qt::gray, 1);
    mMouseSelectAreaPen.setStyle(Qt::DashLine);
    QColor mouseSelectAreaBrushColor = QColor(Qt::gray);
    mouseSelectAreaBrushColor.setAlpha(80);
    mMouseSelectAreaBrush = QBrush(mouseSelectAreaBrushColor, Qt::SolidPattern);
    mMouseSelectAreaLabelsPen = QPen(mMouseSelectAreaPen.color(), 1);

    mAxisXLeftBorderLength = 0;
    mAxisXRightBorderLength = 52;
    mAxisYTopBorderLength = 0;
    mAxisYBottomBorderLength = 20;
    mAxisXDashCount = 10;
    mAxisYDashCount = 10;
    mAxisXDashLen = 4;
    mAxisYDashLen = 4;
    mAxisXDashSpace = 2;
    mAxisYDashSpace = 4;
    mAxisLabelHalfWidth = 20;
    mAxisLabelHalfHeight = 5;
    mMaxAxisLabelLength = 6;
    mAxisLabelXAdditionalLength = 1;
    mAxisLabelYAdditionalLength = 1;
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
    if (optSelectAreaWithMouse) {
        if (event->button() == Qt::LeftButton) {
            mIsMousePressed = true;
            mMousePressPos = event->pos();
            update();
        } else if (event->button() == Qt::RightButton) {
            mIsMousePressed = false;
            mMousePressPos = QPoint(-1, -1);
            mMouseReleasePos = QPoint(-1, -1);
            update();
        }
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (optSelectAreaWithMouse) {
        if (event->button() == Qt::LeftButton) {
            mIsMousePressed = false;
            mMouseReleasePos = event->pos();
            update();
        }
    }
}

void Widget::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
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

    // нарисуем оси
    painter->setPen(mAxisPen);
    painter->drawLine(QPoint(axisMinX, axisMaxY), QPoint(axisMaxX, axisMaxY));
    painter->drawLine(QPoint(axisMaxX, axisMinY), QPoint(axisMaxX, axisMaxY));

    // нарисуем риски и данные на осях координат
    float deltaX = 1.0 * (axisMaxX - axisMinX) / mAxisXDashCount;
    float dataDeltaX = (mDataXBounds.y() - mDataXBounds.x()) / mAxisXDashCount;
    for (int i = 1; i < mAxisXDashCount; ++i) {
        float x = axisMinX + i*deltaX;
        painter->drawLine(QPointF(x, axisMaxY), QPointF(x, axisMaxY + mAxisXDashLen));
        painter->drawText(
            QRect(
                QPoint(
                    x - mAxisLabelHalfWidth,
                    axisMaxY + mAxisXDashLen + mAxisXDashSpace
                ),
                QPoint(
                    x + mAxisLabelHalfWidth,
                    axisMaxY + mAxisXDashLen + 2*mAxisLabelHalfHeight + mAxisXDashSpace
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

    // нарисуем выделение области на графике
    if (optSelectAreaWithMouse) {
        int mx1 = mMousePressPos.x();
        int my1 = mMousePressPos.y();
        if (mx1 != -1 && my1 != -1) {
            // оси выделенной области не должны выходить за основные оси
            if (mx1 < axisMinX) {
                mx1 = axisMinX;
            }
            if (mx1 > axisMaxX - 1) {
                mx1 = axisMaxX - 1;
            }
            if (my1 < axisMinY) {
                my1 = axisMinY;
            }
            if (my1 > axisMaxY - 1) {
                my1 = axisMaxY - 1;
            }
            // оси
            painter->setPen(mMouseSelectAreaPen);
            painter->drawLine(QPoint(mx1, axisMinY), QPoint(mx1, axisMaxY));
            painter->drawLine(QPoint(axisMinX, my1), QPoint(axisMaxX, my1));
            // риски рисуем только если клик был внутри области графика
            painter->setPen(mMouseSelectAreaLabelsPen);
            if (mMousePressPos.x() >= axisMinX &&
                mMousePressPos.x() < axisMaxX &&
                mMousePressPos.y() >= axisMinY &&
                mMousePressPos.y() < axisMaxY
            ) {
                painter->drawLine(
                    QPoint(mx1, axisMaxY),
                    QPoint(mx1, axisMaxY + mAxisXDashLen)
                );
                painter->drawLine(
                    QPoint(axisMaxX, my1),
                    QPoint(axisMaxX + mAxisYDashLen, my1)
                );
            }
            // метки на осях координат
            drawAxisLabels(
                painter,
                QPoint(mx1, my1),
                QPoint(axisMinX, axisMaxX),
                QPoint(axisMinY, axisMaxY)
            );
            int mx2 = mIsMousePressed ? mMousePos.x() : mMouseReleasePos.x();
            int my2 = mIsMousePressed ? mMousePos.y() : mMouseReleasePos.y();
            if ((mx2 != -1 && my2 != -1) || mIsMousePressed) {
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
                // оси
                painter->setPen(mMouseSelectAreaPen);
                painter->drawLine(QPoint(mx2, axisMinY), QPoint(mx2, axisMaxY));
                painter->drawLine(QPoint(axisMinX, my2), QPoint(axisMaxX, my2));
                // риски рисуем только если мышь внутри области графика
                painter->setPen(mMouseSelectAreaLabelsPen);
                if ((mIsMousePressed ? mMousePos.x() : mMouseReleasePos.x()) >= axisMinX &&
                    (mIsMousePressed ? mMousePos.x() : mMouseReleasePos.x()) < axisMaxX &&
                    (mIsMousePressed ? mMousePos.y() : mMouseReleasePos.y()) >= axisMinY &&
                    (mIsMousePressed ? mMousePos.y() : mMouseReleasePos.y()) < axisMaxY
                ) {
                    painter->drawLine(
                        QPoint(mx2, axisMaxY),
                        QPoint(mx2, axisMaxY + mAxisXDashLen)
                    );
                    painter->drawLine(
                        QPoint(axisMaxX, my2),
                        QPoint(axisMaxX + mAxisYDashLen, my2)
                    );
                }
                // метки на осях координат
                drawAxisLabels(
                    painter,
                    QPoint(mx2, my2),
                    QPoint(axisMinX, axisMaxX),
                    QPoint(axisMinY, axisMaxY)
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
                float xVal1 = getCurrentAxisValue(
                    QPoint(axisMinX, axisMaxX),
                    mDataXBounds,
                    mx1
                );
                float yVal1 = getCurrentAxisValue(
                    QPoint(axisMinY, axisMaxY),
                    mDataYBounds,
                    my1
                );
                float xVal2 = getCurrentAxisValue(
                    QPoint(axisMinX, axisMaxX),
                    mDataXBounds,
                    mx2
                );
                float yVal2 = getCurrentAxisValue(
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
                painter->fillRect(
                    QRect(QPoint(mx1, my1), QPoint(mx2, my2)),
                    mMouseSelectAreaBrush
                );
            }
        }
    }

    // нарисуем оси курсора мыши с метками текущих значений
    if (optShowLabelsWithMouse) {
        int mx = mMousePos.x();
        int my = mMousePos.y();
        if (mx >= axisMinX && mx < axisMaxX && my >= axisMinY && my < axisMaxY && mIsMouseEnter) {
            setCursor(Qt::CrossCursor);
            // если включено выделение области мышкой, и нажата кнопка мыши
            // не будем рисовать оси и риски, потому что они затрутся
            if (optSelectAreaWithMouse && mIsMousePressed) {
                painter->setPen(mMouseSelectAreaLabelsPen);
            } else {
                painter->setPen(mMouseAxisPen);
                // оси
                painter->drawLine(
                    QPoint(mx, axisMinY),
                    QPoint(mx, axisMaxY + mAxisXDashLen)
                );
                painter->drawLine(
                    QPoint(axisMinX, my),
                    QPoint(axisMaxX + mAxisYDashLen, my)
                );
                painter->setPen(mMouseLabelPen);
                // риски
                painter->drawLine(
                    QPoint(mx, axisMaxY),
                    QPoint(mx, axisMaxY + mAxisXDashLen)
                );
                painter->drawLine(
                    QPoint(axisMaxX, my),
                    QPoint(axisMaxX + mAxisYDashLen, my)
                );
            }
            // метки на осях координат
            drawAxisLabels(
                painter,
                QPoint(mx, my),
                QPoint(axisMinX, axisMaxX),
                QPoint(axisMinY, axisMaxY)
            );
        } else {
            setCursor(Qt::ArrowCursor);
        }
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
        }
    } else {
        while (label.length() <= mMaxAxisLabelLength) {
            label.append(' ');
        }
    }
    return label;
}

float Widget::getCurrentAxisValue(
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
    const QPoint &axisYBounds
) const
{
    QRect labelRect = getRectForAxisLabel(
        pos.x(),
        axisXBounds,
        axisYBounds,
        true
    );
    QRect biggerRect = getOuterRectForAxisLabel(labelRect);
    painter->fillRect(biggerRect, mBackgroundBrush);
    painter->drawRect(biggerRect);
    float valueX = getCurrentAxisValue(
        axisXBounds,
        mDataXBounds,
        pos.x()
    );
    painter->drawText(
        labelRect,
        Qt::AlignCenter,
        makeAxisLabel(valueX)
    );
    labelRect = getRectForAxisLabel(
        pos.y(),
        axisXBounds,
        axisYBounds,
        false
    );
    biggerRect = getOuterRectForAxisLabel(labelRect);
    painter->fillRect(biggerRect, mBackgroundBrush);
    painter->drawRect(biggerRect);
    float valueY = getCurrentAxisValue(
        axisYBounds,
        mDataYBounds,
        pos.y()
    );
    painter->drawText(
        labelRect,
        Qt::AlignCenter,
        makeAxisLabel(valueY)
    );
}
