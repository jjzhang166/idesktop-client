#ifndef ARRANGEWIDGET_H
#define ARRANGEWIDGET_H

#include "virtualdesktop.h"

#include <QWidget>
#include <QMouseEvent>

class ArrangeCenterWidget;

class ArrangeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ArrangeWidget(QWidget *parent = 0);
    ~ArrangeWidget();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

signals:
    void setPos(QPoint pos);
    
private:
    QPixmap _topPix;
    QPixmap _centerPix;
    QPixmap _bottomPix;

    int _width;
    int _height;

    bool _titlePressed;
    QPoint _startDrag;

    ArrangeCenterWidget *_arrangeCenterWidget;

};

class ArrangeCenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArrangeCenterWidget(QWidget *parent = 0);
    ~ArrangeCenterWidget();

//    int addIcon(const QString &text, const QString &icon,
//                int page, int index, const VirtualDesktop::iconStyle &iSt = VirtualDesktop::localIcon);

private:

};

#endif // ARRANGEWIDGET_H
