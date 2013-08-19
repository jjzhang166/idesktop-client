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
/*
    QImage normalRight = QImage(":images/isoft_normal.png");
    QImage transRight = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    QImage darkRight =  QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
#if 0
    QImage normalLeft = QImage(":images/win_normal.png");
    QImage darkLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    QImage transLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
#endif
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
#if 0
    for (int i = 0; i < normalLeft.width(); i++) {
        for (int j = 0; j < normalLeft.height(); j++) {
            QRgb pixel = normalLeft.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb transPixel = qRgba(qRed(pixel), qGreen(pixel),
                                    qBlue(pixel), a/2);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8,
                                    qBlue(pixel)*0.8, a);
            transLeft.setPixel(i, j, transPixel);
            darkLeft.setPixel(i, j, darkPixel);
        }
    }
#endif

//    QImage normalRight = QImage(":images/win_hover.png");           //win_hover.png
//    QImage transRight = QImage(":images/win_normal.png");           //win_normal.png
//    QImage darkRight =  QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);

//    QImage normalLeft = QImage(":images/isoft_hover.png");
//    QImage transLeft = QImage(":images/isoft_normal.png");
//    QImage darkLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);


    for (int i = 0; i < normalRight.width(); i++) {
        for (int j = 0; j < normalRight.height(); j++) {
            QRgb pixel = normalRight.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                    qBlue(pixel)*0.8, a);
            darkRight.setPixel(i, j, darkPixel);
        }
    }
#if 0
    for (int i = 0; i < normalLeft.width(); i++) {
        for (int j = 0; j < normalLeft.height(); j++) {
            QRgb pixel = normalLeft.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                    qBlue(pixel)*0.8, a);
            darkLeft.setPixel(i, j, darkPixel);
        }
    }
#endif

    _normalRight = QPixmap::fromImage(normalRight);
//    _normalLeft = QPixmap::fromImage(normalLeft);
    _transRight = QPixmap::fromImage(transRight);
//    _transLeft = QPixmap::fromImage(transLeft);
    _darkRight = QPixmap::fromImage(darkRight);
//    _darkLeft = QPixmap::fromImage(darkLeft);
    _pixmap = _transRight;
//    _direction = RIGHT;
    setFixedSize(normalRight.width(), normalRight.height());
*/
}

void Switcher::changed()
{
//    if (_direction == LEFT) {
//        _pixmap = _transRight;
//        _direction = RIGHT;
//    } else {
//        _pixmap = _transLeft;
//        _direction = LEFT;
//    }
//    repaint();

    _pixmap = _transRight;
    repaint();
}

void Switcher::mousePressEvent(QMouseEvent *event)
{
//    if (_direction == LEFT)
//        _pixmap = _darkLeft;
//    else
//        _pixmap = _darkRight;

//    repaint();

//    _pixmap = _darkRight;

    if (_enabled)
        _pixmap = _darkRight;
    else
        _pixmap = _transRight;

    repaint();

    Q_UNUSED(event);
}

void Switcher::mouseReleaseEvent(QMouseEvent *event)
{
//    if (_direction == LEFT) {
//        _pixmap = _normalRight;
//        _direction = RIGHT;
//    } else {
//        _pixmap = _normalLeft;
//        _direction = LEFT;
//    }
//    repaint();

//    _pixmap = _normalRight;

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
//    emit switcherActivated();

    Q_UNUSED(event);
}

void Switcher::enterEvent(QEvent *event)
{
//    if (_direction == LEFT)
//        _pixmap = _normalLeft;
//    else
//        _pixmap = _normalRight;
//    repaint();

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
//    if (_direction == LEFT)
//        _pixmap = _transLeft;
//    else
//        _pixmap = _transRight;
//    repaint();
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
#if 0
    QImage normalLeft = QImage(":images/win_normal.png");
    QImage darkLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
    QImage transLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);
#endif
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
#if 0
    for (int i = 0; i < normalLeft.width(); i++) {
        for (int j = 0; j < normalLeft.height(); j++) {
            QRgb pixel = normalLeft.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb transPixel = qRgba(qRed(pixel), qGreen(pixel),
                                    qBlue(pixel), a/2);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8,
                                    qBlue(pixel)*0.8, a);
            transLeft.setPixel(i, j, transPixel);
            darkLeft.setPixel(i, j, darkPixel);
        }
    }
#endif

//    QImage normalRight = QImage(":images/win_hover.png");           //win_hover.png
//    QImage transRight = QImage(":images/win_normal.png");           //win_normal.png
//    QImage darkRight =  QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);

//    QImage normalLeft = QImage(":images/isoft_hover.png");
//    QImage transLeft = QImage(":images/isoft_normal.png");
//    QImage darkLeft = QImage(normalRight.width(), normalRight.height(), QImage::Format_ARGB32);


    for (int i = 0; i < normalRight.width(); i++) {
        for (int j = 0; j < normalRight.height(); j++) {
            QRgb pixel = normalRight.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                    qBlue(pixel)*0.8, a);
            darkRight.setPixel(i, j, darkPixel);
        }
    }
#if 0
    for (int i = 0; i < normalLeft.width(); i++) {
        for (int j = 0; j < normalLeft.height(); j++) {
            QRgb pixel = normalLeft.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                    qBlue(pixel)*0.8, a);
            darkLeft.setPixel(i, j, darkPixel);
        }
    }
#endif

    _normalRight = QPixmap::fromImage(normalRight);
//    _normalLeft = QPixmap::fromImage(normalLeft);
    _transRight = QPixmap::fromImage(transRight);
//    _transLeft = QPixmap::fromImage(transLeft);
    _darkRight = QPixmap::fromImage(darkRight);
//    _darkLeft = QPixmap::fromImage(darkLeft);
    _pixmap = _transRight;
//    _direction = RIGHT;
    setFixedSize(normalRight.width(), normalRight.height());
}
void Switcher::setSwitcherEnabled(bool enabled)
{
    _enabled = enabled;
}
