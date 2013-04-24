#include "tabwidgets.h"

#include "../tabpushbuttonbar.h"
#include "packageswidgets.h"
#include "tabtitlewidget.h"

//#include "../searchlineedit.h"

#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPainter>

void DrawImg(QPainter* pPainter, const QString &strImgPath, const QRect &rect);

TabWidgets::TabWidgets(QWidget *parent) :
    TabPushButtonBarParent(parent)
{
    _tabBtnBar = new TabPushButtonBar(1,8,this);
    _tabBtnBar->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    _tabBtnBar->resize(300,80);//宽没有用，自动计算宽度

    _widgets = new PackageWidgets(this);
    _widgets->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    _titleWidget = new TabTitleWidget(this);
    _titleWidget->setFixedHeight(38);
    _titleWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _titleWidget->show();

    QSpacerItem* spacer1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
//    QSpacerItem* spacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addItem(spacer1);
    hLayout->addWidget(_tabBtnBar);
 //   hLayout->addItem(spacer2);

    _VLayout = new QVBoxLayout;
    _VLayout->addWidget(_titleWidget);
    _VLayout->addWidget(_widgets);
    _VLayout->addLayout(hLayout);

    mLayout = new QHBoxLayout(this);
    mLayout->addLayout(_VLayout);

    connect(_widgets, SIGNAL(tabSum(int)), _tabBtnBar, SLOT(tabSum(int)));
    connect(_widgets, SIGNAL(currentTabIndex(int)), _tabBtnBar, SLOT(currentTabIndex(int)));

  //  _widgets->RefreshUI();
}

void TabWidgets::showCurrent(const QString &strIndex)
{
    if (_widgets) {
       _widgets->showCurrent(strIndex.toInt());
    }
}

void TabWidgets::tabChanged(const int &tabIndex)
{
    if (_widgets) {
        _widgets->tabChanged(tabIndex);
    }
}

void TabWidgets::clearWidget()
{
    if (_widgets)
        _widgets->clearWidget();
    if (_titleWidget)
        _titleWidget->clearWdg();
}

void TabWidgets::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    DrawImg(&painter, ":/images/bg_subWindow.png", rect());
}
