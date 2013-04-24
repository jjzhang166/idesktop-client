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

    int _TabIndex;//��ǰ��ʾ��ҳ�룬��1����
    int _TabSum;//��ǰ��ʾ��ҳ�룬��1����
 //   int _FirstApp;//�ڴ��ĵ�һλ��app����0�������������-1����ʾ����ҳ����ʾ
    int _curFirstApp;//��ǰ�ĵ�һλapp

    QMovie* _movie;
    QLabel* _movieLabel;

    QString _error;

    int _sortTabIndex;
    int _oneCount;//һҳ�ĸ���
    int _oneRowCount;//ÿ�еĸ���

    int _HEdge;//ˮƽ���
    int _VEdge;//��ֱ���
};

#endif // PACKAGESWIDGETS_H
