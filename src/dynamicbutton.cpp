#include <QLabel>
#include "dynamicbutton.h"

void DynamicButton::enterEvent(QEvent *event)
{
    _entered = true;

    if (_enabled)
        setPixmap(_hover);
    else
        setPixmap(_normal);
    repaint();
    Q_UNUSED(event);
}

void DynamicButton::leaveEvent(QEvent *event)
{
    _entered = false;
    setPixmap(_normal);
    repaint();
    Q_UNUSED(event);
}

void DynamicButton::mousePressEvent(QMouseEvent *event)
{
    setPixmap(_normal);
    repaint();
    Q_UNUSED(event);
}

void DynamicButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (_enabled)
    {
        setPixmap(_hover);
        emit clicked();
        emit valueClicked(_value);
    }
    else
    {
        setPixmap(_normal);
    }

    repaint();
    Q_UNUSED(event);
}

void DynamicButton::setEnable(bool enabled)
{
    _enabled = enabled;
}
