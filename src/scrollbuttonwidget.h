#ifndef SCROLLBUTTONWIDGET_H
#define SCROLLBUTTONWIDGET_H

#include "opacitywidget.h"

const int BUTTON_HEIGHT = 8;

class CustomBtn;
class ScrollSubWidget;
class ScrollUpwardButton;
class ScrollButtonWidget : public OpacityWidget
{
    Q_OBJECT
public:
    explicit ScrollButtonWidget(QWidget *parent = 0);
    
    void setSubWidget(QWidget* subW);
    void setStepSize(const int &stepSize);
    void undoMove();

signals:
    
public slots:

    void upIsUnAvailable();
    void downIsUnAvailable();
    void upIsAvailable();
    void downIsAvailable();

protected:

    void resizeEvent( QResizeEvent* event);

private:

private:

    ScrollUpwardButton* _upwardButton;
    ScrollUpwardButton* _downwardButton;

    ScrollSubWidget* _subWidget;

    int _stepSize;
    QRect _upwardR;
    QRect _downwardR;
};

#endif // SCROLLBUTTONWIDGET_H
