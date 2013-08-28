#include "mask.h"

Mask::Mask(int width, int height, QWidget *parent, Qt::WFlags flags)
        : QDialog(parent, flags)
        , _width(width)
        , _height(height)
        , _text("")
{
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_NoSystemBackground, false);
        setAttribute(Qt::WA_TranslucentBackground, true);
        setFixedSize(_width, _height);
        move(0, 0);

        _loadItem = new RotateWarnningLabel(this);
        _loadItem->move((_width - _loadItem->width()) / 2, _height / 3);
        _loadItem->start();
        _loadItem->setVisible(true);
}

Mask::~Mask()
{

}
void Mask::paintEvent(QPaintEvent *)
{
        QPainter painter(this);
        QLinearGradient grad(0, 0, _width, _height);
        {
                QGradientStops gs;
                gs << QGradientStop(0.0, QColor(0,0,0,100))
                        << QGradientStop(0.5, QColor(0,0,0,100))
                        << QGradientStop(1.0, QColor(0,0,0,100));
                grad.setStops(gs);
        }
        painter.fillRect(0, 0, _width, _height, grad);


        QFont font(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), 12, QFont::Normal);
        QFontMetrics fm(font);
        int _textWidth = fm.width(_text);

        painter.setFont(font);
        painter.setPen(QPen(QColor("#FFFFFF")));
        painter.drawText((_width - _textWidth) / 2, _loadItem->height() + _loadItem->pos().y() + 35, _text);
}

void Mask::setLoadStart()
{
    _loadItem->start();
    _loadItem->setVisible(true);
}

void Mask::setLoadStop()
{
    _loadItem->setVisible(false);
    _loadItem->stop();
}

void Mask::setText(const QString &text)
{
    _text = text;

    repaint();
}

