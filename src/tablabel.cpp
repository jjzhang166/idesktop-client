#include "tablabel.h"
#include <QPainter>
#include <QFontMetrics>
#include <QImage>
#include <QMouseEvent>

TabLabel::TabLabel(QWidget *parent) :
    QLabel(parent)
  , _isMouseIn(false)
  , _isCurrent(false)
{
    _image = new QImage(":images/icon_page_number.png");
    _enterImage = new QImage(":images/icon_page_number_hover.png");
    _disableImage = new QImage(":images/icon_page_number_current_end.png");
    _chosenImage = new QImage(":images/icon_page_number_current.png");

    setCursor(QCursor(Qt::PointingHandCursor));
}

void TabLabel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    if (_isCurrent) {
        painter.drawImage(rect(), *_chosenImage);
    }
    else if (this->isEnabled()){
        if (_isMouseIn)
            painter.drawImage(rect(), *_enterImage);
        else
            painter.drawImage(rect(), *_image);
    }

//    QString strText = text();
//    QFontMetrics metrics(painter.font());

//    int textWidth = metrics.width(strText);

//    painter.setPen(Qt::black);

//    if (_isMouseIn) {
//        painter.drawLine(QPoint(0, height()-1), QPoint(textWidth, height()-1));

//    }
    QFontMetrics metrics(painter.font());
    int textWidth = metrics.width(text());
    int textHeight = metrics.height();

    int left = (width() - textWidth)/2;
    int top = (height() - textHeight)/2;
    QByteArray data = text().toAscii();
    painter.drawText(QRect(left ,top ,width() - 2*left, height() - 2 * top), QString::fromLocal8Bit(data.data()));


 //   QLabel::paintEvent(event);
}

void TabLabel::enterEvent ( QEvent * event )
{
    Q_UNUSED(event);

    if (isEnabled()) {
      _isMouseIn = true;
      repaint();
    }
}

void TabLabel::leaveEvent ( QEvent * event )
{
    Q_UNUSED(event);

    _isMouseIn = false;
    repaint();
}

void TabLabel::mousePressEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);

    emit Chosen(this);
}

void TabLabel::current(const bool &bCurrent)
{
    _isCurrent = bCurrent;
}
