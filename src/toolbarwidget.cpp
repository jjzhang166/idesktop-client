#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include "strings.h"

using namespace std;

#include <QtGui>

#include "idesktopsettings.h"
#include "toolBarWidget.h"
#include "localapps.h"
#include "movingdialog.h"
#include "appmessagebox.h"
#include "config.h"
#include "dirwidget.h"
//#include "movewidget.h"
#include "paascommuinication.h"
#include "toolbarwidget.h"
#include "runapp.h"

#define ICONWIDTH 143
#define ICONHEIGHT 143

#define SPACING 30
#define FONTSIZE 10

#define ICONITEMWIDTH 143
#define ICONITEMHEIGHT 143

#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30
#define SMALLSLIDERWIDTH 38

#define APPICON 0
#define ADDICON 1
#define HSPACING 4
#define VSPACING 8                  //
#define BOTTOMSPACING 16            //24

#define LARGESIZE QSize(143, 143)     //72*72
#define MEDIUMSIZE QSize(119, 119)     //48*48
#define SMALLSIZE QSize(103, 103)   //32*32

#define ICONHSPACING (88 - 40)
#define ICONVSPACING (88 - 36)

extern QString WIN_VAPP_IconPath;

extern QList<TEMP_LIST> dirWidget_FirstLists;

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
    , _dirId(1000)
    , _upDistance(0)
    , _distance(0)
    , _openDir(false)
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
    _height = SMALLSIZE.height() + VSPACING + BOTTOMSPACING;

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

//    initIconItem();

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

    _iconMenu = new MenuWidget(MenuWidget::iconMenu, this);
    _iconMenu->setVisible(false);

//    connect(_iconMenu, SIGNAL(run()), this, SLOT(iconMenuRunClicked()));
//    connect(_iconMenu, SIGNAL(del()), this, SLOT(iconMenuDelClicked()));

//    connect(dragLeftTimer, SIGNAL(timeout()), this, SLOT(dragLeft()));
//    connect(dragRightTimer, SIGNAL(timeout()), this, SLOT(dragRight()));

//    connect(_local, SIGNAL(appAdded(const QString&, const QString&, const QString &, int))
//            , this, SLOT(addIcon(const QString&, const QString&, const QString &, int)));
    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));
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
                                 int index, const QString &url, int type,
                                 const QString &uniqueName)
{
    _iconDragEnter = false;

    //_isDirWidgetObject = false;

    if (_inDrag)
    {
        moveOutIcon(uniqueName);

        _inDrag = NULL;
    }


    if (id == 1000)
    {
        emit toolBarIconToDustbin(text, iconPath, -1, index, url, type, uniqueName);
    }
    else
    {
        emit toolBarIconToDir(id, text, iconPath, -1, index, url, type, uniqueName);
    }

    emit iconDropToolWidget(uniqueName);

}

void toolBarWidget::updateClicked()
{
//    _itemClicked = false;
}

IconItem * toolBarWidget::getIconByName(const QString &uniqueName)
{
    return _iconDict.value(uniqueName, NULL);
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

    _refreshToolBar = false;

    if (_dragItem)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;

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
//                if (_iconDict.value(_dirMinList.at(i)) == _inDrag)
//                    continue;

                _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(false);
            }
        }
        _dragItem = NULL;

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {

        if (_localIconNum == _MaxNum)
        {
             event->ignore();
            return;
        }

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
                , icon->id(), icon->uniqueName());

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
//    event->setDropAction(Qt::MoveAction);
//    event->accept();
//    return;

    if (_refreshToolBar)
        return;

    if (!_isIconMove)
        return;
    if (_iconDragEnter)
        return;

    if (!_inDrag)
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
//        qDebug() << "move in this page start";
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

//                qDebug() << "move in this page end";
    }

}

void toolBarWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    _refreshToolBar = true;

    _iconDragEnter = false;

    qDebug() << "toolBarWidget*********************************LeaveEvent";
    _itemHeld = false;

#if 1
    if (_dragItem != NULL)
    {
        if (!_inDrag)
            return;

        moveOutIcon(_inDrag->uniqueName());

        _inDrag = NULL;

//        _iconTable[p][s] = NULL;

//        moveBackIcons(p, s);

        _dragItem = NULL;

        qDebug() << "toolBarWidget-------------------------->LeaveEvent";

        _isOtherWidgetObject = false;
    }
#endif
//    _isOtherWidgetObject = false;

    _dragEvent = false;


    Q_UNUSED(event);
}

void toolBarWidget::dropEvent(QDropEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
    _itemHeld = false;

    if (!_inDrag)
        return;
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

    _inDrag->show();
    QString dropText = _inDrag->uniqueName();

    _inDrag=NULL;

    if (_dragItem != NULL)
    {
        if (_dragItem->type() == dirIcon)
        {
            if (dirWidget_FirstLists.count() != 0)
            {

                for (int i = 0; i < dirWidget_FirstLists.count(); i++)
                {
                     _iconDict.value(_dragItem->uniqueName())->addDirMinItem(dirWidget_FirstLists.at(i).name
                                                                            , dirWidget_FirstLists.at(i).iconPath
                                                                            , -1, -1
                                                                            , dirWidget_FirstLists.at(i).url
                                                                            , dirWidget_FirstLists.at(i).uniqueName);
                }
                dirWidget_FirstLists.clear();
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

void toolBarWidget::iconItemDragOut()
{
    _itemHeld = false;

    if (!_inDrag)
        return;

    dragLeftTimer->stop();
    dragRightTimer->stop();
    int p = _inDrag->page();
    int s = _inDrag->index();

    QPoint end = _gridTable[p][s].translated(HSPACING, VSPACING).topLeft();

    _iconTable[p][s] = _inDrag;
    _inDrag->move(end);


    _inDrag->show();


    QString dropText = _inDrag->uniqueName();

    _inDrag=NULL;

    if (_dragItem != NULL)
    {
        if (_dragItem->type() == dirIcon)
        {
            if (dirWidget_FirstLists.count() != 0)
            {

                for (int i = 0; i < dirWidget_FirstLists.count(); i++)
                {
                     _iconDict.value(_dragItem->uniqueName())->addDirMinItem(dirWidget_FirstLists.at(i).name
                                                                            , dirWidget_FirstLists.at(i).iconPath
                                                                            , -1, -1
                                                                            , dirWidget_FirstLists.at(i).url
                                                                            , dirWidget_FirstLists.at(i).uniqueName);
                }
                dirWidget_FirstLists.clear();
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
    painter.drawPixmap((_width - gridWidth * _iconsPerPage) / 2 - 40, _height - 70, 40, 70
                       , QPixmap(":images/toolbar_left.png").scaled(40, 70, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter.drawPixmap((_width - gridWidth * _iconsPerPage) / 2, _height - 70, gridWidth * _iconsPerPage , 70
                       , QPixmap(":images/toolbar_center.png").scaled(gridWidth * _iconsPerPage , 70, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter.drawPixmap((_width - gridWidth * _iconsPerPage) / 2 + _iconsPerPage * gridWidth, _height - 70, 40, 70
                       , QPixmap(":images/toolbar_right.png").scaled(40, 70, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

int toolBarWidget::addIcon(const QString &text,
                            const QString &iconPath,
                            int page,
                            int index,
                            const QString &url,
                            int type, int id, const QString &uniqueName)
{
    if (_localIconNum == _MaxNum)
    {
        setAcceptDrops(false);
        return 0;
    }

    _localIconNum ++;

    setShowType();

    qDebug() << "addIcon";

    IconItem *icon = new IconItem(this);
    icon->setSmallSize();
    icon->setSaveDataBool(true);
    icon->setIconSize(2);
    icon->setUniqueName(uniqueName);
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

    icon->setIconClass(type);

    if(type == dirIcon)
    {
        icon->addMinWidget(2);  //  0 : largeSize, 1 : mediumSize, 2 : smallSize;
        icon->setId(id);
        icon->setDirMinItemId(id);
//        emit toolBarDirWidgetRefresh(id);
    }
    else if (type == 4)
    {
        icon->addDustbin();
        icon->setId(1000);
        //emit toolBarAddDirSW(_toolBarDirIndex);


    }
    else
    {
        icon->setId(111);
    }


    /* TODO deal with the icon is full */
    if (page == -1) {
        for (int i = 0; i < _count; i++) {

            if (_nextIdx[i] < _iconsPerPage) {

                page = i;
                index = _nextIdx[i];
//                qDebug() << "toolBarWidget::addIcon() --> setPage(page)" << page  << index;

                break;
            }
        }
    }
    else {
//        if (index == -1) {

//            if ((page > 0) && (_nextIdx[page - 1] == _iconsPerPage) && (_nextIdx[page] < _iconsPerPage)) {
//                index = _nextIdx[page];
//            }
//            else if ((page == 0) && (_nextIdx[page] < _iconsPerPage)) {

//                index = _nextIdx[page];
//            }
//            else {

//                for (int i = 0; i < _count; i++) {
//                    if (_nextIdx[i] < _iconsPerPage) {

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
    _iconDict.insert(uniqueName, icon);
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

        _dirMinList.append(uniqueName);

//        _toolBarDirIndex++;
        if (dirIcon == type)
        {
            emit toolBarDirWidgetRefresh(id);
        }
    }

        connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
        connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(delIcon(const QString&)));
        connect(icon, SIGNAL(iconEnter()), this, SLOT(iconDragEnter()));
        connect(icon, SIGNAL(iconMove()), this, SLOT(iconDragMove()));
        connect(icon, SIGNAL(iconLeave()), this, SLOT(iconDragLeave()));
        connect(icon, SIGNAL(iconDrop(int, const QString &, const QString &, int, int, const QString &, int, const QString&)),
                this, SLOT(iconDragDrop(int, const QString &, const QString &, int, int, const QString &, int, const QString&)));
        connect(icon, SIGNAL(openDir(int, int, int)), this, SLOT(openDir(int, int, int)));
        connect(icon, SIGNAL(dragEnterMinWidget()), this, SLOT(iconDragEnter()));
    //    connect(icon, SIGNAL(showContextMenu(QPoint, QPoint,const QString &))
    //            , this, SLOT(showIconContextMenu(QPoint, QPoint,const QString &)));

//        connect(icon, SIGNAL(showContextMenu(bool, QPoint, QPoint,const QString &))
//                , this, SLOT(showIconContextMenu(bool, QPoint, QPoint,const QString &)));
        connect(icon, SIGNAL(iconItemNameChanged(const QString &, const QString &))
                , this, SIGNAL(iconItemNameChanged(const QString &, const QString &)));
        connect(icon, SIGNAL(dirMinLineEditFocusOut(int, const QString &))
                , this, SIGNAL(changedDirWidgetTitle(int, const QString &)));
        connect(icon, SIGNAL(dragOut()), this, SLOT(iconItemDragOut()));
//        connect(icon, SIGNAL(showContextMenu(bool, QPoint, QPoint,IconItem *))
//                , this, SLOT(showIconContextMenu(bool, QPoint, QPoint, IconItem *)));

//    _iconLists.append(icon);

     return page;
}

//void toolBarWidget::test()
//{
//    emit toolBarAddDirSW(_toolBarDirIndex);

//}

int toolBarWidget::addIcon(const QString &text, const QString &icon, const QString &url, int type, int id, const QString &uniqueName)
{
    return addIcon(text, icon, _current, -1, url, type, id, uniqueName);
}

void toolBarWidget::delIcon(const QString &uniqueName)
{
    if (_iconDict.value(uniqueName) == NULL)
        return;

    IconItem *icon = _iconDict.take(uniqueName);

    for (int i = 0; i < _iconItemLists.count(); i++)
    {
        if (_iconItemLists.at(i)->uniqueName() == icon->uniqueName())
        {
            _iconItemLists.removeAt(i);
            break;
        }
    }

    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();

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
//                QRect start = _gridTable[p][k].translated(HSPACING, VSPACING);
//                QRect end = _gridTable[p][k-1].translated(HSPACING, VSPACING);
//                _iconTable[p][k]->animationMove(start, end);
                _iconTable[p][k]->setIndex(k-1);
                _iconTable[p][k - 1] = _iconTable[p][k];
            }
        }
        else {

            for (int j = 0; j < _nextIdx[i]; j++)
            {
                if (j == 0)
                {
//                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
//                    QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(HSPACING, VSPACING);
//                    _iconTable[i][j]->animationMove(start, end);
                    _iconTable[i][j]->setPage(i-1);
                    _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
                    _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
                }
                else {

//                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
//                    QRect end = _gridTable[i][j-1].translated(HSPACING, VSPACING);
//                    _iconTable[i][j]->animationMove(start, end);
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

    if (_localIconNum < _MaxNum)
    {
        setAcceptDrops(true);
    }

    setShowType();

}

void toolBarWidget::openDir(int id, int page, int index)
{
        int x = _gridTable[page][index].x();
        emit toolOpenDir(id , x, gridWidth);
}


void toolBarWidget::runApp(const QString &uniqueName)
{
    RunApp::getRunApp()->runApp(uniqueName);
}


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

void toolBarWidget::hideDirWidget()
{
//    if (_openDir)
//    {
//        closeDir(_dirPage, _dirIndex);
//        return;
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
    switch(IDesktopSettings::instance()->iconSize())
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

void toolBarWidget::dirWidgetDelIcon(int id, const QString &uniqueName)
{
    _iconDict.value(_dirMinList.at(id))->removeDirMinItem(uniqueName);

    emit desktopDelIcon(uniqueName);
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
                        _local->at(i)->id().toInt(), _local->at(i)->uniqueName());
            }
            else if (_local->at(i)->type().toInt() == vacIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 1,
                        _local->at(i)->id().toInt(), _local->at(i)->uniqueName());
            }
            else if (_local->at(i)->type().toInt() == paasIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 2,
                        _local->at(i)->id().toInt(), _local->at(i)->uniqueName());
            }
            else if (_local->at(i)->type().toInt() == dirIcon)
            {
                addIcon(_local->at(i)->name(), "",
                        _local->at(i)->page(), _local->at(i)->index(),
                        "", 3,
                        _local->at(i)->id().toInt(), _local->at(i)->uniqueName());

            }
            else
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        "", 4,
                        _local->at(i)->id().toInt(), _local->at(i)->uniqueName());
            }
        }
    }
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

    for (int i = 0; i < _iconTable.count(); i++)
    {
        for (int j = 0; j < _iconItemLists.count(); j++)
        {
            if (_iconItemLists.at(j)->index() == i)
            {
                _iconItemLists.at(j)->setGeometry(_gridTable[0][i].translated(HSPACING, VSPACING));
                _iconTable[0][i] = _iconItemLists.at(j);
            }
        }
    }


     repaint();

//     _refreshToolBar = false;
}

void toolBarWidget::moveOutIcon(const QString &uniqueName)
{
//    _refreshToolBar = true;

    if (!_iconDict.value(uniqueName))
        return;

    if ( _iconDict.value(uniqueName)->type() == dirIcon)
    {
        _dirMinList.removeOne(uniqueName);
    }

    for (int i = 0; i < _iconItemLists.count(); i++)
    {

        if (_iconItemLists.at(i)->uniqueName() == uniqueName)
        {
            QList<IconItem*>::iterator iter = _iconItemLists.begin() + i;
            _iconItemLists.at(i)->setParent(NULL);
            _iconItemLists.at(i)->deleteLater();
            _iconItemLists.erase(iter);
            break;
        }
    }

    int p = _iconDict.value(uniqueName)->page();
    int s = _iconDict.value(uniqueName)->index();

    _iconDict.take(uniqueName);

    _iconTable[p][s] = NULL;

    moveBackIcons(p, s);

}

void toolBarWidget::toolBarRemoveDirMinItem(const QString &uniqueName, int dirId)
{
    for (int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->id().toInt() == dirId)
        {
            if (_local->at(i)->dirId() == -2)
            {
                if (!_iconDict.value(_local->at(i)->uniqueName()))
                    break;

//                _iconDict.value(_local->at(i)->uniqueName())->removeDirMinItem(uniqueName);
                _iconDict.value(_local->at(i)->uniqueName())->refreshDirMinWidgetIcon();
            }

            break;
        }
    }
}
void toolBarWidget::toolBarRefreshDirMinWidget(const QString &uniqueName)
{
    if (!_iconDict.value(uniqueName))
        return;
    _iconDict.value(uniqueName)->refreshDirMinWidgetIcon();

}

//void toolBarWidget::showIconContextMenu(bool visiable, QPoint pos, QPoint mPos, IconItem *iconItem)
//{
//    Q_UNUSED(pos);

//    if (!iconItem)
//        return;

//    _currentUniqueName = iconItem->uniqueName();

//    if (!_iconDict.value(_currentUniqueName))
//        return;

//    if ( _iconDict.value(_currentUniqueName)->type() == dirIcon)
//    {
//        if (_iconDict.value(_currentUniqueName)->getMinIconNum() != 0)
//        {
//            _iconMenu->_delBtn->setEnabled(false);
//        }
//        else
//            _iconMenu->_delBtn->setEnabled(true);
//    }
//    else
//    {
//        _iconMenu->_delBtn->setEnabled(true);
//    }

//    _iconMenu->move(mPos.x(), mPos.y());
//    _iconMenu->raise();
//    _iconMenu->setVisible(visiable);
//}

//void toolBarWidget::iconMenuRunClicked()
//{
//    _iconMenu->setVisible(false);

//    if (_iconDict.value(_currentUniqueName)->type() == dirIcon)
//    {

//    }
//    else
//    {
//        if(QItemManager::getManager()->getItemListSize() == 0)
//            runApp(_currentUniqueName);
//        else
//        {
//            for(int i = 0; i < QItemManager::getManager()->getItemListSize(); i++)
//            {
//                  runApp(QItemManager::getManager()->itemAt(i)->text());
//            }
//        }
//    }
//}

//void toolBarWidget::iconMenuDelClicked()
//{
//    _iconMenu->setVisible(false);

//    if(QItemManager::getManager()->getSelectIconItemText().size() == 0)
//    {
//        if ( _iconDict.value(_currentUniqueName)->type() == dirIcon)
//        {
//            int index = _iconDict.value(_currentUniqueName)->id();

//            emit delDirShowWidget(index);

//            _dirMinList.removeOne(_iconDict.value(_currentUniqueName)->uniqueName());

//            LocalAppList::getList()->delApp(_currentUniqueName);

////            emit desktopDelIcon(_currentUniqueName);

//        }
//        else
//        {
//            LocalAppList::getList()->delApp(_currentUniqueName);

////            emit desktopDelIcon(_currentUniqueName);
//        }
//    }
//    else
//    {
//        QList<QString> list = QItemManager::getManager()->getSelectIconItemText();
//        for(int i = 0; i < list.size(); i++)
//        {
//            _currentUniqueName = list.at(i);

//            if ( _iconDict.value(_currentUniqueName)->type() == dirIcon)
//            {
//                int index = _iconDict.value(_currentUniqueName)->id();


//                _dirMinList.removeOne(_iconDict.value(_currentUniqueName)->uniqueName());

//                QItemManager::getManager()->removeByUniqueName(_currentUniqueName);
//                LocalAppList::getList()->delApp(_currentUniqueName);

////                emit desktopDelIcon(_currentUniqueName);

//            }
//            else
//            {
//                QItemManager::getManager()->removeByUniqueName(_currentUniqueName);
//                LocalAppList::getList()->delApp(_currentUniqueName);

////                emit desktopDelIcon(_currentUniqueName);
//            }

//        }
//    }
//}

void toolBarWidget::changedDirId(const QString &uniqueName, int index)
{
    if (_iconDict.value(uniqueName))
    {
        _iconDict.value(uniqueName)->setId(index);
    }
}
