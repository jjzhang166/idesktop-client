#include "appcenterwdg.h"
#include "sorttabbar.h"
#include "tabwidgets.h"
#include "../searchlineedit.h"
#include "appdetaildialogexe.h"
#include "appdatareadexe.h"
#include "../scrollbuttonwidget.h"

#include <QStackedLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QPainter>
#include <QRect>
#include <QBrush>
#include <QLineEdit>

#include <windows.h>
#include <QLibrary>
#include<Tlhelp32.h>

const int Shadow_Edge = 17;

AppCenterWdg::AppCenterWdg(QWidget *parent) :
    CustomWndBase(parent,CustomWndBase::WindowMaximizeButtonHint | NotPainTitleImage | DragWholeWindow)
  , _tabWidgets(NULL)
{
    setWindowTitle("软件商店");
    setBkgImg(":images/bg_mainWindow.png");
    setMinOn(":images/icon_minimize_hover.png");
    setMinOff(":images/icon_minimize.png");

    setCloseOn(":images/icon_close_hover.png");
    setCloseOff(":images/icon_close.png");

    setHEdgeWidth(Shadow_Edge);
    setTopEdgeHeight(Shadow_Edge);
    setBottomEdgeHeight(Shadow_Edge);

    titleHeight(24);

     _searchLineEdit = new SearchLineEdit(SearchLineEdit::button_unavailable,
                                          ":/images/btn_search.png", "",this);
     _searchLineEdit->setFixedSize(202,23);

     _tabWidgets = new TabWidgets(this);

     _leftScroll = new ScrollButtonWidget(this);
     _leftScroll->setStepSize(44);
     _leftScroll->setFixedWidth(96);
  //   _leftScroll->setFixedSize(96,88);
     _leftTabBar = new SortTabBar(96,44,
                                  ":images/bg_minNav.png", ":images/bg_minNav_current.png");
     _leftScroll->setSubWidget(_leftTabBar);
     _leftTabBar->setFixedWidth(96);
     _leftTabBar->resize(96,88);

     _leftTabBar->addTab(QString::fromLocal8Bit("精品推荐"));

     _leftTabBar->hide();
     AppDataReadExe::Instance()->login();
     AppDataReadExe::Instance()->readCategory();

    connect(_searchLineEdit, SIGNAL(chooseItem(QString)), this, SLOT(showApp(const QString &)));
    connect(_leftTabBar,SIGNAL(currentChanged(int)), _tabWidgets,SLOT(tabChanged(const int &)) );
}

void AppCenterWdg::showApp(const QString &text)
{
    AppDetailDialogExe::Instance()->showApp(text);
}

void AppCenterWdg::closeEvent ( QCloseEvent * event )
{
    if (_searchLineEdit)
        _searchLineEdit->closeControls();

    AppDetailDialogExe::Instance()->hideApp();

    if (_tabWidgets)
        _tabWidgets->clearWidget();

    CustomWndBase::closeEvent(event);
}

void AppCenterWdg::moveEvent ( QMoveEvent * event )
{
    _searchLineEdit->gMove();
}

void AppCenterWdg::OnRefreshSort()
{
   // return;
    QByteArray tmp;
    for (int i = 1;i < _leftTabBar->count();) {
        if (i <= AppDataReadExe::Instance()->size()) {
            tmp = AppDataReadExe::Instance()->getCategory(i-1)._category.toAscii();
            _leftTabBar->setTabText(i, QString::fromLocal8Bit(tmp.data()));
            i++;
        }
        else {
            _leftTabBar->removeTab(i);
        }
    }

    for (int i = _leftTabBar->count();i <= AppDataReadExe::Instance()->size();i++) {
        tmp = AppDataReadExe::Instance()->getCategory(i-1)._category.toAscii();
        _leftTabBar->addTab(QString::fromLocal8Bit(tmp.data()));
    }

    _leftTabBar->setCurrentIndex(0);
    if (_tabWidgets)
        _tabWidgets->tabChanged(0);

    refreshCategory();
}

void AppCenterWdg::OnSearchResult(const QStringList &results)
{
    _searchLineEdit->setSearchResult(results);
}

void AppCenterWdg::refresh()
{
    AppDataReadExe::Instance()->readCategory();
}

void AppCenterWdg::resizeEvent( QResizeEvent * event)
{
    CustomWndBase::resizeEvent(event);

    int edgeV = 10;

    QRect rSearch(width() - _searchLineEdit->width() - edgeV - Shadow_Edge, titleHeight() + 10 + Shadow_Edge, 202,23);
    _searchLineEdit->move(rSearch.left(), rSearch.top());

    QRect rScroll(Shadow_Edge + edgeV * 2, rSearch.bottom() + 57 - BUTTON_HEIGHT, 96, height());

    QRect rTabWidget(rScroll.right()- 8, rSearch.bottom() + 10, rSearch.right() - rScroll.right() + 16, rScroll.height() + 76);
    rTabWidget.setBottom(height() - Shadow_Edge - edgeV - 16);
    _tabWidgets->setGeometry(rTabWidget);


    int wH = (rTabWidget.height() - rScroll.top() + rTabWidget.top()) / 44 * 44 + BUTTON_HEIGHT * 2;
    rScroll.setBottom(rScroll.top() + wH);
    _leftScroll->setGeometry(rScroll);

    refreshCategory();
}

void AppCenterWdg::refreshCategory()
{
    int tabHeight = _leftTabBar->count() * 44;

    _leftTabBar->resize(96, tabHeight);
    _leftTabBar->repaint();
    _leftTabBar->show();
    _leftScroll->undoMove();
}
