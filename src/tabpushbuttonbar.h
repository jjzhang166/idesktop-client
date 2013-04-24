#ifndef TABPUSHBUTTON_H
#define TABPUSHBUTTON_H

#include <QWidget>
#include <QList>
#include "opacitywidget.h"

class TabLabel;
class TabPushButtonBarParent :public  OpacityWidget
{

    Q_OBJECT
public:
    TabPushButtonBarParent(QWidget *parent = 0, Qt::WindowFlags f = 0, const int &level = 0);

    //从1开始5*
    virtual void showCurrent(const QString &strIndex){}

public slots:

    void Current(TabLabel* pLabel);

};


class TabLabel;
class TabFlipButton;
class TabPushButtonBarParent;

class QVBoxLayout;
class QHBoxLayout;

class CustomBtn;
class TabPushButtonBar : public QWidget
{
    Q_OBJECT
public:

    //maxNum  >=3
    explicit TabPushButtonBar(const int &tabSum = 0 ,
            const int &maxNum = 5 , TabPushButtonBarParent* parentBase = 0);
    
    void ReShow();
signals:
    
public slots:

    void tabSum(int tabSum);
    void currentTabIndex(int);
    void Refresh(TabLabel* pLabel);
    void PreButtonClicked();
    void NextButtonClicked();

protected:

    void RefreshFlipButtons();
    void Refresh(QString url);

private:

    int _tabSum;
    int _maxNum;//>=3

    int _currentPos;//当前选中的页码，从1开始

    QList<TabLabel*> _LabelList;

    CustomBtn* _preButton;
    CustomBtn* _nextButton;


    QVBoxLayout *_VSpacerLayout;
    QHBoxLayout *_HWidgetLayout;

    TabPushButtonBarParent* _parentBase;

};

#endif // TABPUSHBUTTON_H
