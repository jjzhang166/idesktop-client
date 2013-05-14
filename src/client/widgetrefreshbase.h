#ifndef WIDGETREFRESHBASE_H
#define WIDGETREFRESHBASE_H

#include <QStringList>

class WidgetRefreshBase
{
public:
    WidgetRefreshBase();

    virtual void OnRefresh(){}
    virtual void OnRefreshSort(){}
    virtual void OnSearchResult(const QStringList &results){Q_UNUSED(results);}
    virtual void OnStartReadSort(){}

    virtual void OnuiError(const QString &error){Q_UNUSED(error);}

    virtual void OnUpdateScore(QString appName, int score){Q_UNUSED(appName); Q_UNUSED(score);}
};

#endif // WIDGETREFRESHBASE_H
