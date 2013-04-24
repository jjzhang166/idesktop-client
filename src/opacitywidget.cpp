#include "opacitywidget.h"

OpacityWidget::OpacityWidget(QWidget *parent, Qt::WindowFlags f , const int &level) :
    QWidget(parent, f)
{

    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(215,255,255,level));
    setPalette(pal);
}
