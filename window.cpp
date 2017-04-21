#include "widget.h"
#include "window.h"

#include <QGridLayout>
#include <QLabel>
#include <QString>

Window::Window()
{
    QString fileName = "GAZP_170329_170413.csv";
    setWindowTitle("Chartist - " + fileName);

    Widget *widget = new Widget(
        this,
        "C:\\Works\\chartist\\data\\" + fileName
    );

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(widget);
    setLayout(layout);
}
