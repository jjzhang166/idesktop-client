#include <QLabel>
#include "dynamicbutton.h"

void DynamicButton::enterEvent(QEvent *event)
{
    _entered = true;
    setPixmap(_hover);
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
    setPixmap(_hover);
    repaint();
    emit clicked();
    emit valueClicked(_value);
    Q_UNUSED(event);
}
