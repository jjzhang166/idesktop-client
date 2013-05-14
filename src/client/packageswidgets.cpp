#include "packageswidgets.h"
#include "appwidget.h"
#include "appdatabase.h"
#include "appdata.h"
#include "appdatareadexe.h"

#include <QPainter>
#include <QMovie>
#include <QLabel>

const int Count_In_Row = 3;

PackageWidgets::PackageWidgets(QWidget *parent) :
    OpacityWidget(parent)
  , _TabIndex(1)
  , _TabSum(0)
  , _curFirstApp(-1)
  , _sortTabIndex(0)
  , _oneCount(6)
  , _oneRowCount(0)
  , _HEdge(0)
  , _VEdge(0)
{
    _movieLabel = new QLabel(this);
    _movie = new QMovie(":/images/waiting.gif");
    _movieLabel->setMovie(_movie);
    _movie->start();
}

void PackageWidgets::AddWidget(AppWidget* pWidget)
{
    _AppWidgets.push_back(pWidget);
}
void PackageWidgets::showCurrent(int nCurChosen)
{
    _TabIndex = nCurChosen;
    readData();
}

void PackageWidgets::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (_error != "") {
         QFontMetrics metrics(this->font());
         int length =  metrics.width(_error);
         int textHeight = metrics.height();

         painter.setPen(Qt::gray);
         painter.drawText(QRect((width()-length)/2, height() / 2, length, textHeight + 5),_error);
    }

    QWidget::paintEvent(event);
}

void PackageWidgets::RefreshUI()
{
    calTabSum();
    clearUI();

    //读取数据库，添加应用界面
    for (int i = 0;i < AppDatabase::Instance()->size();i++)
    {
        AppData *data = AppDatabase::Instance()->getData(i);
        AppWidget* pWidget = new AppWidget(this);
        pWidget->hide();
        pWidget->setData(data);
        AddWidget(pWidget);
    }
    RefreshListShow();
}
void PackageWidgets::resizeEvent( QResizeEvent * event)
{
    int wndWidth = this->width();
    int wndHeight = this->height();

    int oldOneCount = _oneCount;
    int oldHEdge = _HEdge;
    int oldVEdge = _VEdge;

    int movieWidth = 50;
    int r = movieWidth / 2;
    _movieLabel->setGeometry(wndWidth / 2 - r,  wndHeight / 2- r, movieWidth,movieWidth);

    _oneRowCount = wndWidth / App_Widget_Width;
    int rowSum = wndHeight / App_Widget_Height;//每页的行数

    _oneCount = _oneRowCount * rowSum;//每页多少个

    if (_curFirstApp > -1)
          _TabIndex = _curFirstApp / _oneCount + 1;

    _HEdge = 0;//水平间距
    _VEdge = 0;//垂直间距
    if (rowSum > 1)
        _VEdge = (wndHeight - rowSum * App_Widget_Height) / (rowSum - 1);

    if (_oneRowCount > 1)
        _HEdge = (wndWidth - _oneRowCount * App_Widget_Width) /(_oneRowCount - 1);

    _VEdge = 10;
    _HEdge = 15;

    if (_oneCount != oldOneCount)
        readData();
    else if (oldHEdge != _HEdge && oldVEdge != _VEdge)
        RefreshListShow();

    if (_TabSum >= 1)
       emit(currentTabIndex(_TabIndex));
    QWidget::resizeEvent(event);
}
void PackageWidgets::RefreshListShow()
{
    int widgetCount = 0;//页面计数
    for (int i = 0;i < _AppWidgets.size();i++) {

        AppWidget* pWidget = _AppWidgets.at(i);

        if (!pWidget) {
            continue;
        }
        pWidget->move((widgetCount % _oneRowCount) * (App_Widget_Width + _HEdge) + _HEdge,
                          widgetCount / _oneRowCount * (App_Widget_Height + _VEdge) + _VEdge);
      //  pWidget->show();
        widgetCount++;
    }
    for (int i = 0;i < _AppWidgets.size();i++) {

        AppWidget* pWidget = _AppWidgets.at(i);
        pWidget->show();
    }
}

void PackageWidgets::OnRefresh()
{
    _movieLabel->hide();
    RefreshUI();
}

void PackageWidgets::OnStartReadSort()
{
    _error = "";
  //  repaint();
    _movieLabel->show();
}

void PackageWidgets::OnuiError(const QString &error)
{
    _error = error;
    _movieLabel->hide();
    repaint();
}

void PackageWidgets::tabChanged(const int &tabIndex)
{
    _sortTabIndex = tabIndex;
    _TabIndex = 1;
    emit currentTabIndex(_TabIndex);
    readData();
}

void PackageWidgets::readData()
{
    clearUI();
    AppDataReadExe::Instance()->readSortData(_sortTabIndex, (_TabIndex - 1) * _oneCount, _oneCount);
}

void PackageWidgets::calTabSum()
{
    int nOldTabSum = _TabSum;
    int nTotalCount = AppDatabase::Instance()->totalCount();

    if (_oneCount > 0) {
        _TabSum = nTotalCount / _oneCount;
        if (nTotalCount % _oneCount > 0)
            _TabSum++;
    }

    if (nTotalCount > 0)
         _curFirstApp = _oneCount * (_TabIndex - 1);

    if (_TabSum != nOldTabSum)
        emit(tabSum(_TabSum));
}

void PackageWidgets::clearUI()
{
    _error = "";
    repaint();
    //清除旧界面
    for (int i = 0;i < _AppWidgets.size();i++) {
        AppWidget* pWidget = _AppWidgets.at(i);
        if (pWidget)
            delete pWidget;
    }
    _AppWidgets.clear();
}

void PackageWidgets::clearWidget()
{
    clearUI();
}

void PackageWidgets::OnUpdateScore(QString appName, int score)
{
    Q_UNUSED(appName);
    Q_UNUSED(score);
//    for (int i = 0;i < _AppWidgets.size();i++)
//        _AppWidgets.at(i)->UpdateScore(appName, score);
}
