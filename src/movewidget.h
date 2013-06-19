#ifndef MOVEWIDGET_H
#define MOVEWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QResizeEvent>

class MoveWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MoveWidget(QWidget *parent = 0);
    ~MoveWidget();

    void setPixmap(const QPixmap &pix);

signals:
    void mousePress();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);
    
private:
    QPixmap _pixmap;

    int _width;
    int _height;
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
