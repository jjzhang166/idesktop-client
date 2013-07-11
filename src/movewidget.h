#ifndef MOVEWIDGET_H
#define MOVEWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QResizeEvent>

#include "iconitem.h"

class MoveWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MoveWidget(QWidget *parent = 0);
    ~MoveWidget();

    void setPixmap(const QPixmap &pix);

signals:
    void mousePress();

    void moveWidgetDragEnter();
    void moveWidgetDragLeave();
    void moveWidgetDragMove();
    void moveWidgetDrop(IconItem *iconItem);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);
//    void mouseMoveEvent(QMouseEvent *);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    
private:
    QPixmap _pixmap;

    int _width;
    int _height;

    bool _dragEvent;
};

class MoveMinWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MoveMinWidget(QWidget *parent = 0);
    ~MoveMinWidget();

    void setPixmap(const QPixmap &pix);
protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

private:
    QPixmap _pixmap;

    int _width;
    int _height;
};
#endif // MOVEWIDGET_H
