#include "rotatewarnningitem.h"

#include <QtDebug>
RotateWarnningItem::RotateWarnningItem(QGraphicsItem * parent)
    :QGraphicsPixmapItem(parent)
    , picture_number(12)
    , counter(0)
{
    this->setVisible(false);
    this->setPos(200,100);
    timeline=new QTimeLine(picture_number*1000,this);
    timeline->setFrameRange(1,picture_number);
    timeline->setUpdateInterval(50);
    timeline->setLoopCount(0);
    connect(timeline,SIGNAL(valueChanged(qreal)),this,SLOT(transCount(qreal)));

}
QRectF RotateWarnningItem::boundingRect()const
{
    return QRect(0,0,42,42);
}
void RotateWarnningItem::transCount(qreal value)
{
    Q_UNUSED(value);

    if(counter > 11)
        counter = 0;
    QPixmap map(QString(":images/loading_%1.png").arg(counter+1));
    setPixmap(map);
    update();
    counter++;
}
void RotateWarnningItem::start()
{
    timeline->start();
    timeline->setLoopCount(0);
    this->setVisible(true);
}
void RotateWarnningItem::stop()
{
     timeline->stop();
     this->setVisible(false);
}

RotateWarnningLabel::RotateWarnningLabel(QWidget * parent)
    : QLabel(parent)
    , picture_number(12)
    , counter(0)
{
    setVisible(false);
    resize(42, 42);
    timeline=new QTimeLine(picture_number*1000,this);
    timeline->setFrameRange(1,picture_number);
    timeline->setUpdateInterval(50);
    timeline->setLoopCount(0);
    connect(timeline,SIGNAL(valueChanged(qreal)),this,SLOT(transCount(qreal)));

}

void RotateWarnningLabel::transCount(qreal value)
{
    Q_UNUSED(value);

    if(counter > 11)
    counter = 0;
    QPixmap map(QString(":images/loading_%1.png").arg(counter+1));
    setPixmap(map);
    update();
    counter++;
}
void RotateWarnningLabel::start()
{
    timeline->start();
    timeline->setLoopCount(0);
    this->setVisible(true);
}
void RotateWarnningLabel::stop()
{
     timeline->stop();
     this->setVisible(false);
}
