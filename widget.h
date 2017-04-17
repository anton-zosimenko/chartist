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
    void wheelEvent(QWheelEvent *event) override;
private:
    void paint(QPainter *painter, QPaintEvent *event);
    QString makeAxisLabel(const float value) const;
    float getCurrentAxisValue(
        const QPoint &axisBounds,
        const QPointF &dataBounds,
        const int currentAxisValue
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

    QBrush mBackgroundBrush;
    QPen mAxisPen;
    QPen mMouseAxisPen;
    QPen mMouseLabelPen;
    QPen mMouseSelectAreaPen;
    QBrush mMouseSelectAreaBrush;
    QPen mMouseSelectAreaLabelsPen;

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

    bool optShowLabelsWithMouse;
    bool optSelectAreaWithMouse;

    DataSeries mDataSeries;
};

#endif
