#ifndef TABTITLEWIDGET_H
#define TABTITLEWIDGET_H

#include "../opacitywidget.h"

#include "widgetrefreshbase.h"
#include <QString>
#include <QFont>

class TabTitleWidget : public OpacityWidget ,public WidgetRefreshBase
{
    Q_OBJECT
public:
    explicit TabTitleWidget(QWidget *parent = 0);

    void OnRefresh();
    void clearWdg();

protected:

    void paintEvent(QPaintEvent* event);

signals:
    
public slots:

    
private:

    QString _text;
    QFont _TitleFont;//
};

#endif // TABTITLEWIDGET_H
