#include <QPropertyAnimation>
#include <QRect>
#include <QDesktopWidget>
#include <QPalette>
#include <QEvent>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QWidget>
#include <QX11Info>
#include <QResizeEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <iostream>

#ifdef LINUX_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif
#include <windows.h>
#include <shldisp.h>

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

extern QString xmlPath;
extern QString iconDirPath;
extern QString WIN_VAPP_IconPath;
extern QString WIN_TtempPath;

extern QString USERNAME;

extern QList<APP_LIST> g_myVappList;
extern QList<APP_LIST> g_RemoteappList;

extern QList<PAAS_LIST> g_myPaasList;
extern QList<PAAS_LIST> g_RemotepaasList;

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
{
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

    _ldialog = new LoginDialog(this);
    if (!_ldialog->exec())
        exit(1);



 //   setFocusPolicy(Qt::ClickFocus);
//    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

    QDesktopWidget *d = QApplication::desktop();
    QRect r = d->availableGeometry();
    _width = r.width();
    _height = r.height();

    setGeoProper();

    vdesktop = new VirtualDesktop(QSize(_width, _height), this);
//    _animationDesktop = new QPropertyAnimation(vdesktop, "pos");
//    connect(_animationDesktop, SIGNAL(valueChanged(QVariant)), this, SLOT(valueChanged(QVariant)));


    indicator = new Indicator(vdesktop, this);
    indicator->move((_width - indicator->width())/2, _height - indicator->height() - 50);
    //indicator->setGeometry((_width - indicator->width())/2, _height - indicator->height() - 28 , 20, 80);
    indicator->setVisible(false);

    _pageNodes = new PageNodes(this);
//    qDebug() << "vdesktop->count()vdesktop->count()-->" << vdesktop->count();
//    qDebug() << "vdesktop->currentPage()vdesktop->currentPage()-->" << vdesktop->currentPage();

    _pageNodes->update(vdesktop->count(), vdesktop->currentPage());
    _pageNodes->move((_width - _pageNodes->width()) / 2, _height - _pageNodes->height() - 100 - 45);
    _pageNodes->setVisible(true);



    panel = new Panel(this);
//    panel->setFixedSize(_width, 28); // 406
//    panel->move(0,0);
    panel->setGeometry(0, 0, _width, 28);
    panel->setVisible(true);
    panel->animationHide();

//    panel->animationShow();

    _switcherLeft = new Switcher(this);
    _switcherLeft->setPixmap(QString(":images/win_normal.png"));
    _switcherLeft->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);
    _switcherLeft->setGeometry(_width / 2 - _switcherLeft->width(), 0, _switcherLeft->width(), _switcherLeft->height());
    _switcherLeft->show();

    _switcherRight = new Switcher(this);
    _switcherRight->setPixmap(QString(":images/isoft_normal.png"));
    _switcherRight->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);
    _switcherRight->setGeometry(_switcherLeft->pos().x() + _switcherLeft->width(), 0, _switcherRight->width(), _switcherRight->height());
    _switcherRight->show();

    _vacShowWidget = new VacShowWidget(QSize(841, 540), this);
    _vacShowWidget->move((_width - _vacShowWidget->width()) / 2, (_height - _vacShowWidget->height()) / 2);
 //   _vacShowWidget->move(_width - panel->width() - 820 + 10, panel->pos().y());
//    _localShowWidget->showApp(true);
    _vacShowWidget->setVisible(false);
//    _vacShowWidget->getIcon();



    //vdesktop->setGeometry(QRect(0, 20, _width * vdesktop->count(), _height - indicator->height() - panel->height()));
    vdesktop->setGeometry(QRect(0, 0, _width * vdesktop->count(), _height));

    _toolBarWidget = new toolBarWidget(QSize(_width, 143 + 0 + 24), this);
    _toolBarWidget->move(0,_height - (143 + 0 + 24));//
    _toolBarWidget->raise();
    _toolBarWidget->show();

    _bsWidget = new BsWidget(_width, _height, this);
    //_bsWidget->setGeometry(0, 0, _width, _height);
    _bsWidget->move(0, 0);
    _bsWidget->setVisible(false);

//    _perWidget = new PersonalizationWidget(this);
//    _perWidget->setGeometry(QRect(0, 0, _width, _height));
//    _perWidget->setVisible(false);

//    quitAction = new QAction("退出", this);
//    showAction = new QAction("显示", this);
//    hideAction = new QAction("隐藏", this);
//    _setVacServer = new QAction("设置服务器地址", this);
//    QMenu *trayMenu = new QMenu(this);
//    trayMenu->addAction(_setVacServer);
//    trayMenu->addSeparator();
//    trayMenu->addAction(showAction);
//    trayMenu->addAction(hideAction);
//    trayMenu->addSeparator();
//    trayMenu->addAction(quitAction);

//    tray = new QSystemTrayIcon(this);
//    QIcon icon(":images/icon.png");
//    tray->setIcon(icon);
//    tray->setContextMenu(trayMenu);
//    tray->show();

    _animation = new QPropertyAnimation(this, "pos");

    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT wallpaper FROM wallpapers where id=\'%1\';").arg(1));
    while (query.next())
        _pixmap = QPixmap(query.value(0).toString());

    _refreshTimer = new QTimer(this);
//    connect(_refreshTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
//    _refreshTimer->start(1000 * 60);

    _vacServerWidget = new VacServerWidget();
    _vacServerWidget->setVisible(false);

    _finished = false;
    _commui = new commuinication();

    // heart beat.5s timer
    heartbeat_timer = new QTimer(this);
//    connect( heartbeat_timer, SIGNAL(timeout()), this, SLOT(heartbeat()));
//    heartbeat_timer->start(5000);
    _retryTimes = 3;
    _Isheartbeat=true;

//    _localShowWidget = new LocalShowWidget(QSize(820, 484), this);
//    _localShowWidget->move(_width - panel->width() - _localShowWidget->width(), panel->pos().y());
////    _localShowWidget->showApp(true);
//    _localShowWidget->setVisible(false);


//    _vacShowWidget = new VacShowWidget(QSize(820, 484), this);
//    _vacShowWidget->move(_width - panel->width() - 820 + 10, panel->pos().y());
////    _localShowWidget->showApp(true);
//    _vacShowWidget->setVisible(false);
////    _vacShowWidget->getIcon();

    _skinShowWidget = new SkinShowWidget(this);
    _skinShowWidget->resize(841, 530);
    _skinShowWidget->move((_width - _skinShowWidget->width()) / 2, (_height - _skinShowWidget->height()) / 2);
    _skinShowWidget->setVisible(false);


//    _dirWidget = new DirShowWidget(QSize(r.width() ,290), this);
//    _dirWidget->setVisible(false);

    _upMoveWidget = new MoveWidget(this);
    _upMoveWidget->setVisible(false);

    connect(_upMoveWidget, SIGNAL(moveWidgetDrop(IconItem*)), vdesktop, SLOT(moveWidgetDrop(IconItem*)));

    _animationUp = new QPropertyAnimation(_upMoveWidget, "geometry");

    QPixmap minPix(":/images/min_mask_icon.png");
    _minLabel = new QLabel(this);
    _minLabel->setPixmap(minPix); //_minLabel->width(), _minLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
    _minLabel->setVisible(false);
    _animationUpMin = new QPropertyAnimation(_minLabel, "geometry");

    _downMoveWidget = new MoveWidget(this);
    _downMoveWidget->setVisible(false);
    connect(_downMoveWidget, SIGNAL(moveWidgetDrop(IconItem*)), vdesktop, SLOT(moveWidgetDrop(IconItem*)));

    _animationDown = new QPropertyAnimation(_downMoveWidget, "geometry");

    _downMinW = new MoveMinWidget(this);
    _downMinW->setVisible(false);

    _animationDownMin = new QPropertyAnimation(_downMinW, "geometry");

    connect(_upMoveWidget, SIGNAL(mousePress()), vdesktop, SLOT(hideDirWidget()));
    connect(_downMoveWidget, SIGNAL(mousePress()), vdesktop, SLOT(hideDirWidget()));

    connect(_vacServerWidget, SIGNAL(serverChanged()), this, SLOT(updateVacServer()));
    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));
    connect(_skinShowWidget, SIGNAL(setBgPixmap(const QString&)), this, SLOT(setBgPixmap(const QString&)));

    connect(_switcherLeft, SIGNAL(switcherActivated()), this, SLOT(outOfScreen()));
    connect(_switcherRight, SIGNAL(switcherActivated()), this, SLOT(inOfScreen()));
    connect(_switcherLeft, SIGNAL(switcherHover()), panel, SLOT(animationShow()));
    connect(_switcherRight, SIGNAL(switcherHover()), panel, SLOT(animationShow()));
    connect(_switcherLeft, SIGNAL(switcherLeave()), panel, SLOT(animationHide()));
    connect(_switcherRight, SIGNAL(switcherLeave()), panel, SLOT(animationHide()));

    connect(panel, SIGNAL(quit()), this, SLOT(quit()));
    connect(panel, SIGNAL(showSkinWidget()), this, SLOT(onShowPerDesktop()));
    connect(panel, SIGNAL(showSoftwareWidget()), this, SLOT(onShowVacDesktop()));
//    connect(panel, SIGNAL(showSwitcherDesktop()), this, SLOT(onShowSwitcherDesktop()));
//    connect(panel, SIGNAL(showSwitcherDesktop()), this, SLOT(switchBetween()));

//    connect(panel, SIGNAL(showLocalDesktop()), this, SLOT(onShowLocalDesktop()));
 //   connect(panel, SIGNAL(showDirDesktop()), this, SLOT(onShowDirDesktop()));
//    connect(panel, SIGNAL(pageChanged(int)), this, SLOT(goPage(int)));
    connect(panel, SIGNAL(checkDirState()), vdesktop, SLOT(hideMenuWidget()));

    connect(_ldialog, SIGNAL(dQuit()), this, SLOT(quit()));
    connect(_ldialog, SIGNAL(dShow()), this, SLOT(show()));
    connect(_ldialog, SIGNAL(dHide()), this, SLOT(hide()));
    connect(_ldialog, SIGNAL(pShow()), panel, SLOT(show()));
    connect(_ldialog, SIGNAL(pHide()), panel, SLOT(hide()));
    connect(_ldialog, SIGNAL(dVacServer()), this, SLOT(setVacServer()));
    connect(_ldialog, SIGNAL(dActivated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(vdesktop, SIGNAL(toOrigin()), this, SLOT(switchBetween()));
    connect(vdesktop, SIGNAL(largeIcon()), this, SLOT(largeIcon()));
    connect(vdesktop, SIGNAL(mediumIcon()), this, SLOT(mediumIcon()));
    connect(vdesktop, SIGNAL(smallIcon()), this, SLOT(smallIcon()));
    connect(vdesktop, SIGNAL(desktopDelIcon(const QString &)), _vacShowWidget, SLOT(desktopDelIcon(const QString &)));
    connect(vdesktop, SIGNAL(pageChanged(int)), this, SLOT(desktopPageChanged(int)));
    connect(vdesktop, SIGNAL(pageIncreased()), this, SLOT(updateNodes()));
    connect(vdesktop, SIGNAL(pageDecreased()), this, SLOT(updateNodes()));
    connect(vdesktop, SIGNAL(refreshVac()), this, SLOT(timeOut()));
//    connect(vdesktop, SIGNAL(bgMove(int, int)), this, SLOT(bgMove(int, int)));
//    connect(vdesktop, SIGNAL(toOrigin()), switcher, SLOT(changed()));////////
    connect(panel, SIGNAL(setEqual(int)), vdesktop, SLOT(arrangeEqually(int)));
    connect(panel, SIGNAL(setMini()), vdesktop, SLOT(arrangeMinimum()));
//    connect(indicator, SIGNAL(iNeedMove()), this, SLOT(moveIndicator()));

    connect(_pageNodes, SIGNAL(choosePage(int)), vdesktop, SLOT(goPage(int)));
    //connect(panel, SIGNAL(showDesktop()), this, SLOT(onShowDesktop()));
    //setGeoProper();

    connect(_vacShowWidget, SIGNAL(addApp(const QString&,const QString&, const QString&, int)),
            vdesktop, SLOT(addDesktopApp(const QString&,const QString&, const QString&, int))); //
    connect(_vacShowWidget, SIGNAL(delItem(QString)), vdesktop, SLOT(vacWidgetDelIcon(const QString &)));
    connect(_skinShowWidget, SIGNAL(skinCloseBtnClicked()), _skinShowWidget, SLOT(hide()));
    connect(_vacShowWidget, SIGNAL(vacCloseBtnClicked()), _vacShowWidget, SLOT(hide()));
    connect(vdesktop, SIGNAL(sendUrl(const QString&)), this, SLOT(showBs(const QString&)));
//    connect(_dirWidget, SIGNAL(sendUrl(const QString&)), this, SLOT(showBs(const QString&)));
    connect(_bsWidget,SIGNAL(goBack()), this, SLOT(goDesktop()));
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

    connect(vdesktop, SIGNAL(desktopClicked()), this, SLOT(desktopClicked()));

    connect(vdesktop, SIGNAL(delToolBarWidgetIcon(const QString &))
            , _toolBarWidget, SLOT(moveOutIcon(const QString&)));
    connect(vdesktop, SIGNAL(delToolBarIcon(const QString &))
            , _toolBarWidget, SLOT(delIcon(const QString&)));

    connect(_animationDown,SIGNAL(finished()), this, SLOT(animationFinished()));
    connect(_animationDownMin,SIGNAL(finished()), this, SLOT(animationMinDownFinished()));
    connect(_animationUp,SIGNAL(finished()), this, SLOT(animationFinished()));

    connect(_animationDownMin, SIGNAL(valueChanged(const QVariant&)), this, SLOT(valueChanged(const QVariant&)));

    connect(_toolBarWidget, SIGNAL(iconDropToolWidget(const QString &))
            , vdesktop, SLOT(desktopIconMoveOtherWidget(const QString &)));
//    connect(_toolBarWidget, SIGNAL(toolBarAddDirSW(int)), this, SLOT(test(int)));
    connect(_toolBarWidget, SIGNAL(toolOpenDir(int,int,int)), vdesktop, SLOT(toolOpenDir(int,int,int)));
    connect(_toolBarWidget, SIGNAL(toolCloseDir(int, int)), vdesktop, SLOT(toolCloseDir(int, int)));
    //    connect(vdesktop, SIGNAL(setDirIcon(const QString&, const QString&, const QString&)),
//            _dirWidget, SLOT(addDirIcon(const QString&, const QString&, const QString&)));


//    QPixmap backPix(":images/bs_goback.png");
//    QPixmap backPixHover(":images/bs_goback_hove.png");
//    _backBtn = new DynamicButton(backPix, backPixHover, this);

//    _backBtn = new QPushButton(this);
//    _backBtn->setStyleSheet
//                    ("QPushButton{background-image:url(:images/bs_goback.png);
//                       border-style:flat;background-color:transparent;}
//                    QPushButton:hover:pressed{
//                        background-image:url(:images/bs_goback_hover.png);border-style:flat;background-color:transparent;}
//                    QPushButton:hover{
//                        background-image:url(:images/bs_goback.png);border-style:flat;background-color:transparent;}");

//    _backBtn->setGeometry(60, r.height() / 2,
//                                   50, 50);
//    _backBtn->setVisible(false);
//    connect(_backBtn, SIGNAL(clicked()), this, SLOT(goDesktop()));//

//    _blurPicker = new BlurPicker(this);
//    _blurPicker->setFixedSize(832, 482);
//    _blurPicker->move(200,200);
//    _blurPicker->setVisible(true);
        _nam = new QNetworkAccessManager(this);
//        connect(_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onOutFinished(QNetworkReply*)));
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
    _pageNodes->move(( _width - _pageNodes->width()) / 2, _height - _pageNodes->height() - 50);
    _pageNodes->setVisible(true);
}

//dir open and close
void Dashboard::desktopClicked()
{
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);
//    panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();
    vdesktop->setIconEnabled(true);
}

void Dashboard::upMove(int x, int y, int w, int h, int distance)
{
    if (_animationUp->state() == QAbstractAnimation::Running)
    {
        return;
    }

//    panel->setVisible(false);
//    _vacShowWidget->setVisible(false);
//    _skinShowWidget->setVisible(false);

//    indicator->setVisible(false);
//    _dirWidget->move(x, y);
//    _dirWidget->setVisible(true);

    _upMoveWidget->resize(QSize(w, h));

    QPixmap result = QPixmap();
    result = QPixmap::grabWindow(this->winId(), \
                                 x, y, \
                                 w, h); //抓取当前屏幕的图片

    _upMoveWidget->setPixmap(result);
    _upMoveWidget->setVisible(true);

    _animationUp->setDuration(500);
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

    _animationUp->setDuration(500);
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
        //        _dirWidget->setVisible(false);
        vdesktop->setDirHide();
        _downMinW->setVisible(false);
        _minLabel->setVisible(false);
        _upMoveWidget->setVisible(false);
        _downMoveWidget->setVisible(false);

//        indicator->setVisible(true);

        if (!panel->isVisible())
        {
            panel->setVisible(true);
//            panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
            panel->show();
            panel->setAutoHide(true);
            panel->animationHide();
        }

        _animationUpFinished = false;
        _animationUpFinished = false;

        vdesktop->setIconEnabled(true);
        vdesktop->setIconMove(true);

        vdesktop->dirMovingFinished();
        _toolBarWidget->setVisible(true);
    }
}

void Dashboard::downMove(int x, int y, int w, int h, int distance)
{
    qDebug() << x << y << w << h;
    if (_animationDown->state() == QAbstractAnimation::Running)
    {
        return;
    }

    panel->setVisible(false);
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

//    indicator->setVisible(false);
//    _dirWidget->move(x, y);
//    _dirWidget->setVisible(true);

    _downMoveWidget->resize(QSize(w, h));

    QPixmap result = QPixmap();
    result = QPixmap::grabWindow(this->winId(), \
                                 x, y, \
                                 w, h); //抓取当前屏幕的图片
    _downMoveWidget->setPixmap(result);
    _downMoveWidget->setVisible(true);
    _toolBarWidget->setVisible(false);

    _animationDown->setDuration(500);
    _animationDown->setStartValue(QRect(x, y, w, h));
    _animationDown->setEndValue(QRect(x, y + distance, w, h));
    _animationDown->start();
    _animationDownFinished = false;
}

void Dashboard::downBackMove(int x, int y, int w, int h, int distance)
{
    if (_animationDown->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animationDown->setDuration(500);
    _animationDown->setStartValue(QRect(x, y, w, h));
    _animationDown->setEndValue(QRect(x, y - distance, w, h));
    _animationDown->start();

    _animationDownFinished = true;

}
/*
void Dashboard::animationDownFinished()
{
    if (_animationDownFinished)
    {
//        _dirWidget->setVisible(false);
        _downMinW->setVisible(false);
        _minLabel->setVisible(false);
        vdesktop->setDirHide();
        _upMoveWidget->setVisible(false);
        _downMoveWidget->setVisible(false);

        _animationDownFinished = false;
        indicator->setVisible(true);

        panel->setVisible(true);
        panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
        panel->show();
        panel->setAutoHide(true);
        panel->animationHide();
        vdesktop->setIconEnabled(true);
        vdesktop->setIconMove(true);
    }
}
*/
void Dashboard::upMinMove(int x, int y, int w, int h, int distance)
{
    if (_animationUpMin->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _minLabel->setGeometry(x, y + 2, w, h);
    _minLabel->setVisible(true);


    _animationUpMin->setDuration(500);
    _animationUpMin->setStartValue(QRect(x, y + 2, w, h));
    _animationUpMin->setEndValue(QRect(x, y - distance + 2, w, h));
    _animationUpMin->start();
}

void Dashboard::upMinBackMove(int x, int y, int w, int h, int distance)
{
    if (_animationUpMin->state() == QAbstractAnimation::Running)
    {
        return;
    }

//    _downMinW->setVisible(true);
    _animationUpMin->setDuration(500);
    _animationUpMin->setStartValue(QRect(x, y + 2, w, h));
    _animationUpMin->setEndValue(QRect(x, y + 2 + distance, w, h));
    _animationUpMin->start();
}

void Dashboard::openMinWidget(int x, int y, int w, int h, int distance)
{
    if (_animationDownMin->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _distance = distance;
    _minY = y;
    _minUpward = false;

//    _minLabel->setGeometry(x, y + 1, w, h);
//    _minLabel->setVisible(true);

    _downMinW->resize(QSize(w, h));

    QPixmap result = QPixmap();
    result = QPixmap::grabWindow(this->winId(), \
                                 x, y, \
                                 w, h); //抓取当前屏幕的图片
    _downMinW->setPixmap(result);
//    _downMinW->setVisible(true);

    _animationDownMin->setDuration(500);
    _animationDownMin->setStartValue(QRect(x, y, w, h));
    _animationDownMin->setEndValue(QRect(x, y + distance, w, h));
    _animationDownMin->start();
}

void Dashboard::closeMinWidget(int x, int y, int w, int h, int distance)
{
    if (_animationDownMin->state() == QAbstractAnimation::Running)
    {
        return;
    }

//    _distance = distance;
    _minY = y;
    _minUpward = true;

//    _downMinW->setVisible(true);
    _animationDownMin->setDuration(500);
    _animationDownMin->setStartValue(QRect(x, y, w, h));
    _animationDownMin->setEndValue(QRect(x, y - distance, w, h));
    _animationDownMin->start();
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
        //qDebug() << rect.y() - _minY;
        if (!_minUpward)
        {
            _downMinW->setVisible(false);
        }
        else
        {
            _downMinW->setVisible(true);
        }
    }
    else
    {
        if (!_minUpward)
        {
            _downMinW->setVisible(true);
        }
        else
        {
            _downMinW->setVisible(false);
        }
    }

}


void Dashboard::showBs(const QString &url)
{
//    qDebug() << "url"<< "url" << url;
    vdesktop->setVisible(false);
    _bsWidget->setUrl(url);
    _bsWidget->setVisible(true);
//    _backBtn->setVisible(true);
//    panel->setVisible(false);

}

void Dashboard::goDesktop()
{
//    _backBtn->setVisible(false);
    vdesktop->setVisible(true);
    _bsWidget->setVisible(false);
//    panel->setVisible(true);


}

void Dashboard::onDone()
{
    //get the Application List;
    _finished = true;
}

void Dashboard::heartbeat()
{
//    if (vdesktop->dragEventState())
//    {
//        return;
//    }

//    if (vdesktop->trembleState())
//    {
//        return;
//    }

//    if (vdesktop->addAppState())
//    {
//        return;
//    }

//    if (vdesktop->iconDragState())
//    {
//        return;
//    }

    if(!_Isheartbeat)
        return;
    _commui->heartBeat();

    if(_commui->_isNetError)
    {
        if( _retryTimes > 0)
        {
            heartbeat_timer->start(10);
            _retryTimes--;
        }
        else
        {
            heartbeat_timer->start(5000);
        }
    }
    else
    {
        _retryTimes = 3;
        heartbeat_timer->start(5000);
    }

    while (!_finished)
        QApplication::processEvents();
    _finished = false;

    if(_commui->errID == "10036")
    {
#ifdef Q_WS_WIN
        m_dllCloseAppAll();
#endif
        _Isheartbeat=false;
        QMessageBox::warning(this, tr("your seesion has to be ticked out."),tr("Your session has to be kicked out by the administrator, please contact the administrator"), tr("ok"));
        //qApp->quit();
    }
}

void Dashboard::goPage(int page)
{
//    _perWidget->setVisible(false);
//    _bsWidget->setVisible(false);

    vdesktop->setVisible(true);
    vdesktop->goPage(page);
}

//void Dashboard::onShowSwitcherDesktop()
//{
////    if (_outOfScreen)
////        getIn();
//}

void Dashboard::onShowVacDesktop()
{
    if (_skinShowWidget->isVisible())
        _skinShowWidget->setVisible(false);

    if (_vacShowWidget->isVisible())
    {
        _vacShowWidget->setVisible(false);
    }
    else
    {
        _vacShowWidget->setVisible(true);
    }

}

void Dashboard::onShowPerDesktop()
{
    if (_vacShowWidget->isVisible())
        _vacShowWidget->setVisible(false);

    if (_skinShowWidget->isVisible())
    {
        _skinShowWidget->setVisible(false);
    }
    else
    {
        _skinShowWidget->setVisible(true);
    }

}

void Dashboard::moveIndicator()
{
//    indicator->move((_width - indicator->width())/2, _height - indicator->height() - 50);
}

void Dashboard::getIn()
{
    QPoint start;
    QPoint end;
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    start = pos();
    end = QPoint(0, 0);
    _animation->setDuration(300);
    _animation->setEasingCurve(QEasingCurve::InExpo);
//    panel->setWindowFlags(panel->windowFlags() & ~Qt::WindowStaysOnTopHint);
//    panel->show();
//    panel->setAutoHide(false);
//    panel->animationShow();
//    panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
//    panel->show();
//    panel->setAutoHide(true);
//    panel->animationHide();

//    indicator->show();

    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _outOfScreen = !_outOfScreen;
    _animation->start();
}

void Dashboard::getOut()
{
    QPoint start;
    QPoint end;
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    start = pos();
    end = QPoint(_width, 0);
    _animation->setDuration(900);
    _animation->setEasingCurve(QEasingCurve::OutElastic);
//    panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
//    panel->show();
//    panel->setAutoHide(true);
//    panel->animationHide();
//    indicator->hide();
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _outOfScreen = !_outOfScreen;
    _animation->start();
}

void Dashboard::switchBetween()
{
//    if (_localShowWidget->isVisible())
//    {
//        _localShowWidget->setVisible(false);
////        return;
//    }

    if (_vacShowWidget->isVisible())
    {
        _vacShowWidget->setVisible(false);
//        return;
    }

    if (_skinShowWidget->isVisible())
    {
        _skinShowWidget->setVisible(false);
//        return;
    }

    qDebug() <<"********************_outOfScreen"<< _outOfScreen;
    if (_outOfScreen)
        getIn();
    else
        getOut();
}

void Dashboard::inOfScreen()
{
//    if (_vacShowWidget->isVisible())
//    {
//        _vacShowWidget->setVisible(false);
////        return;
//    }

    if (_skinShowWidget->isVisible())
    {
        _skinShowWidget->setVisible(false);
//        return;
    }

//    vdesktop->goPage(0);

    if (_outOfScreen)
        getIn();
}

void Dashboard::outOfScreen()
{
//    if (_vacShowWidget->isVisible())
//    {
//        _vacShowWidget->setVisible(false);
////        return;
//    }

    if (_skinShowWidget->isVisible())
    {
        _skinShowWidget->setVisible(false);
//        return;
    }

    if (!_outOfScreen)
        getOut();
}

void Dashboard::setGeoProper()
{
    setGeometry(0, 0, _width, _height);
	setFixedSize(_width, _height);
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

//    _localShowWidget->setVisible(false);
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

//    panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();

    AppMessageBox box(true, NULL);
    box.setText("           是否确定退出？");
    if (box.exec()) {

        QString quitUrl = Config::get("Server") + ":8080/idesktop/logout.action";

        QString data = "username=" + USERNAME;

        _nam->post(QNetworkRequest(QUrl(quitUrl)), data.toAscii());
#if 1
        QSqlQuery query(QSqlDatabase::database("local"));

        QString updateIconType = QString("update sizetype "\
                                    "set type=%1 where id=1;")\
                                    .arg(ICON_TYPE);
        if(!query.exec(updateIconType)) {
            qDebug() <<"query failed";
            return;
        }


        vdesktop->atExit();

        _Isheartbeat = false;
        this->hide();
        _switcherLeft->setVisible(false);
        _switcherRight->setVisible(false);
        this->hide();

        _commui->logoff();
        while (!_finished)
        {
            QApplication::processEvents();
        }

        qApp->quit();
#endif
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
//    QPalette palette;
//    palette.setBrush(backgroundRole(), QBrush(_pixmap.scaled(width(), height())));
//    setPalette(palette);

    QSqlQuery query(QSqlDatabase::database("local"));

    QString qstr = QString("update wallpapers "\
                           "set wallpaper=\'%1\' where id=\'%2\';")\
                           .arg(_pixText)\
                           .arg(1);
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return;
    }

    _bsWidget->setPixmap(pixText);
    qDebug() << "setBgPixmap(const QString &pixText)setBgPixmap(const QString &pixText)setBgPixmap(const QString &pixText)" << pixText;
    setBgImage(pixText);

}
//miya add change background imgae
void Dashboard::setBgImage(QString url)
{
    qDebug() << "Dashboard::setBgImage(QString url)" << url;
    QString path = QCoreApplication::applicationDirPath();
    url.replace(QString(":"), QString(""));
    qDebug() << url;
    path += url;
    path.replace(QString("/"), QString("\\"));
    qDebug() << path;
    QLibrary lib("changebg.dll");
    if(lib.load()) {
        qDebug() << "success load dll";
        if(changebg = (DLL_Changebg)lib.resolve("SetMyWallpaper")) {
            qDebug() << "success find func";
            changebg(path.toLatin1().data(), 0);
        } else {
            qDebug() << "failed find func";
        }
    } else {
        qDebug() << "failed load dll";
    }
}

void Dashboard::errOut()
{
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, _ldialog->GetSystemInfo());

    while (!_finished)
        QApplication::processEvents();
    _finished = false;

    _Isheartbeat = true;
}

void Dashboard::refreshVapp()
{
//    if (vdesktop->dragEventState())
//    {
//        return;
//    }

//    if (vdesktop->trembleState())
//    {
//        return;
//    }

//    if (vdesktop->addAppState())
//    {
//        return;
//    }

    qDebug() << "refresh Vapp";
    if (!_Isheartbeat)
    {
        qDebug() << "_commui->errID:" << _commui->errID;
        if(_commui->errID == "10036")
        {
            qDebug() << "虚拟应用服务连接失败，正在重新连接。。。";

            QTimer::singleShot(1000 * 5, this, SLOT(errOut()));
        }

        return;
    }

    if (_commui->errID == "10000") {
        if (!_Isheartbeat)
        {
            _Isheartbeat = true;
            qDebug() << "虚拟应用服务器已成功连接";
        }
        g_myVappList.clear();

        //get vapp list
        _commui->getAppList();
        while (!_finished)
            QApplication::processEvents();
        _finished = false;

        g_myVappList = _commui->getList();
        qDebug()<<"g_myList.count()="<<g_myVappList.count();

        iconDirPath = WIN_VAPP_IconPath ;

        QDir iconDir(iconDirPath);
        if(!iconDir.exists())
        {
            iconDir.mkdir(iconDirPath);
        }
        //store ico file locally
        for(int i = 0; i < g_myVappList.count(); i++)
        {
            QString iconPath = QString("%1%2.png")
                    .arg(iconDirPath)
                    .arg(g_myVappList[i].id);
            QString tempPath = QString("%1%2.ico")
                    .arg(iconDirPath)
                    .arg(g_myVappList[i].id);

//            g_RemoteappList.insert(i, g_myVappList[i]);
//            g_RemoteappList[i].icon = iconPath;

            //check if ico file is existed, or dont donwload
            QFile chkFile(iconPath);
            if(chkFile.exists())
            {
                chkFile.close();
                continue;
            }
            chkFile.close();

            //qDebug()<<"iconPath"<<iconPath;
            _commui->downloadIcon(QUrl(g_myVappList[i].icon), tempPath);
            while (!_finished)
                QApplication::processEvents();
            _finished = false;

            _ldialog->setIcon(iconDirPath, tempPath);
        }

        modify();
    }

}

void Dashboard::timeOut()
{
    if (vdesktop->dragEventState())
    {
        return;
    }

    if (vdesktop->trembleState())
    {
        return;
    }

    if (vdesktop->addAppState())
    {
        return;
    }

    if (vdesktop->iconDragState())
    {
        return;
    }

    refreshVapp();

    refreshPaas();
}

void Dashboard::modify()
{
    if (g_myVappList.count() > 0 || g_RemoteappList.count() > 0)
    {
    //delete
        for (int i = 0; i < g_RemoteappList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < g_myVappList.count(); j++)
            {
                if (g_RemoteappList[i].id == g_myVappList[j].id)
                {
                    isExist = true;
                    break;
                }
            }
            if(!isExist)
            {
                //delete
//                LocalAppList::getList()->delApp(g_RemoteappList[i].name);
                _vacShowWidget->delIcon(g_RemoteappList[i].name);
            }
        }
        // add
        for (int i = 0; i < g_myVappList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < g_RemoteappList.count(); j++)
            {
                if (g_myVappList[i].id == g_RemoteappList[j].id)
                {
                    isExist = true;
                    break;
                }
            }

            if (!isExist)
            {
//                LocalApp *RemoteApp = new LocalApp();
//                RemoteApp->setName(g_myVappList[i].name);
//                RemoteApp->setId(g_myVappList[i].id);
//                RemoteApp->setIcon(WIN_VAPP_IconPath + g_myVappList[i].id + ".ico");
//                RemoteApp->setPage(vdesktop->count() - 1);
//                RemoteApp->setIndex(-1);
//                RemoteApp->setType(g_myVappList[i].type);
//                RemoteApp->setIsRemote(true);
 //               LocalAppList::getList()->addRemoteApp(RemoteApp);

                _vacShowWidget->addIcon(g_myVappList[i].name, WIN_VAPP_IconPath + g_myVappList[i].id + ".png", \
                                        vdesktop->count() - 1, -1, QString(""), 1);
            }
        }
    }

    g_RemoteappList.clear();

    for(int i = 0; i < g_myVappList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(iconDirPath)
                .arg(g_myVappList[i].id);

        g_RemoteappList.insert(i, g_myVappList[i]);
//        qDebug()<<"g_mylist:"<<g_myVappList[i].icon;
        g_RemoteappList[i].icon = iconPath;
        qDebug()<<"g_Rmote:"<<g_RemoteappList.at(i).icon;
    }

}

void Dashboard::setVacServer()
{
    _vacServerWidget->setVisible(true);
}

void Dashboard::updateVacServer()
{
    _ldialog->updateVacServer();
//    _ldialog->getPaas();

//    _Isheartbeat = false;
    if (_commui->errID == "10000")
        _commui->logoff();
    else
        errOut();

}


void Dashboard::refreshPaas()
{
    qDebug() << "refresh paas";
    g_myPaasList.clear();

    _ldialog->getPaas(false);

    paasModify();
}

void Dashboard::paasModify()
{
    if (g_myPaasList.count() > 0 || g_RemotepaasList.count() > 0)
    {
    //delete
        for (int i = 0; i < g_RemotepaasList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < g_myPaasList.count(); j++)
            {
                if (g_RemotepaasList[i].cnName == g_myPaasList[j].cnName)
                {
                    isExist = true;
                    break;
                }
            }
            if(!isExist)
            {
                //delete
//                LocalAppList::getList()->delApp(g_RemoteappList[i].name);
                _vacShowWidget->delIcon(g_RemotepaasList[i].cnName);
            }
        }
        // add
        for (int i = 0; i < g_myPaasList.count(); i++)
        {
            bool isExist = false;
            for (int j = 0; j < g_RemotepaasList.count(); j++)
            {
                if (g_myPaasList[i].cnName == g_RemotepaasList[j].cnName)
                {
                    isExist = true;
                    break;
                }
            }

            if (!isExist)
            {
                _vacShowWidget->addIcon(g_myPaasList[i].cnName, g_myPaasList[i].iconPath, \
                                        - 1, -1, g_myPaasList[i].urls, 2);
            }
        }
    }

    g_RemotepaasList.clear();

    for(int i = 0; i < g_myPaasList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(iconDirPath)
                .arg(g_myPaasList[i].cnName);

        g_RemotepaasList.insert(i, g_myPaasList[i]);
//        qDebug()<<"g_mylist:"<<g_myVappList[i].icon;
        g_RemotepaasList[i].iconPath = iconPath;
        qDebug()<<"g_Rmote:"<<g_RemotepaasList.at(i).iconPath;
    }
}

void Dashboard::largeIcon()
{
    _vacShowWidget->largeIcon();
}

void Dashboard::mediumIcon()
{
    _vacShowWidget->mediumIcon();
}

void Dashboard::smallIcon()
{
    _vacShowWidget->smallIcon();
}

//void Dashboard::onOutFinished(QNetworkReply *reply)
//{
//    QSqlQuery query(QSqlDatabase::database("local"));

//    QString updateIconType = QString("update sizetype "\
//                                "set type=%1 where id=1;")\
//                                .arg(ICON_TYPE);
//    if(!query.exec(updateIconType)) {
//        qDebug() <<"query failed";
//        return;
//    }


////    vdesktop->atExit();

//    _Isheartbeat = false;
//    _commui->logoff();
//    while (!_finished)
//    {
//        QApplication::processEvents();
//    }

//    qApp->quit();
//}
