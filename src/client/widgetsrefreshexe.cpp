#include "widgetsrefreshexe.h"
#include "widgetrefreshbase.h"

WidgetsRefreshExe* WidgetsRefreshExe::_instance = NULL;
WidgetsRefreshExe::WidgetsRefreshExe()
{
}

WidgetsRefreshExe* WidgetsRefreshExe::Instance()
{
    if (_instance == NULL)
        _instance = new WidgetsRefreshExe;
    return _instance;
}

void WidgetsRefreshExe::destroy()
{
    if (_instance)
        delete _instance;
    _instance = NULL;
}

void WidgetsRefreshExe::atech(WidgetRefreshBase* widget)
{
    _widgets.push_back(widget);
}

void WidgetsRefreshExe::refreshUI()
{
    for (int i = 0;i < _widgets.size();i++)
        _widgets.at(i)->OnRefresh();
}

void WidgetsRefreshExe::refreshSort()
{
    for (int i = 0;i < _widgets.size();i++)
        _widgets.at(i)->OnRefreshSort();
}

void WidgetsRefreshExe::searchResult(const QStringList &results)
{
    for (int i = 0;i < _widgets.size();i++)
        _widgets.at(i)->OnSearchResult(results);
}

void WidgetsRefreshExe::startReadSort()
{
    for (int i = 0;i < _widgets.size();i++)
        _widgets.at(i)->OnStartReadSort();
}

void WidgetsRefreshExe::uiError(const QString &error)
{
    for (int i = 0;i < _widgets.size();i++)
        _widgets.at(i)->OnuiError(error);
}

void WidgetsRefreshExe::updateScore(QString appName, int score)
{
    for (int i = 0;i < _widgets.size();i++)
        _widgets.at(i)->OnUpdateScore(appName, score);
}
