#ifndef WIDGETSREFRESHEXE_H
#define WIDGETSREFRESHEXE_H

#include <QList>
#include <QStringList>

class WidgetRefreshBase;
class WidgetsRefreshExe
{
public:

    static WidgetsRefreshExe* Instance();
    static void destroy();

    void atech(WidgetRefreshBase* widget);

    void refreshUI();
    void uiError(const QString &error);

    void refreshSort();
    void searchResult(const QStringList &results);
    void startReadSort();

    void updateScore(QString appName, int score);

private:

    WidgetsRefreshExe();
    static WidgetsRefreshExe* _instance;

    QList<WidgetRefreshBase*> _widgets;
};

#endif // WIDGETSREFRESHEXE_H
