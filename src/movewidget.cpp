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

//}
#if 1
void MoveWidget::dragEnterEvent(QDragEnterEvent *event)
{
    _dragEvent = true;

    event->setDropAction(Qt::MoveAction);
    event->accept();

    emit moveWidgetDragEnter();
}


void MoveWidget::dragMoveEvent(QDragMoveEvent *event)
{
    _dragEvent = true;

    event->setDropAction(Qt::MoveAction);
    event->accept();

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

    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (icon)
        emit moveWidgetDrop(icon);

    event->setDropAction(Qt::MoveAction);
    event->accept();

    _dragEvent = false;
}
#endif

MoveMinWidget::MoveMinWidget(QWidget *parent)
    : QWidget(parent)
    , _width(0)
    , _height(0)
    , _isUp(true)
{
}

MoveMinWidget::~MoveMinWidget()
{

}

void MoveMinWidget::paintEvent(QPaintEvent *)
{
    static const QPointF upPoints[3] = {
        QPointF(_width / 2, 0),
        QPointF(0, _height),
        QPointF(_width, _height)
    };

    static const QPointF downPoints[3] = {
        QPointF(0, 0),
        QPointF(_width, 0),
        QPointF(_width / 2, _height)
    };

    QPainter painter(this);
    painter.setPen(QPen(QBrush(QColor(255,255,255,0)), 1, Qt::SolidLine,
                        Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(QBrush(_pixmap.scaled(_width, _height)));
    if (_isUp)
        painter.drawConvexPolygon(upPoints, 3);
    else
        painter.drawConvexPolygon(downPoints, 3);
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

void MoveMinWidget::setPixmapPointF(bool up)
{
    if (up)
    {
        _isUp = true;
        update();

    }
    else
    {
        _isUp = false;
        update();
    }
}

