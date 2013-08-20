#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include "strings.h"
#include "qitemmanager.h"
using namespace std;

#include <QtGui>
#include <QUuid>

#include "idesktopsettings.h"
#include "virtualdesktop.h"
#include "localapps.h"
#include "movingdialog.h"
#include "appmessagebox.h"
#include "config.h"
#include "dirwidget.h"
//#include "movewidget.h"
#include "paascommuinication.h"
#include "runapp.h"

//#define INDICATOR_ITEMSIZE QSize(14, 14)
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

#define KEY "\\Windows\\CurrentVersion\\App Paths\\"
extern QString WIN_VAPP_IconPath;
extern QList<APP_LIST> g_myVappList;
extern QList<LOCAL_LIST> g_RemotelocalList;
extern QList<APP_LIST> g_RemoteappList;
extern QList<PAAS_LIST> g_RemotepaasList;

extern QList<TEMP_LIST> dirWidget_FirstLists;

static QPoint gap;

/* ########################################################
 * definition of VirtualDesktop methods
 */
VirtualDesktop::VirtualDesktop(QSize pageSize,  QWidget *parent)
    : QWidget(parent)
    , _movingDialog(NULL)
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
//    , _iconNum(-1)
    , _upDistance(0)
    , _distance(0)
    , _openDir(false)
    , _dragEnterMinWidget(true)
    , _isIconMove(true)
    , _isDirWidgetObject(false)
    , _dragItem(NULL)
    , _localIconNum(0)
    , _dirMovingFinished(true)
    , _inDrag(NULL)
    , _openToolBar(false)
    , _dirShowWidget(NULL)
    , _restoreIcon(NULL)

{
    _settings = IDesktopSettings::instance();
    setFocusPolicy(Qt::ClickFocus);

    _count = 1;

#ifdef Q_WS_WIN
    mylib= new QLibrary("DllClientEngineMain.dll");   //
    if (mylib->load())              //check whether load successfully
    {
        //QMessageBox::information(NULL,"OK","DLL load is OK!");

        m_dllInitClass=(Dll_InitClass)mylib->resolve("DllCEM_InitClass");    //get function add()
        m_dllCloseClass=(Dll_CloseClass)mylib->resolve("DllCEM_CloseClass");
        m_dllCloseAppAll=(Dll_CloseAppAll)mylib->resolve("DllCEM_closeAppAll");
        m_dllStartAppEx=(Dll_StartAppEx)mylib->resolve("DllCEM_startAppEx");
    }
    else
        QMessageBox::information(NULL,"NO","DllClientEngineMain is not loaded!");
#endif

    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = _pageSize.height();
    changeSpacing();

    _col = (_width - ICONHSPACING) / gridWidth;
    _row = (_height - ICONVSPACING - (SMALLSIZE.height() + VSPACING + BOTTOMSPACING)) / gridHeight;

    _current  = 0;

    _local = LocalAppList::getList();

    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == -1)
        {
            _localIconNum ++;
        }
    }

    _count = qMax(_localIconNum / iconsPerPage(), 1);

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            //horizontal display
            int x =  _pageSize.width() * i \
                    + (j % _col) * gridWidth + ICONHSPACING;
            int y = (j / _col) * gridHeight + ICONVSPACING;

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

    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setEasingCurve(QEasingCurve::OutBack);

    setAcceptDrops(true);
    setAutoFillBackground(true);
    setFixedSize(_width * _count, _height);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255,0));
    setPalette(pal);

    _desktopWidget = QApplication::desktop();
    _desktopRect = _desktopWidget->availableGeometry();
    toolBarAddDirShowWidget();

    initIconItem();

    dragLeftTimer = new QTimer(this);
    dragLeftTimer->setInterval(500);
    dragLeftTimer->setSingleShot(true);

    dragRightTimer = new QTimer(this);
    dragRightTimer->setInterval(500);
    dragRightTimer->setSingleShot(true);

    _normalMenu = new MenuWidget(MenuWidget::normal, this);
    _normalMenu->setVisible(false);

    _showIconMenu = new MenuWidget(MenuWidget::showIcon, this);
    _showIconMenu->setVisible(false);

    _createMenu = new MenuWidget(MenuWidget::create, this);
    _createMenu->setVisible(false);

    _iconMenu = new MenuWidget(MenuWidget::iconMenu, this);
    _iconMenu->setVisible(false);

//    connect(_iconMenu, SIGNAL(rename()), this, SLOT(rename));

    connect(_normalMenu, SIGNAL(menuChanged(int)), this, SLOT(menuChanged(int)));
    connect(_normalMenu, SIGNAL(createDir()), this, SLOT(addDirItem()));
    connect(_normalMenu, SIGNAL(refresh()), this, SLOT(refreshMenu()));
    connect(_showIconMenu, SIGNAL(largeIcon()), this, SLOT(setLargeIcon()));
    connect(_showIconMenu, SIGNAL(mediumIcon()), this, SLOT(setMediumIcon()));
    connect(_showIconMenu, SIGNAL(smallIcon()), this, SLOT(setSmallIcon()));
    connect(_createMenu, SIGNAL(createDir()),this, SLOT(addDirItem()));
    connect(_createMenu, SIGNAL(hideDesktop()), this, SIGNAL(hideDesktop()));
    connect(_createMenu, SIGNAL(addDesktopLink()), this, SLOT(appAdd()));
    connect(_iconMenu, SIGNAL(run()), this, SLOT(iconMenuRunClicked()));
    connect(_iconMenu, SIGNAL(del()), this, SLOT(iconMenuDelClicked()));

    connect(dragLeftTimer, SIGNAL(timeout()), this, SLOT(dragLeft()));
    connect(dragRightTimer, SIGNAL(timeout()), this, SLOT(dragRight()));

    connect(_local, SIGNAL(appAdded(const QString&, const QString&, const QString &, int, int, const QString &))
            , this, SLOT(addIcon(const QString&, const QString&, const QString &, int, int, const QString &)));
    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));
}

VirtualDesktop::~VirtualDesktop()
{
//    delete /;
#if 0
    for (int j = 0; j < _iconDict.size(); j++)
    {
        delete &_iconDict[0][j];
    }
#endif

    for (int i = 0; i < _dirShowWidgetList.count(); i++) {
        _dirShowWidgetList.at(i)->setParent(NULL);
        delete _dirShowWidgetList.at(i);
    }
    _dirShowWidgetList.clear();

    _dirMinList.clear();
}

void VirtualDesktop::iconDragEnter()
{
    _iconDragEnter = true;
}

void VirtualDesktop::iconDragMove()
{
    _iconDragEnter = true;
}

void VirtualDesktop::iconDragLeave()
{
    _iconDragEnter = false;
    _dragEnterMinWidget = false;
}

void VirtualDesktop::iconDragDrop(int id, const QString &text,
                                  const QString &iconPath, int page,
                                  int index, const QString &url,
                                  int type, const QString &uniqueName)
{
    _iconDragEnter = false;
    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        if (_dirShowWidgetList.at(i)->id() == id)
        {
            _dirShowWidgetList.at(i)->addDirIcon(text, iconPath, -1, index, url, type, uniqueName);
        }
    }
#if 1
    if (!_inDrag)
        return;

    int p = _inDrag->page();
    int s = _inDrag->index();
    _inDrag->hide();

    _iconDict.take(_inDrag->uniqueName());
    _inDrag = NULL;
    _iconTable[p][s]->deleteLater();
    _iconTable[p][s] = NULL;

    moveBackIcons(p, s);
#endif
    _isDirWidgetObject = false;
}

//void VirtualDesktop::arrangeWidgetMove(QPoint pos)
//{
//    _arrangeWidget->move(pos);
//}

void VirtualDesktop::updateClicked()
{
    _itemClicked = false;
}

void VirtualDesktop::itemHeld()
{
    if (!_itemHeld)
        return;
    _trembling = true;
    emit trembleStarted();
}

IconItem * VirtualDesktop::getIconByName(const QString &uniqueName)
{
    return _iconDict.value(uniqueName, NULL);
}

void VirtualDesktop::atExit()
{
    _local->save();
    m_dllCloseClass();
}

void VirtualDesktop::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(status);

    _inProgress = false;
}

void VirtualDesktop::uninstall(const QString &uniqueName)
{
    LocalApp *app = _local->getAppByUniqueName(uniqueName);
    if (app == NULL)
        return;
    if (uniqueName.startsWith("/")) {
        AppMessageBox box(true, NULL);
        box.setText("确定删除图标？");
        if (!box.exec())
            return;
        _local->delApp(uniqueName);
    } else {
        QString uninstCommand = LocalAppList::getList()->getUninstExec(app->uninstName());

        if (!uninstCommand.isEmpty()) {
            AppMessageBox box(true, NULL);
            box.setText("确定删除图标并调用卸载程序？");
            if (!box.exec())
                return;

            _local->delApp(uniqueName);
            QByteArray tmp = uninstCommand.toAscii();
            if (uninstCommand.startsWith("\""))
                tmp = uninstCommand.mid(1, uninstCommand.length() - 2).toAscii();

            /////////////////////////////////////
            STARTUPINFO si; //一些必备参数设置
            memset(&si, 0, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOW;
            PROCESS_INFORMATION pi; //必备参数设置结束

            CreateProcess(tmp.data(),NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
        }
        else {
            AppMessageBox box(true, NULL);
            box.setText("应用已经被卸载，是否删除图标？");
            if (!box.exec())
                return;
            _local->delApp(uniqueName);
        }
    }
}

void VirtualDesktop::moveItem(IconItem *item, int target)
{
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    /* move the items in the source page */
    //printDesktop();
    int i;
    int source = item->page();
    int srcI = item->index();
    for (i = srcI + 1; _iconTable[source][i] != NULL; i++) {
        QPoint end = _gridTable[source][i-1]\
                .translated(HSPACING, VSPACING).topLeft();
        _iconTable[source][i]->move(end);
        _iconTable[source][i]->setIndex(i-1);
        _iconTable[source][i - 1] = _iconTable[source][i];
        _iconTable[source][i-1]->setIndex(i-1);
    }
    _iconTable[source][i-1] = NULL;
    _nextIdx[source]--;
    /* The page is empty now, need to be deleted */
    if (_nextIdx[source] == 0) {
        for (int i = source; i < (_count - 1); i++) {
            for (int j = 0; j < _col * _row; j++) {
                _iconTable[i][j] = _iconTable[i+1][j];
                IconItem *icon = _iconTable[i][j];
                if (icon != NULL) {
                    icon->setPage(i);
                    icon->setIndex(j);
                    icon->setGeometry(_gridTable[i][j]\
                                      .translated(HSPACING, VSPACING));
                }
            }
        }
        _iconTable.removeLast();
        _gridTable.removeLast();
        _pages.removeLast();
        _nextIdx.removeAt(source);
        _count--;
        if (target > source)
            target--;
        setFixedSize( _count * _pageSize.width(), _pageSize.height());
        _current = target;
        move(_pages[_current], y());
        emit pageChanged(_current);
        emit pageDecreased();
    }
    item->setPage(target);
    item->setIndex(_nextIdx[target]);
    _iconTable[item->page()][item->index()] = item;
    QPoint dest = _gridTable[item->page()][item->index()]\
            .translated(HSPACING, VSPACING).topLeft();
    _nextIdx[target]++;
    item->move(dest);
    //move(_pages[_current], y());
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), \
                                    width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    //printDesktop();
    //_current = target;
    //move(_pages[target], y());
    //emit pageChanged(_current);
}

void VirtualDesktop::reload()
{
    emit reInitiate();
}

void VirtualDesktop::arrangeEqually(int pageCount)
{
    QList<IconItem*> seqList;
    for (int i = 0; i < _count; i++)
        for (int j = 0; j < _row * _col; j++)
            if (_iconTable[i][j]) {
                seqList.push_back(_iconTable[i][j]);
                _iconTable[i][j] = NULL;
            }
    int iconCount = seqList.count();
    if (iconCount < pageCount)
        pageCount = iconCount;

    _iconTable.clear();
    _gridTable.clear();
    _nextIdx.clear();
    _count = pageCount;
    _current = 0;
    move(_pages[0], y());
    _pages.clear();
    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

//    gridWidth = ICONWIDTH + SPACING * 2;
//    gridHeight = ICONHEIGHT + SPACING * 2;
    gridWidth = ICONITEMWIDTH + HSPACING * 2;
    gridHeight = ICONITEMWIDTH + VSPACING * 2;

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j % _col) * gridWidth + ICONHSPACING;
            int y = (j / _col) * gridHeight + ICONVSPACING;
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
    setFixedSize(_width * _count, _height);

    int eachPage = iconCount / pageCount;
    int currentPage = 0;
    int tmpCount = 0;
    int m = 0;
    while (currentPage < pageCount) {
        while (tmpCount < eachPage) {
            _iconTable[currentPage][tmpCount] = seqList[m];
            seqList[m]->move(_gridTable[currentPage][tmpCount].translated(HSPACING, VSPACING).topLeft());
            seqList[m]->setPage(currentPage);
            seqList[m]->setIndex(tmpCount);
            m++;
            tmpCount++;
        }
        _nextIdx[currentPage] = eachPage;
        currentPage++;
        tmpCount = 0;
    }
    currentPage = 0;
    for (int i = m; i < iconCount; i++) {
        _iconTable[currentPage][eachPage] = seqList[i];
        seqList[i]->move(_gridTable[currentPage][eachPage].translated(HSPACING, VSPACING).topLeft());
        seqList[m]->setPage(currentPage);
        seqList[m]->setIndex(eachPage);
        _nextIdx[currentPage]++;
        currentPage++;
    }
    emit reInitiate();
}

void VirtualDesktop::arrangeMinimum()
{
    //printDesktop();
    QList<IconItem*> seqList;
    for (int i = 0; i < _count; i++)
        for (int j = 0; j < _row * _col; j++)
            if (_iconTable[i][j]) {
                seqList.push_back(_iconTable[i][j]);
                _iconTable[i][j] = NULL;
            }
    int iconCount = seqList.count();
    int pageCount = iconCount / (_row * _col);
    if (iconCount % (_row * _col) != 0)
        pageCount++;

    _iconTable.clear();
    _gridTable.clear();
    _nextIdx.clear();
    _count = pageCount;
    _current = 0;
    move(_pages[0], y());
    _pages.clear();
    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

//    gridWidth = ICONWIDTH + SPACING * 2;
//    gridHeight = ICONHEIGHT + SPACING * 2;
    gridWidth = ICONITEMWIDTH + HSPACING * 2;
    gridHeight = ICONITEMWIDTH + VSPACING * 2;

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j % _col) * gridWidth + ICONHSPACING;
            int y = (j / _col) * gridHeight + ICONVSPACING;
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
    setFixedSize(_width * _count, _height);

    int page = 0;
    int idx = 0;
    int m = 0;
    while (m < iconCount ) {
        if (idx == _row * _col) {
            page++;
            idx = 0;
            continue;
        }
        _iconTable[page][idx] = seqList[m];
        seqList[m]->move(_gridTable[page][idx].translated(HSPACING, VSPACING).topLeft());
        seqList[m]->setPage(page);
        seqList[m]->setIndex(idx);
        _nextIdx[page]++;
        idx++;
        m++;
    }

    //printDesktop();
    emit reInitiate();
}

void VirtualDesktop::mousePressEvent(QMouseEvent *event)
{
//    if (_openDir)
//    {
//        closeDir(_dirPage, _dirIndex);
//        return;
//    }

    if (event->modifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

    if (_openDir || _openToolBar )
    {
        event->ignore();
        return;
    }

    if(event->button() == Qt::LeftButton)
    {
        setFocus();
        QItemManager::getManager()->clearFocus();
    }
    emit desktopClicked();
    _iconMenu->setVisible(false);
    hideDirWidget("", -1);
    hideMenuWidget();

    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    if (_movingDialog) {
        _movingDialog->reject();
        delete _movingDialog;
        _movingDialog = NULL;
    }
    IconItem *item = static_cast<IconItem*>(childAt(event->pos()));

    if (event->button() == Qt::RightButton) {
        if (item) {
            return;
            if (count() == 1) {
                return;
            }
            _movingDialog = new MovingDialog(this, item, this);
            _movingDialog->move(mapToGlobal(event->pos()));
            _movingDialog->show();
            return;
        }
        /*       else if (_trembling) {
            emit trembleStoped();
            _trembling = false;
            return;
        }
*/
    }

    /*
    if (_trembling && event->button() == Qt::RightButton) {
        emit trembleStoped();
        _trembling = false;
    }
    */

    if (item) {
        if (_trembling) {
            if (QRect(0, 0, 30, 30).contains(event->pos() - item->pos())) {
                //delIcon(item->text());
                uninstall(item->text());
                return;
            }
        }
        if (_itemClicked) {
            if (!_trembling) {
                //runApp(item->text());
                _itemClicked = false;
            }
        } else {
            QTimer::singleShot(400, this, SLOT(updateClicked()));
            QTimer::singleShot(1000, this, SLOT(itemHeld()));
            _itemHeld = true;
            _itemClicked = true;
        }
    }
    else
    {
        emit pageChanged(_current);
    }
    m2v = event->x();
    gm2v = mapToGlobal(event->pos()).x();
    event->accept();
}

void VirtualDesktop::dragRight()
{
    if (!_inDrag)
        return;
    if (QRect(_pageSize.width() - 20, y(), 20, _pageSize.height())\
            .contains(cursor().pos())) {
        /* last page, expand */
        if (_current == _count - 1) {
            expand();
            qDebug() << "EXPAND";
        }
        move(_pages[++_current], y());
        emit pageChanged(_current);
        int i;
        int p = _inDrag->page();
        int s = _inDrag->index();
        for (i = s + 1; i < _nextIdx[p]; i++) {
            QPoint end = _gridTable[p][i-1].translated(HSPACING, VSPACING).topLeft();
            _iconTable[p][i]->move(end);
            _iconTable[p][i]->setIndex(i-1);
            _iconTable[p][i - 1] = _iconTable[p][i];
            _iconTable[p][i-1]->setIndex(i-1);
        }
        _iconTable[p][i-1] = NULL;
        _nextIdx[p]--;
        if (_nextIdx[_current] == iconsPerPage()) {
            int source = _current;
            int target = _current + 1;
            IconItem *moving = _inDrag;
            moving->setPage(source);
            moving->setIndex(iconsPerPage() - 1);
            IconItem *last = _iconTable[source][iconsPerPage() - 1];
            while (true) {
                if (target == _count) {
                    expand();
                    _iconTable[target][0] = last;
                    last->move(_gridTable[target][0].translated(HSPACING, VSPACING).topLeft());
                    last->setPage(target);
                    last->setIndex(0);
                    _nextIdx[target]++;
                    if (moving != _inDrag) {
                        int idx = _nextIdx[source];
                        if (idx >= iconsPerPage())
                            idx = iconsPerPage() - 1;
                        for (int i = idx; i > 0; i--) {
                            _iconTable[source][i] = _iconTable[source][i-1];
                            _iconTable[source][i]->move(_gridTable[source][i].translated(HSPACING, VSPACING).topLeft());
                            _iconTable[source][i]->setIndex(i);
                        }
                        _iconTable[source][0] = moving;
                        moving->move(_gridTable[source][0].translated(HSPACING, VSPACING).topLeft());
                        moving->setPage(source);
                        moving->setIndex(0);
                    } else {
                        _iconTable[source][iconsPerPage()-1] = moving;
                    }
                    break;
                }
                else if (_nextIdx[target] < iconsPerPage()) {
                    for (int i = _nextIdx[target]; i > 0; i--) {
                        _iconTable[target][i] = _iconTable[target][i-1];
                        _iconTable[target][i]->move(_gridTable[target][i].translated(HSPACING, VSPACING).topLeft());
                        _iconTable[target][i]->setIndex(i);
                    }
                    _iconTable[target][0] = last;
                    last->move(_gridTable[target][0].translated(HSPACING, VSPACING).topLeft());
                    last->setPage(target);
                    last->setIndex(0);
                    _nextIdx[target]++;
                    if (moving != _inDrag) {
                        int idx = _nextIdx[source];
                        if (idx >= iconsPerPage())
                            idx = iconsPerPage() - 1;
                        for (int i = idx; i > 0; i--) {
                            _iconTable[source][i] = _iconTable[source][i-1];
                            _iconTable[source][i]->move(_gridTable[source][i].translated(HSPACING, VSPACING).topLeft());
                            _iconTable[source][i]->setIndex(i);
                        }
                        _iconTable[source][0] = moving;
                        moving->move(_gridTable[source][0].translated(HSPACING, VSPACING).topLeft());
                        moving->setPage(source);
                        moving->setIndex(0);
                    } else {
                        _iconTable[source][iconsPerPage()-1] = moving;
                    }
                    break;
                }
                else {
                    source++;
                    target++;
                    moving = last;
                    last = _iconTable[source][iconsPerPage() - 1];
                    continue;
                }
            }
        } else {
            _inDrag->setPage(_current);
            _inDrag->setIndex(_nextIdx[_current]);
            _nextIdx[_current]++;
        }
        if (_nextIdx[p] == 0) {
            delPage(p);
        }
        cursor().setPos(_pageSize.width()/3, _pageSize.height()/3);
        qDebug() << "END OF DRAGRIGHT";
    }
}

void VirtualDesktop::dragLeft()
{
    if (!_inDrag)
        return;
    if (!QRect(0, y(), 20, _pageSize.height())\
            .contains(cursor().pos()))
        return;
    if (_current != 0) {
        move(_pages[--_current], y());
        int i;
        int p = _inDrag->page();
        int s = _inDrag->index();
        for (i = s + 1; i < _nextIdx[p]; i++) {
            QPoint end = _gridTable[p][i-1].translated(HSPACING, VSPACING).topLeft();
            _iconTable[p][i]->move(end);
            _iconTable[p][i]->setIndex(i-1);
            _iconTable[p][i-1] = _iconTable[p][i];
            _iconTable[p][i-1]->setIndex(i-1);
        }
        _iconTable[p][i-1] = NULL;
        _nextIdx[p]--;
        if (_nextIdx[_current] == iconsPerPage()) {
            int source = _current;
            int target = _current + 1;
            IconItem *last = _iconTable[_current][iconsPerPage()-1];
            for (int i = _nextIdx[target]; i > 0; i--) {
                _iconTable[target][i] = _iconTable[target][i-1];
                _iconTable[target][i]->move(_gridTable[target][i].translated(HSPACING, VSPACING).topLeft());
                _iconTable[target][i]->setIndex(i);
            }
            _iconTable[target][0] = last;
            last->move(_gridTable[target][0].translated(HSPACING, VSPACING).topLeft());
            last->setPage(target);
            last->setIndex(0);
            _nextIdx[target]++;
            _iconTable[source][iconsPerPage()-1] = _inDrag;
            _inDrag->setPage(source);
            _inDrag->setIndex(iconsPerPage()-1);
        } else {
            _inDrag->setPage(_current);
            _inDrag->setIndex(_nextIdx[_current]);
            _nextIdx[_current]++;
        }
        if (_nextIdx[p] == 0) {
            delPage(p);
        }
        emit pageChanged(_current);
    }
    cursor().setPos(_pageSize.width()/3, _pageSize.height()/3);
}

void VirtualDesktop::expand()
{
    _count++;
    setFixedSize(_count * _pageSize.width(), _pageSize.height());
    _pages.append(-(_count-1) * _pageSize.width());
    QList<QRect> newGridList;
//    gridWidth = ICONWIDTH + SPACING * 2;
//    gridHeight = ICONHEIGHT + SPACING * 2;
//    gridWidth = ICONITEMWIDTH + HSPACING * 2;
//    gridHeight = ICONITEMWIDTH + VSPACING * 2;
    changeSpacing();

    for (int i = 0; i < _col * _row; i++) {
        //vertical display
//        int x =  _pageSize.width() * (_count - 1)
//                + (i / _row) * gridWidth;
//        int y = (i % _row) * gridHeight;

            //horizontal display
            int x =  _pageSize.width() * (_count - 1)\
                    + (i % _col) * gridWidth + ICONHSPACING;
            int y = (i / _col) * gridHeight + ICONVSPACING;

        newGridList.insert(i, \
                           QRect(x, y, gridWidth, gridHeight));
    }
    _gridTable.append(newGridList);
    QList<IconItem*> newIconList;
    for (int i = 0; i < _col * _row; i++)
        newIconList.insert(i, NULL);
    _iconTable.append(newIconList);
    _nextIdx.append(0);
    emit pageIncreased();
}

void VirtualDesktop::delPage(int page)
{
//    qDebug() << "delete Page  start!!!!";
    if (_count == 1)
        return;
    if (_iconTable[page][0] != NULL) {
        assert("Error while delete page");
    }
    /* start from page, end before the last page
     * move the icon backward
     */
    for (int i = page; i < (_count - 1); i++) {
        for (int j = 0; j < _col * _row; j++) {
            _iconTable[i][j] = _iconTable[i+1][j];
            IconItem *icon = _iconTable[i][j];
            if (icon != NULL) {
                icon->setPage(i);
                icon->setIndex(j);
                icon->setGeometry(_gridTable[i][j].translated(HSPACING, VSPACING));
            }
        }
    }

    _iconTable.removeLast();
    _gridTable.removeLast();
    _pages.removeLast();
    _nextIdx.removeAt(page);
    _count--;

    if (_current > page) {
        _current--;
        if (_inDrag)
            _inDrag->setPage(_inDrag->page() - 1);
        emit pageChanged(_current);
    }

    setFixedSize( _count * _pageSize.width(), _pageSize.height());
//    move(_pages[_current], y());

    emit(pageDecreased());
}

bool VirtualDesktop::largerThan(const QSize &a, const QSize &b)
{
    int areaA = abs(a.width() * a.height());
    int areaB = abs(b.width() * b.height());
    return areaA > areaB ? true : false;
}

int VirtualDesktop::getNearestIndex(const QRect &rect)
{
    int page = rect.x()/_pageSize.width();
    int index = -1;
    QRect inter;
    for (int i = 0; i < _row * _col; i++) {
        inter = rect.intersected(_gridTable[page][i]);
        int area = abs(inter.width() * inter.height());
        if (area > 500) {
            index = i;
//            if (_iconTable[page][index])
//            {
//                if (!_dragEnterMinWidget)
//                {
//                    _dragEnterMinWidget = true;
//                    break;
//                }

//                if (_iconTable[page][index]->type() == dirIcon)
//                    index = -1;
//            }
            if(_inDrag->type() == dirIcon)
                break;
            if (_iconTable[page][index])
            {
                if (!_dragEnterMinWidget)
                {
                    _dragEnterMinWidget = true;

                    int s = _inDrag->index();

                    if(s<index){
                        return index-1;
                }
                else
                    break;
            }

            if (_iconTable[page][index]->type() == dirIcon)
                index = -1;
            }
            break;
        }
    }
    return index;
}

void VirtualDesktop::dragEnterEvent(QDragEnterEvent *event)
{
    _dragEvent = true;

    _itemHeld = false;
	
    if (event->keyboardModifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

    IconItem *icon = qobject_cast<IconItem*> (event->source());

    if (icon->id() == 1000)
    {
        event->ignore();
        return;
    }

    if (icon->parent() == this)
    {
        _inDrag = icon;
        icon->hide();

        if (_inDrag->type() == dirIcon)
        {
            for (int i = 0; i <_dirMinList.count(); i++)
            {
//                if (_iconDict.value(_dirMinList.at(i)) == _inDrag)
//                    continue;

                _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(false);
            }
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->acceptProposedAction();

        if (_iconDict.value(icon->uniqueName()))
            return;

        if (_isDirWidgetObject)
        {
            return;
        }

        _isDirWidgetObject = true;

        for (int i = 0; i <_dirMinList.count(); i++)
        {
            _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(false);
        }

        if (!_isIconMove)
            closeDir(_dirPage, _dirIndex);

        if (_dragItem != NULL)
            _dragItem = NULL;

        if (icon->dirId() < 0)              //判断是否是常用工具栏上的iconitem
            _dragItem = icon;

        addIcon(icon->text(), icon->pix()
                , -1, -1
                , icon->url(), icon->type(), icon->id(), icon->uniqueName());
    }
}

void VirtualDesktop::dragMoveEvent(QDragMoveEvent *event)
{
    if (!_isIconMove)
        return;
    if (_iconDragEnter)
        return;

    if (event->keyboardModifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}


    _dragEvent = true;

    event->setDropAction(Qt::MoveAction);
    event->accept();

    _itemHeld = false;

    if (!_inDrag)
        return;

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
        QRect moving(event->pos() - gap, _inDrag->size() * 1.0);
        int index = getNearestIndex(moving);
//        qDebug() << "index" << index;
        if (index == -1 || s == index)
        {
            return;
        }
#if 0
        if ((ADDICON == _iconTable[_current][index]->_icontype)||(ADDICON == _iconTable[_current][s]->_icontype))
        {
            return;
        }
#endif
        index = index > _nextIdx[_current] - 1 ? _nextIdx[_current] - 1: index;
        if (s > index) {

            qDebug() << "s" << s << "index" << index;

            for (int i = s; i > index; i--) {
               qDebug () << "s" << s << "i" << i;
                QPoint end = _gridTable[_current][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_current][i-1]->move(end);
                _iconTable[_current][i] = _iconTable[_current][i-1];
                _iconTable[_current][i]->setIndex(i);
            }
            update();
        }
        else {
            for (int i = s; i < index; i++) {

                QPoint end = _gridTable[_current][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_current][i+1]->move(end);
                _iconTable[_current][i] = _iconTable[_current][i+1];
                _iconTable[_current][i]->setIndex(i);
            }
            update();
        }
        //}
        _iconTable[_current][index] = _inDrag;
        _inDrag->setPage(_current);
        _inDrag->setIndex(index);
    }

}

void VirtualDesktop::dragLeaveEvent(QDragLeaveEvent *event)
{
//    _iconDragEnter = false;
    qDebug() << "*********************************dragLeaveEventdragLeaveEvent";
    _itemHeld = false;

//    hideDirWidget();

//    if (!_dragItem || ((_dragItem->dirId() > 0) && (_dragItem->type() != 3)))
//    {
//        _inDrag->show();
//        int p = _inDrag->page();
//        int s = _inDrag->index();
//        _inDrag->move(_gridTable[p][s].translated(HSPACING, VSPACING).topLeft());
//    }
//    drag->setPixmap(QPixmap(""));
//    icon->show();
//    event->accept();

    if ((_dragItem != NULL) && (_dragItem->dirId() < 0))
    {
        if (!_inDrag)
            return;

        _iconDict.take(_inDrag->uniqueName());


        _iconTable[_inDrag->page()][_inDrag->index()] = NULL;

        moveBackIcons(_inDrag->page(), _inDrag->index());

        _inDrag = NULL;

        _dragItem = NULL;

        _isDirWidgetObject = false;
        qDebug() << "------------------>del _dragItem";
    }

    _dragEvent = false;


    Q_UNUSED(event);

}

void VirtualDesktop::dropEvent(QDropEvent *event)
{

    if (event->keyboardModifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

	event->setDropAction(Qt::MoveAction);
    event->accept();

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

//    _inDrag=NULL;


    _inDrag->show();

    _inDrag=NULL;


    if (_dragItem != NULL)
    {

        if (_dragItem->type() == dirIcon)
        {
            if (dirWidget_FirstLists.count() != 0)
            {

                for (int i = 0; i < dirWidget_FirstLists.count(); i++)
                {
                    qDebug() << "dirWidget_FirstLists.at(i).name" <<dirWidget_FirstLists.at(i).name
                             <<dirWidget_FirstLists.count();
                     _iconDict.value(_dragItem->uniqueName())->addDirMinItem(dirWidget_FirstLists.at(i).name
                                                                            , dirWidget_FirstLists.at(i).iconPath
                                                                            , -1, -1
                                                                            , dirWidget_FirstLists.at(i).url
                                                                            , dirWidget_FirstLists.at(i).uniqueName);
                }
                dirWidget_FirstLists.clear();
            }

        }

        emit delToolBarWidgetIcon(_dragItem->uniqueName());

        _dragItem = NULL;

    }

    for (int i = 0; i <_dirMinList.count(); i++)
    {
        _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(true);
    }

    _dragEvent = false;
    _isDirWidgetObject = false;
}

void VirtualDesktop::iconItemDragOut()
{
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
//    _inDrag->show();

//    _inDrag=NULL;


    _inDrag->show();

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

        emit delToolBarWidgetIcon(_dragItem->uniqueName());

        _dragItem = NULL;

    }



    for (int i = 0; i <_dirMinList.count(); i++)
    {
        _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(true);
    }

    _dragEvent = false;
    _isDirWidgetObject = false;

}

void VirtualDesktop::mouseReleaseEvent(QMouseEvent *event)
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

void VirtualDesktop::mouseMoveEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

    if (_animation->state() == QAbstractAnimation::Running)
        _animation->stop();

//    VirtualDesktop *mouseM = static_cast<VirtualDesktop*>(this);
//    if (mouseM == this) {
        event->accept();
        int right_bound = -_pageSize.width() * (_count - 1) - 100;

        if (x() < right_bound)
            return;
        int to = x() + event->x() - m2v;
        move(to, y());
//        event->accept();
//    }
}

void VirtualDesktop::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);

    QItemManager::getManager()->savePressedKey(event->key());
//	qDebug() << "\nkey event in board:" << event->key();
//    if (event->modifiers()==Qt::ControlModifier)
//	{
//	  //做选中效果操作
//	  //如果是ctrl+点击控件，实现IconItem::enterEvent相同的动作
//        qDebug() <<  "VirtualDesktop::keyPressEvent to do\r\n";
//        QWidget::enterEvent(event);
//	}
//    update();
//    event->accept();
}
void VirtualDesktop::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);
    QItemManager::getManager()->deleteRecordKey(event->key());
}

void VirtualDesktop::goPage(int page)
{
    if (_current == page)
        return;

    _current = page;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();

    emit pageChanged(_current);

    update();
}

void VirtualDesktop::goNext()
{
    _current++;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    emit pageChanged(_current);
}

void VirtualDesktop::goPrevious()
{
    _current--;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    emit pageChanged(_current);
}

int VirtualDesktop::addIcon(const QString &text, \
                            const QString &iconPath, \
                            int page, \
                            int index,\
                            const QString &url,\
                            int type, int i,\
                            const QString &uniqueName)
{

    if (_iconDict.value(uniqueName))
    {
        qDebug() << text << "is already exist!!!";
        return -3;
    }
//    qDebug() << "addIcon()" << "------> text" << text;


    _localIconNum ++;

    int expandPageCount = (_localIconNum - 1) / iconsPerPage() + 1;
    if (expandPageCount > _count)
        expand();

    IconItem *icon = new IconItem(this);

    switch(_settings->iconSize())
    {
        case IconItem::large_size :
            icon->setLargeSize();
            break;

        case IconItem::medium_size :
            icon->setMediumSize();
            break;

        default:
            icon->setSmallSize();
            break;
    }

    icon->setSaveDataBool(true);
    icon->setIconSize(_settings->iconSize());
    icon->setUniqueName(uniqueName);
    icon->setText(text);
    icon->setTimeLine(false);
    icon->setPropertyAnimation(true);
    icon->setType(type);

    if(type == dirIcon)
    {
        icon->addMinWidget(_settings->iconSize());
    }
    icon->setIconClass(type);

    if (type != dirIcon)
        icon->setRunAction(true);

    icon->setDelAction(true);
    icon->setClosePixBool(false);
    icon->setSelectPixBool(false);
    icon->setDoubleClickBool(true);
    icon->setContextMenuBool(true);
    icon->setTrembleBool(false);
    icon->setDragEventBool(true);
    icon->setEnterEventBool(true);
    icon->setLeaveEventBool(true);
    icon->setEqualIcon(false);

    /* TODO deal with the icon is full */

    if (page == -1) {
        for (int i = 0; i < _count; i++) {
            if (_nextIdx[i] < _row * _col) {
                page = i;
                index = _nextIdx[i];
//                move(_pages[page], y());
                goPage(page);
                break;
            }
        }
    } else {
        if (index == -1) {

            if (_nextIdx[page] < iconsPerPage())
            {
                index = _nextIdx[page];
//                move(_pages[page], y());
                goPage(page);
            }
            else
            {
                for (int i = 0; i < _count; i++) {
                    if (_nextIdx[i] < _row * _col)
                    {
                        page = i;
                        index = _nextIdx[i];
//                        move(_pages[page], y());
                        goPage(page);
                        break;
                    }
                }
            }
        }
        else
        {
            if (index >= iconsPerPage())
            {
                for (int i = 0; i < _count; i++) {
                    if (_nextIdx[i] < _row * _col)
                    {
                        page = i;
                        index = _nextIdx[i];
//                        move(_pages[page], y());
                        goPage(page);
                        break;
                    }
                }
            }
        }
    }

    icon->setPixmap(iconPath, text);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(url);
    _iconDict.insert(uniqueName, icon);

//    QMap<QString, IconItem*>::iterator it; //遍历map
//    for ( it = _iconDict.begin(); it != _iconDict.end(); ++it )
//    {
//        qDebug() << "key" << it.key(); //用key()和data()分别获取“键”和“值”
//    }

    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    if (!_isDirWidgetObject)
    {
        icon->show();
    }
    else
    {
        _inDrag = icon;
        icon->hide();
    }
    icon->setDirId(-1);

    if (dirIcon == type)
    {

        icon->setDirMinItemId(i);

        _dirMinList.append(uniqueName);

        bool hasDirShowWidget = false;

        if (_dragItem != NULL)
        {
            icon->setDirMinItemId(_dragItem->id());
            hasDirShowWidget = true;

            for (int j = 0; j < _dirShowWidgetList.count(); j++)
            {
                if (_dirShowWidgetList.at(j)->id() == _dragItem->id())
                {
                    _dirShowWidgetList.at(j)->getFirstIconItem();

                    break;
                }
            }
        }
        if (!hasDirShowWidget)
        {
            bool isExist = false;
            for (int j = 0; j < _dirShowWidgetList.count(); j++)
            {
                if (_dirShowWidgetList.at(j)->id() == i)
                {
                    isExist = true;

                    _dirShowWidgetList.at(j)->getFirstIconItem();

                    break;
                }
            }

            if (!isExist)
                addShowWidget(text, i, uniqueName);
        }
    }
    else
    {
        icon->setId(111);
    }

    if (_restoreIcon || _openToolBar)
    {
        _restoreIcon = icon;
        icon->hide();
    }
//    _current = page;
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(vacWidgetDelIcon(const QString&)));
    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));
//    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
   //dirIcon
    connect(icon, SIGNAL(iconEnter()), this, SLOT(iconDragEnter()));
    connect(icon, SIGNAL(iconMove()), this, SLOT(iconDragMove()));
    connect(icon, SIGNAL(iconLeave()), this, SLOT(iconDragLeave()));
    connect(icon, SIGNAL(iconDrop(int, const QString &, const QString &, int, int, const QString &, int, const QString &)),
            this, SLOT(iconDragDrop(int, const QString &, const QString &, int, int, const QString &, int, const QString &)));
    connect(icon, SIGNAL(openDir(int, int, int)), this, SLOT(openDir(int, int, int)));
    connect(icon, SIGNAL(dragEnterMinWidget()), this, SLOT(iconDragEnter()));
    connect(icon, SIGNAL(showContextMenu(bool, QPoint, QPoint,IconItem *))
            , this, SLOT(showIconContextMenu(bool, QPoint, QPoint, IconItem *)));
    connect(icon, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , this, SIGNAL(iconItemNameChanged(const QString &, const QString &)));
    connect(icon, SIGNAL(dirMinLineEditFocusOut(int, const QString &))
            , this, SLOT(changedDirWidgetTitle(int, const QString &)));
    connect(icon, SIGNAL(dragOut()), this, SLOT(iconItemDragOut()));

    /*addicon add the last*/
//    showAddIcon(page, -1);

//    _iconLists.append(icon);

     return page;
    //LocalAppList::getList()->save();
}


int VirtualDesktop::addIcon(const QString &text, const QString &icon, const QString &url, int type, int i, const QString &uniqueName)
{
    return addIcon(text, icon, -1, -1, url, type, i, uniqueName);
}
#if 0
int VirtualDesktop::showAddIcon(int page, int index)
{

    /* TODO deal with the icon is full */
    if (page == -1) {
        for (int i = 0; i < _count; i++) {
            if (_nextIdx[i] < _row * _col) {
                page = i;
                index = _nextIdx[i];
                break;
            }
        }
    } else {
        if (index == -1) {
            if (_nextIdx[page] < iconsPerPage()) {
                index = _nextIdx[page];
            } else {
                for (int i = 0; i < _count; i++) {
                    if (_nextIdx[i] < _row * _col) {
                        page = i;
                        index = _nextIdx[i];
                        move(_pages[page], y());
                        break;
                    }
                }
            }
        }
    }
//   g_addIcon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
//   g_addIcon->show();
    
    return page;  
}
#endif
void VirtualDesktop::delIcon(const QString &uniqueName)
{
    if (!_iconDict.value(uniqueName))
        return;

    IconItem *icon = _iconDict.take(uniqueName);
    //icon->setHidden(true);
    qDebug() << icon->page();
    int p = icon->page();
    int s = icon->index();

    delete _iconTable[p][s];

    moveBackIcons(p, s);

}

void VirtualDesktop::moveBackIcons(int page, int index)
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

    if (_nextIdx[_count - 1] == 0)
    {
        if (_count != 1 )
        {
            goPage(_count - 2);
        }

        delPage(_count - 1);
    }

    _localIconNum--;
}

void VirtualDesktop::runApp(const QString &uniqueName)
{
    RunApp::getRunApp()->runApp(uniqueName);

}

void VirtualDesktop::printDesktop()
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

void VirtualDesktop::contextMenuEvent(QContextMenuEvent *event)
{
    _mousePos = event->pos();
    _normalMenu->move(_mousePos.x(), _mousePos.y());

    hideMenuWidget();
    _normalMenu->raise();
    _normalMenu->setVisible(true);
    _mousePos = event->pos();
    if(QItemManager::getManager()->getItemListSize() != 0)
    {
        _iconMenu->move(_mousePos.x() + _current * _width, _mousePos.y());
        hideMenuWidget();
        _iconMenu->raise();
        _iconMenu->setVisible(true);
    }
    else
    {
        _normalMenu->move(_mousePos.x(), _mousePos.y());
        hideMenuWidget();
        _normalMenu->raise();
        _normalMenu->setVisible(true);
    }

}

void VirtualDesktop::appAdd()
{
    hideMenuWidget();

    _addAppState = true;
    QString path = QFileDialog::getOpenFileName(this, tr("创建快捷方式"),
                                                Config::get("CommonProgramDir"),
                                                tr("app (*.*)"));   //tr("app (*.lnk *.exe)"));

    if (path.isEmpty())
    {
        _addAppState = false;
        return;
    }

    path.replace('/','\\');
    //AppDataReadExe::Instance()->addLocalApp(path);
    LocalAppList::getList()->addLocalApp(path);
    _addAppState = false;
}

void VirtualDesktop::appDelete()
{
    _trembling = true;
    emit trembleStarted();
}

void VirtualDesktop::appCancel()
{
    _trembling = false;
    emit trembleStoped();
}

void VirtualDesktop::upLoad()
{
//    _manageWidget->upLoad();
}

void VirtualDesktop::movetoFist()
{
    _current = 0;
//    move(_pages[_current], y());
//    _current = page;
    goPage(_current);
    emit pageChanged(_current);

}

void VirtualDesktop::reloadApplist()
{
    //we need check the page. because we only have one page now;

    int expandPageCount =  _localIconNum / iconsPerPage() + 1;  //_local->count()
    for(int i = _count; i < expandPageCount; i++)
    {
        expand();
    }

    // we need init some QList
    for (int i = 0; i < _count; i++)
           _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
            QList<QRect> newList;
            for (int j = 0; j < _col * _row; j++) {
                int x =  _pageSize.width() * i \
                        + (j % _col) * gridWidth + ICONHSPACING;
                int y = (j / _col) * gridHeight + ICONVSPACING;

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

    qDebug()<<"VirtualDesktop reload all.";
}


void VirtualDesktop::addDirItem()
{
    hideMenuWidget();

    QUuid uuid = QUuid::createUuid();
    QString strUuid = uuid.toString();

    strUuid.replace("{", "");
    strUuid.replace("}", "");

    addDesktopApp(QString(tr("新建文件夹")), "", "", 3, "3_" + strUuid);

}

void VirtualDesktop::openDir(int id, int page, int index)
{
    hideMenuWidget();

    if (!_dirMovingFinished)
        return;

    _dirMovingFinished = false;

    setContextMenuPolicy(Qt::NoContextMenu);

    _openDir = true;
    _isIconMove = false;

    _dirPage = page;
    _dirIndex = index;

//    setIconEnabled(false);

    if (_animationScreenDown)
    {
        closeDir(page, index);
        return;
    }

    _dirId = id;

    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        if (_dirShowWidgetList.at(i)->id() == _dirId)
        {
            _distance = _dirShowWidgetList.at(i)->getHeight();
            break;
        }
    }

//    qDebug() << _distance;
    int x = page * _desktopRect.width();
    int y = _gridTable[page][index].y() + gridHeight;
    int w = _desktopRect.width();
    int upH = _gridTable[page][index].y() + gridHeight;

    int downH = _desktopRect.height() - _gridTable[page][index].y() - gridHeight;

//    int mx = _gridTable[page][index].x() + (gridWidth - SMALLSLIDERWIDTH) / 2 + x;
    int mx = _gridTable[0][index].x() + (gridWidth - SMALLSLIDERWIDTH) / 2;
    int my = y - 15;
    int mw = 38;
    int mh = 15;

    int row = (_gridTable[page][index].y() + gridHeight) / gridHeight;

    if ((_pageSize.height() / gridHeight - row - _distance / gridHeight) >= 0)
    {
        _upDistance = 0;
    }
    else
    {
        _upDistance = (_pageSize.height() / gridHeight - row - _distance / gridHeight) * gridHeight * -1;
    }

    emit setMinMoveLabel(true);

    emit upMove(0, 0, w, upH, _upDistance);
    emit desktopOpenMove(0, y, w, downH, _distance);

    emit openMinWidget(mx, my, mw, mh, _distance);
    emit upMinMove(mx, my, mw, mh, _upDistance);


    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        if (_dirShowWidgetList.at(i)->id() == _dirId)
        {
            _dirShowWidgetList.at(_dirId)->move(x, y - _upDistance);
            _dirShowWidgetList.at(i)->raise();
            _dirShowWidgetList.at(i)->setVisible(true);
            break;
        }
    }

    _animationScreenDown = true;
}

void VirtualDesktop::closeDir(int page, int index)
{
    hideMenuWidget();
    if (!_dirMovingFinished)
        return;

    _dirMovingFinished = false;

        for (int i = 0; i < _dirShowWidgetList.count(); i++)
        {
            if (_dirShowWidgetList.at(i)->id() == _dirId)
            {
                _distance = _dirShowWidgetList.at(_dirId)->getHeight();
                break;
            }
        }


    int x = _current * _desktopRect.width();
    int y = _gridTable[page][index].y() + gridHeight + _distance;
    int w = _desktopRect.width();
    int upH = _gridTable[page][index].y() + gridHeight;
//    int downH = _desktopRect.height() - upH;
    int downH = _desktopRect.height() - _gridTable[page][index].y() - gridHeight;

//    int mx = _gridTable[page][index].x() + (gridWidth - SMALLSLIDERWIDTH) / 2 + x; // 39
    int mx = _gridTable[0][index].x() + (gridWidth - SMALLSLIDERWIDTH) / 2;
    int my = y - 15;              //_gridTable[page][index].y() + gridHeight + 249
    int mw = 38;                       //gridWidth - 40
    int mh = 15;                       //40

    emit closeMinWidget(mx, my, mw, mh, _distance);
    emit desktopCloseMove(0, y, w, downH, _distance);
    emit upBackMove(0, -_upDistance, w, upH, _upDistance);
    emit upMinBackMove(mx, my - _distance - _upDistance, mw, mh, _upDistance);
//    if (_upDistance > 0)
//        emit desktopBgBack(gridHeight);
    _openDir = false;
    _animationScreenDown = false;

    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void VirtualDesktop::setDirHide()
{
    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        if (_dirShowWidgetList.at(i)->id() == _dirId)
        {
            _dirShowWidgetList.at(_dirId)->setVisible(false);
            break;
        }
    }

   _dustbinDirShowWidget->setVisible(false);

}

void VirtualDesktop::dirMovingFinished()
{
    _dirMovingFinished = true;

    if (_restoreIcon)
    {
        _restoreIcon->show();

        _restoreIcon = NULL;
    }
}

void VirtualDesktop::addDesktopApp(const QString &text, const QString &pix, const QString &url, int type, const QString &uniqueName)
{
    _url = url;
    qDebug() << _url;

    if (localIcon == type)
    {
            LocalApp *app = new LocalApp();
            app->setName(text);
            app->setIcon(pix);
            app->setType(QString("%1").arg(type));
            app->setUrl(url);
            app->setPage(-1);
            app->setIndex(-1);
            app->setDirId(-1);
            app->setId("111");
            app->setUniqueName(uniqueName);

            for (int i = 0; i < g_RemotelocalList.count(); i++)
            {
                if("0_" + g_RemotelocalList.at(i).uniqueName == uniqueName)
                {
                    app->setExecname(g_RemotelocalList.at(i).execname);
                    break;
                }
            }

            if (LocalAppList::getList()->getAppByUniqueName(uniqueName)) {
                QApplication::processEvents();
                AppMessageBox box(false, NULL);
                box.setText("已添加该图标");
                box.exec();
            } else {
                LocalAppList::getList()->addApp(app);
            }

    }
    else if (vacIcon == type)
    {
            LocalApp *RemoteApp = new LocalApp();

            for (int i = 0; i < g_myVappList.count(); i++)
            {
                if(g_myVappList.at(i).name == text)
                {
                    RemoteApp->setName(g_myVappList[i].name);
                    RemoteApp->setId(g_myVappList[i].id);
                    RemoteApp->setIcon(pix);
                    RemoteApp->setPage(-1);
                    RemoteApp->setIndex(-1);
                    RemoteApp->setType(QString("%1").arg(type));
                    RemoteApp->setIsRemote(true);
                    RemoteApp->setUrl(url);
                    RemoteApp->setDirId(-1);
                    RemoteApp->setId("111");
                    RemoteApp->setUniqueName(uniqueName);
                    break;
                }
            }

            LocalAppList::getList()->addRemoteApp(RemoteApp);
            addIcon(RemoteApp->name(), RemoteApp->icon()
                    , RemoteApp->page(), RemoteApp->index()
                    , RemoteApp->url(), RemoteApp->type().toInt()
                    , RemoteApp->id().toInt(), RemoteApp->uniqueName());

    }
    else if (paasIcon == type)
    {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(-1);
        app->setIndex(-1);
        app->setExecname(url);
        app->setDirId(-1);
        app->setId("111");
        app->setUniqueName(uniqueName);


        if (LocalAppList::getList()->getAppByUniqueName(uniqueName)) {
            QApplication::processEvents();
            AppMessageBox box(false, NULL);
            box.setText("已添加该图标");
            box.exec();
        } else {
            LocalAppList::getList()->addApp(app);
        }
    }
    else if (dirIcon == type)
    {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(-1);
        app->setIndex(-1);
        app->setExecname("");
        app->setDirId(-1);
        app->setId(QString("%1").arg(_dirShowWidgetList.count()));
        app->setUniqueName(uniqueName);

        LocalAppList::getList()->addApp(app);

    }
}

void VirtualDesktop::hideDirWidget(const QString &uniqueName, int dirId)
{

   if (_openDir)
    {
        closeDir(_dirPage, _dirIndex);
    }
    else if (_openToolBar)
    {
        toolBarCloseDir(_dirId, _toolBarClickedPosX, _toolBarIconItemWidth);
    }

//    if (uniqueName.isEmpty())
//        return;

    if (dirId == 1000)
        return;

//    for (int j = 0; j < _local->count(); j++)
//    {
//        if (_local->at(j)->id().toInt() == dirId)   // 查找当前文件夹
//        {
//            if (_local->at(j)->dirId() == -1)   //文件夹的位置
//            {
//                if (!_iconDict.value(_local->at(j)->uniqueName()))
//                    break;


                for (int j = 0; j < _dirShowWidgetList.count(); j++)
                {
                    if (_dirShowWidgetList.at(j)->id() == _dirId)
                    {
                        _dirShowWidgetList.at(j)->getFirstIconItem();
                    }
                }
//            }
//            else
//            {
//                emit toolBarRemoveDirMinItem(_local->at(j)->uniqueName(), dirId);
//            }

//            break;
//        }
//    }
}

void VirtualDesktop::setIconEnabled(bool enabled)
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

void VirtualDesktop::menuChanged(int value)
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
    case 2 :
        if (_showIconMenu->isVisible())
            _showIconMenu->setVisible(false);

        _createMenu->move(_mousePos.x() + 162 - 25, _mousePos.y() + 20 + 19 + 19 - 10);
        _createMenu->raise();
        _createMenu->setVisible(true);

        break;
    default:
        if (_createMenu->isVisible())
            _createMenu->setVisible(false);

        if (_showIconMenu->isVisible())
            _showIconMenu->setVisible(false);
        break;
    }


}

void VirtualDesktop::hideMenuWidget()
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

void VirtualDesktop::setLargeIcon()
{
    hideMenuWidget();

    if (_settings->iconSize() == IconItem::large_size)
        return;

    _settings->setIconSize(IconItem::large_size);

    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        _dirShowWidgetList.at(i)->largeIcon();
    }

    _dustbinDirShowWidget->largeIcon();

//    LocalAppList::getList()->save();


    refresh(LARGESIZE);

    emit largeIcon();

}

void VirtualDesktop::setMediumIcon()
{
    hideMenuWidget();

    if (_settings->iconSize() == IconItem::medium_size)
        return;

    _settings->setIconSize(IconItem::medium_size);

    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        _dirShowWidgetList.at(i)->mediumIcon();
    }
    _dustbinDirShowWidget->mediumIcon();

//    LocalAppList::getList()->save();

    refresh(MEDIUMSIZE);

    emit mediumIcon();

}

void VirtualDesktop::setSmallIcon()
{
    hideMenuWidget();

    if (_settings->iconSize() == IconItem::small_size)
        return;

    _settings->setIconSize(IconItem::small_size);

    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        _dirShowWidgetList.at(i)->smallIcon();
    }
    _dustbinDirShowWidget->smallIcon();

//    LocalAppList::getList()->save();



    refresh(SMALLSIZE);

    emit smallIcon();

}

void VirtualDesktop::refresh(QSize size)
{
    movetoFirst();
    deleteAllIconItem();

    for(int i = _count; i > 1; i--)
    {
        delPage(i-1);
    }

    _pages.clear();
    _iconTable.clear();
    _nextIdx.clear();
    _iconDict.clear();
    _gridTable.clear();
//    _dirShowWidgetList.clear();
    _dirMinList.clear();

    reloadApplist(size);

    setDirWidgetMaxRow();
    qDebug() << "refresh desktop start!!";
    _localIconNum = 0;
    qDebug() << "_tempIconLists.count() : " << _tempLists.count();
    for (int i = 0; i < _tempLists.count(); i++)
    {

        addIcon(_tempLists.at(i).name, _tempLists.at(i).iconPath,
                -1, -1,
                _tempLists.at(i).url, _tempLists.at(i).type,
                _tempLists.at(i).id, _tempLists.at(i).uniqueName);
    }
    qDebug() << "desktop init iconItem over !!!!";
//    for (int i = 0; i < _dirShowWidgetList.count(); i++)
//    {
//        for (int j = 0; j < _local->count(); j++) {
//            if (_local->at(j)->hidden())
//                continue;
//            if (_local->at(j)->dirId() == _dirShowWidgetList.at(i)->id())
//            {
//                qDebug() << "dirShowWidgetAddIcon start!!!";
//                _dirShowWidgetList.at(i)->addDirIcon(_local->at(j)->name(), _local->at(j)->icon(),
//                                           _local->at(j)->page(), _local->at(j)->index(),
//                                           _local->at(j)->url(), _local->at(j)->type().toInt(), _local->at(j)->uniqueName());
//                qDebug() << "dirShowWidgetAddIcon end!!!";
//            }
//        }
//    }

    _tempLists.clear();

}

void VirtualDesktop::movetoFirst()
{
    _current = 0;
    move(_pages[_current], y());
//    emit pageChanged(_current);

}

void VirtualDesktop::reloadApplist(QSize size)
{
    _count = 1;

//    gridWidth = size.width() + HSPACING * 2;
//    gridHeight = size.height() + BOTTOMSPACING;

    changeSpacing();

    _col = (_width - _iconHSpacing) / gridWidth;
    _row = (_height - _iconVSpacing - (LARGESIZE.height() + VSPACING + BOTTOMSPACING)) / gridHeight;

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

void VirtualDesktop::setDirWidgetMaxRow()
{
    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        _dirShowWidgetList.at(i)->setMaxRow(_row);
    }
    _dustbinDirShowWidget->setMaxRow(_row);
}

void VirtualDesktop::deleteAllIconItem()
{
//    _local->save();

    _tempLists.clear();

    TEMP_LIST tempList;

    for (int i = 0; i < _count; i++)
    {
        for(int j = 0;j < iconsPerPage(); j++)
        {
            if(_iconTable[i][j] != NULL)
            {
                tempList.name = _iconTable[i][j]->text();
                tempList.iconPath = _iconTable[i][j]->pix();
                tempList.page = -1;
                tempList.index = -1;
                tempList.url = _iconTable[i][j]->url();
                tempList.type = _iconTable[i][j]->type();
                tempList.id = _iconTable[i][j]->id();
                tempList.uniqueName = _iconTable[i][j]->uniqueName();

                _tempLists.append(tempList);

                delete _iconTable[i][j];
                _iconTable[i][j] = NULL;
            }
        }
        _nextIdx[i]=0;
    }
}

void VirtualDesktop::changeSpacing()
{
    switch(_settings->iconSize())
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

void VirtualDesktop::showIconContextMenu(bool visiable, QPoint pos, QPoint mPos, IconItem *iconItem)
{
    Q_UNUSED(pos);
//    _currentIconItem = text;

//    for (int i = 0; i < _local->count(); i++)
//    {
//        if (_local->at(i)->name() == _currentIconItem)
//        {
//            _currentUniqueName = _local->at(i)->uniqueName();
//        }
//    }

    if (!iconItem)
        return;

    _currentUniqueName = iconItem->uniqueName();

    if (!_iconDict.value(_currentUniqueName))
        return;

    if ( _iconDict.value(_currentUniqueName)->type() == dirIcon)
    {
        if (_iconDict.value(_currentUniqueName)->getMinIconNum() != 0)
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

    _iconMenu->move(mPos.x() + _current * _width, mPos.y());
    _iconMenu->raise();
    _iconMenu->setVisible(visiable);
}

void VirtualDesktop::iconMenuRunClicked()
{
    _iconMenu->setVisible(false);

    if (_iconDict.value(_currentUniqueName)->type() == dirIcon)
    {
//        IconItem *iconItem = _iconDict.value(_currentIconItem);
        openDir(_iconDict.value(_currentUniqueName)->id(), _iconDict.value(_currentUniqueName)->page(), _iconDict.value(_currentUniqueName)->index());
//        iconItem = NULL;
    }
    else
    {
        if(QItemManager::getManager()->getItemListSize() == 0)
            runApp(_currentUniqueName);
        else
        {
            for(int i = 0; i < QItemManager::getManager()->getItemListSize(); i++)
            {
                  runApp(QItemManager::getManager()->itemAt(i)->text());
            }
        }
    }
}

void VirtualDesktop::iconMenuDelClicked()
{
    _iconMenu->setVisible(false);
 //   qDebug() << _currentIconItem;
//    delIcon(_currentIconItem);
//    qDebug() << "    if (_iconDict.value(_currentIconItem)->getMinIconNum() != 0)" << _iconDict.value(_currentIconItem)->getMinIconNum();
//    if (_iconDict.value(_currentIconItem)->getMinIconNum() != 0)
//    {
//        return;
//    }
    if(QItemManager::getManager()->getSelectIconItemText().size() == 0)
    {
        if ( _iconDict.value(_currentUniqueName)->type() == dirIcon)
        {
            int index = _iconDict.value(_currentUniqueName)->id();

            QList<DirShowWidget*>::iterator iter = _dirShowWidgetList.begin() + index;
            for (int j = 0; j < _dirShowWidgetList.count(); j++)
            {
                if (_dirShowWidgetList.at(j)->id() == index)
                {
                    _dirShowWidgetList.at(j)->setParent(NULL);
                    _dirShowWidgetList.at(j)->deleteLater();
                    break;
                }
            }

            _dirShowWidgetList.erase(iter);

            _dirMinList.removeOne(_iconDict.value(_currentUniqueName)->uniqueName());

            LocalAppList::getList()->delApp(_currentUniqueName);

            emit desktopDelIcon(_currentUniqueName);

            for (int i = 0; i < _dirShowWidgetList.count(); i++)
            {
                if (index > _dirShowWidgetList.count())
                    break;

                if (_dirShowWidgetList.at(i)->id() == index + 1)
                {
                    _dirShowWidgetList.at(i)->setId(index);

//                    _iconDict.value(_dirMinList.at(i))->setId(index);



                    index ++;
                }
            }
        }
        else
        {
            LocalAppList::getList()->delApp(_currentUniqueName);

            emit desktopDelIcon(_currentUniqueName);
        }
    }
    else
    {
        QList<QString> list = QItemManager::getManager()->getSelectIconItemText();
        for(int i = 0; i < list.size(); i++)
        {
            _currentUniqueName = list.at(i);

            if ( _iconDict.value(_currentUniqueName)->type() == dirIcon)
            {
#if 0
                int index = _iconDict.value(_currentIconItem)->id();

                QList<DirShowWidget*>::iterator iter = _dirShowWidgetList.begin() + index;
                for (int j = 0; j < _dirShowWidgetList.count(); j++)
                {
                    if (_dirShowWidgetList.at(j)->id() == index)
                    {
                        _dirShowWidgetList.at(j)->setParent(NULL);
                        _dirShowWidgetList.at(j)->deleteLater();
                    }
                }
                _dirShowWidgetList.erase(iter);

                _dirMinList.removeAt(index);

                for (int i = index; i < _dirShowWidgetList.count(); i++)
                {
                    _dirShowWidgetList.at(i)->setId(i);

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
#endif
                int index = _iconDict.value(_currentUniqueName)->id();

                QList<DirShowWidget*>::iterator iter = _dirShowWidgetList.begin() + index;
                for (int j = 0; j < _dirShowWidgetList.count(); j++)
                {
                    if (_dirShowWidgetList.at(j)->id() == index)
                    {
                        _dirShowWidgetList.at(j)->setParent(NULL);
                        _dirShowWidgetList.at(j)->deleteLater();
                        break;
                    }
                }

                _dirShowWidgetList.erase(iter);

                _dirMinList.removeOne(_iconDict.value(_currentUniqueName)->uniqueName());

                QItemManager::getManager()->removeByUniqueName(_currentUniqueName);
                LocalAppList::getList()->delApp(_currentUniqueName);

                emit desktopDelIcon(_currentUniqueName);

                for (int i = 0; i < _dirShowWidgetList.count(); i++)
                {
                    if (index > _dirShowWidgetList.count())
                        break;

                    if (_dirShowWidgetList.at(i)->id() == index + 1)
                    {

                        _dirShowWidgetList.at(i)->setId(index);
                        _iconDict.value(_dirMinList.at(i))->setId(index);

//                        for (int j = 0; j < _local->count(); j++) {
//                            if (_local->at(j)->hidden())
//                                continue;
//                            if (_local->at(j)->dirId() == index + 1)
//                            {
//                                _local->at(j)->setDirId(index);
//                            }
//                        }

                        index ++;
                    }
                }
            }
            else
            {
                QItemManager::getManager()->removeByUniqueName(_currentUniqueName);
                LocalAppList::getList()->delApp(_currentUniqueName);

                emit desktopDelIcon(_currentUniqueName);
            }

        }
    }
}

void VirtualDesktop::vacWidgetDelIcon(const QString &uniqueName)
{
    for (int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->uniqueName() == uniqueName)
        {
            if (_local->at(i)->dirId() == -1)
            {
//                LocalAppList::getList()->delApp(uniqueName);
            }
            else if (_local->at(i)->dirId() == -2)
            {
                emit delToolBarIcon(uniqueName);
//                LocalAppList::getList()->delApp(uniqueName);
            }
            else
            {
                if (_local->at(i)->dirId() == 1000)
                {
                    //_dustbinDirShowWidget->removeIcon(uniqueName);
                }
                else
                {
                    for (int j = 0; j < _dirShowWidgetList.count(); j++)
                    {
                        if (_dirShowWidgetList.at(j)->id() == _local->at(i)->dirId())
                            _dirShowWidgetList.at(j)->removeIcon(uniqueName);
                    }
 //                   _iconDict.value(_local->at(i)->name())->removeDirMinItem(uniqueName);
                }

            }

            LocalAppList::getList()->delApp(uniqueName);
        }
    }
}

void VirtualDesktop::refreshMenu()
{
    hideMenuWidget();
    emit refreshVac();
}

void VirtualDesktop::dirWidgetDelIcon(int id, const QString &uniqueName)
{
        emit desktopDelIcon(uniqueName);
}

void VirtualDesktop::moveWidgetDrop(IconItem *iconItem)
{
    if (!iconItem)
        return;

    _isDirWidgetObject = false;

    if (_iconDict.value(iconItem->uniqueName()))
    {
        _iconTable[_iconDict.value(iconItem->uniqueName())->page()][_iconDict.value(iconItem->uniqueName())->index()]->show();
        if (_inDrag)
            _inDrag=NULL;
    }
    else
    {

        addIcon(iconItem->text(), iconItem->pix()
                , - 1, _nextIdx[_count - 1]
                , iconItem->url(), iconItem->type(), iconItem->id(), iconItem->uniqueName());
    }

//    _dirShowWidgetList.at(iconItem->dirId())->removeIcon(iconItem->text());

//    _iconDict.value(_dirMinList.at(iconItem->dirId()))->removeDirMinItem(iconItem->text());

}

void VirtualDesktop::iconMoveOtherWidget(const QString &uniqueName)
{
   if (!_iconDict.value(uniqueName))
    {

        return;
    }

    if ( _iconDict.value(uniqueName)->type() == dirIcon)
    {
        int index = _iconDict.value(uniqueName)->id();
         _dirMinList.removeOne(uniqueName);

    }

    IconItem *icon = _iconDict.take(uniqueName);

    int p = icon->page();
    int s = icon->index();
//    _iconTable[p][s]->hide();
    _iconTable[p][s] = NULL;

    moveBackIcons(p, s);
}

void VirtualDesktop::initIconItem()
{
    _localIconNum = 0;

    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == -1)
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
        }

        if (_local->at(i)->dirId() == -2)
        {
            if (_local->at(i)->type().toInt() == dirIcon)
            {
                qDebug() << "addShowWidget------>_local->at(i)->id().toInt()" << _local->at(i)->id().toInt();
                addShowWidget(_local->at(i)->name(), _local->at(i)->id().toInt()
                              , _local->at(i)->uniqueName());
            }
        }
    }
//    for (int i = 0; i < _dirShowWidgetList.count(); i++)
//    {
//        for (int j = 0; j < _local->count(); j++) {
//            if (_local->at(j)->hidden())
//                continue;
//            if (_local->at(j)->dirId() == _dirShowWidgetList.at(i)->id())
//            {
//                _dirShowWidgetList.at(i)->addDirIcon(_local->at(j)->name(), _local->at(j)->icon(),
//                                           _local->at(j)->page(), _local->at(j)->index(),
//                                           _local->at(j)->url(), _local->at(j)->type().toInt(), _local->at(j)->uniqueName());
//            }
//        }
//    }

    qDebug() << "init iconItem over!!!";

}

void VirtualDesktop::toolBarOpenDir(int id, int posX, int width)
{
        hideMenuWidget();

        if (!_dirMovingFinished)
            return;

        _dirMovingFinished = false;

        setContextMenuPolicy(Qt::NoContextMenu);


    //    setIconEnabled(false);

    _toolBarClickedPosX = posX;
    _toolBarIconItemWidth = width;
        if (_animationScreenDown)
        {
            toolBarCloseDir(id, _toolBarClickedPosX, _toolBarIconItemWidth);
            return;
        }
        _isIconMove = false;
        _openToolBar = true;

        if(id == 1000)
        {
            _upDistance = _dustbinDirShowWidget->getDustBinNewHeight();
        }
        else
        {
            for (int i = 0; i < _dirShowWidgetList.count(); i++)
            {
                if (_dirShowWidgetList.at(i)->id() == id)
                {
                    _upDistance = _dirShowWidgetList.at(i)->getHeight();

//                    _dirPage = page;
//                    _dirIndex = index;
                    break;
                }
            }
        }


        _dirId = id;

        int h = _height - (SMALLSIZE.height() + VSPACING + BOTTOMSPACING);

        int x = _current * _desktopRect.width();
        int y = 0;                              //_gridTable[page][index].y() + gridHeight;
        int w = _desktopRect.width();
        int upH = h;

        int downH = _desktopRect.height() - h;

        int mx = posX + (width - SMALLSLIDERWIDTH) / 2;  // 39
        int my = upH;                            //_gridTable[page][index].y() + gridHeight + 21;
        int mw = 38;                                //gridWidth - 40;
        int mh = 15;                                //40;

        _distance = 0;

        emit setMinMoveLabel(false);
        emit upMove(0, y, w, upH, _upDistance);    //_current * w, upH, _upDistance
        emit upMinMove(mx, my, mw, mh, _upDistance);

        emit desktopOpenMove(0, upH, w, downH, _distance);
        emit openMinWidget(mx, my, mw, mh, _distance);


        qDebug() << "before" ;

        if(id == 1000)
        {
            _dustbinDirShowWidget->move(x, upH - _upDistance);
            _dustbinDirShowWidget->raise();
            _dustbinDirShowWidget->setVisible(true);
        }
        else
        {
            for (int i = 0; i < _dirShowWidgetList.count(); i++)
            {
                if (_dirShowWidgetList.at(i)->id() == _dirId)
                {
                    _dirShowWidgetList.at(i)->move(x, upH - _upDistance);
                    _dirShowWidgetList.at(i)->raise();
                    _dirShowWidgetList.at(i)->setVisible(true);
                    break;
                }
            }
        }

        qDebug() << "after";

        _animationScreenDown = true;
}

void VirtualDesktop::toolBarCloseDir(int id, int posX, int width)
{
    if (!_dirMovingFinished)
        return;

    _dirMovingFinished = false;

//    _upDistance = _dustbinDirShowWidget->getHeight();

    int h = _height - (SMALLSIZE.height() + VSPACING + BOTTOMSPACING);
    int x = _current * _desktopRect.width();
    int y = - 1 * _upDistance;
    int w = _desktopRect.width();
    int upH = h;

    int downH = _desktopRect.height() - h;

    int mx = posX + (width - SMALLSLIDERWIDTH) / 2;

    int my = upH - 15;
    int mw = 38;
    int mh = 15;

    _distance = 0;

    emit upBackMove(0, y, w, upH, _upDistance);
    emit upMinBackMove(mx, upH + y, mw, mh, _upDistance);

    emit desktopCloseMove(x, upH + _distance, w, downH, _distance);
    emit closeMinWidget(mx, my + _distance, mw, mh, _distance);

//    if (_upDistance > 0)
//        emit desktopBgBack(gridHeight);

    _openToolBar = false;


    _animationScreenDown = false;

    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void VirtualDesktop::toolBarIconToDustbin(const QString &text, const QString &iconPath, int page
                          , int index, const QString &url, int type, const QString &uniqueName)
{
    _dustbinDirShowWidget->addDirIcon(text, iconPath, page, index, url, type, uniqueName);
}

void VirtualDesktop::toolBarIconToDir(int id, const QString &text, const QString &iconPath, int page
                      , int index, const QString &url, int type, const QString &uniqueName)
{
    for (int i = 0; i < _dirShowWidgetList.count(); i++)
    {
        if (_dirShowWidgetList.at(i)->id() == id)
        {
            _dirShowWidgetList.at(i)->addDirIcon(text, iconPath, page, index, url, type, uniqueName);
            break;
        }
    }
}

void VirtualDesktop::addShowWidget(const QString &text, int i, const QString &uniqueName)
{
    _dirShowWidget = new DirShowWidget(i, QSize(_desktopRect.width(), 290), this);
//    _dirShowWidget->setId(i);
    _dirShowWidget->setMaxRow(_row);
    _dirShowWidget->setContextMenuPolicy(Qt::NoContextMenu);

    _dirShowWidget->setVisible(false);
    _dirShowWidget->setUniqueName(uniqueName);
    _dirShowWidgetList.append(_dirShowWidget);

    _dirShowWidget->showTitle(text);

    connect(_dirShowWidget, SIGNAL(dirWidgetDragLeave(const QString &, int))
            , this, SLOT(hideDirWidget(const QString &, int)));
    connect(_dirShowWidget, SIGNAL(dirWidgetDelIcon(int, const QString &))
            , this, SLOT(dirWidgetDelIcon(int, const QString &)));
    connect(_dirShowWidget, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , this, SIGNAL(iconItemNameChanged(const QString &, const QString &)));
    connect(_dirShowWidget, SIGNAL(refreshDirMinWidget(int))
            , this, SLOT(refreshDirMinWidget(int)));

    _dirShowWidget->getFirstIconItem();
}

void VirtualDesktop::toolBarAddDirShowWidget()
{

    _dustbinDirShowWidget = new DirShowWidget(1000, QSize(_desktopRect.width(), 290), this);
//    _dustbinDirShowWidget->setId(1000);
    _dustbinDirShowWidget->setMaxRow(_row);
    _dustbinDirShowWidget->setContextMenuPolicy(Qt::NoContextMenu);

    //    _dirShowWidget->raise();
    _dustbinDirShowWidget->setVisible(false);
    _dustbinDirShowWidget->showClearBtn();
    _dustbinDirShowWidget->showTitle(QString("废纸篓"));
//    _dirShowWidgetList.append(_dirShowWidget);

    connect(_dustbinDirShowWidget, SIGNAL(dirWidgetDragLeave(const QString &, int))
            , this, SLOT(hideDirWidget(const QString &, int)));
    connect(_dustbinDirShowWidget, SIGNAL(dirWidgetDelIcon(int, const QString &))
            , this, SLOT(dirWidgetDelIcon(int, const QString &)));
    connect(_dustbinDirShowWidget, SIGNAL(dustbinRestore(IconItem*))
            , this, SLOT(dustbinRestore(IconItem*)));


//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;1

//        if (_local->at(i)->dirId() == 1000)
//        {
//            _dustbinDirShowWidget->addDirIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                                              _local->at(i)->page(), _local->at(i)->index(),
//                                              _local->at(i)->url(), _local->at(i)->type().toInt(),
//                                              _local->at(i)->uniqueName());
//        }
//    }
}

void VirtualDesktop::changedDirWidgetTitle(int i, const QString &text)
{
    if (_dirShowWidget)
    {
        for (int j = 0; j < _dirShowWidgetList.count(); j++)
        {
            if (_dirShowWidgetList.at(j)->id() == i)
            {
                _dirShowWidgetList.at(j)->showTitle(text);
            }
        }
    }
}

void VirtualDesktop::refreshDirMinWidget(int i)
{
    for (int k = 0; k < _local->count(); k++) {
        if (_local->at(k)->hidden())
            continue;

        if (_local->at(k)->id().toInt() == i)
        {
            if (_iconDict.value(_local->at(k)->uniqueName()) )
                _iconDict.value(_local->at(k)->uniqueName())->refreshDirMinWidgetIcon();
            else
                emit toolBarRefreshDirMinWidget(_local->at(k)->uniqueName());

            break;
        }
    }
}

void VirtualDesktop::dirWidgetRefresh(int i)
{
    for (int j = 0; j < _dirShowWidgetList.count(); j++)
    {
        if (_dirShowWidgetList.at(j)->id() == i)
        {
            _dirShowWidgetList.at(j)->getFirstIconItem();

            break;
        }
    }
}
//dustbinWidget
void VirtualDesktop::dustbinRestore(IconItem *iconItem)
{
    if (!iconItem)
        return;

    _restoreIcon = iconItem;

    addIcon(iconItem->text(), iconItem->pix()
            , -1, -1
            , iconItem->url(), iconItem->type()
            , iconItem->id(), iconItem->uniqueName()) ;
}
