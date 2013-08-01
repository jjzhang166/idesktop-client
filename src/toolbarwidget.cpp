#include <QLabel>
#include <QTimeLine>
#include <QColor>
#include <QTimer>
#include <QApplication>
#include <QPainter>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QProcess>
#include <QSettings>
#include <QTextEdit>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include "strings.h"
using namespace std;
#include <QMessageBox>
#include <QCursor>
#include <QDesktopServices>
#include <QMessageBox>

#include "toolBarWidget.h"
#include "localapps.h"
#include "movingdialog.h"
#include "appmessagebox.h"
#include "config.h"
#include "dirwidget.h"
//#include "movewidget.h"
#include "paascommuinication.h"
#include "toolbarwidget.h"

#define ICONWIDTH 143                                   //72
#define ICONHEIGHT 143                                  //72

#define SPACING 30
#define FONTSIZE 10

#define ICONITEMWIDTH 143                               //(ICONWIDTH + SELECTWIDTH/2 + 8)
#define ICONITEMHEIGHT 143                              //(ICONHEIGHT + 15 + FONTSIZE * 2)

#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30
#define SMALLSLIDERWIDTH 38

#define APPICON 0
#define ADDICON 1
#define HSPACING 4
#define VSPACING 0
#define BOTTOMSPACING 24

#define LARGESIZE QSize(143, 143)     //72*72
#define MEDIUMSIZE QSize(119, 119)     //48*48
#define SMALLSIZE QSize(103, 103)   //32*32

#define ICONHSPACING (88 - 40)
#define ICONVSPACING (88 - 36)

extern QString WIN_VAPP_IconPath;
extern QList<APP_LIST> g_myVappList;
extern QList<LOCAL_LIST> g_RemotelocalList;
extern QList<APP_LIST> g_RemoteappList;
extern QList<PAAS_LIST> g_RemotepaasList;
extern int ICON_TYPE;

static QPoint gap;

toolBarWidget::toolBarWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent,Qt::FramelessWindowHint/* | Qt::Tool | Qt::WindowStaysOnTopHint*/)
    , _vertical(true)
    , _itemClicked(false)
    , _itemHeld(false)
    , _trembling(false)
    , _dragEvent(false)
    , _addAppState(false)
    , _animationScreenDown(false)
    , _url("")
    , _iconDragEnter(false)
    , _dirId(-1)
    , _upDistance(0)
    , _distance(0)
    , _openDir(false)
//    , _iconSize(ICON_TYPE)
    , _dragEnterMinWidget(false)
    , _isIconMove(true)
    , _isOtherWidgetObject(false)
    , _dragItem(NULL)
    , _localIconNum(0)
    , _dirMovingFinished(true)
    , _count(1)
    , _iconsPerPage(0)
    , _toolBarDirIndex(1000)
    , _inDrag(NULL)
    , _refreshToolBar(false)

{

    setFocusPolicy(Qt::ClickFocus);

    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = LARGESIZE.height() + VSPACING + BOTTOMSPACING;

    _iconHSpacing = ICONHSPACING;
    _iconVSpacing = ICONVSPACING;

    gridWidth = SMALLSIZE.width() + HSPACING;
    gridHeight = SMALLSIZE.height() + VSPACING + BOTTOMSPACING;

    _col = (_width - ICONHSPACING) / gridWidth;
    _row = 1;

    _MaxNum = _col * _row;

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++)
        _nextIdx.insert(i, 0);

    qDebug() << "toolBarWidget()-->MaxNum" << _MaxNum;

    setAcceptDrops(true);
    setAutoFillBackground(true);
    setFixedSize(_width, _height);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255,0));
    setPalette(pal);

//    _desktopWidget = QApplication::desktop();
//    _desktopRect = _desktopWidget->availableGeometry();
    _local = LocalAppList::getList();

    initIconItem();

//    if (_iconsPerPage == 0)
//    {
//        addIcon("dustbin", ":images/dustbin_normal.png", -1, -1, "", dustbinIcon);
//    }

    dragLeftTimer = new QTimer(this);
    dragLeftTimer->setInterval(500);
    dragLeftTimer->setSingleShot(true);

    dragRightTimer = new QTimer(this);
    dragRightTimer->setInterval(500);
    dragRightTimer->setSingleShot(true);

//    connect(dragLeftTimer, SIGNAL(timeout()), this, SLOT(dragLeft()));
//    connect(dragRightTimer, SIGNAL(timeout()), this, SLOT(dragRight()));

//    connect(_local, SIGNAL(appAdded(const QString&, const QString&, const QString &, int))
//            , this, SLOT(addIcon(const QString&, const QString&, const QString &, int)));
//    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));

//    connect(&_communi, SIGNAL(appRun()), this, SLOT(runServerApp()));//

}
toolBarWidget::~toolBarWidget()
{

#if 0
    for (int j = 0; j < _iconDict.size(); j++)
    {
        delete &_iconDict[0][j];
    }
#endif

//    for (int i = 0; i < _dirList.count(); i++) {
//        _dirList.at(i)->setParent(NULL);
//        delete _dirList.at(i);
//    }
//    _dirList.clear();

//    for (int i = 0; i < _dirMinList.count(); i++) {
//        _dirMinList.at(i).setParent(NULL);
//        delete _dirMinList.at(i);
//    }
    _dirMinList.clear();

//    delete dragLeftTimer;
//    delete dragRightTimer;
//    delete _dirMinShowWidget;
}

void toolBarWidget::iconDragEnter()
{
    _iconDragEnter = true;

    qDebug() << "DirWidget*************************_iconDragEnter";
//    _dragEnterMinWidget = true;
}

void toolBarWidget::iconDragMove()
{
    _iconDragEnter = true;

        qDebug() << "DirWidget*************************iconDragMove";
//    _dragEnterMinWidget = true;
}

void toolBarWidget::iconDragLeave()
{
    _iconDragEnter = false;
//    _dragEnterMinWidget = false;
        qDebug() << "DirWidget*************************iconDragLeave";
}

void toolBarWidget::iconDragDrop(int id, const QString &text,
                                  const QString &iconPath, int page,
                                  int index, const QString &url, int type)
{
    _iconDragEnter = false;

    //_isDirWidgetObject = false;

    if (_inDrag)
    {
        moveOutIcon(text);

        _inDrag = NULL;
    }


    if (id == 1000)
    {
        emit toolBarIconToDustbin(text, iconPath, -1, index, url, type);
    }
    else
    {
        emit toolBarIconToDir(id, text, iconPath, -1, index, url, type);
    }

    emit iconDropToolWidget(text);

}

void toolBarWidget::updateClicked()
{
//    _itemClicked = false;
}

IconItem * toolBarWidget::getIconByName(const QString &name)
{
    return _iconDict.value(name, NULL);
}

void toolBarWidget::atExit()
{
//    _local->save();
}

//void toolBarWidget::reload()
//{
//    emit reInitiate();
//}

//void toolBarWidget::mousePressEvent(QMouseEvent *event)
//{
////    if (_openDir)
////    {
////        closeDir(_dirPage, _dirIndex);
////        return;
////    }

//    emit toolBarClicked();
////    _iconMenu->setVisible(false);
////    hideDirWidget();
////    hideMenuWidget();

//    if (_animation->state() == QAbstractAnimation::Running) {
//        _animation->stop();
//    }

//    IconItem *item = static_cast<IconItem*>(childAt(event->pos()));

//    if (item) {
//        if (_trembling) {
//            if (QRect(0, 0, 30, 30).contains(event->pos() - item->pos())) {
//                //delIcon(item->text());
//               // uninstall(item->text());
//                return;
//            }
//        }
//        if (_itemClicked) {
//            if (!_trembling) {
//                //runApp(item->text());
//                _itemClicked = false;
//            }
//        } else {
//            QTimer::singleShot(400, this, SLOT(updateClicked()));
//            QTimer::singleShot(1000, this, SLOT(itemHeld()));
//            _itemHeld = true;
//            _itemClicked = true;
//        }
//    }
//    else
//    {
//        emit pageChanged(_current);
//    }
//    m2v = event->x();
//    gm2v = mapToGlobal(event->pos()).x();
//    event->accept();
//}

bool toolBarWidget::largerThan(const QSize &a, const QSize &b)
{
    int areaA = abs(a.width() * a.height());
    int areaB = abs(b.width() * b.height());
    return areaA > areaB ? true : false;
}

int toolBarWidget::getNearestIndex(const QRect &rect)
{
    int page = rect.x()/_pageSize.width();
    int index = -1;
    QRect inter;
    for (int i = 0; i < _iconsPerPage; i++) {
        inter = rect.intersected(_gridTable[page][i]);
        int area = abs(inter.width() * inter.height());
        if (area > 500) {
            index = i;
            break;
        }
    }
    return index;
}

void toolBarWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (_dragItem)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;

    }

    if (_localIconNum == _MaxNum)
    {
 //       QMessageBox::information(NULL, "TIP", tr("工具栏已满"));

        event->ignore();
        setAcceptDrops(false);

    }
    else
    {
        setAcceptDrops(true);
    }

    _dragEvent = true;

    _itemHeld = false;

    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (icon->parent() == this)
    {
        _inDrag = icon;
        icon->hide();

        if (_inDrag->type() == dirIcon || _inDrag->type() == dustbinIcon)
        {
            for (int i = 0; i <_dirMinList.count(); i++)
            {
                if (_iconDict.value(_dirMinList.at(i)) == _inDrag)
                    continue;

                _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(false);
            }
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->acceptProposedAction();

        if (_isOtherWidgetObject)
            return;


        if (!_isIconMove)
        {
            emit toolCloseDir(_dirPage, _dirIndex);
        }

        _isOtherWidgetObject = true;

//        if (_dragItem != NULL)
//            _dragItem = NULL;
        _dragItem = icon;

        addIcon(icon->text(), icon->pix()
                , 0, _nextIdx[_count - 1]
                , icon->url(), icon->type()
                , icon->id());

        if (_inDrag->type() == dirIcon)
        {
            for (int i = 0; i <_dirMinList.count(); i++)
            {
                if (_iconDict.value(_dirMinList.at(i)) == _inDrag)
                    continue;

                _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(false);
            }
        }
    }
}

void toolBarWidget::dragMoveEvent(QDragMoveEvent *event)
{
//    qDebug() << "123123123123213213213213213213";

//    event->setDropAction(Qt::MoveAction);
//    event->accept();
//    return;

    if (_refreshToolBar)
        return;

    if (!_isIconMove)
        return;
    if (_iconDragEnter)
        return;

    _dragEvent = true;

    event->setDropAction(Qt::MoveAction);
    event->accept();

    _itemHeld = false;
    int p = _inDrag->page();
    int s = _inDrag->index();
    /* The virtual desktop can only expand to right */
    /* If the page is the last page and there is only one icon
     * on this page, this icon can't be moved to add a page
     */

    /* move to the right page */
    if (QRect(_pageSize.width() - 20, 0, 20, _pageSize.height())\
            .contains(mapToGlobal(event->pos()))) {
        if (p == (_count-1) && _iconTable[p][1] == NULL)
            return;
        //dragRightTimer->start();
        //QTimer::singleShot(500, this, SLOT(dragRight()));
    }
    /* move to the left page */
    else if (QRect(0, 0, 20, _pageSize.height())\
             .contains(mapToGlobal(event->pos()))) {
        if (_current == 0)
            return;
        //dragLeftTimer->start();
    }
    /* move in this page */
    else {
        qDebug() << "move in this page start";
        QRect moving(event->pos() - gap, _inDrag->size() * 1.0);
        int index = getNearestIndex(moving);
        if (index == -1 || s == index)
            return;
#if 0
        if ((ADDICON == _iconTable[_current][index]->_icontype)||(ADDICON == _iconTable[_current][s]->_icontype))
        {
            return;
        }
#endif
        index = index > _nextIdx[_current] - 1 ? _nextIdx[_current] - 1: index;
        if (s > index) {
            for (int i = s; i > index; i--) {
                QPoint end = _gridTable[_current][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_current][i-1]->move(end);
                _iconTable[_current][i] = _iconTable[_current][i-1];
                _iconTable[_current][i]->setIndex(i);
            }
        }
        else {
            for (int i = s; i < index; i++) {
                QPoint end = _gridTable[_current][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_current][i+1]->move(end);
                _iconTable[_current][i] = _iconTable[_current][i+1];
                _iconTable[_current][i]->setIndex(i);
            }
        }
        //}
        _iconTable[_current][index] = _inDrag;
        _inDrag->setPage(_current);
        _inDrag->setIndex(index);

                qDebug() << "move in this page end";
    }

}

void toolBarWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    _refreshToolBar = true;

    _iconDragEnter = false;

    qDebug() << "*********************************dragLeaveEventdragLeaveEvent";
    _itemHeld = false;

 //   hideDirWidget();
//    _inDrag->show();
//    int p = _inDrag->page();
//    int s = _inDrag->index();
//    _inDrag->move(_gridTable[p][s].translated(HSPACING, VSPACING).topLeft());
    //drag->setPixmap(QPixmap(""));
    //icon->show();
    //event->accept();
#if 1
    if (_dragItem != NULL)
    {
        if (!_inDrag)
            return;

        moveOutIcon(_inDrag->text());

        _inDrag = NULL;

//        _iconTable[p][s] = NULL;

//        moveBackIcons(p, s);

        _dragItem = NULL;
    }
#endif
    _isOtherWidgetObject = false;

    _dragEvent = false;


    Q_UNUSED(event);
}

void toolBarWidget::dropEvent(QDropEvent *event)
{
    qDebug() << "111111111111111111111111111111111111111";

    event->setDropAction(Qt::MoveAction);
    event->accept();
    qDebug() << "222222222222222222222222222222222222222";
    _itemHeld = false;
    /*
    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (!icon)
        return;
    icon->show();
    int p = icon->page();
    int s = icon->index();
    qDebug() << "p: " << p << "\ts: " << s;
    qDebug() << "inDrag: " << _inDrag->page() << "  " << _inDrag->index();
    */
    if (!_inDrag)
        return;
    qDebug() << "333333333333333333333333333333333333333";
    dragLeftTimer->stop();
    dragRightTimer->stop();
    int p = _inDrag->page();
    int s = _inDrag->index();
    qDebug() << "P: " << p << " S: " << s;
    QPoint end = _gridTable[p][s].translated(HSPACING, VSPACING).topLeft();
    qDebug() << "End: " << end;
    _iconTable[p][s] = _inDrag;
    _inDrag->move(end);
    qDebug() << "Pos: " << _inDrag->pos();
//    _inDrag->show();
//    qDebug() << "444444444444444444444444444444444444444";
//    _inDrag=NULL;
//    qDebug() << "555555555555555555555555555555555555555";

    _inDrag->show();
    qDebug() << "444444444444444444444444444444444444444";

    QString dropText = _inDrag->text();

    _inDrag=NULL;
    qDebug() << "555555555555555555555555555555555555555";

    if (_dragItem != NULL)
    {

        if (_dragItem->type() == dirIcon)
        {
            for (int i = 0; i < _local->count(); i++) {
                if (_local->at(i)->hidden())
                    continue;

                if (_dragItem->id() == _local->at(i)->dirId())
                {
                    _iconDict.value(_dragItem->text())->addDirMinItem(_local->at(i)->name(), _local->at(i)->icon(),
                                                                      _local->at(i)->page(), _local->at(i)->index(),
                                                                      _local->at(i)->url());
                }
            }
        }

        for (int i = 0; i <_dirMinList.count(); i++)
        {
            _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(true);
        }

        _dragItem = NULL;

    }

    _dragEvent = false;
    _isOtherWidgetObject = false;

    emit iconDropToolWidget(dropText);

}

#if 0
void toolBarWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _itemHeld = false;

    if (event->modifiers() == Qt::ControlModifier)
    {
        event->ignore();
        return;
    }

    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }

    event->accept();
    int offset = x() - _pages[_current];
    _animation->setDuration(900);
    //_animation->setEasingCurve(QEasingCurve::OutBack);
    if (abs(offset) > _pageSize.width()/6) {
        if (offset < 0) {
            if (_current == _count - 1) {
                _animation->setStartValue(QRect(x(), y(), \
                                                width(), height()));
                _animation->setEndValue(QRect(_pages[_current], y(), \
                                              width(), height()));
            } else {
                goNext();
            }
        } else {
            if (_current == 0) {
                emit toOrigin();
                _animation->setStartValue(QRect(x(), y(), \
                                                width(), height()));
                _animation->setEndValue(QRect(_pages[_current], y(), \
                                              width(), height()));
            } else {
                goPrevious();
            }
        }
    } else {
        _animation->setStartValue(QRect(x(), y(), \
                                        width(), height()));
        _animation->setEndValue(QRect(_pages[_current], y(), \
                                      width(), height()));
    }
    _animation->start();
}
#endif

void toolBarWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.drawPixmap((_width - gridWidth * _iconsPerPage) / 2 - 31, _height - 60,
                       31, 60, QPixmap(":images/toolbar_left.png"));
    painter.drawPixmap((_width - gridWidth * _iconsPerPage) / 2, _height - 60,
                       gridWidth * _iconsPerPage , 60, QPixmap(":images/toolbar_center.png"));
    painter.drawPixmap((_width - gridWidth * _iconsPerPage) / 2 + _iconsPerPage * gridWidth, _height - 60,
                       31, 60, QPixmap(":images/toolbar_right.png"));
}

int toolBarWidget::addIcon(const QString &text, \
                            const QString &iconPath, \
                            int page, \
                            int index,\
                            const QString &url,\
                            int type, int id)
{
    qDebug() << text << iconPath << page << index << url << type << id;
    _localIconNum ++;

    setShowType();

    qDebug() << "addIcon";

    IconItem *icon = new IconItem(this);

//    switch(_iconSize)
//    {
//        case IconItem::large_size :
//           icon->setLargeSize();
//            break;

//        case IconItem::medium_size :
//            icon->setMediumSize();
//            break;

//        default:
    icon->setSmallSize();
//            break;
//    }
    icon->setText(text);
    icon->setTimeLine(false);
    icon->setPropertyAnimation(true);
    icon->setType(type);

    icon->setRunAction(false);
    icon->setDelAction(false);
    icon->setClosePixBool(false);
    icon->setSelectPixBool(false);
    icon->setDoubleClickBool(true);
    icon->setContextMenuBool(true);
    icon->setTrembleBool(false);
    icon->setDragEventBool(true);
    icon->setEnterEventBool(true);
    icon->setLeaveEventBool(true);
    icon->setEqualIcon(false);
    icon->setSaveDataBool(true);

    if(type == dirIcon)
    {

        icon->addMinWidget(2);  //  0 : largeSize, 1 : mediumSize, 2 : smallSize;
        icon->setId(id);
//        icon->showSmallSize(true);

    }
    else if (type == 4)
    {
        icon->addDustbin();
        icon->setId(1000);
        //emit toolBarAddDirSW(_toolBarDirIndex);


    }


    /* TODO deal with the icon is full */
    if (page == -1) {
            qDebug() << "toolBarWidget::addIcon() --> setPage(page) -- > _count" << page << _count;
        for (int i = 0; i < _count; i++) {

            if (_nextIdx[i] < _iconsPerPage) {

                page = i;
                index = _nextIdx[i];
                qDebug() << "toolBarWidget::addIcon() --> setPage(page)" << page  << index;

                break;
            }
        }
    }
    else {
//            qDebug() << "toolBarWidget::addIcon() --> setPage(page)-- >1" << page;
//            qDebug() << "toolBarWidget::addIcon() --> setPage(page)-- >2" << index;
//            qDebug() << "toolBarWidget::addIcon() --> setPage(page)-- >3" << _nextIdx[page];
//            qDebug() << "toolBarWidget::addIcon() --> setPage(page)-- >4" << _iconsPerPage;
//        if (index == -1) {

//            if ((page > 0) && (_nextIdx[page - 1] == _iconsPerPage) && (_nextIdx[page] < _iconsPerPage)) {
//                index = _nextIdx[page];
//            }
//            else if ((page == 0) && (_nextIdx[page] < _iconsPerPage)) {
//qDebug() << "toolBarWidget::addIcon() --> setPage(page)-- >4" << page;
//                index = _nextIdx[page];
//            }
//            else {
//qDebug() << "toolBarWidget::addIcon() --> setPage(page)-- >5" << page;
//                for (int i = 0; i < _count; i++) {
//                    if (_nextIdx[i] < _iconsPerPage) {
//qDebug() << "toolBarWidget::addIcon() --> setPage(page)-- >6" << page;
//                        page = i;
//                        index = _nextIdx[i];
//                        move(_pages[page], y());
//                        break;
//                    }
//                }
//            }
            if (_nextIdx[page] < _iconsPerPage)
            {
                index = _nextIdx[page];
            }
            else {
                for (int i = 0; i < _count; i++) {
                    if (_nextIdx[i] < _iconsPerPage) {
                        page = i;
                        index = _nextIdx[i];
                        move(_pages[page], y());
                        break;
                    }
                }
            }
       }
//    }


    icon->setPixmap(iconPath,text);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(url);
    icon->setDirId(-2);
    _iconDict.insert(text, icon);
    _iconItemLists.append(icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    if (!_isOtherWidgetObject)
    {
        icon->show();
    }
    else
    {
        _inDrag = icon;
        icon->hide();
    }

    if (dirIcon == type || dustbinIcon == type)
    {
//        icon->setId(_toolBarDirIndex);

        _dirMinList.append(text);

//        _toolBarDirIndex++;

    }


        connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(delIcon(const QString&)));
        connect(icon, SIGNAL(iconEnter()), this, SLOT(iconDragEnter()));
        connect(icon, SIGNAL(iconMove()), this, SLOT(iconDragMove()));
        connect(icon, SIGNAL(iconLeave()), this, SLOT(iconDragLeave()));
        connect(icon, SIGNAL(iconDrop(int, const QString &, const QString &, int, int, const QString &, int)),
                this, SLOT(iconDragDrop(int, const QString &, const QString &, int, int, const QString &, int)));
        connect(icon, SIGNAL(openDir(int, int, int)), this, SLOT(openDir(int, int, int)));
        connect(icon, SIGNAL(dragEnterMinWidget()), this, SLOT(iconDragEnter()));
    //    connect(icon, SIGNAL(showContextMenu(QPoint, QPoint,const QString &))
    //            , this, SLOT(showIconContextMenu(QPoint, QPoint,const QString &)));
        qDebug() << "toolBarWidget::addIcon() --> setPage(page)" << "2222222"<< type;

//    _iconLists.append(icon);


     return page;
}

//void toolBarWidget::test()
//{
//    qDebug() << "testttttttttttttttttttttttttttttttttttttttttttttttttt";
//    emit toolBarAddDirSW(_toolBarDirIndex);

//}

int toolBarWidget::addIcon(const QString &text, const QString &icon, const QString &url, int type, int id)
{
    return addIcon(text, icon, _current, -1, url, type, id);
}

void toolBarWidget::delIcon(const QString &text)
{
    if (_iconDict.value(text) == NULL)
        return;

    IconItem *icon = _iconDict.take(text);

    for (int i = 0; i < _iconItemLists.count(); i++)
    {
        if (_iconItemLists.at(i)->text() == icon->text())
        {
            _iconItemLists.removeAt(i);
            break;
        }
    }

    //icon->setHidden(true);
    qDebug() << icon->page();
    int p = icon->page();
    int s = icon->index();
    //icon->setPage(-1);
    //icon->setIndex(-1);
    delete _iconTable[p][s];

    moveBackIcons(p, s);

//    for(int i = p; i < _count; i++)
//    {
//        if (i == p)
//        {
//            int k;
//            for (k = s + 1; k < _nextIdx[p]; k++) {
//                QRect start = _gridTable[p][k].translated(HSPACING, VSPACING);
//                QRect end = _gridTable[p][k-1].translated(HSPACING, VSPACING);
//                _iconTable[p][k]->animationMove(start, end);
//                _iconTable[p][k]->setIndex(k-1);
//                _iconTable[p][k - 1] = _iconTable[p][k];
//            }
//        }
//        else {

//            for (int j = 0; j < _nextIdx[i]; j++)
//            {
//                if (j == 0)
//                {

//                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
//                    QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(HSPACING, VSPACING);
//                    _iconTable[i][j]->animationMove(start, end);
//                    _iconTable[i][j]->setPage(i-1);
//                    _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
//                    _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
//                }
//                else {

//                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
//                    QRect end = _gridTable[i][j-1].translated(HSPACING, VSPACING);
//                    _iconTable[i][j]->animationMove(start, end);
//                    _iconTable[i][j]->setIndex(j-1);
//                    _iconTable[i][j - 1] = _iconTable[i][j];
//                }
//            }
//        }
//    }

//    _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
//    _nextIdx[_count - 1]--;


//    if (_nextIdx[_count - 1] == 0)
//        delPage(_count - 1);

//    if (_nextIdx[_count - 1] == 0)
//    {
//        _iconTable[_count - 1 - 1][_nextIdx[_count - 1 - 1] - 1] = NULL;
//        _nextIdx[_count - 1 - 1]--;
//    }else
//    {
    //        _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
    //        _nextIdx[_count - 1]--;
    //    }

//    if ((_count > 1) && (_local->count() < _iconsPerPage * (_count - 1)))
//        delPage(_count - 1);

//    /* move addicon add the last*/
//    if (_nextIdx[_count - 1] == _iconsPerPage - 1)
//        showAddIcon(_count - 1, _nextIdx[_count - 1]);
//    else
//        showAddIcon(_count - 1, -1);

//    if (_local->count() == 0) {
//        appCancel();
//    }

//    bool trembling = true;

//    for (int i = 0; i < _local->count(); i++)
//    {
//        if (!_local->at(i)->isRemote())
//        {
//            trembling = false;
//            break;
//        }
//    }

//    if (trembling) {
//        appCancel();
//    }

}

void toolBarWidget::moveBackIcons(int page, int index)
{
    int p = page;
    int s = index;

    for(int i = p; i < _count; i++)
    {
        if (i == p)
        {
            int k;
            for (k = s + 1; k < _nextIdx[p]; k++) {
                QRect start = _gridTable[p][k].translated(HSPACING, VSPACING);
                QRect end = _gridTable[p][k-1].translated(HSPACING, VSPACING);
                _iconTable[p][k]->animationMove(start, end);
                _iconTable[p][k]->setIndex(k-1);
                _iconTable[p][k - 1] = _iconTable[p][k];
            }
        }
        else {

            for (int j = 0; j < _nextIdx[i]; j++)
            {
                if (j == 0)
                {
                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
                    QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(HSPACING, VSPACING);
                    _iconTable[i][j]->animationMove(start, end);
                    _iconTable[i][j]->setPage(i-1);
                    _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
                    _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
                }
                else {

                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
                    QRect end = _gridTable[i][j-1].translated(HSPACING, VSPACING);
                    _iconTable[i][j]->animationMove(start, end);
                    _iconTable[i][j]->setIndex(j-1);
                    _iconTable[i][j - 1] = _iconTable[i][j];
                }
            }
        }
    }

    _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
    _nextIdx[_count - 1]--;

//    if (_nextIdx[_count - 1] == 0)
//        delPage(_count - 1);

    _localIconNum--;

    setShowType();

}

void toolBarWidget::openDir(int id, int page, int index)
{
        int x = _gridTable[page][index].x();
        emit toolOpenDir(id , x, gridWidth);
}

#if 0
void toolBarWidget::runApp(const QString &text)
{
    QString tmp;
    QString pram;

//    if (!_iconDict.value(text))
//        return;

    if (!_iconDict.value(text)->url().isEmpty())
    {

        QDesktopServices::openUrl(QUrl(_iconDict.value(text)->url()));
        return;
    }

    bool isRmote = false;
    for (int i = 0; i < g_myVappList.count(); i++)
    {
        qDebug() << "g_myVappList.at(i).name" << g_myVappList.at(i).name;
        qDebug() << "text" << text;
        if(g_myVappList.at(i).name == text)
        {
            _appid = g_myVappList.at(i).id;
            Commui._type = g_myVappList.at(i).type;
            isRmote = true;
        }
    }
    if(isRmote)
    {
        appText = text;
        _communi.loadBalance(Commui._name,_appid);
        if(_communi._isNetError)
        {
            QMessageBox::warning(this, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
        }
        return;
    }

    LocalApp *la = _local->getAppByName(text);
    if (!la)
        return;

    char command[2048];
    if (la->type().toInt() == localIcon) {
        QFileInfo link(la->execname());

        qDebug() << "la->execname():" << la->execname();
        if (!link.exists()) {
            AppMessageBox box(true, NULL);
            box.setText("指向的应用或快捷方式已不存在\n点击\"确定\"删除图标");
            if (box.exec())
                LocalAppList::getList()->delApp(la->name());
            return;
        }
        sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());
    } else {
        QFileInfo link(la->execname());
        //   QString exec = AppDataReadExe::Instance()->getExec(la->execname());
        if (!link.exists()) {
//            QString msg = QString("%1没有被正确安装, 请在\n软件商店中重新安装, 是否删除图标?").arg(text);
//            AppMessageBox box(true, NULL);
//            box.setText(msg);
//            if (box.exec())
//                LocalAppList::getList()->delApp(la->name());

            QMessageBox::information(NULL,"NO","Linke to Function is not OK!!!!");
            return;
        }
        sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());
    }
    ShellExecute(NULL, "open", command, "", "", SW_SHOW);

}

void toolBarWidget::runServerApp()
{

    QString tmp;
    QString pram;
    QString _appid;

    qDebug()<<appText;
    for (int i = 0; i < g_myVappList.count(); i++)
    {
        if(g_myVappList.at(i).name == appText)
        {
            qDebug() << g_myVappList.at(i).name;

            _appid = g_myVappList.at(i).id;
            Commui._type = g_myVappList.at(i).type;
        }
    }


    if(_communi.errID == "10000")
    {
        if (m_dllInitClass && m_dllCloseClass && m_dllCloseAppAll && m_dllStartAppEx )                  //??????? add() ??
        {
            char buf[100];
            DWORD len = GetLogicalDriveStringsA(sizeof(buf)/sizeof(char),buf);
            //
            //
            string strDiskVolume;
            string strFixed;
            string strCDROM;
            string strRemovable;

            for (char* s=buf; *s; s+=strlen(s)+1)
            {
                string strDrivePath = s;
                // GetDriveType
                //
                UINT uDriveType;
                uDriveType = GetDriveTypeA(strDrivePath.c_str());
                strDiskVolume.append(strDrivePath);

                if (uDriveType == DRIVE_CDROM)
                {
                    strCDROM.append(strDrivePath);
                    //qDebug() << "cdrom : " << strDrivePath.c_str();
                }
                else if (uDriveType == DRIVE_REMOVABLE)//mass storage
                {
                    strRemovable.append(strDrivePath);
                    //qDebug() << "removable : " << strDrivePath.c_str();
                }
                else if (uDriveType == DRIVE_FIXED)
                {
                    strFixed.append(strDrivePath);
                    //qDebug() << "fixed : " << strDrivePath.c_str();
                }
            }

            qDebug()<<"string:"<<QString::fromLocal8Bit(strDiskVolume.c_str())<<endl;
            QString diskpath;
            //            if(Commui._driver_loadClientDriverOnLogin == "1")
            //            {
            //                diskpath = QString::fromLocal8Bit(strDiskVolume.c_str());
            //            }
            //            else
            //            {
            //                if(Commui._driver_mapClientStorageOnLogin_check_harddisk == 0)
            //                    diskpath = QString::fromLocal8Bit(strFixed.c_str());
            //                if(Commui._driver_mapClientStorageOnLogin_check_CD == 0)
            //                    diskpath = diskpath + QString::fromLocal8Bit(strCDROM.c_str());
            //            }
            if(Commui._driver_mapClientStorageOnLogin_check_harddisk == "1")
            {
                diskpath = QString::fromLocal8Bit(strFixed.c_str());
                //qDebug() << "fixed : " << strFixed.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_CD == "1")
            {
                diskpath = diskpath + QString::fromLocal8Bit(strCDROM.c_str());
                //qDebug() << "CDROM : " << strCDROM.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_floppy == "1")
            {
                diskpath = diskpath + QString::fromLocal8Bit(strRemovable.c_str());
                //qDebug() << "Removable : " << strRemovable.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_shareStorage == "1")
            {
                //diskpath = diskpath + QString::fromLocal8Bit(strRemovable.c_str());
            }

            qDebug()<<"diskpath:"<<diskpath;
            diskpath.replace("\\" , ",");
            qDebug()<<"count:"<<diskpath.count();
            diskpath.remove((diskpath.count() - 1),1);
            qDebug()<<"diskPath:"<<diskpath;
            qDebug()<<"cdrom:"<<QString::fromLocal8Bit(strCDROM.c_str());

            tmp = Commui._path;
            pram= Commui._param;
            const wchar_t* _param=reinterpret_cast<const wchar_t*>(pram.utf16());
            const wchar_t* ty = reinterpret_cast<const wchar_t*>(tmp.utf16());
            qDebug() << "passwd::" << Commui._pswd;
            //Commui._pswd = decryptRsa();
            qDebug() << "passwd::" << Commui._pswd;
            const wchar_t* pswd = reinterpret_cast<const wchar_t*>(Commui._pswd.utf16());
            const wchar_t* localime = reinterpret_cast<const wchar_t*>(Commui._redirect_inputMethodRedirect.utf16());

            if(Commui._userType == "1" || Commui._userType == "2")
            {
                Commui._applogin = Commui._domain +  "\\" + Commui._name;
            }
            else
            {
                Commui._applogin = Commui._name;
            }
            const wchar_t* name = reinterpret_cast<const wchar_t*>(Commui._applogin.utf16());
            const wchar_t* appName = reinterpret_cast<const wchar_t*>(appText.utf16());
            const wchar_t* ipPort = reinterpret_cast<const wchar_t*>(Commui._rdpIpPort.utf16());
            const wchar_t* audio = reinterpret_cast<const wchar_t*>(Commui._audio_clientAudioOutput.utf16());
            const wchar_t* _diskpath = reinterpret_cast<const wchar_t*>(diskpath.utf16());
            qDebug()<<"name:"<<Commui._name;
            qDebug()<<"ipPort:"<<Commui._rdpIpPort<<endl;
            bool tray;
            if("1" == Commui._type)
            {
                tray = true;
            }
            else
            {
                tray = false;
            }

            bool ok;
            m_dllInitClass(L"", L"");
            qDebug()<<"audio::"<<Commui._audio_clientAudioOutput.toULong(&ok,10);
            int _audio;
            if(Commui._audio_clientAudioOutput.toULong(&ok,10)==1)
            {
                _audio = 3;
            }
            else
            {
                _audio = 1;
            }


            DWORD colorDepth=24;
            switch (Commui._viewLimited_colorDepth.toUInt(&ok,10))
            {
            case 8:
                colorDepth = 8;
                break;
            case 16:
                colorDepth = 16;
                break;
            case 24:
                colorDepth = 24;
                break;
            case 32:
                colorDepth = 32;
                break;
            default:
                colorDepth = 24;
                break;
            }

            //    qDebug()<<"_param"<<pram;
            BOOL FontSmooth;
            if(Commui._FontSmooth=="1")
                FontSmooth=true;
            else
                FontSmooth=false;


            int _DesktopWidth,_DesktopHeight;
            /*if(Commui._viewLimited_resolution_width=="0")
             {
                 _DesktopWidth=qApp->desktop()->rect().width();
                 _DesktopHeight=qApp->desktop()->rect().height();
             }
            else
            {*/
            _DesktopWidth=Commui._DesktopWidth.toULong(&ok,10);
            _DesktopHeight=Commui._DesktopHeight.toULong(&ok,10);
            //}

            DWORD dwForbidApps = 0x00;
            if(Commui._applicationForbid_cmd == "1")
            {
                dwForbidApps |= 0x0001;
            }
            if(Commui._applicationForbid_explorer == "1")
            {
                dwForbidApps |= 0x0002;
            }
            if(Commui._applicationForbid_SysManage == "1")
            {
                dwForbidApps |= 0x0004;
            }
            if(Commui._applicationForbid_help == "1")
            {
                dwForbidApps |= 0x0008;
            }

            if(tray)
                qDebug()<<"tray program";
            else
                qDebug()<<"no tray program.";

            qDebug()<<"-----dwForbidApps-" << dwForbidApps;
            m_dllStartAppEx(0, 0, Commui._redirect_inputMethodRedirect.toULong(&ok,10), _audio, \
                            colorDepth, 0, Commui._type.toULong(&ok, 10), 0, L"{123654789}",pswd, \
                            name, ipPort, appName, ty,\
                            L"", _diskpath, _param , Commui._interface_COM.toULong(&ok,10),\
                            Commui._printer_supportVirtualPrint.toULong(&ok,10) , tray,\
                            _DesktopHeight ,_DesktopWidth,Commui._other_closeClipboard.toULong(&ok,10),\
                            FontSmooth,Commui._RdpPort.toULong(&ok,10),dwForbidApps);
        }
        else
        {
            QMessageBox::information(NULL,"NO","Linke to Function is not OK!!!!");
            return;  //???????
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
        return;
    }
}


void toolBarWidget::printDesktop()
{
    qDebug() << "-----------------------------------------------";
    qDebug() << "Count : " << _count  << "; " \
             << "Current: " << _current << "; " \
             << "Geo: " << geometry() << "; ";
    for (int i = 0; i < _count; i++) {
        qDebug() << "@@@@@@@@@@@@@  " \
                 << "Page: " << i \
                 << "  @@@@@@@@@@@@@";
        for (int j = 0; j < _nextIdx[i]; j++) {
            IconItem *item = _iconTable[i][j];
            qDebug() << item->text()<< "\t\t" \
                     << item->page() << ":" \
                     << item->index()<< "\t" \
                     << item->geometry();
        }
    }
    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
}

#endif

#if 0
void toolBarWidget::reloadApplist()
{
    //we need check the page. because we only have one page now;

    int expandPageCount =  _localIconNum / _iconsPerPage + 1;  //_local->count()
    for(int i = _count; i < expandPageCount; i++)
    {
        expand();
    }
    qDebug()<<"reloadApplist  the _count is:"<<_count;

    // we need init some QList
    for (int i = 0; i < _count; i++)
           _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
            QList<QRect> newList;
            for (int j = 0; j < _col * _row; j++) {
                int x =  _pageSize.width() * i \
                        + (j % _col) * gridWidth + ICONHSPACING;
                int y = (j / _col) * gridHeight + ICONVSPACING;

//                qDebug()<<"new Grid:"<<x<<","<<y<<","<<gridWidth<<","<<gridHeight;
                newList.insert(j, QRect(x, y, gridWidth, gridHeight));
            }
            _gridTable.insert(i, newList);
        }

    for (int i = 0; i < _count; i++) {
            QList<IconItem*> newList;
            for (int j = 0; j < _col * _row; j++)
                newList.insert(j, NULL);
            _iconTable.insert(i, newList);
        }

    for (int i = 0; i < _count; i++)
           _nextIdx.insert(i, 0);

//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;

//        if(_local->at(i)->isRemote())
//        {
//            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                    _local->at(i)->page(), _local->at(i)->index(), vappIcon);
//        }else{
//            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                    _local->at(i)->page(), _local->at(i)->index(), localIcon);
//        }
//    }

    qDebug()<<"toolBarWidget reload all.";
}
#endif
//void toolBarWidget::deleteAllIconItem()
//{
//    _local->save();

//    for (int i=0;i<_count;i++)
//    {
//        for(int j=0;j<_iconsPerPage;j++) // also you can use _nextIdx[i]
//        {
//            if(_iconTable[i][j]!=NULL)
//            {
//                delete _iconTable[i][j];
//                _iconTable[i][j]=NULL;
//            }
//        }
//        _nextIdx[i]=0;
//    }
//}

//void toolBarWidget::addDirItem()
//{
//    hideMenuWidget();

//    addDesktopApp(QString("_%1").arg(_dirMinList.count()), "", "", 3);

//}

//void toolBarWidget::closeDir(int page, int index)
//{
//    if (!_dirMovingFinished)
//        return;

//    _dirMovingFinished = false;

//        qDebug() << "void toolBarWidget::closeDir(int page, int index)void toolBarWidget::closeDir(int page, int index)";

//    emit toolCloseDir(page, index);

//#if 0
//    _distance = _dirList.at(_dirId)->getHeight();

//    int x = page * _desktopRect.width();
//    int y = - 1 * _distance;
//    int w = _desktopRect.width();
//    int upH = _desktopRect.height() - _height;
////    int downH = _desktopRect.height() - upH;
//    int downH = gridHeight;

//    int mx = _gridTable[page][index].x() + (gridWidth - SMALLSLIDERWIDTH) / 2 + x; // 39
//    int my = y - 15;              //_gridTable[page][index].y() + gridHeight + 249
//    int mw = 38;                       //gridWidth - 40
//    int mh = 15;                       //40

//    emit closeMinWidget(mx, my, mw, mh, _upDistance);
//    emit desktopCloseMove(x, upH, w, downH, _upDistance);
//    emit upBackMove(0, -_distance, x + w, upH, _distance);
//    emit upMinBackMove(mx, upH - _distance - 15, mw, mh, _distance);
////    if (_upDistance > 0)
////        emit desktopBgBack(gridHeight);
//#endif
//    _openDir = false;
//    _animationScreenDown = false;

////    setContextMenuPolicy(Qt::DefaultContextMenu);
//}

void toolBarWidget::setDirHide()
{
//    _dirList.at(_dirId)->setVisible(false);
}

void toolBarWidget::dirMovingFinished()
{
    _dirMovingFinished = true;
}

void toolBarWidget::addDesktopApp(const QString &text, const QString &pix, const QString &url, int type)
{
    /*
    _url = url;
    qDebug() << _url;

    if (toolBarWidget::localIcon == type)
    {
            LocalApp *app = new LocalApp();
            app->setName(text);
            app->setIcon(pix);
            app->setType(QString("%1").arg(type));
            app->setUrl(url);
            app->setPage(_count - 1);
            app->setIndex(_nextIdx[_count - 1]);
            app->setDirId(-1);
            app->setId("111");

            for (int i = 0; i < g_RemotelocalList.count(); i++)
            {
                if(g_RemotelocalList.at(i).name == text)
                {
                    qDebug() << "text" << text;
                    app->setExecname(g_RemotelocalList.at(i).execname);
                    qDebug() << "***************execname*******************" << g_RemotelocalList.at(i).execname;
                    break;
                }
            }

            if (LocalAppList::getList()->getAppByName(app->name())) {
                QApplication::processEvents();
                AppMessageBox box(false, NULL);
                box.setText("    已添加该图标");
                box.exec();
            } else {
                LocalAppList::getList()->addApp(app);
            }

    }
    else if (toolBarWidget::vacIcon == type)
    {
            LocalApp *RemoteApp = new LocalApp();

            for (int i = 0; i < g_myVappList.count(); i++)
            {
                if(g_myVappList.at(i).name == text)
                {
                    RemoteApp->setName(g_myVappList[i].name);
                    RemoteApp->setId(g_myVappList[i].id);
                    RemoteApp->setIcon(pix);
                    RemoteApp->setPage(_count - 1);
                    RemoteApp->setIndex(_nextIdx[_count - 1]);
                    RemoteApp->setType(QString("%1").arg(type));
                    RemoteApp->setIsRemote(true);
                    RemoteApp->setUrl(url);
                    RemoteApp->setDirId(-1);
                    RemoteApp->setId("111");
                    break;
                }
            }

            LocalAppList::getList()->addRemoteApp(RemoteApp);
            addIcon(RemoteApp->name(), RemoteApp->icon()
                    , RemoteApp->page(), RemoteApp->index()
                    , RemoteApp->url(), RemoteApp->type().toInt());

    }
    else if (toolBarWidget::paasIcon == type)
    {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(_count - 1);
        app->setIndex(_nextIdx[_count - 1]);
        app->setExecname(url);
        app->setDirId(-1);
        app->setId("111");


        if (LocalAppList::getList()->getAppByName(app->name())) {
            QApplication::processEvents();
            AppMessageBox box(false, NULL);
            box.setText("    已添加该图标");
            box.exec();
        } else {
            LocalAppList::getList()->addApp(app);
        }
    }
    else if (toolBarWidget::dirIcon == type)
    {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(_count - 1);
        app->setIndex(_nextIdx[_count - 1]);
        app->setExecname("");
        app->setDirId(-1);
        app->setId(QString("%1").arg(_dirMinList.count()));

        LocalAppList::getList()->addApp(app);

    }

*/
//    addIcon(text, pix, - 1, -1, localIcon);

//    LocalApp *app = new LocalApp();
//    app->setName("/" + info.baseName());
//    app->setIcon(newApp);
//    app->setExecname(appPath);
//    if (LocalAppList::getList()->getAppByName(app->name())) {
//        QApplication::processEvents();
//        AppMessageBox box(false, NULL);
//        box.setText("    已添加该图标");
//        box.exec();
//    } else {
//        addApp(app);
//    }

//    AppDataReadExe::Instance()->addLocalApp(path);

}

void toolBarWidget::hideDirWidget()
{
//    if (_openDir)
//    {
//        closeDir(_dirPage, _dirIndex);
//        return;
//    }
}

void toolBarWidget::setIconEnabled(bool enabled)
{
//    for (int i = 0; i < _count; i++)
//    {
//        for(int j = 0; j < _nextIdx[i]; j++)
//        {
//            if ((i == _dirPage) && (j == _dirIndex))
//                continue;

//            _iconTable[i][j]->setEnabled(enabled);
//            if(enabled)
//                _iconTable[i][j]->setNormalPixmap();
//            else
//                _iconTable[i][j]->setGrayPixmap();
//        }
//    }
}
#if 0
void toolBarWidget::menuChanged(int value)
{
    _menuValue = value;

    switch(_menuValue)
    {
    case 0 :
        if (_createMenu->isVisible())
            _createMenu->setVisible(false);

        _showIconMenu->move(_mousePos.x() + 162 - 25, _mousePos.y());
        _showIconMenu->raise();
        _showIconMenu->setVisible(true);

        break;

    default:
        if (_createMenu->isVisible())
            _createMenu->setVisible(false);

        if (_showIconMenu->isVisible())
            _showIconMenu->setVisible(false);
        break;
    }

    //    case 2 :
    //        if (_showIconMenu->isVisible())
    //            _showIconMenu->setVisible(false);

    //        _createMenu->move(_mousePos.x() + 162 - 25, _mousePos.y() + 20 + 19 + 19 - 10);
    //        _createMenu->raise();
    //        _createMenu->setVisible(true);

    //        break;
}


void toolBarWidget::hideMenuWidget()
{
    if (_normalMenu->isVisible())
        _normalMenu->setVisible(false);

    if (_createMenu->isVisible())
        _createMenu->setVisible(false);

    if (_showIconMenu->isVisible())
        _showIconMenu->setVisible(false);

    if (_iconMenu->isVisible())
        _iconMenu->setVisible(false);
}
#endif
void toolBarWidget::setLargeIcon()
{
//    hideMenuWidget();

//    LocalAppList::getList()->save();

    _iconSize = IconItem::large_size;
    ICON_TYPE = _iconSize;

//    for (int i = 0; i < _dirList.count(); i++)
//    {
//        _dirList.at(i)->largeIcon();
//    }


    refresh(LARGESIZE);

    emit largeIcon();

//    for (int i = 0; i < _local->count(); i++)
//    {
//        addIcon(_local->at(i)->name(), _local->at(i)->icon()
//                , _local->at(i)->page(),_local->at(i)->index()
//                , _local->at(i)->url(), _local->at(i)->type().toInt());
//    }
    initIconItem();
}

void toolBarWidget::setMediumIcon()
{
//    hideMenuWidget();

//    LocalAppList::getList()->save();

    _iconSize = IconItem::medium_size;
    ICON_TYPE = _iconSize;
//    for (int i = 0; i < _dirList.count(); i++)
//    {
//        _dirList.at(i)->mediumIcon();
//    }

    refresh(MEDIUMSIZE);

    emit mediumIcon();

//    for (int i = 0; i < _local->count(); i++)
//    {
//        addIcon(_local->at(i)->name(), _local->at(i)->icon()
//                , _local->at(i)->page(),_local->at(i)->index()
//                , _local->at(i)->url(), _local->at(i)->type().toInt());
//    }

    initIconItem();
}

void toolBarWidget::setSmallIcon()
{
//    hideMenuWidget();

//    LocalAppList::getList()->save();

    _iconSize = IconItem::small_size;
    ICON_TYPE = _iconSize;

//    for (int i = 0; i < _dirList.count(); i++)
//    {
//        _dirList.at(i)->smallIcon();
//    }

    refresh(SMALLSIZE);

    emit smallIcon();

//    for (int i = 0; i < _local->count(); i++)
//    {
//        addIcon(_local->at(i)->name(), _local->at(i)->icon()
//                , _local->at(i)->page(),_local->at(i)->index()
//                , _local->at(i)->url(), _local->at(i)->type().toInt());
//    }

    initIconItem();
}

void toolBarWidget::refresh(QSize size)
{
    movetoFirst();
    deleteAllIconItem();

//    for(int i = _count; i > 1; i--)
//    {
//        delPage(i-1);
//    }

    _pages.clear();
    _iconTable.clear();
    _nextIdx.clear();
    _iconDict.clear();
    _gridTable.clear();
    //_dirList.clear();
    _dirMinList.clear();

    reloadApplist(size);

//    getLocalIcon();
//    getVacIcon();
//    getPaasIcon();
}

void toolBarWidget::movetoFirst()
{
    _current = 0;
    move(_pages[_current], y());
//    emit pageChanged(_current);

}

void toolBarWidget::reloadApplist(QSize size)
{
    _count = 1;

//    gridWidth = size.width() + HSPACING * 2;
//    gridHeight = size.height() + BOTTOMSPACING;

    changeSpacing();

    _col = (_width - _iconHSpacing) / gridWidth;
    _row = (_height - _iconVSpacing) / gridHeight;
    _iconsPerPage = _col * _row;
    _current  = 0;

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _height));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {

            int x = (j % _col) * gridWidth +  _iconHSpacing;
            int y = _pageSize.height() * i \
                    + (j / _col) * gridHeight + _iconVSpacing;

            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);

    }

    for (int i = 0; i < _count; i++) {
        QList<IconItem*> newList;
        for (int j = 0; j < _col * _row; j++)
            newList.insert(j, NULL);
        _iconTable.insert(i, newList);
    }

    for (int i = 0; i < _count; i++)
        _nextIdx.insert(i, 0);

    _localIconNum= 0;

//    for(int i = 0; i < _iconLists.count(); i++)
//    {
//        _tempLists[i] = _iconLists[i];
////        _iconLists[i] = NULL;
////        delete _iconLists[i];
//    }

//    _iconLists.clear();

//    QString name;

//    for(int i = 0; i < _tempLists.count(); i++)
//    {
//        qDebug() << _tempLists.count();
//        qDebug() << _tempLists[i]->text();
//        qDebug() << _tempLists[i]->url();
        //_url = _iconLists.at(i)->url();

//        if (_iconLists[i]->type() == toolBarWidget::dirIcon)
//        {
//            name = _iconLists[i]->getDirText();
//        }
//        else
//            name = _iconLists[i]->text();

//        addIcon(name, _iconLists[i]->pix()
//                , _iconLists[i]->page(), _iconLists[i]->index()
//                , _iconLists[i]->type());

//    }

    qDebug()<<"reload all.";
}

void toolBarWidget::deleteAllIconItem()
{
//    _local->save();

    for (int i=0;i<_count;i++)
    {
        for(int j=0;j<_iconsPerPage;j++) // also you can use _nextIdx[i]
        {
            if(_iconTable[i][j]!=NULL)
            {
                delete _iconTable[i][j];
                _iconTable[i][j]=NULL;
            }
        }
        _nextIdx[i]=0;
    }
}

void toolBarWidget::changeSpacing()
{
    switch(_iconSize)
    {
        case IconItem::large_size :

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = LARGESIZE.width() + HSPACING * 2;
            gridHeight = LARGESIZE.height() + VSPACING + BOTTOMSPACING;

            break;

        case IconItem::medium_size :

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = MEDIUMSIZE.width() + HSPACING * 2;
            gridHeight = MEDIUMSIZE.height() + BOTTOMSPACING;
            break;

        default:

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = SMALLSIZE.width() + HSPACING * 2;
            gridHeight = SMALLSIZE.height() + BOTTOMSPACING;
            break;
    }
}
#if 0
void toolBarWidget::showIconContextMenu(QPoint pos, QPoint mPos, const QString &text)
{
    Q_UNUSED(pos);
    _currentIconItem = text;

    if ( _iconDict.value(_currentIconItem)->type() == toolBarWidget::dirIcon)
    {
        if (_iconDict.value(_currentIconItem)->getMinIconNum() != 0)
        {
            _iconMenu->_delBtn->setEnabled(false);
        }
        else
            _iconMenu->_delBtn->setEnabled(true);
    }
    else
    {
        _iconMenu->_delBtn->setEnabled(true);
    }

    _iconMenu->move(mPos);
    _iconMenu->raise();
    _iconMenu->setVisible(true);
}

void toolBarWidget::iconMenuRunClicked()
{
    _iconMenu->setVisible(false);

    if (_iconDict.value(_currentIconItem)->type() == toolBarWidget::dirIcon)
    {
//        IconItem *iconItem = _iconDict.value(_currentIconItem);
        openDir(_iconDict.value(_currentIconItem)->id(), _iconDict.value(_currentIconItem)->page(), _iconDict.value(_currentIconItem)->index());
//        iconItem = NULL;
    }
    else
    {
        runApp(_currentIconItem);
    }
}

void toolBarWidget::iconMenuDelClicked()
{
    _iconMenu->setVisible(false);
 //   qDebug() << _currentIconItem;
//    delIcon(_currentIconItem);
//    qDebug() << "    if (_iconDict.value(_currentIconItem)->getMinIconNum() != 0)" << _iconDict.value(_currentIconItem)->getMinIconNum();
//    if (_iconDict.value(_currentIconItem)->getMinIconNum() != 0)
//    {
//        return;
//    }

    if ( _iconDict.value(_currentIconItem)->type() == toolBarWidget::dirIcon)
    {
        int index = _iconDict.value(_currentIconItem)->id();

        QList<DirShowWidget*>::iterator iter = _dirList.begin() + index;
        _dirList.at(index)->setParent(NULL);
        _dirList.at(index)->deleteLater();
        _dirList.erase(iter);

        _dirMinList.removeAt(index);

        for (int i = index; i < _dirList.count(); i++)
        {
            _dirList.at(i)->setId(i);

            _iconDict.value(_dirMinList.at(i))->setId(i);

            for (int j = 0; j < _local->count(); j++) {
                if (_local->at(j)->hidden())
                    continue;
                if (_local->at(j)->dirId() == i + 1)
                {
                    _local->at(j)->setDirId(i);
                }
            }
        }
    }

    LocalAppList::getList()->delApp(_currentIconItem);

    emit desktopDelIcon(_currentIconItem);
}
#endif
void toolBarWidget::refreshMenu()
{
    switch(ICON_TYPE)
    {
        case IconItem::large_size :
            setLargeIcon();
            break;

        case IconItem::medium_size :
            setMediumIcon();
            break;

        default:
            setSmallIcon();
            break;
    }
}

void toolBarWidget::dirWidgetDelIcon(int id, const QString &text)
{
    qDebug() <<"toolBarWidget::dirWidgetDelIcon"<<id;
    _iconDict.value(_dirMinList.at(id))->removeDirMinItem(text);

    emit desktopDelIcon(text);
}

void toolBarWidget::initIconItem()
{
    _localIconNum = 0;

    for (int i = 0; i < _local->count(); i++) { //_localIconNum
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == -2)                                       //dushbin id is -2
        {

            if(_local->at(i)->type().toInt() == localIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 0,
                        _local->at(i)->id().toInt());
            }
            else if (_local->at(i)->type().toInt() == vacIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 1, _local->at(i)->id().toInt());
            }
            else if (_local->at(i)->type().toInt() == paasIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 2, _local->at(i)->id().toInt());
            }
            else if (_local->at(i)->type().toInt() == dirIcon)
            {
                addIcon(_local->at(i)->name(), "",
                        _local->at(i)->page(), _local->at(i)->index(),
                        "", 3, _local->at(i)->id().toInt());
            }
            else
            {
                qDebug() << "ajsdfjaslkdjflasj;flkajsldkfjal;skdfjalsjdf;alsdf";
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        "", 4, _local->at(i)->id().toInt());
            }
        }
    }

//    for (int i = 0; i < _dirMinList.count(); i++)
//    {
//        for (int j = 0; j < _local->count(); j++) {
//            if (_local->at(j)->hidden())
//                continue;

//            if (_local->at(j)->dirId() == 1000)
//                continue;

//            if (_local->at(j)->dirId() == _iconDict.value(_dirMinList.at(i))->id())
//            {
//                _iconDict.value(_dirMinList.at(_local->at(j)->dirId()))->addDirMinItem(_local->at(j)->name(), _local->at(j)->icon(),
//                                                                                       _local->at(j)->page(), _local->at(j)->index(),
//                                                                                       _local->at(j)->url());
//            }
//        }
//    }
}

void toolBarWidget::setShowType()
{

//    _pages.clear();
//    _iconTable.clear();
//    _nextIdx.clear();
//    _iconDict.clear();
    _gridTable.clear();

    _iconsPerPage = _localIconNum;
    _current = 0;

//    for (int i = 0; i < _count; i++)
//        _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _iconsPerPage; j++) {

            int x = (_width - gridWidth * _iconsPerPage) / 2 + j * gridWidth;
            int y = _height - gridHeight;

            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);

        for (int i = 0; i < _gridTable.count() ; i++)
        {
            qDebug() << _gridTable.at(i);
        }
    }

    for (int i = 0; i < _count; i++) {
        QList<IconItem*> newList;
        for (int j = 0; j < _iconsPerPage; j++)
            newList.insert(j, NULL);
        _iconTable.insert(i, newList);
    }

    for (int i = 0; i < _iconItemLists.count(); i++)
    {
        _iconItemLists.at(i)->setGeometry(_gridTable[0][i].translated(HSPACING, VSPACING));
        qDebug() << _iconItemLists.at(i)->pos().x() << _iconItemLists.at(i)->pos().y();
        _iconTable[0][i] = _iconItemLists.at(i);

    }


     repaint();

     _refreshToolBar = false;
}

void toolBarWidget::moveOutIcon(const QString &text)
{
//    _refreshToolBar = true;

    if (!_iconDict.value(text))
        return;

    if ( _iconDict.value(text)->type() == dirIcon)
    {
        _dirMinList.removeOne(text);
    }

    for (int i = 0; i < _iconItemLists.count(); i++)
    {

        if (_iconItemLists.at(i)->text() == text)
        {
            QList<IconItem*>::iterator iter = _iconItemLists.begin() + i;
            _iconItemLists.at(i)->setParent(NULL);
            _iconItemLists.at(i)->deleteLater();
            _iconItemLists.erase(iter);
            break;
        }
    }

    int p = _iconDict.value(text)->page();
    int s = _iconDict.value(text)->index();

    _iconDict.take(text);

    _iconTable[p][s] = NULL;

    moveBackIcons(p, s);

}

void toolBarWidget::toolBarRemoveDirMinItem(const QString &text, int dirId)
{
    for (int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->id().toInt() == dirId)
        {
            if (_local->at(i)->dirId() == -2)
            {
                if (!_iconDict.value(_local->at(i)->name()))
                    break;

                _iconDict.value(_local->at(i)->name())->removeDirMinItem(text);
            }

            break;
        }
    }
}
