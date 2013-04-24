#include "scrollsubwidget.h"

#include <QEvent>

ScrollSubWidget::ScrollSubWidget(QWidget *parent) :
    OpacityWidget(parent, 0, 0)
  , _subWidget(NULL)
  , _stepSize(1)
{
}

void ScrollSubWidget::setSubWidget(QWidget* subW)
{
    _subWidget = subW;
    if (_subWidget)
    {
        _subWidget->setParent(this);
        _subWidget->installEventFilter(this);
    }
    refreshState();
}

void ScrollSubWidget::setStepSize(const int &stepSize)
{
    _stepSize = stepSize;
}

void ScrollSubWidget::upwardMove()
{
    if (!_subWidget)
        return;

    int y = _subWidget->geometry().top() + _stepSize;
    if (y > 0)
        y = 0;

    _subWidget->move(0, y);

    if (y >= 0)
        emit upIsUnAvailable();
    if (_subWidget->geometry().bottom() > height())
        emit downIsAvailable();
}

void ScrollSubWidget::downwardMove()
{
    if (!_subWidget)
         return;

     int y = _subWidget->geometry().top() - _stepSize;
     if ((_subWidget->geometry().bottom() - _stepSize) < height())
         y = height() - _subWidget->height();//geometry().bottom() - _subWidget->geometry().bottom();

     _subWidget->move(0, y);

     if (_subWidget->geometry().bottom() <= height())
         emit downIsUnAvailable();
     if (_subWidget->geometry().top() < 0)
         emit upIsAvailable();
}

void ScrollSubWidget::refreshState()
{
    if (!_subWidget)
        return;

    if (_subWidget->height() > height()) {
        _subWidget->move(0,0);
        emit downIsAvailable();
        emit upIsUnAvailable();
    }
    else {
        int top = (height() - _subWidget->height())/2;
        _subWidget->move(0, top);
        emit upIsUnAvailable();
        emit downIsUnAvailable();
    }

}
void ScrollSubWidget::resizeEvent( QResizeEvent * event)
{
    refreshState();
}

bool ScrollSubWidget::eventFilter( QObject * watched, QEvent * event )
{
    if (event->type() == QEvent::Resize) {
        refreshState();
    }
    return false;
}
