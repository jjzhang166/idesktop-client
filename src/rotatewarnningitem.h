#ifndef ROTATEWARNNINGITEM_H
#define ROTATEWARNNINGITEM_H
#include <QObject>
#include <QTimeLine>
#include <QRect>
#include <QGraphicsPixmapItem>
#include <QWidget>
#include <QLabel>
#include <QPainter>

class RotateWarnningItem :public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:

    RotateWarnningItem(QGraphicsItem * parent = 0);

public slots:

     void transCount(qreal);
     void start();
     void stop();
     QRectF boundingRect()const;

protected:

private:

    QTimeLine *timeline;
    int picture_number;
    int counter;

};

class RotateWarnningLabel :public QLabel
{
    Q_OBJECT

public:

    RotateWarnningLabel(QWidget * parent = 0);

public slots:

     void transCount(qreal);
     void start();
     void stop();

protected:

private:

    QTimeLine *timeline;
    int picture_number;
    int counter;

};

#endif // ROTATEWARNNINGITEM_H
