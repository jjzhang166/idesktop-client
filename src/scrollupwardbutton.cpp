#include "scrollupwardbutton.h"
#include <QPainter>

ScrollUpwardButton::ScrollUpwardButton(QString image, QWidget *parent) :
    CustomBtn(parent)
  , _bEnter(false)
  , _image(image)
{
    setAutoFillBackground(true);
}

void ScrollUpwardButton::paintEvent(QPaintEvent *event)
{
    QImage image(_image);

    QPainter painter(this);
    painter.drawImage(width()/2 - 6 , 0, image);


    int alpha = 0;
    if (_bEnter)
        alpha = 100;

    QPen pen(QColor(136,136,136,alpha));
    QBrush bsh(QColor(136,136,136,alpha));
    painter.setPen(pen);
    painter.setBrush(bsh);

    painter.drawRoundedRect(QRect(5, 0, width() - 5, height()),1.0, 1.0 );
}

void ScrollUpwardButton::leaveEvent(QEvent *event)
{
    _bEnter = false;
    repaint();
}

void ScrollUpwardButton::enterEvent ( QEvent * event )
{
    _bEnter = true;
    repaint();
}
