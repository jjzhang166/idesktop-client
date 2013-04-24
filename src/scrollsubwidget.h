#ifndef SCROLLSUBWIDGET_H
#define SCROLLSUBWIDGET_H

#include "opacitywidget.h"

class ScrollSubWidget : public OpacityWidget
{
    Q_OBJECT
public:
    explicit ScrollSubWidget(QWidget *parent = 0);

    void setSubWidget(QWidget* subW);
    void setStepSize(const int &stepSize);

    void refreshState();
signals:

    void upIsUnAvailable();
    void downIsUnAvailable();
    void upIsAvailable();
    void downIsAvailable();

public slots:
    void upwardMove();
    void downwardMove();

protected:

    void resizeEvent( QResizeEvent * event);
    bool eventFilter( QObject * watched, QEvent * event );

private:


private:

    QWidget* _subWidget;
    int _stepSize;
};

#endif // SCROLLSUBWIDGET_H
