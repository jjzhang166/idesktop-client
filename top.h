#ifndef TOP_H
#define TOP_H
#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

class TopWidget : public QWidget
{
    Q_OBJECT
public:
    TopWidget(QWidget *parent = NULL);
    ~TopWidget(){}
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
signals:
    void activated();
public slots:
    void changed();
private:
    QPixmap _pixmap;
    QPixmap _normalRight;
    QPixmap _normalLeft;
    QPixmap _transRight;
    QPixmap _transLeft;
    QPixmap _darkRight;
    QPixmap _darkLeft;
    int _direction;
};

#endif
