#include <QPainter>
#include <QtDebug>

#include "movewidget.h"

class IconItem;
MoveWidget::MoveWidget(QWidget *parent)
    : QWidget(parent)
    , _width(0)
    , _height(0)
{
    setAcceptDrops(true);
}

MoveWidget::~MoveWidget()
{

}

void MoveWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _width, _height, _pixmap);
}

void MoveWidget::resizeEvent(QResizeEvent *event)
{
    _width = width();
    _height = height();

    update();

    QWidget::resizeEvent(event);
}

void MoveWidget::setPixmap(const QPixmap &pix)
{
    _pixmap = pix;

    update();
}

void MoveWidget::mousePressEvent(QMouseEvent *)
{
    emit mousePress();
}

//void MoveWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    qDebug() << event->pos().x();
//    qDebug() << event->pos().y();
//}
#if 1
void MoveWidget::dragEnterEvent(QDragEnterEvent *event)
{
    _dragEvent = true;

    event->acceptProposedAction();

    emit moveWidgetDragEnter();
}


void MoveWidget::dragMoveEvent(QDragMoveEvent *event)
{
    _dragEvent = true;

    event->acceptProposedAction();

    emit moveWidgetDragMove();
}

void MoveWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    _dragEvent = false;

    emit moveWidgetDragLeave();

    Q_UNUSED(event);
}

void MoveWidget::dropEvent(QDropEvent *event)
{

    event->acceptProposedAction();

    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (icon)
        emit moveWidgetDrop(icon);

    _dragEvent = false;
}
#endif

MoveMinWidget::MoveMinWidget(QWidget *parent)
    : QWidget(parent)
    , _width(0)
    , _height(0)
{

}

MoveMinWidget::~MoveMinWidget()
{

}

void MoveMinWidget::paintEvent(QPaintEvent *)
{
    static const QPointF points[4] = {
        QPointF(_width / 2, 0),
        QPointF(0, _height),
        QPointF(_width, _height),
    };

    QPainter painter(this);
    painter.setPen(QPen(QBrush(QColor(255,255,255,0)), 1, Qt::SolidLine,
                        Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(QBrush(_pixmap.scaled(_width, _height)));
    painter.drawConvexPolygon(points, 3);
}

void MoveMinWidget::resizeEvent(QResizeEvent *event)
{
    _width = width();
    _height = height();

    update();

    QWidget::resizeEvent(event);
}

void MoveMinWidget::setPixmap(const QPixmap &pix)
{
    _pixmap = pix;

    update();
}

