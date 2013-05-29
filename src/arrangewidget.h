#ifndef ARRANGEWIDGET_H
#define ARRANGEWIDGET_H

#include <QWidget>

class ArrangeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ArrangeWidget(QWidget *parent = 0);
    ~ArrangeWidget();
    
private:

};

class ArrangeCenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArrangeCenterWidget(QWidget *parent = 0);
    ~ArrangeCenterWidget();

private:

};

#endif // ARRANGEWIDGET_H
