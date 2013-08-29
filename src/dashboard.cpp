#include <QtGui>
#include <QtSql/QtSql>

#include <iostream>
#include <QFile>

#ifdef LINUX_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif
#include <windows.h>
#include <shldisp.h>

#include "public.h"
#include "ShlObj.h"
#include "idesktopsettings.h"
#include "dashboard.h"
#include "virtualdesktop.h"
#include "switcher.h"
#include "panel.h"
#include "config.h"
#include "appmessagebox.h"

extern QString VacServer;
extern QString VacPort;
extern QString VacUser;
extern QString VacPassword;
extern QString PaasServer;

QList<PAAS_LIST> myPaasList;
QList<PAAS_LIST> remotepaasList;
QList<LOCAL_LIST> remotelocalList;

QString WIN_LOCAL_IconPath;
QString WIN_VAPP_IconPath;
QString WIN_PAAS_IconPath;
QString WIN_TtempPath;
QString iniPath;

extern QString USERNAME;

typedef void (*DLL_getApp2)();
 DLL_getApp2 my_getApp2;

#ifdef Q_WS_WIN
    QLibrary *mylib;   //
    Dll_CloseAppAll m_dllCloseAppAll;
#endif

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool)
    , _outOfScreen(false)
    , _animationUpFinished(false)
    , _animationDownFinished(false)
    , _minUpward(false)
    , _dustbinSelectIconItem(NULL)
{
    _settings = IDesktopSettings::instance();

#ifdef Q_WS_WIN

    mylib= new QLibrary("DllClientEngineMain.dll");   //
    if (mylib->load())              //check whether load successfully
    {
            m_dllCloseAppAll=(Dll_CloseAppAll)mylib->resolve("DllCEM_closeAppAll");
    }
#endif

#ifdef LINUX_X11
    Atom net_wm_state_skip_taskbar = XInternAtom(QX11Info::display(), 
                                        "_NET_WM_STATE_SKIP_TASKBAR", False);
    Atom net_wm_state_skip_pager = XInternAtom(QX11Info::display(), 
                                        "_NET_WM_STATE_SKIP_PAGER", False);
    Atom net_wm_state = XInternAtom(QX11Info::display(), "_NET_WM_STATE", False);
    XChangeProperty(QX11Info::display(), winId(), net_wm_state, 
                    XA_ATOM, 32, PropModeAppend, 
                    (unsigned char *)&net_wm_state_skip_taskbar, 1);
    XChangeProperty(QX11Info::display(), winId(), net_wm_state, 
                    XA_ATOM, 32, PropModeAppend, 
                    (unsigned char *)&net_wm_state_skip_pager, 1);
#else
    HWND hWnd= ::FindWindow("Progman", "Program Manager");
    ::SetParent(winId(),hWnd);
#endif

//    QTextCodec *codec = QTextCodec::codecForName("utf-8"); //System
//    QTextCodec::setCodecForCStrings(codec);

    _ldialog = new LoginDialog(this);
    if (!_ldialog->exec())
        exit(1);

    char folder[MAX_PATH] = {0};
    SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
    WIN_TtempPath = QString(folder);

    WIN_LOCAL_IconPath=WIN_TtempPath+"\\App Center\\Licons\\";
    WIN_VAPP_IconPath=WIN_TtempPath+"\\App Center\\Vicons\\";
    WIN_PAAS_IconPath=WIN_TtempPath+"\\App Center\\Picons\\";
    iniPath = WIN_TtempPath + "\\App Center\\app.ini";
    qDebug()<<"icon path:"<<WIN_VAPP_IconPath;

    QDesktopWidget *d = QApplication::desktop();
    QRect r = d->availableGeometry();
    _width = r.width();
    _height = r.height();

    setGeometry(0, 0, _width, _height);

    vdesktop = new VirtualDesktop(QSize(_width, _height), this);

    panel = new Panel(this);
    panel->setGeometry(0, 0, _width, 28);
    panel->setVisible(false);
//    panel->animationHide();

    _switcherLeft = new Switcher(this);
    _switcherLeft->setPixmap(QString(":images/win_normal.png"));
    _switcherLeft->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);
    _switcherLeft->setGeometry(_width / 2 - _switcherLeft->width(), 0, _switcherLeft->width(), _switcherLeft->height());
    _switcherLeft->setVisible(false);
    _switcherLeft->activateWindow();

    _switcherRight = new Switcher(this);
    _switcherRight->setPixmap(QString(":images/isoft_normal.png"));
    _switcherRight->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);
    _switcherRight->setGeometry(_switcherLeft->pos().x() + _switcherLeft->width(), 0, _switcherRight->width(), _switcherRight->height());
    _switcherRight->setVisible(false);
    _switcherRight->activateWindow();

    vdesktop->setGeometry(QRect(0, 0, _width * vdesktop->count(), _height));

    _toolBarWidget = new toolBarWidget(QSize(_width, 103 + 0 + 24), this);
    _toolBarWidget->move(0,_height - (103 + 0 + 24));//
    _toolBarWidget->raise();
    _toolBarWidget->setVisible(false);

    _pageNodes = new PageNodes(this);

    _pageNodes->update(vdesktop->count(), vdesktop->currentPage());
    _pageNodes->move((_width - _pageNodes->width()) / 2, _height - _pageNodes->height() - 100 - 35);
    _pageNodes->setVisible(false);

    _vacShowWidget = new VacShowWidget(QSize(841, 540), this);
    _vacShowWidget->move((_width - _vacShowWidget->width()) / 2, (_height - _vacShowWidget->height()) / 2);
    _vacShowWidget->setVisible(false);

    _animation = new QPropertyAnimation(this, "pos");

    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT wallpaper FROM wallpapers where id=\'%1\';").arg(1));
    while (query.next())
        _pixmap = QPixmap(query.value(0).toString());

    _finished = false;
    _commui = new commuinication();

    //paas
    _paasFinished = false;
    _paasCommui = new PaasCommuinication();

    _skinShowWidget = new SkinShowWidget(this);
    _skinShowWidget->resize(841, 530);
    _skinShowWidget->move((_width - _skinShowWidget->width()) / 2, (_height - _skinShowWidget->height()) / 2);
    _skinShowWidget->setVisible(false);

    _upMoveWidget = new MoveWidget(this);
    _upMoveWidget->setVisible(false);

    connect(_upMoveWidget, SIGNAL(moveWidgetDrop(IconItem*)), vdesktop, SLOT(moveWidgetDrop(IconItem*)));

    _animationUp = new QPropertyAnimation(_upMoveWidget, "geometry");
    _animationUp->setEasingCurve(QEasingCurve::Linear);
    _animationUp->setDuration(200);

    _downMoveWidget = new MoveWidget(this);
    _downMoveWidget->setVisible(false);

    _minDirLabel = new QLabel(this);
    _minDirLabel->setVisible(false);
    _animationUpMin = new QPropertyAnimation(_minDirLabel, "geometry");
    _animationUpMin->setEasingCurve(QEasingCurve::Linear);
    _animationUpMin->setDuration(200);
    connect(_downMoveWidget, SIGNAL(moveWidgetDrop(IconItem*)), vdesktop, SLOT(moveWidgetDrop(IconItem*)));

    _animationDown = new QPropertyAnimation(_downMoveWidget, "geometry");
    _animationDown->setEasingCurve(QEasingCurve::Linear);
    _animationDown->setDuration(200);
    _downMinW = new MoveMinWidget(this);
    _downMinW->setVisible(false);

    _animationDownMin = new QPropertyAnimation(_downMinW, "geometry");
    _animationDownMin->setEasingCurve(QEasingCurve::Linear);
    _animationDownMin->setDuration(200);

    _normalMenu = new MenuWidget(MenuWidget::normal, this);
    _normalMenu->setVisible(false);
    _normalMenuSize = _normalMenu->getSize();

    _showIconMenu = new MenuWidget(MenuWidget::showIcon, this);
    _showIconMenu->setVisible(false);
    _showIconMenuSize = _showIconMenu->getSize();

    _createMenu = new MenuWidget(MenuWidget::create, this);
    _createMenu->setVisible(false);
    _createMenuSize = _createMenu->getSize();

    _iconMenu = new MenuWidget(MenuWidget::iconMenu, this);
    _iconMenu->setVisible(false);
    _iconMenuSize = _iconMenu->getSize();


    _dustbinMenu = new MenuWidget(MenuWidget::dustbinMenu, this);
    _dustbinMenu->setVisible(false);
    _dustbinMenuSize = _dustbinMenu->getSize();

    _dirMenu = new MenuWidget(MenuWidget::dirMenu, this);
    _dirMenu->setVisible(false);
    _dirMenuSize = _dirMenu->getSize();

    _mask = new Mask(_width, _height, this);
    _mask->raise();
    _mask->setVisible(true);

    _local = LocalAppList::getList();

    connect(_normalMenu, SIGNAL(menuChanged(int)), this, SLOT(menuChanged(int)));
    connect(_normalMenu, SIGNAL(createDir()), this, SLOT(virtualDesktopAddDirItem()));
    connect(_normalMenu, SIGNAL(refresh()), this, SLOT(refreshMenu()));

    connect(_showIconMenu, SIGNAL(largeIcon()), this, SLOT(largeIcon()));
    connect(_showIconMenu, SIGNAL(mediumIcon()), this, SLOT(mediumIcon()));
    connect(_showIconMenu, SIGNAL(smallIcon()), this, SLOT(smallIcon()));

    connect(_createMenu, SIGNAL(createDir()),vdesktop, SLOT(addDirItem()));
    connect(_createMenu, SIGNAL(hideDesktop()), this, SLOT(getOut()));
    connect(_createMenu, SIGNAL(addDesktopLink()), vdesktop, SLOT(appAdd()));

    connect(_iconMenu, SIGNAL(run()), this, SLOT(menuIconRun()));
    connect(_iconMenu, SIGNAL(del()), this, SLOT(menuIconDel()));

    connect(_dirMenu, SIGNAL(open()), this, SLOT(menuDirOpen()));
    connect(_dirMenu, SIGNAL(clear()), this, SLOT(menuDirClear()));
    connect(_dirMenu, SIGNAL(del()), this, SLOT(menuDirDel()));

    connect(_dustbinMenu, SIGNAL(restore()), this, SLOT(dustbinMenuRestoreClicked()));
    connect(_dustbinMenu, SIGNAL(del()), this, SLOT(dustbinMenuDelClicked()));

    connect(vdesktop, SIGNAL(vdesktopNormalMenu(QPoint)), this, SLOT(showNormalMenu(QPoint)));
    connect(vdesktop, SIGNAL(hideMenu()), this, SLOT(hideMenuWidget()));

    connect(vdesktop, SIGNAL(vdesktopShowDirMenu(QPoint, const QString &))
            , this, SLOT(showDirMenu(QPoint, const QString &)));
    connect(vdesktop, SIGNAL(vdesktopShowIconMenu(QPoint, const QString &))
            , this, SLOT(showIconMenu(QPoint, const QString &)));
    connect(vdesktop, SIGNAL(vdesktopShowDustbinMenu(QPoint, IconItem *))
            , this, SLOT(vdesktopShowDustbinMenu(QPoint, IconItem *)));
    connect(_toolBarWidget, SIGNAL(tBarWidgetDirMenu(QPoint, const QString &))
            , this, SLOT(showDirMenu(QPoint, const QString &)));
    connect(_toolBarWidget, SIGNAL(tBarWidgetIconMenu(QPoint, const QString &))
            , this, SLOT(showIconMenu(QPoint, const QString &)));

    connect(_upMoveWidget, SIGNAL(mousePress()), this, SLOT(vdesktopHideDirWidget()));
    connect(_downMoveWidget, SIGNAL(mousePress()), this, SLOT(vdesktopHideDirWidget()));

    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));
    connect(_paasCommui, SIGNAL(done()), this, SLOT(onPaasDone()));
    connect(_skinShowWidget, SIGNAL(setBgPixmap(const QString&)), this, SLOT(setBgPixmap(const QString&)));

    connect(_switcherLeft, SIGNAL(switcherActivated()), this, SLOT(outOfScreen()));
    connect(_switcherRight, SIGNAL(switcherActivated()), this, SLOT(inOfScreen()));
    connect(_switcherLeft, SIGNAL(switcherHover()), panel, SLOT(animationShow()));
    connect(_switcherRight, SIGNAL(switcherHover()), panel, SLOT(animationShow()));
    connect(_switcherLeft, SIGNAL(switcherLeave()), panel, SLOT(animationHide()));
    connect(_switcherRight, SIGNAL(switcherLeave()), panel, SLOT(animationHide()));

    connect(panel, SIGNAL(quit()), this, SLOT(quit()));
    connect(panel, SIGNAL(showSkinWidget()), this, SLOT(onShowSkinDesktop()));
    connect(panel, SIGNAL(showSoftwareWidget()), this, SLOT(onShowVacDesktop()));
    connect(panel, SIGNAL(checkDirState()), vdesktop, SLOT(hideMenuWidget()));

    connect(_ldialog, SIGNAL(dQuit()), this, SLOT(quit()));
    connect(_ldialog, SIGNAL(dShow()), this, SLOT(show()));
    connect(_ldialog, SIGNAL(dHide()), this, SLOT(hide()));
    connect(_ldialog, SIGNAL(pShow()), panel, SLOT(show()));
    connect(_ldialog, SIGNAL(pHide()), panel, SLOT(hide()));

    connect(_ldialog, SIGNAL(dActivated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(vdesktop, SIGNAL(toOrigin()), this, SLOT(switchBetween()));
//    connect(vdesktop, SIGNAL(largeIcon()), this, SLOT(largeIcon()));
//    connect(vdesktop, SIGNAL(mediumIcon()), this, SLOT(mediumIcon()));
//    connect(vdesktop, SIGNAL(smallIcon()), this, SLOT(smallIcon()));
    connect(vdesktop, SIGNAL(desktopDelIcon(const QString &))
            , _vacShowWidget, SLOT(desktopDelIcon(const QString &)));
    connect(vdesktop, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , _vacShowWidget, SLOT(iconItemNameChanged(const QString &, const QString &)));
    connect(vdesktop, SIGNAL(pageChanged(int)), this, SLOT(desktopPageChanged(int)));
    connect(vdesktop, SIGNAL(pageIncreased()), this, SLOT(updateNodes()));
    connect(vdesktop, SIGNAL(pageDecreased()), this, SLOT(updateNodes()));
//    connect(vdesktop, SIGNAL(refreshVac()), this, SLOT(timeOut()));
    connect(vdesktop, SIGNAL(hideDesktop()), this, SLOT(getOut()));
    connect(panel, SIGNAL(setEqual(int)), vdesktop, SLOT(arrangeEqually(int)));
    connect(panel, SIGNAL(setMini()), vdesktop, SLOT(arrangeMinimum()));

    connect(_pageNodes, SIGNAL(choosePage(int)), vdesktop, SLOT(goPage(int)));

    connect(_vacShowWidget, SIGNAL(addApp(const QString&,const QString&, const QString&, int,const QString &)),
            vdesktop, SLOT(addDesktopApp(const QString&,const QString&, const QString&, int, const QString &))); //
    connect(_vacShowWidget, SIGNAL(delItem(QString)), vdesktop, SLOT(vacWidgetDelIcon(const QString &)));
    connect(_skinShowWidget, SIGNAL(skinCloseBtnClicked()), _skinShowWidget, SLOT(hide()));
    connect(_vacShowWidget, SIGNAL(vacCloseBtnClicked()), _vacShowWidget, SLOT(hide()));
    //move
    connect(vdesktop, SIGNAL(upMove(int,int,int,int,int)),
            this, SLOT(upMove(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(upBackMove(int, int, int, int, int)),
            this, SLOT(upBackMove(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(desktopOpenMove(int ,int ,int, int, int)),
            this, SLOT(downMove(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(desktopCloseMove(int, int, int, int, int)),
            this, SLOT(downBackMove(int ,int ,int, int, int)));

    connect(vdesktop, SIGNAL(openMinWidget(int ,int ,int, int, int)),
            this, SLOT(openMinWidget(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(closeMinWidget(int, int, int, int, int)),
            this, SLOT(closeMinWidget(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(upMinMove(int,int,int,int,int)),
            this, SLOT(upMinMove(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(upMinBackMove(int,int,int,int,int)),
            this, SLOT(upMinBackMove(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(setMinMoveLabel(bool)), this, SLOT(setMinMoveLabel(bool)));

    connect(vdesktop, SIGNAL(desktopClicked()), this, SLOT(desktopClicked()));

    connect(vdesktop, SIGNAL(delToolBarWidgetIcon(const QString &))
            , _toolBarWidget, SLOT(moveOutIcon(const QString&)));
    connect(vdesktop, SIGNAL(delToolBarIcon(const QString &))
            , _toolBarWidget, SLOT(delIcon(const QString&)));
    connect(vdesktop, SIGNAL(toolBarRemoveDirMinItem(const QString &, int))
            , _toolBarWidget, SLOT(toolBarRemoveDirMinItem(const QString &, int)));
    connect(vdesktop, SIGNAL(toolBarRefreshDirMinWidget(const QString &))
            , _toolBarWidget, SLOT(toolBarRefreshDirMinWidget(const QString &)));
    connect(vdesktop, SIGNAL(changedDirId(const QString&, int))
            , _toolBarWidget, SLOT(changedDirId(const QString&, int)));

    connect(_animationDown,SIGNAL(finished()), this, SLOT(animationFinished()));
    connect(_animationDownMin,SIGNAL(finished()), this, SLOT(animationMinDownFinished()));
    connect(_animationUp,SIGNAL(finished()), this, SLOT(animationFinished()));

    connect(_animationDownMin, SIGNAL(valueChanged(const QVariant&)), this, SLOT(valueChanged(const QVariant&)));

    connect(_toolBarWidget, SIGNAL(iconDropToolWidget(const QString &))
            , vdesktop, SLOT(iconMoveOtherWidget(const QString &)));
    connect(_toolBarWidget, SIGNAL(toolOpenDir(int,int,int)), vdesktop, SLOT(toolBarOpenDir(int,int,int)));
    connect(_toolBarWidget, SIGNAL(toolBarIconToDustbin(const QString &,const QString &, int, int, const QString &, int, const QString &))
            , vdesktop, SLOT(toolBarIconToDustbin(const QString &, const QString &, int, int, const QString &, int, const QString &)));
    connect(_toolBarWidget, SIGNAL(toolBarIconToDir(int, const QString &,const QString &, int, int, const QString &, int, const QString &))
            , vdesktop, SLOT(toolBarIconToDir(int, const QString &,const QString &, int, int, const QString &, int, const QString &)));
    connect(_toolBarWidget, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , _vacShowWidget, SLOT(iconItemNameChanged(const QString &, const QString &)));
    connect(_toolBarWidget, SIGNAL(changedDirWidgetTitle(int, const QString &))
            , vdesktop, SLOT(changedDirWidgetTitle(int, const QString &)));
    connect(_toolBarWidget, SIGNAL(toolBarDirWidgetRefresh(int))
            , vdesktop, SLOT(dirWidgetRefresh(int)));

    _nam = new QNetworkAccessManager(this);

    QTimer::singleShot(100, this, SLOT(initIconItem()));
}

void Dashboard::initIconItem()
{
#if 1
    _mask->setText(tr("加载本地应用..."));
    getLocalIcon();
    _mask->setText(tr("加载虚拟应用..."));
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //vac
    _ldialog->updateVacServer();
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, GetSystemInfo());
    while (!_finished)
        QApplication::processEvents();
    _finished = false;
//    qDebug()<<"login3";
    getVacIcon();
    _mask->setText(tr("加载平台服务应用..."));
    getPaasIcon(true);

    _mask->setText(tr("加载软件库..."));
    _vacShowWidget->initIconItem();
    _mask->setText(tr("加载壁纸库..."));
    _skinShowWidget->initIconItem();
    _mask->setText(tr("加载桌面..."));
    vdesktop->initIconItem();
    _toolBarWidget->initIconItem();
#endif

    _mask->setVisible(false);
    _mask->setText(tr(""));
    panel->setVisible(true);
    panel->animationHide();
    _toolBarWidget->setVisible(true);
    _pageNodes->setVisible(true);
    _switcherLeft->setVisible(true);
    _switcherRight->setVisible(true);
    _switcherLeft->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);
    _switcherRight->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);
}

//page nodes
void Dashboard::desktopPageChanged(int page)
{
    _pageNodes->setCurrent(page);
}

void Dashboard::updateNodes()
{
    _pageNodes->setVisible(false);
    _pageNodes->update(vdesktop->count(), vdesktop->currentPage());
    _pageNodes->move(( _width - _pageNodes->width()) / 2, _height - _pageNodes->height() - 100 - 35);
    _pageNodes->setVisible(true);
}

//dir open and close

void Dashboard::vdesktopHideDirWidget()
{
    vdesktop->hideDirWidget("", -1);
}

void Dashboard::desktopClicked()
{
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);
    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();
}

void Dashboard::setMinMoveLabel(bool up)
{
    QPixmap minDirPix;
    if (up)
    {
        _minMoveLabelUp = true;
        minDirPix.load(":/images/min_mask_icon.png");
        _downMinW->setPixmapPointF(true);

    }
    else
    {
        _minMoveLabelUp = false;
        minDirPix.load(":/images/min_toolbar_mask_icon.png");
        _downMinW->setPixmapPointF(false);
    }
    _minDirLabel->setPixmap(minDirPix);
}

void Dashboard::upMove(int x, int y, int w, int h, int distance)
{
    if (_animationUp->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _upMoveWidget->resize(QSize(w, h));

    QPixmap result = QPixmap();
    result = QPixmap::grabWindow(this->winId(), \
                                 x, y, \
                                 w, h); //抓取当前屏幕的图片

    _upMoveWidget->setPixmap(result);
    _upMoveWidget->setVisible(true);

    _animationUp->setStartValue(QRect(x, y, w, h));
    _animationUp->setEndValue(QRect(x, y - distance, w, h));
    _animationUp->start();

    _animationUpFinished = false;
}

void Dashboard::upBackMove(int x, int y, int w, int h, int distance)
{
    if (_animationUp->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animationUp->setStartValue(QRect(x, y, w, h));
    _animationUp->setEndValue(QRect(x, y + distance, w, h));
    _animationUp->start();


    _animationUpFinished = true;

}

void Dashboard::animationFinished()
{
    if ((!_animationDownFinished) && (!_animationUpFinished))
    {
        vdesktop->dirMovingFinished();
    }

    if (_animationDownFinished && _animationUpFinished)
    {
        vdesktop->setDirHide();
        _downMinW->setVisible(false);
        _minDirLabel->setVisible(false);
        _upMoveWidget->setVisible(false);

        if (!panel->isVisible())
        {
            panel->setVisible(true);
            panel->show();
            panel->setAutoHide(true);
            panel->animationHide();

            _switcherLeft->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);
            _switcherRight->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);
        }

        _animationUpFinished = false;

        _pageNodes->setVisible(true);
        vdesktop->setIconMove(true);
        _toolBarWidget->setVisible(true);

        _downMoveWidget->setVisible(false);

        vdesktop->dirMovingFinished();
    }
}

void Dashboard::downMove(int x, int y, int w, int h, int distance)
{
    hideMenuWidget();
    if (_animationDown->state() == QAbstractAnimation::Running)
    {
        return;
    }

    panel->setVisible(false);
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);
    _pageNodes->setVisible(false);
    _downMoveWidget->resize(QSize(w, h));

    QPixmap result = QPixmap();
    result = QPixmap::grabWindow(this->winId(), \
                                 x, y, \
                                 w, h); //抓取当前屏幕的图片
    _downMoveWidget->setPixmap(result);
    _downMoveWidget->setVisible(true);
    _toolBarWidget->setVisible(false);

    _animationDown->setStartValue(QRect(x, y, w, h));
    _animationDown->setEndValue(QRect(x, y + distance, w, h));
    _animationDown->start();
    _animationDownFinished = false;
}

void Dashboard::downBackMove(int x, int y, int w, int h, int distance)
{
    hideMenuWidget();
    if (_animationDown->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animationDown->setStartValue(QRect(x, y, w, h));
    _animationDown->setEndValue(QRect(x, y - distance, w, h));
    _animationDown->start();

    _animationDownFinished = true;

}

void Dashboard::upMinMove(int x, int y, int w, int h, int distance)
{

    if (_minMoveLabelUp)
    {
        if (_animationUpMin->state() == QAbstractAnimation::Running)
        {
            return;
        }
        _minDirLabel->setGeometry(x, y + 2, w, h);
        _minDirLabel->setVisible(true);

        _animationUpMin->setStartValue(QRect(x, y + 2, w, h));
        _animationUpMin->setEndValue(QRect(x, y - distance + 2, w, h));
        _animationUpMin->start();
    }
    else
    {
        if (_animationDownMin->state() == QAbstractAnimation::Running)
        {
            return;
        }

        _distance = distance;
        _minY = y;
        _minUpward = false;

        _downMinW->resize(QSize(w, h));

        QPixmap result = QPixmap();
        result = QPixmap::grabWindow(this->winId(), \
                                     x, y, \
                                     w, h); //抓取当前屏幕的图片
        _downMinW->setPixmap(result);

        _animationDownMin->setStartValue(QRect(x, y, w, h));
        _animationDownMin->setEndValue(QRect(x, y - distance, w, h));
        _animationDownMin->start();

    }
}

void Dashboard::upMinBackMove(int x, int y, int w, int h, int distance)
{
    if (_minMoveLabelUp)
    {
        if (_animationUpMin->state() == QAbstractAnimation::Running)
        {
            return;
        }

        _animationUpMin->setStartValue(QRect(x, y + 2, w, h));
        _animationUpMin->setEndValue(QRect(x, y + 2 + distance, w, h));
        _animationUpMin->start();
    }
    else
    {
        if (_animationDownMin->state() == QAbstractAnimation::Running)
        {
            return;
        }

            _minY = y;
            _minUpward = true;

            _animationDownMin->setStartValue(QRect(x, y, w, h));
            _animationDownMin->setEndValue(QRect(x, y + distance, w, h));
            _animationDownMin->start();

    }
}

void Dashboard::openMinWidget(int x, int y, int w, int h, int distance)
{
    if (_minMoveLabelUp)
    {
        if (_animationDownMin->state() == QAbstractAnimation::Running)
        {
            return;
        }

        _distance = distance;
        _minY = y;
        _minUpward = false;

        _downMinW->resize(QSize(w, h));

        QPixmap result = QPixmap();
        result = QPixmap::grabWindow(this->winId(), \
                                     x, y, \
                                     w, h); //抓取当前屏幕的图片
        _downMinW->setPixmap(result);

        _animationDownMin->setStartValue(QRect(x, y, w, h));
        _animationDownMin->setEndValue(QRect(x, y + distance, w, h));
        _animationDownMin->start();
    }
    else
    {
        if (_animationUpMin->state() == QAbstractAnimation::Running)
        {
            return;
        }

        _minDirLabel->setGeometry(x, y + 2, w, h);
        _minDirLabel->setVisible(true);

        _animationUpMin->setStartValue(QRect(x, y - 2, w, h));
        _animationUpMin->setEndValue(QRect(x, y + distance - 2, w, h));
        _animationUpMin->start();

    }
}

void Dashboard::closeMinWidget(int x, int y, int w, int h, int distance)
{

    if (_minMoveLabelUp)
    {
        if (_animationDownMin->state() == QAbstractAnimation::Running)
        {
            return;
        }

        _minY = y;
        _minUpward = true;

        _animationDownMin->setStartValue(QRect(x, y, w, h));
        _animationDownMin->setEndValue(QRect(x, y - distance, w, h));
        _animationDownMin->start();
    }
    else
    {
        if (_animationUpMin->state() == QAbstractAnimation::Running)
        {
            return;
        }
            _animationUpMin->setStartValue(QRect(x, y + 15 - 2, w, h));
            _animationUpMin->setEndValue(QRect(x, y + 15 - distance - 2, w, h));
            _animationUpMin->start();
    }
}

void Dashboard::animationMinDownFinished()
{
    _downMinW->setVisible(false);
}

void Dashboard::valueChanged(const QVariant &value)
{
    QRect rect = value.value<QRect>();

    if (((rect.y() - _minY) > 0 ? rect.y() - _minY : _minY - rect.y()) >= _distance / 2)
    {
        _downMinW->setVisible(_minUpward);
    }
    else
    {
        _downMinW->setVisible(_minUpward);
    }

}

void Dashboard::onDone()
{
    //get the Application List;
    _finished = true;
}

void Dashboard::goPage(int page)
{
    vdesktop->setVisible(true);
    vdesktop->goPage(page);
}

void Dashboard::onShowVacDesktop()
{
    hideMenuWidget();
    _skinShowWidget->setVisible(false);
    if (!_vacShowWidget->isVisible())
    {
        _vacShowWidget->movetoFirst();
    }
    _vacShowWidget->setVisible(!_vacShowWidget->isVisible());
}

void Dashboard::onShowSkinDesktop()
{
    hideMenuWidget();
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(!_skinShowWidget->isVisible());
}

void Dashboard::getIn()
{
    hideMenuWidget();

    QPoint start;
    QPoint end;
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    start = pos();
    end = QPoint(0, 0);
    _animation->setDuration(300);
    _animation->setEasingCurve(QEasingCurve::InExpo);
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _outOfScreen = !_outOfScreen;
    _animation->start();
}

void Dashboard::getOut()
{
    hideMenuWidget();

    QPoint start;
    QPoint end;
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    start = pos();
    end = QPoint(_width, 0);
    _animation->setDuration(900);
    _animation->setEasingCurve(QEasingCurve::OutElastic);
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _outOfScreen = !_outOfScreen;
    _animation->start();
}

void Dashboard::switchBetween()
{
    hideMenuWidget();

    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

    qDebug() <<"********************_outOfScreen"<< _outOfScreen;
    if (_outOfScreen)
        getIn();
    else
        getOut();
}

void Dashboard::inOfScreen()
{
    _skinShowWidget->setVisible(false);
    _vacShowWidget->setVisible(false);

    if (_outOfScreen)
        getIn();
}

void Dashboard::outOfScreen()
{
    _skinShowWidget->setVisible(false);
    if (!_outOfScreen)
        getOut();
}

void Dashboard::iconActivated(QSystemTrayIcon::ActivationReason)
{ 
    _ldialog->_dShowAction->setEnabled(true);
    _ldialog->_dHideAction->setEnabled(true);
    if (this->isVisible() && !isMinimized())
        _ldialog->_dShowAction->setEnabled(false);
    else
        _ldialog->_dHideAction->setEnabled(false);
}

void Dashboard::quit()
{
    hideMenuWidget();

    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();

    AppMessageBox box(true, NULL);
    box.setText("是否确定退出？");
    if (box.exec()) {

        QString ip;
        QSqlQuery ipQuery = \
                QSqlDatabase::database("local").exec(QString("SELECT verifyServer FROM paasservers where id=1;"));
        while (ipQuery.next())
        {
            ip = ipQuery.value(0).toString();
        }
        qDebug() << "-----------USERNAME-------------->" << USERNAME;
        qDebug() << "-------------ip------------>" << ip;


        QString quitUrl ="http://" + ip + ":8080/idesktop/logout.action";
        qDebug() << "-------------quitUrl------------>" << quitUrl;

        QString data = "username=" + USERNAME;

        _nam->post(QNetworkRequest(QUrl(quitUrl)), data.toAscii());

//        QString jsonUrl = "http://192.168.30.37:9080/idesktop/saveUserStatusData.action";
//        QString dataJson = "username=" + USERNAME + "&jsondata=" + LocalAppList::getList()->uploadJson();
//        _nam->post(QNetworkRequest(QUrl(jsonUrl)), dataJson.toAscii());

        _switcherLeft->setVisible(false);
        _switcherRight->setVisible(false);
        vdesktop->setVisible(false);
        this->hide();
        vdesktop->atExit();

        _commui->logoff();
        while (!_finished)
        {
            QApplication::processEvents();
        }

        qApp->quit();
    }

} 
    
void Dashboard::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

void Dashboard::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
//    QPixmap background(_pixmap);
//    QPixmap resizeBackgroud=background.scaled(event->size());
//    QPalette palette;
//    palette.setBrush(backgroundRole(), QBrush(resizeBackgroud));
//    setPalette(palette);
}

void Dashboard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    painter.drawPixmap(0, 0, _width, _height, _pixmap);
}

//miya add
typedef bool (*DLL_Changebg)(char* path, int flag);
DLL_Changebg changebg;

void Dashboard::setBgPixmap(const QString &pixText)
{
    _pixText = pixText;

    _pixmap.load(_pixText);
    repaint();

    QSqlQuery query(QSqlDatabase::database("local"));

    QString qstr = QString("update wallpapers "\
                           "set wallpaper=\'%1\' where id=\'%2\';")\
                           .arg(_pixText)\
                           .arg(1);
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return;
    }

    setBgImage(pixText);
}

//miya add change background imgae
void Dashboard::setBgImage(QString url)
{
    qDebug() << "Dashboard::setBgImage(QString url)" << url;
//    QString path = QCoreApplication::applicationDirPath();
//    url.replace(QString(":"), QString(""));
//    qDebug() << url;
//    qDebug() << path;
//    path += url;
//    path.replace(QString("/"), QString("\\"));

    QString path = url;

    QLibrary lib("changebg.dll");

    if(lib.load()) {
        qDebug() << "success load dll";
        if((changebg = (DLL_Changebg)lib.resolve("SetMyWallpaper"))) {
            qDebug() << "success find func";
            changebg(path.toLatin1().data() , 2);       //changebg(path.toLatin1().data() , 0);
        } else {
            qDebug() << "failed find func";
        }
    } else {
        qDebug() << "GetLastError :" << GetLastError();
        qDebug() << "failed load dll";
    }
}

void Dashboard::errOut()
{
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, GetSystemInfo());

    while (!_finished)
        QApplication::processEvents();
    _finished = false;
}

void Dashboard::refreshVapp()
{
    qDebug() << "refresh Vapp";

    _settings->vappList().clear();

    //get vapp list
    _commui->getAppList();
    while (!_finished)
        QApplication::processEvents();
    _finished = false;

    QList<APP_LIST> myVappList = _commui->getList();
    qDebug()<<"myList.count()="<<myVappList.count();

    QDir iconDir(WIN_VAPP_IconPath);
    if(!iconDir.exists())
    {
        iconDir.mkdir(WIN_VAPP_IconPath);
    }
    //store ico file locally
    for(int i = 0; i < myVappList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(WIN_VAPP_IconPath)
                .arg(myVappList[i].id);
        QString tempPath = QString("%1%2.ico")
                .arg(WIN_VAPP_IconPath)
                .arg(myVappList[i].id);

        //check if ico file is existed, or dont donwload
        QFile chkFile(iconPath);
        if(chkFile.exists())
        {
            chkFile.close();
            continue;
        }
        chkFile.close();

        //qDebug()<<"iconPath"<<iconPath;
        _commui->downloadIcon(QUrl(myVappList[i].icon), tempPath);
        while (!_finished)
            QApplication::processEvents();
        _finished = false;

        setIcon(WIN_VAPP_IconPath, tempPath);
    }

    _settings->setVappList(myVappList);

    if (!_commui->_isNetTimeout)
        modify();
}

void Dashboard::refreshMenu()
{
    hideMenuWidget();

    _switcherLeft->setVisible(false);
    _switcherRight->setVisible(false);
    panel->setVisible(false);

    _mask->setVisible(true);
    _mask->setText(tr("正在刷新虚拟应用..."));
    refreshVapp();
    _mask->setText(tr("正在刷新平台服务应用..."));
    refreshPaas();
    _mask->setVisible(false);
    _mask->setText(tr(""));

    panel->setVisible(true);
    _switcherLeft->setVisible(true);
    _switcherRight->setVisible(true);
    _switcherLeft->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);
    _switcherRight->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool);

    _vacShowWidget->movetoFirst();
}

void Dashboard::modify()
{
    QList<APP_LIST>& myVappList = _settings->vappList();
    QList<APP_LIST>& remoteAppList = _settings->remoteAppList();

    if (myVappList.count() > 0 || remoteAppList.count() > 0)
    {
    //delete
        for (int i = 0; i < remoteAppList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < myVappList.count(); j++)
            {
                if (remoteAppList[i].id == myVappList[j].id)
                {
                    isExist = true;
                    break;
                }
            }
            if(!isExist)
            {
                //delete
                _vacShowWidget->delIcon("1_" + remoteAppList[i].id);
				LocalAppList::getList()->delApp("1_" + remoteAppList[i].id);
            }
        }
        // add
        for (int i = 0; i < myVappList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < remoteAppList.count(); j++)
            {
                if (myVappList[i].id == remoteAppList[j].id)
                {
                    isExist = true;
                    break;
                }
            }

            if (!isExist)
            {
                _vacShowWidget->addIcon(myVappList[i].name, WIN_VAPP_IconPath + myVappList[i].id + ".png", \
                                        vdesktop->count() - 1, -1, QString(""), 1, "1_" + myVappList[i].id);
            }
        }
    }

    remoteAppList.clear();

    for(int i = 0; i < myVappList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(WIN_VAPP_IconPath)
                .arg(myVappList[i].id);

        remoteAppList.insert(i, myVappList[i]);
        remoteAppList[i].icon = iconPath;
        qDebug()<<"g_Rmote:"<<remoteAppList.at(i).icon;
    }

}

void Dashboard::refreshPaas()
{
    qDebug() << "refresh paas";
    QList<PAAS_LIST>& myPaasList = _settings->paasList();
    myPaasList.clear();

    getPaasIcon(false);

    if (!_paasCommui->_isNetTimeout)
        paasModify();
}

void Dashboard::paasModify()
{
    QList<PAAS_LIST>& myPaasList = _settings->paasList();
    QList<PAAS_LIST>& remotePaasList = _settings->remotePaasList();

    if (myPaasList.count() > 0 || remotePaasList.count() > 0)
    {
    //delete
        for (int i = 0; i < remotePaasList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < myPaasList.count(); j++)
            {
                if (remotePaasList[i].name == myPaasList[j].name)
                {
                    isExist = true;
                    break;
                }
            }
            if(!isExist)
            {
                //delete
                _vacShowWidget->delIcon("2_" + remotePaasList[i].name);
				LocalAppList::getList()->delApp("2_" + remotePaasList[i].name);
            }
        }
        // add
        for (int i = 0; i < myPaasList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < remotePaasList.count(); j++)
            {
                if (myPaasList[i].name == remotePaasList[j].name)
                {
                    isExist = true;
                    break;
                }
            }

            if (!isExist)
            {
                _vacShowWidget->addIcon(myPaasList[i].cnName, myPaasList[i].iconPath, \
                                        - 1, -1, myPaasList[i].urls, 2, "2_" + myPaasList[i].name);
            }
        }
    }

    remotePaasList.clear();

    for(int i = 0; i < myPaasList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(WIN_PAAS_IconPath)
                .arg(myPaasList[i].cnName);

        remotePaasList.insert(i, myPaasList[i]);
        remotePaasList[i].iconPath = iconPath;
    }
}

void Dashboard::largeIcon()
{
    hideMenuWidget();
    vdesktop->setLargeIcon();
    _vacShowWidget->largeIcon();
}

void Dashboard::mediumIcon()
{
    hideMenuWidget();
    vdesktop->setMediumIcon();
    _vacShowWidget->mediumIcon();
}

void Dashboard::smallIcon()
{
    hideMenuWidget();
    vdesktop->setSmallIcon();
    _vacShowWidget->smallIcon();
}

void Dashboard::onPaasDone()
{
    //get the Application List;
    _paasFinished = true;
}

void Dashboard::getPaasIcon(bool isLogin)
{

    //paas
    //get paas list
    _paasCommui->login(PaasServer);
    while (!_paasFinished)
        QApplication::processEvents();
    _paasFinished = false;

    myPaasList = _paasCommui->getList();

    if (myPaasList.count() == 0)
    {
        return;
    }

    QDir iconDir(WIN_PAAS_IconPath);
    if(!iconDir.exists())
    {
        iconDir.mkdir(WIN_PAAS_IconPath);
    }
    //store ico file locally
    for(int i = 0; i < myPaasList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(WIN_PAAS_IconPath)
                .arg(myPaasList[i].cnName);
        QString tempPath = QString("%1%2.png")
                .arg(WIN_PAAS_IconPath)
                .arg(myPaasList[i].cnName);
        //        qDebug()<<"iconPath="<<iconPath;
        myPaasList[i].iconPath = iconPath;

        if (isLogin)
        {
            remotepaasList.insert(i, myPaasList[i]);
            remotepaasList[i].iconPath = iconPath;
        }
        //check if ico file is existed, or dont donwload

        QFile chkFile(iconPath);
        if(chkFile.exists())
        {
            chkFile.close();
            continue;
        }
        chkFile.close();

        //qDebug()<<"iconPath"<<iconPath;
        if (myPaasList[i].logoURL.isEmpty())
        {
            //url = myPaasList.at(i).urls.section('/', 1, 1, flag);
            //url = QString("http://" + url + "/Favicon.ico");

            //_paasCommui->downloadIcon(QUrl(url), tempPath);


            QPixmap pix(":images/url_normal.png");
            pix.save(iconPath, "PNG", -1);

            continue;
        }
        else
        {
            _paasCommui->downloadIcon(QUrl(myPaasList[i].logoURL), tempPath);

            while (!_paasFinished)
                QApplication::processEvents();
            _paasFinished = false;
        }

        setIcon(WIN_PAAS_IconPath, tempPath);
    }
    _settings->setPaasList(myPaasList);
    _settings->setRemotePaasList(remotepaasList);
}

void Dashboard::getVacIcon()
{
    if(_commui->errID == "10000")
    {
        //        char folder[MAX_PATH] = {0};
        //        SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
        //        WIN_TtempPath = QString(folder);
        //        WIN_VAPP_IconPath=WIN_TtempPath+"\\App Center\\Vicons\\";

        //get vapp list

        _commui->getAppList();
        while (!_finished)
            QApplication::processEvents();
        _finished = false;

        QList<APP_LIST> myVappList;
        myVappList = _commui->getList();
        qDebug()<<"g_myList.count()="<<myVappList.count();
        _settings->setVappList(myVappList);

        QList<APP_LIST> remoteAppList;

        QDir iconDir(WIN_VAPP_IconPath);
        if(!iconDir.exists())
        {
            iconDir.mkdir(WIN_VAPP_IconPath);
        }
        //store ico file locally
        for(int i = 0; i < myVappList.count(); i++)
        {
            QString iconPath = QString("%1%2.png")
                    .arg(WIN_VAPP_IconPath)
                    .arg(myVappList[i].id);
            QString tempPath = QString("%1%2.ico")
                    .arg(WIN_VAPP_IconPath)
                    .arg(myVappList[i].id);

            remoteAppList.insert(i, myVappList[i]);
            remoteAppList[i].icon = iconPath;

            //check if ico file is existed, or dont donwload
            QFile chkFile(iconPath);
            if(chkFile.exists())
            {
                chkFile.close();
                continue;
            }
            chkFile.close();

            //qDebug()<<"iconPath"<<iconPath;
            _commui->downloadIcon(QUrl(myVappList[i].icon), tempPath);
            while (!_finished)
                QApplication::processEvents();
            _finished = false;

            setIcon(WIN_VAPP_IconPath, tempPath);
        }
        _settings->setRemoteAppList(remoteAppList);
    }
    else
    {
        if ((_commui->errInfo == "会话已存在") || (_commui->errID == "10045"))
        {
            _commui->logoff();

            _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, GetSystemInfo());

            while (!_finished)
                QApplication::processEvents();
            _finished = false;

            getVacIcon();

        }
        else if ((_commui->errInfo == "没有可用Licenses") || (_commui->errID == "10059"))
        {
            AppMessageBox box(false, NULL);
            box.setText(tr("no used Licenses, please contact the administrator."));
            box.exec();
        }
        else if ((_commui->errInfo == "用户名或密码错误") || (_commui->errID == "10062"))
        {
            AppMessageBox box(false, NULL);
            box.setText(tr("name or password error, please contact the administrator."));
            box.exec();
        }
        else
        {
//            QMessageBox::warning(this, tr("vapp login failed"), _commui->errInfo, tr("OK"));

            AppMessageBox box(false, NULL);
            box.setText(tr("vapp login failed, please contact the administrator."));
            box.exec();
        }

    }

}

void Dashboard::getLocalIcon()
{
    //local
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QLibrary dllLib("GetApp.dll");
    if(!dllLib.load())
    {
        qDebug()<< "GetLastError :" << GetLastError();
        qDebug()<<"load failed!";
    }
    else
    {
        qDebug()<<"load succeed!";
        my_getApp2 = (DLL_getApp2)dllLib.resolve("GetApp2");
        if(my_getApp2 == NULL)
        {
            qDebug()<< "GetLastError :" <<GetLastError();
            qDebug()<<"resolve failed!";

        }
        else
        {
            qDebug() << "my_getApp2() start";
            my_getApp2();
            qDebug() << "my_getApp2() end!!!";
        }
    }
//    qDebug()<<"inipath:"<<iniPath;
    //vc ini
    char  chSectionNames[8192]={0};       //所有节名组成的字符数组
    char *pSectionName; //保存找到的某个节名字符串的首地址
    int i;       //i指向数组chSectionNames的某个位置，从0开始，顺序后移
    int j=0;      //j用来保存下一个节名字符串的首地址相对于当前i的位置偏移量
    int _id = 0;
    //int count=0;      //统计节的个数
    char _folder[MAX_PATH] = {0};
    char _iniPath[MAX_PATH] = {0};
    SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,_folder);
    sprintf(_iniPath, "%s\\App Center\\app.ini", _folder);
    DWORD m_lRetValue =::GetPrivateProfileSectionNames(chSectionNames,8192,_iniPath);
    qDebug()<<"num:"<<m_lRetValue;
    for(i=0;i<m_lRetValue;i++,j++)
    {
        if(chSectionNames[0]=='\0')
            break;       //如果第一个字符就是0，则说明ini中一个节也没有
        if(chSectionNames[i]=='\0')
        {
            pSectionName=&chSectionNames[i-j]; //找到一个0，则说明从这个字符往前，减掉j个偏移量，
            //就是一个节名的首地址
            j=-1;         //找到一个节名后，j的值要还原，以统计下一个节名地址的偏移量
            //赋成-1是因为节名字符串的最后一个字符0是终止符，不能作为节名
            //的一部分
            char strBuff[256];
            //在获取节名的时候可以获取该节中键的值，前提是我们知道该节中有哪些键。
            ::GetPrivateProfileString(pSectionName,"ExePath",NULL,strBuff,256,_iniPath);
            //qDebug()<<"name:"<<QString::fromLocal8Bit(pSectionName);     //把找到的显示出来
            //qDebug()<<"name:"<<QString(pSectionName).toLocal8Bit();
            //qDebug()<<"path:"<<QString::fromLocal8Bit(strBuff);
            //QString path = QString::fromLocal8Bit(strBuff);
            QString path = strBuff;

            QFileInfo fio(path);
            if (!fio.exists())
                continue;

//            qDebug()<<"path"<<strBuff;
            QFileInfo fi = QFileInfo(path);
            QString iPath(Config::get("IconDir"));
            iPath = iPath + fi.baseName();
            iPath += ".png"; //png

            QString localIconPath;

            QFile chkFile(iPath);
            if(!chkFile.exists())
            {
                chkFile.close();

                localIconPath = getLocalIcon(path);
                setIcon(WIN_LOCAL_IconPath, localIconPath);
                //            continue;
            }
            else
            {
                chkFile.close();
                localIconPath = iPath;
            }

            LOCAL_LIST tempLocalList;
            tempLocalList.id = _id;
            tempLocalList.iconPath = localIconPath;
            //qDebug() << "name-->pSectionName" << pSectionName;
//            tempLocalList.name = QString::fromLocal8Bit(pSectionName);


            QTextCodec *codec = QTextCodec::codecForName("utf-8"); //utf-8

            QString temp = codec->toUnicode(pSectionName);
//            QString temp = pSectionName;
            temp.replace("]", "");
            tempLocalList.name = temp;
//            qDebug() << "tempLocalList.name" << tempLocalList.name;

            //tempLocalList.name = QString(pSectionName);
            tempLocalList.execname = path;

            QString md5;
            QByteArray bb;
            bb = QCryptographicHash::hash(path.toLower().toAscii(), \
                                          QCryptographicHash::Md5);
            md5.append(bb.toHex());

            tempLocalList.uniqueName = md5;

            remotelocalList.append(tempLocalList);
            _id++;
            if(chSectionNames[i+1]==0)
            {
                break;      //当两个相邻的字符都是0时，则所有的节名都已找到，循环终止
            }
        }
    }
    _settings->setRemoteLocalList(remotelocalList);

    qDebug() << "load " << _settings->remoteLocalList().size() << " local apps";

    QFile b(iniPath);
    b.remove();
    //end

}

QString Dashboard::getLocalIcon(QString localPath)
{
    typedef HICON (*tempFuc)(CONST TCHAR *filePath);
    QLibrary myLib;
#ifdef DEBUG
    myLib.setFileName("IconGetD.dll");
#else
    myLib.setFileName("IconGet.dll");
#endif

    tempFuc myFunction = (tempFuc) myLib.resolve("getJumbIcon");
    if (myFunction) {
        HICON jumbIcon = myFunction((CONST TCHAR *)localPath.utf16 ());

        QPixmap picon = QPixmap::fromWinHICON(jumbIcon);

        QFileInfo info = QFileInfo(localPath);
        QString path(Config::get("IconDir"));
        path = path + "\\" + info.baseName();
        path += ".png"; //png
        QPixmap newicon =  picon.scaled(59, 59, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        newicon.save(path, "PNG",-1);
        return path;
    }
    return "";

}

void Dashboard::setIcon(const QString &dirPath, const QString &iconPath)
{
    QString newApp = iconPath;

    if (newApp.isEmpty())
        return;

    QImage image = QImage(newApp).scaled(59, 59, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage normal = QImage(":images/icon_shadow.png");
    QImage middle = QImage(":images/icon_middle_shadow.png");

    QPainter pt1(&normal);
    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
    pt1.drawImage(QRect(35, 36, 72, 72), middle.scaled(72, 72, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    pt1.drawImage(QRect(35 + 7, 36 + 3, 59, 59), image);
    pt1.end();

    QFileInfo info = QFileInfo(newApp);
    if (newApp.right(4) == ".ico")
    {
        if (info.exists())
            QFile::remove(newApp);//刪除 .ico 文件

        QString path = dirPath + info.baseName();
        path += ".png";

        QPixmap pix = QPixmap::fromImage(normal);
        pix.save(path, "PNG", -1);
    }
    else
    {
        QPixmap pix = QPixmap::fromImage(normal);
        pix.save(newApp, "PNG", -1);
    }

}

QString Dashboard::GetSystemInfo()
{
    //login ivapp sm
    QString sysInfo;
#ifdef Q_WS_WIN
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_2000:
        qDebug()<<"System info:windows 2000 \n";
        sysInfo="windows 2000";
        break;
    case QSysInfo::WV_2003:
        qDebug()<<"System info:windows 2003 \n";
        sysInfo="windows 2003";
        break;
    case QSysInfo::WV_XP:
        qDebug()<<"System info:windows xp \n";
        sysInfo="windows xp";
        break;
    case QSysInfo::WV_VISTA:
        qDebug()<<"System info:windows vista \n";
        sysInfo="windows vista";
        break;
    case QSysInfo::WV_WINDOWS7:
        qDebug()<<"System info:windows 7 \n";
        sysInfo="windows 7";
        break;
    default:
        qDebug()<<"System info:windows 7 \n";
        sysInfo="windows 7";
        break;

    }
#endif
#ifdef Q_WS_X11
    sysInfo = "linux";
#endif
    return sysInfo;
}

//menu
void Dashboard::menuChanged(int value)
{
    switch(value)
    {
    case 0 :

        _createMenu->setVisible(false);

        _showIconMenuX = _mousePos.x() + 162 - 25;
        _showIconMenuY = _mousePos.y();

        if (_width - (_normalMenuX + _normalMenuSize.width()) < _showIconMenuSize.width())
        {
            _showIconMenuX = _normalMenuX - _showIconMenuSize.width() + 25;
        }

        if ( _height - _normalMenuY < _showIconMenuSize.height())
        {
            _showIconMenuY = _height - _showIconMenuSize.height();
        }
        _showIconMenu->move(_showIconMenuX, _showIconMenuY);
        _showIconMenu->raise();
        _showIconMenu->setVisible(true);

        break;
    case 2 :
        _showIconMenu->setVisible(false);

        _createMenuX = _mousePos.x() + 162 - 25;
        _createMenuY = _mousePos.y() + 20 + 19 + 19 - 10;

        if (_width - (_normalMenuX + _normalMenuSize.width()) < _createMenuSize.width())
        {
            _createMenuX = _normalMenuX - _createMenuSize.width() + 25;
        }

        if ( _height - (_normalMenuY + 20 + 19 + 19 - 10) < _createMenuSize.height())
        {
            _createMenuY = _height - _createMenuSize.height();
        }
        _createMenu->move(_createMenuX, _createMenuY);
        _createMenu->raise();
        _createMenu->setVisible(true);

        break;
    default:
        _createMenu->setVisible(false);
        _showIconMenu->setVisible(false);
        break;
    }
}

void Dashboard::hideMenuWidget()
{
    _normalMenu->setVisible(false);
    _createMenu->setVisible(false);
    _showIconMenu->setVisible(false);
    _iconMenu->setVisible(false);
    _dustbinMenu->setVisible(false);
    _dirMenu->setVisible(false);
}

void Dashboard::virtualDesktopAddDirItem()
{
    hideMenuWidget();
    vdesktop->addDirItem();
}

void Dashboard::showDirMenu(QPoint mPos, const QString &uniqueName)
{
    _uniqueName = uniqueName;
    hideMenuWidget();

    _dirMenuX = mPos.x();

    qDebug() << "_dirMenuX--->" << _dirMenuX;
    _dirMenuY = mPos.y();

    if (_width - _dirMenuX < _dirMenuSize.width())
    {
        _dirMenuX = _width - _dirMenuSize.width();
    }

    if ( _height - _dirMenuY < _dirMenuSize.height())
    {
        _dirMenuY = _height - _dirMenuSize.height();
    }

    _dirMenu->move(_dirMenuX, _dirMenuY);
    _dirMenu->raise();
    _dirMenu->setVisible(true);
}

void Dashboard::showIconMenu(QPoint mPos, const QString &uniqueName)
{
    _uniqueName = uniqueName;
    hideMenuWidget();

    _iconMenuX = mPos.x();

    qDebug() << "_iconMenuX--->" << _iconMenuX;
    _iconMenuY = mPos.y();

    if (_width - _iconMenuX < _iconMenuSize.width())
    {
        _iconMenuX = _width - _iconMenuSize.width();
    }

    if ( _height - _iconMenuY < _iconMenuSize.height())
    {
        _iconMenuY = _height - _iconMenuSize.height();
    }


    _iconMenu->move(_iconMenuX, _iconMenuY);
    _iconMenu->raise();
    _iconMenu->setVisible(true);
}

void Dashboard::vdesktopShowDustbinMenu(QPoint mPos, IconItem *iconItem)
{
    if (!iconItem)
        return;
    _dustbinSelectIconItem = iconItem;

    _uniqueName = _dustbinSelectIconItem->uniqueName();
    hideMenuWidget();

    _dustbinMenuX = mPos.x();

    qDebug() << "_dustbinMenuX--->" << _dustbinMenuX;
    _dustbinMenuY = mPos.y();

    if (_width - _dustbinMenuX < _dustbinMenuSize.width())
    {
        _dustbinMenuX = _width - _dustbinMenuSize.width();
    }

    if ( _height - _dustbinMenuY < _dustbinMenuSize.height())
    {
        _dustbinMenuY = _height - _dustbinMenuSize.height();
    }


    _dustbinMenu->move(_dustbinMenuX, _dustbinMenuY);
    _dustbinMenu->raise();
    _dustbinMenu->setVisible(true);
}

void Dashboard::showNormalMenu(QPoint mousePos)
{
    hideMenuWidget();

    _mousePos = mousePos;
    _normalMenuX = _mousePos.x();
    _normalMenuY = _mousePos.y();

    if (_width - _mousePos.x() < _normalMenuSize.width())
    {
        _normalMenuX = _width - _normalMenuSize.width() + 14;
    }

    if (_height - _mousePos.y() < _normalMenuSize.height())
    {
        _normalMenuY = _height - _normalMenuSize.height();
    }

        _normalMenu->move(_normalMenuX, _normalMenuY);
        _normalMenu->raise();
        _normalMenu->setVisible(true);

}

void Dashboard::contextMenuEvent(QContextMenuEvent *)
{
    hideMenuWidget();
}

void Dashboard::menuDirOpen()
{
    hideMenuWidget();

    for(int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->uniqueName() == _uniqueName)
        {
            if (_local->at(i)->dirId() == -1)
            {
                vdesktop->openDir(_local->at(i)->id().toInt()
                                  , _local->at(i)->page(), _local->at(i)->index());
            }
            else if(_local->at(i)->dirId() == -2)
            {
                _toolBarWidget->openDir(_local->at(i)->id().toInt()
                                         , _local->at(i)->page(), _local->at(i)->index());
            }
            else
            {

            }
        }
    }
}

void Dashboard::menuDirDel()
{
    hideMenuWidget();

    for(int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->uniqueName() == _uniqueName)
        {
            if (_local->at(i)->dirId() == -1)
            {
                vdesktop->iconMenuDelClicked(_uniqueName);
            }
            else if(_local->at(i)->dirId() == -2)
            {
                int id = _local->at(i)->id().toInt();
                QList<QString> iconsInDirList;
                for (int j = 0; j < _local->count(); j++)
                {
                    if (_local->at(j)->hidden())
                        continue;

                    if (_local->at(j)->dirId() == id)
                    {
                        iconsInDirList.append(_local->at(j)->uniqueName());
                    }
                }

                for (int k = iconsInDirList.count(); k > 0; k--)
                {
                    LocalAppList::getList()->delApp(iconsInDirList.at(k - 1));
                    _vacShowWidget->desktopDelIcon(iconsInDirList.at(k - 1));
                }
                iconsInDirList.clear();

                LocalAppList::getList()->delApp(_uniqueName);

                vdesktop->tBarIconMenuDelClicked(id, _uniqueName);
//                LocalAppList::getList()->delApp(_uniqueName);
            }
            else
            {

            }
        }
    }
//    bool del = false;

//    for(int i = 0; i < _local->count(); i++)
//    {
//        if (_local->at(i)->hidden())
//            continue;

//        if (_local->at(i)->uniqueName() == _uniqueName)
//        {
//            if (_local->at(i)->dirId() == -1)
//            {
//                vdesktop->delIcon(_uniqueName);
//                del = true;
//                break;
//            }
//            else if(_local->at(i)->dirId() == -2)
//            {
//                _toolBarWidget->delIcon(_uniqueName);
//                del = true;
//                break;
//            }
//            else
//            {
//                break;
//            }
//        }
//    }
//    if (del)
//    {
//        vdesktop->iconToDushbin(_uniqueName);
//    }
}

void Dashboard::menuDirClear()
{
    hideMenuWidget();

    for(int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->uniqueName() == _uniqueName)
        {
            vdesktop->clearDirShowWidget(_local->at(i)->id().toInt());
        }
    }
}

void Dashboard::menuIconRun()
{
    hideMenuWidget();
    RunApp::getRunApp()->runApp(_uniqueName);
}

void Dashboard::menuIconDel()
{
    hideMenuWidget();
    bool del = false;
    for(int i=0; i<_local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->uniqueName() == _uniqueName)
        {
            if (_local->at(i)->dirId() >= 0)
            {
                vdesktop->dirShowWidgetDelIcon(_local->at(i)->dirId(), _uniqueName);
                del = true;
                break;
            }
            else
            {
                if (_local->at(i)->dirId() == -1)
                {
                    vdesktop->delIcon(_uniqueName);
                    del = true;
                    break;
                }
                else if(_local->at(i)->dirId() == -2)
                {
                    _toolBarWidget->delIcon(_uniqueName);
                    del = true;
                    break;
                }
            }

        }
    }
    if (del)
    {
        vdesktop->iconToDushbin(_uniqueName);
    }
}

void Dashboard::dustbinMenuRestoreClicked()
{
    if (!_dustbinSelectIconItem)
        return;

    hideMenuWidget();
    vdesktop->dustbinMenuRestoreIcon(_dustbinSelectIconItem);
}

void Dashboard::dustbinMenuDelClicked()
{
    hideMenuWidget();
    LocalAppList::getList()->delApp(_uniqueName);
    _vacShowWidget->desktopDelIcon(_uniqueName);
//    vdesktop->dustbinMenuDelIcon(_uniqueName);
}
