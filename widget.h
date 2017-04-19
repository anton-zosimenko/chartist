#ifndef WIDGET_H
#define WIDGET_H

#include "core.h"

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QString>

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent, const QString &fileName);
    bool showLabelsWithMouse() const;
    void setShowLabelsWithMouse(bool newValue);
    bool selectAreaWithMouse() const;
    void setSelectAreaWithMouse(bool newValue);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    void paint(QPainter *painter, QPaintEvent *event);
    QString makeAxisLabel(const float value) const;
    float getCurrentDataValue(
        const QPoint &axisBounds,
        const QPointF &dataBounds,
        const int currentAxisValue
    ) const;
    int getCurrentAxisValue(
        const QPoint &axisBounds,
        const QPointF &dataBounds,
        const float currentDataValue
    ) const;
    QRect getRectForAxisLabel(
        int val,
        const QPoint &axisXBounds,
        const QPoint &axisYBounds,
        bool isAxisLabelX
    ) const;
    QRect getOuterRectForAxisLabel(
        const QRect &labelRect
    ) const;
    void drawAxisLabels(
        QPainter *painter,
        const QPoint &pos,
        const QPoint &axisXBounds,
        const QPoint &axisYBounds
    ) const;

    QPointF mDataXBounds;
    QPointF mDataYBounds;
    QPoint mMousePos;
    QPoint mMousePressPos;
    QPoint mMouseReleasePos;
    bool mIsMouseEnter;
    bool mIsMousePressed;
    bool mIsResize;
    bool mIsCandleWidthChanged;

    QBrush mBackgroundBrush;
    QPen mAxisPen;
    QPen mMouseAxisPen;
    QPen mMouseLabelPen;
    QPen mMouseSelectAreaPen;
    QBrush mMouseSelectAreaBrush;
    QPen mMouseSelectAreaLabelsPen;
    QPen mCandlePen;
    QBrush mCandleUpBrush;
    QBrush mCandleDownBrush;

    int mAxisXLeftBorderLength;
    int mAxisXRightBorderLength;
    int mAxisYTopBorderLength;
    int mAxisYBottomBorderLength;
    int mAxisXDashCount;
    int mAxisYDashCount;
    int mAxisXDashLen;
    int mAxisYDashLen;
    int mAxisXDashSpace;
    int mAxisYDashSpace;
    int mAxisLabelHalfWidth;
    int mAxisLabelHalfHeight;
    int mMaxAxisLabelLength;
    int mAxisLabelXAdditionalLength;
    int mAxisLabelYAdditionalLength;
    int mCandleWidth;
    int mCandleCount;
    int mCandleMinWidth;
    int mCandleMaxWidth;

    bool optShowLabelsWithMouse;
    bool optSelectAreaWithMouse;

    DataSeries mDataSeries;
};

#endif
