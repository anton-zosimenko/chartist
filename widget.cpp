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

    // нарисуем риски и метки
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

    // нарисуем оси курсора мыши с метками текущих значений
    if (optShowLabelsWithMouse) {
        int mx = mMousePos.x();
        int my = mMousePos.y();
        if (mx >= axisMinX && mx < axisMaxX && my >= axisMinY && my < axisMaxY && mIsMouseEnter) {
            setCursor(Qt::CrossCursor);
            painter->setPen(mMouseAxisPen);
            // оси
            painter->drawLine(QPoint(mx, axisMinY), QPoint(mx, axisMaxY + mAxisXDashLen));
            painter->drawLine(QPoint(axisMinX, my), QPoint(axisMaxX + mAxisYDashLen, my));
            painter->setPen(mMouseLabelPen);
            // риски
            painter->drawLine(QPoint(mx, axisMaxY), QPoint(mx, axisMaxY + mAxisXDashLen));
            painter->drawLine(QPoint(axisMaxX, my), QPoint(axisMaxX + mAxisYDashLen, my));
            // метки
            QPoint lefttop, rightbottom;
            lefttop = QPoint(
                mx - mAxisLabelHalfWidth,
                axisMaxY + mAxisXDashLen + mAxisXDashSpace
            );
            rightbottom = QPoint(
                mx + mAxisLabelHalfWidth,
                axisMaxY + mAxisXDashLen + 2*mAxisLabelHalfHeight + mAxisXDashSpace
            );
            if (lefttop.x() <= axisMinX + 1) {
                lefttop.setX(axisMinX + 1);
                rightbottom.setX(lefttop.x() + 2*mAxisLabelHalfWidth);
            } else if (rightbottom.x() >= axisMaxX - 1) {
                rightbottom.setX(axisMaxX - 1);
                lefttop.setX(rightbottom.x() - 2*mAxisLabelHalfWidth);
            }
            QRect biggerRect;
            biggerRect = QRect(
                QPoint(
                    lefttop.x() - mAxisLabelXAdditionalLength,
                    lefttop.y() - mAxisLabelYAdditionalLength
                ),
                QPoint(
                    rightbottom.x() + mAxisLabelXAdditionalLength,
                    rightbottom.y() + mAxisLabelYAdditionalLength
                )
            );
            painter->fillRect(biggerRect, mBackgroundBrush);
            painter->drawRect(biggerRect);
            painter->drawText(
                QRect(lefttop, rightbottom),
                Qt::AlignCenter,
                makeAxisLabel(getCurrentValue(QPoint(axisMinX, axisMaxX), mDataXBounds, mx))
            );
            lefttop = QPoint(
                axisMaxX + mAxisYDashLen + mAxisYDashSpace,
                my - mAxisLabelHalfHeight
            );
            rightbottom = QPoint(
                axisMaxX + mAxisYDashLen + 2*mAxisLabelHalfWidth + mAxisYDashSpace,
                my + mAxisLabelHalfHeight
            );
            if (lefttop.y() <= axisMinY + 1) {
                lefttop.setY(axisMinY + 1);
                rightbottom.setY(lefttop.y() + 2*mAxisLabelHalfHeight);
            } else if (rightbottom.y() >= axisMaxY - 1) {
                rightbottom.setY(axisMaxY - 1);
                lefttop.setY(rightbottom.y() - 2*mAxisLabelHalfHeight);
            }
            biggerRect = QRect(
                QPoint(
                    lefttop.x() - mAxisLabelXAdditionalLength,
                    lefttop.y() - mAxisLabelYAdditionalLength
                ),
                QPoint(
                    rightbottom.x() + mAxisLabelXAdditionalLength,
                    rightbottom.y() + mAxisLabelYAdditionalLength
                )
            );
            painter->fillRect(biggerRect, mBackgroundBrush);
            painter->drawRect(biggerRect);
            painter->drawText(
                QRect(lefttop, rightbottom),
                Qt::AlignLeft,
                makeAxisLabel(getCurrentValue(QPoint(axisMinY, axisMaxY), mDataYBounds, axisMaxY - my))
            );
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }

    // нарисуем выделение области на графике
    if (optSelectAreaWithMouse) {
        int mx = mMousePressPos.x();
        int my = mMousePressPos.y();
        if (mx != -1 && my != -1) {
            // оси выделенной области не должны выходить за основные оси
            if (mx < axisMinX) {
                mx = axisMinX;
            }
            if (mx > axisMaxX - 1) {
                mx = axisMaxX - 1;
            }
            if (my < axisMinY) {
                my = axisMinY;
            }
            if (my > axisMaxY - 1) {
                my = axisMaxY - 1;
            }
            painter->setPen(mMouseSelectAreaPen);
            painter->drawLine(QPoint(mx, axisMinY), QPoint(mx, axisMaxY));
            painter->drawLine(QPoint(axisMinX, my), QPoint(axisMaxX, my));
            mx = mIsMousePressed ? mMousePos.x() : mMouseReleasePos.x();
            my = mIsMousePressed ? mMousePos.y() : mMouseReleasePos.y();
            if ((mx != -1 && my != -1) || mIsMousePressed) {
                // оси выделенной области не должны выходить за основные оси
                if (mx < axisMinX) {
                    mx = axisMinX;
                }
                if (mx > axisMaxX - 1) {
                    mx = axisMaxX - 1;
                }
                if (my < axisMinY) {
                    my = axisMinY;
                }
                if (my > axisMaxY - 1) {
                    my = axisMaxY - 1;
                }
                painter->drawLine(QPoint(mx, axisMinY), QPoint(mx, axisMaxY));
                painter->drawLine(QPoint(axisMinX, my), QPoint(axisMaxX, my));
            }
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
    }
    return label;
}

float Widget::getCurrentValue(
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
