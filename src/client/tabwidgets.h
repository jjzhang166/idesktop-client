#ifndef TABWIDGETS_H
#define TABWIDGETS_H

#include <QWidget>

#include "../tabpushbuttonbar.h"

class TabPushButtonBar;
class PackageWidgets;
class TabLabel;
class TabTitleWidget;
//class SearchLineEdit;

class QVBoxLayout;
class QHBoxLayout;

class TabWidgets : public TabPushButtonBarParent
{
    Q_OBJECT
public:
    explicit TabWidgets(QWidget *parent = 0);

    void showCurrent(const QString &strIndex);

    void clearWidget();

signals:
    
public slots:
    void tabChanged(const int &tabIndex);

protected:
 //    void resizeEvent(QResizeEvent *);

    void paintEvent(QPaintEvent *event);
private:
    TabPushButtonBar* _tabBtnBar;
    PackageWidgets* _widgets;
    TabTitleWidget* _titleWidget;

    QVBoxLayout *_VLayout;
    QVBoxLayout *_VMainLayout;
    QHBoxLayout* mLayout;


 //   SearchLineEdit* _searchLineEdit;
};

#endif // TABWIDGETS_H
