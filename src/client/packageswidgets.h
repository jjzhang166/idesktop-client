#ifndef PACKAGESWIDGETS_H
#define PACKAGESWIDGETS_H


#include "../opacitywidget.h"
#include "widgetrefreshbase.h"

class AppWidget;
class QMovie;
class QLabel;
class PackageWidgets : public OpacityWidget ,public WidgetRefreshBase
{
    Q_OBJECT
public:
    explicit PackageWidgets(QWidget *parent = 0);

public:

    void tabChanged(const int &tabIndex);

    void RefreshUI();

    void AddWidget(AppWidget* pWidget);
    void showCurrent(int nCurChosen);

    void OnRefresh();
    void OnStartReadSort();
    void OnuiError(const QString &error);
    void OnUpdateScore(QString appName, int score);

    void clearWidget();
//    void maxWnd(const bool &bMax);

protected:


    void paintEvent(QPaintEvent *event);
    void resizeEvent( QResizeEvent * event);

signals:

    void tabSum(int);
    void currentTabIndex(int);
    
public slots:


private:

    void RefreshListShow();
    void readData();
    void calTabSum();
    void clearUI();

private:

    QList<AppWidget*> _AppWidgets;

    int _TabIndex;//当前显示的页码，从1计数
    int _TabSum;//当前显示的页码，从1计数
 //   int _FirstApp;//期待的第一位的app，从0计数，如果等于-1，表示正常页码显示
    int _curFirstApp;//当前的第一位app

    QMovie* _movie;
    QLabel* _movieLabel;

    QString _error;

    int _sortTabIndex;
    int _oneCount;//一页的个数
    int _oneRowCount;//每行的个数

    int _HEdge;//水平间距
    int _VEdge;//垂直间距
};

#endif // PACKAGESWIDGETS_H
