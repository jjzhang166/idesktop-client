#include <QWidget>
#include <QEvent>
#include <QPainter>
#include <QDebug>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QUrl>
#include <QLibrary>

#include <windows.h>
#include <shellapi.h>

#include "config.h"
#include "panel.h"
#include "dynamicbutton.h"

#include "appmessagebox.h"

Panel::Panel(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool), \
      autoHide(true), visible(false), animation(NULL)//, _center(NULL)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0xdd));
    setPalette(pal);

    animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(300);

    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry();

    _switcherQuit = new Switcher(this);
    _switcherQuit->setParent(this);
    _switcherQuit->setPixmap(QString(":images/panel_quit_normal.png"));
    _switcherQuit->setGeometry(rect.width() - _switcherQuit->width(), 0
                               , _switcherQuit->width(), _switcherQuit->height());
    _switcherQuit->setVisible(true);


    _switcherSoftware = new Switcher(this);
    _switcherSoftware->setPixmap(QString(":images/panel_software_normal.png"));
    _switcherSoftware->setGeometry(rect.width() - _switcherQuit->width() - _switcherSoftware->width(), 0
                                   , _switcherSoftware->width(), _switcherSoftware->height());
    _switcherSoftware->setVisible(true);

    _switcherSkin = new Switcher(this);
    _switcherSkin->setPixmap(QString(":images/panel_skin_normal.png"));
    _switcherSkin->setGeometry(rect.width() - _switcherQuit->width() - _switcherSoftware->width() - _switcherSkin->width(), 0
                               , _switcherSkin->width(), _switcherSkin->height());
    _switcherSkin->setVisible(true);


    connect(_switcherSkin, SIGNAL(switcherActivated()), this, SLOT(skinClicked()));
    connect(_switcherSoftware, SIGNAL(switcherActivated()), this, SLOT(softwareClicked()));
    connect(_switcherQuit, SIGNAL(switcherActivated()), this, SLOT(quitClicked()));
}

void Panel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), 28, QPixmap(":images/panel_bg.png"));

    QWidget::paintEvent(event);
}

void Panel::enterEvent(QEvent *event)
{
    emit checkDirState();
    if (!visible)
        animationShow();

    Q_UNUSED(event);
}

void Panel::leaveEvent(QEvent *event)
{
    if (autoHide)
        animationHide();

    Q_UNUSED(event);
}

void Panel::animationShow()
{
    if (animation->state() == QAbstractAnimation::Running) {
        animation->stop();
    }

    QPoint end(0, 0);
    animation->setStartValue(pos());
    animation->setEndValue(end);
    animation->start();
    visible = true;
}

void Panel::animationHide()
{
    if (animation->state() == QAbstractAnimation::Running) {
        animation->stop();
    }

    QPoint end(0, -27);
    animation->setStartValue(pos());
    animation->setEndValue(end);
    animation->start();
    visible = false;
}

void Panel::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Panel::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Panel::skinClicked()
{
    emit showSkinWidget();
}

void Panel::softwareClicked()
{
    emit showSoftwareWidget();
}

void Panel::quitClicked()
{
    emit quit();
}

QPixmap Panel::setTransparentPixmap(const QString &pix)
{
    QImage normal = QImage(pix);

    for (int i = 0; i < normal.width(); i++) {
        for (int j = 0; j < normal.height(); j++) {
            QRgb pixel = normal.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel) * 0.6, qGreen(pixel) * 0.6, \
                                    qBlue(pixel) * 0.6, a * 180 / 255);
            normal.setPixel(i, j, lightPixel);
        }
    }
    return QPixmap::fromImage(normal);
}
