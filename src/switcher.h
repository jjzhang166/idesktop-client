#ifndef TOP_H
#define TOP_H
#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

class Switcher : public QWidget
{
    Q_OBJECT
public:
    Switcher(QWidget *parent = NULL);
    ~Switcher(){}
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void setPixmap(const QString &pix);
signals:
    void switcherActivated();

    void switcherHover();
    void switcherLeave();

public slots:
    void changed();
private:
    QPixmap _pixmap;
//    QPixmap _pixmapLeft;
    QPixmap _pixmapRight;
    QPixmap _normalRight;
//    QPixmap _normalLeft;
    QPixmap _transRight;
//    QPixmap _transLeft;
    QPixmap _darkRight;
//    QPixmap _darkLeft;
//    int _direction;
};

#endif
