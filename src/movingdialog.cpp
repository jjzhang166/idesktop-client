#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include "dynamicbutton.h"
#include "movingdialog.h"
#include "virtualdesktop.h"
#include "config.h"

#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 45

MovingDialog::MovingDialog(VirtualDesktop *vdesk, IconItem *item, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint/* | Qt::WindowStaysOnTopHint*/), _vdesk(vdesk), _item(item)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0xff));
    setPalette(pal);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 3, 2, 3);
    layout->setSpacing(3);

    QString dt = Config::get("Wallpaper");
    QPixmap p0 = getCurrent(dt);
    QPixmap p1 = QPixmap(dt).scaled(BUTTON_WIDTH, BUTTON_HEIGHT);
    QPixmap p2 = getDark(dt);
    for (int i = 0; i < _vdesk->count(); i++) {
        DynamicButton *b;
        if (i == _vdesk->currentPage())
            b = new DynamicButton(p0, p0, this);
        else
            b = new DynamicButton(p1, p2, this);
        b->setValue(i);
        layout->addWidget(b);
        connect(b, SIGNAL(valueClicked(int)), this, SLOT(moveTo(int)));
    }
}

void MovingDialog::moveTo(int index)
{
    if (index != _vdesk->currentPage())
        _vdesk->moveItem(_item, index);
    accept();
}

QPixmap MovingDialog::getDark(const QString &icon)
{
    QImage image = QImage(icon).scaled(BUTTON_WIDTH, BUTTON_HEIGHT);
    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            QRgb pixel = image.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                             qBlue(pixel)*0.8, a);
            image.setPixel(i, j, darkPixel);
        }
    }
    return QPixmap::fromImage(image);
}

QPixmap MovingDialog::getCurrent(const QString &icon)
{
    QImage result = QImage(icon).scaled(BUTTON_WIDTH, BUTTON_HEIGHT);
    QImage spin = QImage(":images/spin.png");
    QPainter painter(&result);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(20, 10, spin);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.end();
    return QPixmap::fromImage(result);
}
