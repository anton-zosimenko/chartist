#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QBrush>
#include <QPen>

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent);
    bool showMouseLabels() const;
    void setShowMouseLabels(bool newValue);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    void paint(QPainter *painter, QPaintEvent *event);
    QString makeAxisLabel(const float value) const;
    float getCurrentValue(
        const QPoint &axisBounds,
        const QPointF &dataBounds,
        const int currentAxisValue
    ) const;

    QPointF mDataXBounds;
    QPointF mDataYBounds;
    QPoint mMousePos;
    QPoint mMousePressPos;
    QPoint mMouseReleasePos;

    QBrush mBackgroundBrush;
    QPen mAxisPen;
    QPen mMouseAxisPen;
    QPen mMouseLabelPen;

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

    bool optShowMouseLabels;
};

#endif
