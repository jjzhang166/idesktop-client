#include "scrollbuttonwidget.h"
#include "custombtn.h"
#include "scrollsubwidget.h"
#include "scrollupwardbutton.h"

#include <QPoint>
#include <QEvent>
#include <QMouseEvent>


ScrollButtonWidget::ScrollButtonWidget(QWidget *parent) :
    OpacityWidget(parent,0,0)
  , _subWidget(NULL)
  , _stepSize(1)
{
    setMouseTracking(true);

    _upwardButton = new ScrollUpwardButton(":images/scroll_up.png",this);
    _downwardButton = new ScrollUpwardButton(":images/scroll_down.png", this);

    _subWidget = new ScrollSubWidget(this);

    connect(_upwardButton, SIGNAL(clicked()), _subWidget, SLOT(upwardMove()));
    connect(_downwardButton, SIGNAL(clicked()), _subWidget, SLOT(downwardMove()));

    connect(_subWidget, SIGNAL(downIsAvailable()), this, SLOT(downIsAvailable()));
    connect(_subWidget, SIGNAL(upIsAvailable()), this, SLOT(upIsAvailable()));
    connect(_subWidget, SIGNAL(downIsUnAvailable()), this, SLOT(downIsUnAvailable()));
    connect(_subWidget, SIGNAL(upIsUnAvailable()), this, SLOT(upIsUnAvailable()));

}

void ScrollButtonWidget::resizeEvent( QResizeEvent * event)
{
    _upwardR = QRect(0,0,width(),BUTTON_HEIGHT);
    _downwardR = QRect(0,height() - BUTTON_HEIGHT ,width(),BUTTON_HEIGHT);
    _upwardButton->setGeometry(_upwardR);
    _downwardButton->setGeometry(_downwardR);


    _subWidget->setGeometry(QRect(0, BUTTON_HEIGHT, width(), height() - 2 * BUTTON_HEIGHT));
    OpacityWidget::resizeEvent(event);
}

void ScrollButtonWidget::setSubWidget(QWidget* subW)
{
    _subWidget->setSubWidget(subW);
}

void ScrollButtonWidget::setStepSize(const int &stepSize)
{
    _subWidget->setStepSize(stepSize);
}

void ScrollButtonWidget::upIsUnAvailable()
{
    _upwardButton->hide();
}

void ScrollButtonWidget::downIsUnAvailable()
{
    _downwardButton->hide();
}

void ScrollButtonWidget::upIsAvailable()
{
    _upwardButton->show();
}

void ScrollButtonWidget::downIsAvailable()
{
    _downwardButton->show();
}

void ScrollButtonWidget::undoMove()
{
    _subWidget->refreshState();
}
