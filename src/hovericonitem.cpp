#include <QPainter>
#include <QPen>

#include "hovericonitem.h"

HoverIconItem::HoverIconItem(int width, int height, QWidget *parent)
    : QWidget(parent)
    , _width(width)
    , _height(height)
{
    resize(_width, _height);
}

HoverIconItem::~HoverIconItem()
{

}

void HoverIconItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(QBrush(QColor(255, 255, 255, 25)));
    painter.drawRoundRect(QRect(0, 0, _width - 1, _height - 1), 5, 5);

    Q_UNUSED(event);
}
