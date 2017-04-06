#include "widget.h"
#include "window.h"

#include <QGridLayout>
#include <QLabel>

Window::Window()
{
    setWindowTitle(tr("Chartist"));

    Widget *native = new Widget(this);
    QLabel *nativeLabel = new QLabel(tr("Look at chart below"));
    nativeLabel->setAlignment(Qt::AlignHCenter);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(native, 1, 0);
    layout->addWidget(nativeLabel, 0, 0);
    setLayout(layout);
}
