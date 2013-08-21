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

    void setSwitcherEnabled(bool enabled);
signals:
    void switcherActivated();

    void switcherHover();
    void switcherLeave();

public slots:
    void changed();
private:
    QPixmap _pixmap;
    QPixmap _pixmapRight;
    QPixmap _normalRight;
    QPixmap _transRight;
    QPixmap _darkRight;

    bool _enabled;
};

#endif
