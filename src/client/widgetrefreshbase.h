#ifndef WIDGETREFRESHBASE_H
#define WIDGETREFRESHBASE_H

#include <QStringList>

class WidgetRefreshBase
{
public:
    WidgetRefreshBase();

    virtual void OnRefresh(){}
    virtual void OnRefreshSort(){}
    virtual void OnSearchResult(const QStringList &results){}
    virtual void OnStartReadSort(){}

    virtual void OnuiError(const QString &error){}

    virtual void OnUpdateScore(QString appName, int score){}
};

#endif // WIDGETREFRESHBASE_H
