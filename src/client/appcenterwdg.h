#ifndef APPCENTERWDG_H
#define APPCENTERWDG_H

#include "../customwndbase.h"

#include "widgetrefreshbase.h"

class QStackedLayout;
class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;

class SearchLineEdit;
class SortTabBar;
class TabWidgets;
class ScrollButtonWidget;
class AppCenterWdg : public CustomWndBase ,public WidgetRefreshBase
{
    Q_OBJECT
public:
    explicit AppCenterWdg(QWidget *parent = 0);

    void OnRefreshSort();
signals:
    
public slots:
    
    void showApp(const QString &text);
    void OnSearchResult(const QStringList &results);

    void refresh();

protected:

    void closeEvent ( QCloseEvent * event );
    void moveEvent ( QMoveEvent * event );
    void resizeEvent( QResizeEvent * event);

private:

    void refreshCategory();

private:

    SortTabBar* _leftTabBar;
    ScrollButtonWidget* _leftScroll;

    QVBoxLayout *_mainLayoutV;
    QVBoxLayout *_VSpacerLayout;
    QHBoxLayout *_HWidgetLayout;
    QHBoxLayout *_HSearchLayout;


    QVBoxLayout *_searchLayoutV;

    QLineEdit *_searchEdit;
//    SearchButton *_searchBtn;

    SearchLineEdit* _searchLineEdit;

    TabWidgets* _tabWidgets;

};

#endif // APPCENTERWDG_H
