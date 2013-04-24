#include <QDebug>
#include "switcher.h"

#define RIGHT 1
#define LEFT -1

Switcher::Switcher(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool /*| Qt::WindowStaysOnTopHint*/)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setWindowOpacity(0.7);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);
    QImage normalRight = QImage(":images/Arrow.png");
    QImage transRight = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    QImage darkRight =  QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    QImage normalLeft = QImage(":images/Arrow2.png");
    QImage darkLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    QImage transLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    for (int i = 0; i < normalRight.width(); i++) {
        for (int j = 0; j < normalRight.height(); j++) {
            QRgb pixel = normalRight.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb transPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                    qBlue(pixel), a/2);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                    qBlue(pixel)*0.8, a);
            transRight.setPixel(i, j, transPixel);
            darkRight.setPixel(i, j, darkPixel);
        }
    }
    for (int i = 0; i < normalLeft.width(); i++) {
        for (int j = 0; j < normalLeft.height(); j++) {
            QRgb pixel = normalLeft.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb transPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                    qBlue(pixel), a/2);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                    qBlue(pixel)*0.8, a);
            transLeft.setPixel(i, j, transPixel);
            darkLeft.setPixel(i, j, darkPixel);
        }
    }
    _normalRight = QPixmap::fromImage(normalRight);
    _normalLeft = QPixmap::fromImage(normalLeft);
    _transRight = QPixmap::fromImage(transRight);
    _transLeft = QPixmap::fromImage(transLeft);
    _darkRight = QPixmap::fromImage(darkRight);
    _darkLeft = QPixmap::fromImage(darkLeft);
    _pixmap = _transRight;
    _direction = RIGHT;
    setFixedSize(normalRight.width(), normalRight.height());
}

void Switcher::changed()
{
    if (_direction == LEFT) {
        _pixmap = _transRight;
        _direction = RIGHT;
    } else {
        _pixmap = _transLeft;
        _direction = LEFT;
    }
    repaint();
}

void Switcher::mousePressEvent(QMouseEvent *event)
{
    if (_direction == LEFT)
        _pixmap = _darkLeft;
    else
        _pixmap = _darkRight;
    repaint();
}

void Switcher::mouseReleaseEvent(QMouseEvent *event)
{
    if (_direction == LEFT) {
        _pixmap = _normalRight;
        _direction = RIGHT;
    } else {
        _pixmap = _normalLeft;
        _direction = LEFT;
    }
    repaint();
    emit activated();
}

void Switcher::enterEvent(QEvent *event)
{
    if (_direction == LEFT)
        _pixmap = _normalLeft;
    else
        _pixmap = _normalRight;
    repaint();
}

void Switcher::leaveEvent(QEvent *event)
{
    if (_direction == LEFT)
        _pixmap = _transLeft;
    else
        _pixmap = _transRight;
    repaint();
}

void Switcher::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _pixmap); 
    painter.end();
    QWidget::paintEvent(event);
}
