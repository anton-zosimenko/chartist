#include "widget.h"
#include "window.h"

#include <QGridLayout>
#include <QLabel>
#include <QString>

Window::Window()
{
    setWindowTitle(tr("Chartist"));

    QString fileName = "GAZP_170329_170413.csv";
    QLabel *nativeLabel = new QLabel(fileName);
    Widget *native = new Widget(
        this,
        "C:\\Works\\chartist\\data\\" + fileName
    );
    nativeLabel->setAlignment(Qt::AlignHCenter);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(native, 1, 0);
    layout->addWidget(nativeLabel, 0, 0);
    setLayout(layout);
}
