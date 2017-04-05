#include "widget.h"
#include "window.h"

#include <QGridLayout>
#include <QLabel>

Window::Window()
{
    setWindowTitle(tr("2D Painting"));

    Widget *native = new Widget(this);
    QLabel *nativeLabel = new QLabel(tr("Native"));
    nativeLabel->setAlignment(Qt::AlignHCenter);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(native, 1, 0);
    layout->addWidget(nativeLabel, 0, 0);
    setLayout(layout);
}
