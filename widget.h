#ifndef WIDGET_H
#define WIDGET_H

#include "core.h"

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QString>
#include <QPoint>
#include <QRect>
#include <QPointF>
#include <QRectF>

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent, const QString &fileName);
    bool showLabelsWithMouse() const;
    void setShowLabelsWithMouse(bool newValue);
    bool selectAreaWithMouse() const;
    void setSelectAreaWithMouse(bool newValue);
    bool showVolumeGraph() const;
    void setShowVolumeGraph(bool newValue);
    bool showScrollArea() const;
    void setShowScrollArea(bool newValue);
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
    float getCurrentAxisValue(
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
        const QPoint &axisYBounds, const QPointF dataYBounds,
        int offset
    ) const;
    void drawAxisLines(
        QPainter *painter,
        const QPen &axisLabelsPen,
        const QPoint &pos,
        const QPoint &axisXBounds,
        const QPoint &axisYBounds,
        int offset,
        bool isDrawDashs=true
    ) const;

    QPointF mDataXBounds;
    QPointF mDataYBounds;
    QPointF mVolumeBounds;
    QPoint mMousePos;
    QPoint mMousePressPos;
    QPoint mMouseReleasePos;
    QPoint mMouseGraphPressPos;
    QPoint mMouseGraphReleasePos;
    bool mIsMouseEnter;
    bool mIsLmbMousePressed;
    bool mIsLmbMousePress;
    bool mIsLmbMouseRelease;
    bool mIsResize;
    bool mIsCandleWidthChanged;
    bool mIsNeedClearArea;
    bool mIsMousePressInGraph;

    QBrush mBackgroundBrush;
    QPen mAxisPen;
    QPen mMouseAxisPen;
    QPen mMouseAxisVolumePen;
    QPen mMouseLabelPen;
    QPen mMouseVolumeLabelPen;
    QPen mMouseSelectAreaPen;
    int mMouseSelectAreaBrushAlpha;
    QPen mMouseSelectAreaLabelsPen;
    QPen mCandlePen;
    QPen mScrollBarPen;
    QBrush mCandleUpBrush;
    QBrush mCandleDownBrush;
    int mCandleBrushAlpha;

    int mAxisXLeftBorderLength;
    int mAxisXRightBorderLength;
    int mAxisYTopBorderLength;
    int mAxisYBottomBorderLength;
    int mAxisXDashCount;
    int mAxisYDashCount;
    int mAxisYVolumeDashCount;
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
    int mBetweenCandlesWidth;
    int mViewedCandleCount;
    int mCandleMinWidth;
    int mCandleMaxWidth;
    int mAxisYVolumeHeight;
    int mAxisYScrollBarHeight;
    int mCandleOffsetFromEnd;

    bool optShowLabelsWithMouse;
    bool optSelectAreaWithMouse;
    bool optShowVolumeGraph;
    bool optShowScrollArea;

    DataSeries mDataSeries;
};

#endif
