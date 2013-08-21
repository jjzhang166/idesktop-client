#include <QDebug>
#include "switcher.h"

#define RIGHT 1
#define LEFT -1

Switcher::Switcher(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint/* | Qt::Tool | Qt::WindowStaysOnTopHint*/)
    , _enabled(true)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setWindowOpacity(0.7);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);
}

void Switcher::changed()
{

    _pixmap = _transRight;
    repaint();
}

void Switcher::mousePressEvent(QMouseEvent *event)
{
    if (_enabled)
        _pixmap = _darkRight;
    else
        _pixmap = _transRight;

    repaint();

    Q_UNUSED(event);
}

void Switcher::mouseReleaseEvent(QMouseEvent *event)
{
    if (_enabled)
    {
        _pixmap = _normalRight;
        emit switcherActivated();
    }
    else
    {
        _pixmap = _transRight;
    }

    repaint();

    Q_UNUSED(event);
}

void Switcher::enterEvent(QEvent *event)
{
    emit switcherHover();

    if (_enabled)
        _pixmap = _normalRight;
    else
        _pixmap = _transRight;
    repaint();

    Q_UNUSED(event);
}

void Switcher::leaveEvent(QEvent *event)
{
    emit switcherLeave();
    _pixmap = _transRight;
    repaint();

    Q_UNUSED(event);
}

void Switcher::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _pixmap); 
    painter.end();
    QWidget::paintEvent(event);
}

void Switcher::setPixmap(const QString &pix)
{

    QImage normalRight = QImage(pix);
    QImage transRight = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    QImage darkRight =  QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);

    for (int i = 0; i < normalRight.width(); i++) {
        for (int j = 0; j < normalRight.height(); j++) {
            QRgb pixel = normalRight.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb transPixel = qRgba(qRed(pixel), qGreen(pixel),
                                    qBlue(pixel), a/2);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8,
                                    qBlue(pixel)*0.8, a);
            transRight.setPixel(i, j, transPixel);
            darkRight.setPixel(i, j, darkPixel);
        }
    }

    for (int i = 0; i < normalRight.width(); i++) {
        for (int j = 0; j < normalRight.height(); j++) {
            QRgb pixel = normalRight.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                    qBlue(pixel)*0.8, a);
            darkRight.setPixel(i, j, darkPixel);
        }
    }

    _normalRight = QPixmap::fromImage(normalRight);
    _transRight = QPixmap::fromImage(transRight);
    _darkRight = QPixmap::fromImage(darkRight);
    _pixmap = _transRight;
    setFixedSize(normalRight.width(), normalRight.height());
}
void Switcher::setSwitcherEnabled(bool enabled)
{
    _enabled = enabled;
}
