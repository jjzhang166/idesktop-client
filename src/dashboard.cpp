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

extern QList<APP_LIST> g_myVappList;
extern QList<APP_LIST> g_RemoteappList;

#ifdef Q_WS_WIN
    QLibrary *mylib;   //
    Dll_CloseAppAll m_dllCloseAppAll;
#endif

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool)
    , _outOfScreen(false)
    , _animationFinished(false)
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

    setFocusPolicy(Qt::ClickFocus);
//    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

    QDesktopWidget *d = QApplication::desktop();
    QRect r = d->availableGeometry();
    _width = r.width();
    _height = r.height();

    setGeoProper();

    vdesktop = new VirtualDesktop(QSize(_width, _height), this);
    _animationDesktop = new QPropertyAnimation(vdesktop, "pos");
    connect(_animationDesktop, SIGNAL(valueChanged(QVariant)), this, SLOT(valueChanged(QVariant)));

    indicator = new Indicator(vdesktop, this);
    indicator->move((_width - indicator->width())/2, _height - indicator->height() - 50);
    //indicator->setGeometry((_width - indicator->width())/2, _height - indicator->height() - 28 , 20, 80);
    indicator->show();

//    switcher = new Switcher(this);
//    switcher->setGeometry(_width - switcher->width(), (_height - switcher->height())/2, 20, 80);
//    switcher->show();

    panel = new Panel(this);
    panel->setFixedSize(78, 406);
    //panel->setFixedSize(_width / 3, 40);
    panel->move(_width - panel->width(), (_height - panel->height()) / 2);
    panel->show();
    panel->animationHide();
//    panel->animationShow();

    //vdesktop->setGeometry(QRect(0, 20, _width * vdesktop->count(), _height - indicator->height() - panel->height()));
    vdesktop->setGeometry(QRect(0, 0, _width * vdesktop->count(), _height));

    _bsWidget = new BsWidget(_width, _height, this);
    //_bsWidget->setGeometry(0, 0, _width, _height);
    _bsWidget->move(0, 0);
    _bsWidget->setVisible(false);

//    _perWidget = new PersonalizationWidget(this);
//    _perWidget->setGeometry(QRect(0, 0, _width, _height));
//    _perWidget->setVisible(false);

    quitAction = new QAction("退出", this);
    showAction = new QAction("显示", this);
    hideAction = new QAction("隐藏", this);
    _setVacServer = new QAction("设置服务器地址", this);
    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction(_setVacServer);
    trayMenu->addSeparator();
    trayMenu->addAction(showAction);
    trayMenu->addAction(hideAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    tray = new QSystemTrayIcon(this);
    QIcon icon(":images/icon.png");
    tray->setIcon(icon);
    tray->setContextMenu(trayMenu);
    tray->show();

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

    _localShowWidget = new LocalShowWidget(QSize(820, 484), this);
    _localShowWidget->move(_width - panel->width() - _localShowWidget->width(), panel->pos().y());
//    _localShowWidget->showApp(true);
    _localShowWidget->setVisible(false);


    _vacShowWidget = new VacShowWidget(QSize(820, 484), this);
    _vacShowWidget->move(_width - panel->width() - _vacShowWidget->width() + 10, panel->pos().y());
//    _localShowWidget->showApp(true);
    _vacShowWidget->setVisible(false);

    _skinShowWidget = new SkinWidget(this);
    _skinShowWidget->resize(820, 484);
    _skinShowWidget->move(_width - panel->width() - _skinShowWidget->width() + 10, panel->pos().y() - 78);
    _skinShowWidget->setVisible(false);

    QPixmap minPix(":/images/min_mask_icon.png");
    _minLabel = new QLabel(this);
    _minLabel->setPixmap(minPix); //_minLabel->width(), _minLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
    _minLabel->setVisible(false);

//    _dirWidget = new DirShowWidget(QSize(r.width() ,290), this);
//    _dirWidget->setVisible(false);

    _mW = new MoveWidget(this);
    _mW->setVisible(false);

    _animationScreen = new QPropertyAnimation(_mW, "geometry");

    _minW = new MoveMinWidget(this);
    _minW->setVisible(false);

    _animationMinScreen = new QPropertyAnimation(_minW, "geometry");

    connect(_mW, SIGNAL(mousePress()), vdesktop, SLOT(dirState()));

    connect(_vacServerWidget, SIGNAL(serverChanged()), this, SLOT(updateVacServer()));
    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));
    connect(_skinShowWidget, SIGNAL(setBgPixmap(const QString&)), this, SLOT(setBgPixmap(const QString&)));
    connect(panel, SIGNAL(quit()), this, SLOT(quit()));
//    connect(panel, SIGNAL(showSwitcherDesktop()), this, SLOT(onShowSwitcherDesktop()));
    connect(panel, SIGNAL(showSwitcherDesktop()), this, SLOT(switchBetween()));
    connect(panel, SIGNAL(showVacDesktop()), this, SLOT(onShowVacDesktop()));
    connect(panel, SIGNAL(showLocalDesktop()), this, SLOT(onShowLocalDesktop()));
    connect(panel, SIGNAL(showDirDesktop()), this, SLOT(onShowDirDesktop()));
    connect(panel, SIGNAL(showPerDesktop()), this, SLOT(onShowPerDesktop()));
    connect(panel, SIGNAL(pageChanged(int)), this, SLOT(goPage(int)));
    connect(panel, SIGNAL(checkDirState()), vdesktop, SLOT(dirState()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(showAction, SIGNAL(triggered()), this, SLOT(show()));
    connect(hideAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(showAction, SIGNAL(triggered()), panel, SLOT(show()));
    connect(hideAction, SIGNAL(triggered()), panel, SLOT(hide()));
//    connect(showAction, SIGNAL(triggered()), panel, SLOT(show()));
//    connect(hideAction, SIGNAL(triggered()), panel, SLOT(hide()));
    connect(_setVacServer, SIGNAL(triggered()), this, SLOT(setVacServer()));
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(vdesktop, SIGNAL(toOrigin()), this, SLOT(switchBetween()));
//    connect(vdesktop, SIGNAL(bgMove(int, int)), this, SLOT(bgMove(int, int)));
//    connect(vdesktop, SIGNAL(toOrigin()), switcher, SLOT(changed()));
    connect(panel, SIGNAL(setEqual(int)), vdesktop, SLOT(arrangeEqually(int)));
    connect(panel, SIGNAL(setMini()), vdesktop, SLOT(arrangeMinimum()));
    connect(indicator, SIGNAL(iNeedMove()), this, SLOT(moveIndicator()));
    //connect(panel, SIGNAL(showDesktop()), this, SLOT(onShowDesktop()));
    //setGeoProper();

    connect(_localShowWidget, SIGNAL(addLocalApp(const QString&,const QString&, const QString&)),
            vdesktop, SLOT(addLocalApp(const QString&,const QString&, const QString&))); //
    connect(_vacShowWidget, SIGNAL(addVacApp(const QString&,const QString&, const QString&)),
            vdesktop, SLOT(addLocalApp(const QString&,const QString&, const QString&))); //
    connect(vdesktop, SIGNAL(sendUrl(const QString&)), this, SLOT(showBs(const QString&)));
//    connect(_dirWidget, SIGNAL(sendUrl(const QString&)), this, SLOT(showBs(const QString&)));
    connect(_bsWidget,SIGNAL(goBack()), this, SLOT(goDesktop()));
    connect(vdesktop, SIGNAL(desktopOpenMove(int ,int ,int, int, int, int)),
            this, SLOT(desktopOpenMove(int ,int ,int, int, int, int)));
    connect(vdesktop, SIGNAL(desktopCloseMove(int, int, int, int, int, int)),
            this, SLOT(desktopCloseMove(int ,int ,int, int, int, int)));
    connect(vdesktop, SIGNAL(openMinWidget(int ,int ,int, int, int)),
            this, SLOT(openMinWidget(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(closeMinWidget(int, int, int, int, int)),
            this, SLOT(closeMinWidget(int ,int ,int, int, int)));
    connect(vdesktop, SIGNAL(desktopClicked()), this, SLOT(desktopClicked()));

    connect(vdesktop, SIGNAL(desktopBgMove(int)), this, SLOT(desktopBgMove(int)));
    connect(vdesktop, SIGNAL(desktopBgBack(int)), this, SLOT(desktopBgBack(int)));

    connect(_animationScreen,SIGNAL(finished()), this, SLOT(scrFinished()));
    connect(_animationMinScreen,SIGNAL(finished()), this, SLOT(scrMinFinished()));
    connect(_animationMinScreen, SIGNAL(valueChanged(const QVariant&)), this, SLOT(valueChanged(const QVariant&)));
//    connect(vdesktop, SIGNAL(setDirIcon(const QString&, const QString&, const QString&)),
//            _dirWidget, SLOT(addDirIcon(const QString&, const QString&, const QString&)));


//    QPixmap backPix(":images/bs_goback.png");
//    QPixmap backPixHover(":images/bs_goback_hove.png");
//    _backBtn = new DynamicButton(backPix, backPixHover, this);

//    _backBtn = new QPushButton(this);
//    _backBtn->setStyleSheet\
//                    ("QPushButton{background-image:url(:images/bs_goback.png);\
//                       border-style:flat;background-color:transparent;}\
//                    QPushButton:hover:pressed{\
//                        background-image:url(:images/bs_goback_hover.png);border-style:flat;background-color:transparent;} \
//                    QPushButton:hover{\
//                        background-image:url(:images/bs_goback.png);border-style:flat;background-color:transparent;}");

//    _backBtn->setGeometry(60, r.height() / 2,\
//                                   50, 50);
//    _backBtn->setVisible(false);
//    connect(_backBtn, SIGNAL(clicked()), this, SLOT(goDesktop()));//
}

void Dashboard::desktopClicked()
{
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);
}

void Dashboard::desktopOpenMove(int x, int y, int w, int h, int distance, int desktopDistance)
{
    if (_animationScreen->state() == QAbstractAnimation::Running)
    {
        return;
    }

    panel->setVisible(false);
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

    indicator->setVisible(false);
//    _dirWidget->move(x, y);
//    _dirWidget->setVisible(true);

    _mW->resize(QSize(w, h));

    QPixmap result = QPixmap();
    result = QPixmap::grabWindow(this->winId(), \
                                 x, y, \
                                 w, h); //抓取当前屏幕的图片
    _mW->setPixmap(result);
    _mW->setVisible(true);

//    _animationScreen = new QPropertyAnimation(_mW, "geometry");
    _animationScreen->setDuration(500);
    _animationScreen->setStartValue(QRect(x, y, w, h));
    _animationScreen->setEndValue(QRect(x, y + distance, w, h));
    _animationScreen->start();
}

void Dashboard::desktopCloseMove(int x, int y, int w, int h, int distance, int desktopDistance)
{
    if (_animationScreen->state() == QAbstractAnimation::Running)
    {
        return;
    }

    qDebug() << desktopDistance;

    _animationScreen->setDuration(500);
    _animationScreen->setStartValue(QRect(x, y, w, h));
    _animationScreen->setEndValue(QRect(x, y - distance, w, h));
    _animationScreen->start();
    _animationFinished = true;

}

void Dashboard::scrFinished()
{
    if (_animationFinished)
    {
//        _dirWidget->setVisible(false);
        _mW->setVisible(false);
//        _minW->setVisible(false);
        _minLabel->setVisible(false);
        _animationFinished = false;
        indicator->setVisible(true);
        vdesktop->setDirHide();
        panel->setVisible(true);
        panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
        panel->show();
        panel->setAutoHide(true);
        panel->animationHide();
        vdesktop->setIconEnabled(true);
    }
}

void Dashboard::openMinWidget(int x, int y, int w, int h, int distance)
{
    if (_animationMinScreen->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _distance = distance;
    _minY = y;
    _minUpward = false;

    _minLabel->setGeometry(x, y + 1, w, h);
    _minLabel->setVisible(true);

    _minW->resize(QSize(w, h));

    QPixmap result = QPixmap();
    result = QPixmap::grabWindow(this->winId(), \
                                 x, y, \
                                 w, h); //抓取当前屏幕的图片
    _minW->setPixmap(result);
    _minW->setVisible(true);

//    _animationScreen = new QPropertyAnimation(_mW, "geometry");
    _animationMinScreen->setDuration(500);
    _animationMinScreen->setStartValue(QRect(x, y, w, h));
    _animationMinScreen->setEndValue(QRect(x, y + distance, w, h));
    _animationMinScreen->start();
}

void Dashboard::closeMinWidget(int x, int y, int w, int h, int distance)
{
    if (_animationMinScreen->state() == QAbstractAnimation::Running)
    {
        return;
    }

//    _distance = distance;
    _minY = y;
    _minUpward = true;

    _minW->setVisible(true);
    _animationMinScreen->setDuration(500);
    _animationMinScreen->setStartValue(QRect(x, y, w, h));
    _animationMinScreen->setEndValue(QRect(x, y - distance, w, h));
    _animationMinScreen->start();
}

void Dashboard::scrMinFinished()
{
    _minW->setVisible(false);
}

void Dashboard::desktopBgMove(int distance)
{
    qDebug() << "*************!@#*********!@#*********@#$*******@#$*************$#$%*#$*%*********!!****** ";
//    if (_animationDesktop->state() == QAbstractAnimation::Running)
//    {
//        return;
//    }
//    qDebug() <<"()()()()()()()()()()()()()()() "<< vdesktop->y();
//    qDebug() <<"()()()()()()()()()()()()()()() "<< vdesktop->y() - distance;

    _animationDesktop->setDuration(500);
    _animationDesktop->setEasingCurve(QEasingCurve::InExpo);
    _animationDesktop->setStartValue(QPoint(0, 0));
    _animationDesktop->setEndValue(QPoint(0, vdesktop->y() - distance));
    _animationDesktop->start();
}

void Dashboard::valueChanged(const QVariant &value)
{
    QRect rect = value.value<QRect>();
    if (((rect.y() - _minY) > 0 ? rect.y() - _minY : _minY - rect.y()) >= _distance / 2)
    {
        qDebug() << rect.y() - _minY;
        if (!_minUpward)
        {
            _minW->setVisible(false);
        }
        else
        {
            _minW->setVisible(true);
        }
    }
    else
    {
        if (!_minUpward)
        {
            _minW->setVisible(true);
        }
        else
        {
            _minW->setVisible(false);
        }
    }

}

void Dashboard::desktopBgBack(int distance)
{
    qDebug() << "*********%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%^%^%%%%%$%%%%%^^^%%^%^%^%^%^%^%^%^%^%^%^%^%^***** ";
//    if (_animationDesktop->state() == QAbstractAnimation::Running)
//    {
//        return;
//    }
//    qDebug() <<"()()()()()()()()()()()()()()() "<< vdesktop->y();
//    qDebug() <<"()()()()()()()()()()()()()()() "<< vdesktop->y() + distance;

    _animationDesktop->setDuration(500);
    _animationDesktop->setStartValue(QPoint(0, -168));
    _animationDesktop->setEndValue(QPoint(0, 0));
    _animationDesktop->start();
}

void Dashboard::showBs(const QString &url)
{
    qDebug() << "url"<< "url" << url;
    vdesktop->setVisible(false);
    _bsWidget->setUrl(url);
    _bsWidget->setVisible(true);
//    _backBtn->setVisible(true);
    panel->setVisible(false);

}

void Dashboard::goDesktop()
{
//    _backBtn->setVisible(false);
    vdesktop->setVisible(true);
    _bsWidget->setVisible(false);
    panel->setVisible(true);


}

void Dashboard::onDone()
{
    //get the Application List;
    _finished = true;
}

void Dashboard::heartbeat()
{
//    if(!_Isheartbeat)
//        return;
//    _commui->heartBeat();

//    if(_commui->_isNetError)
//    {
//        if( _retryTimes > 0)
//        {
//            heartbeat_timer->start(10);
//            _retryTimes--;
//        }
//        else
//        {
//            heartbeat_timer->start(5000);
//        }
//    }
//    else
//    {
//        _retryTimes = 3;
//        heartbeat_timer->start(5000);
//    }

//    while (!_finished)
//        QApplication::processEvents();
//    _finished = false;

//    if(_commui->errID == "10036")
//    {
//#ifdef Q_WS_WIN
//        m_dllCloseAppAll();
//#endif
//        _Isheartbeat=false;
//        QMessageBox::warning(this, tr("your seesion has to be ticked out."),tr("Your session has to be kicked out by the administrator, please contact the administrator"), tr("ok"));
//        //qApp->quit();
//    }
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
//    qDebug() << " 11111111111111";
//}

void Dashboard::onShowVacDesktop()
{
//    qDebug() << "onShowVacDesktoponShowVacDesktoponShowVacDesktoponShowVacDesktoponShowVacDesktop";
//    _perWidget->setVisible(false);

//    vdesktop->setVisible(false);

//    _bsWidget->setVisible(true);


    _localShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

    if (_vacShowWidget->isVisible())
    {
        _vacShowWidget->setVisible(false);
        panel->setAutoHide(true);
    }
    else
    {
        _vacShowWidget->setVisible(true);
        panel->setWindowFlags(panel->windowFlags() & ~Qt::WindowStaysOnTopHint);
        panel->show();
        panel->setAutoHide(false);
        panel->animationShow();
    }

}

void Dashboard::onShowLocalDesktop()
{
//    panel->setWindowFlags(panel->windowFlags() & ~Qt::WindowStaysOnTopHint);
//    panel->show();
//    panel->setAutoHide(false);
//    panel->animationShow();

    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);


    if (_localShowWidget->isVisible())
    {
        _localShowWidget->setVisible(false);
        panel->setAutoHide(true);
    }
    else
    {
        _localShowWidget->setVisible(true);
        panel->setWindowFlags(panel->windowFlags() & ~Qt::WindowStaysOnTopHint);
        panel->show();
        panel->setAutoHide(false);
        panel->animationShow();
    }

}

void Dashboard::onShowDirDesktop()
{
    _vacShowWidget->setVisible(false);
    _localShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

    panel->setAutoHide(true);

    vdesktop->addDirItem();

}

void Dashboard::onShowPerDesktop()
{

    _localShowWidget->setVisible(false);
    _vacShowWidget->setVisible(false);

    if (_skinShowWidget->isVisible())
    {
        _skinShowWidget->setVisible(false);
        panel->setAutoHide(true);
    }
    else
    {
        _skinShowWidget->setVisible(true);
        panel->setWindowFlags(panel->windowFlags() & ~Qt::WindowStaysOnTopHint);
        panel->show();
        panel->setAutoHide(false);
        panel->animationShow();
    }


//    if (_bsWidget->isVisible())
//        _bsWidget->setVisible(false);
//    if (vdesktop->isVisible())
//        vdesktop->setVisible(false);

//    _perWidget->setVisible(true);
}

void Dashboard::moveIndicator()
{
    indicator->move((_width - indicator->width())/2, _height - indicator->height() - 50);
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
    panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();

    indicator->show();

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
    panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();
    indicator->hide();
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _outOfScreen = !_outOfScreen;
    _animation->start();
}

void Dashboard::switchBetween()
{
    if (_localShowWidget->isVisible())
    {
        _localShowWidget->setVisible(false);
//        return;
    }

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

void Dashboard::setGeoProper()
{
    setGeometry(0, 0, _width, _height);
	setFixedSize(_width, _height);
}

void Dashboard::iconActivated(QSystemTrayIcon::ActivationReason)
{ 
    showAction->setEnabled(true);
    hideAction->setEnabled(true);
    if (this->isVisible() && !isMinimized())
        showAction->setEnabled(false);
    else
        hideAction->setEnabled(false);
}

void Dashboard::quit()
{
    AppMessageBox box(true, NULL);
    box.setText("是否确定退出？");
    if (box.exec()) {

        vdesktop->atExit();

//        _Isheartbeat = false;
//        _commui->logoff();
//        while (!_finished)
//        {
//            QApplication::processEvents();
//        }

        qApp->quit();
    }
} 
    
void Dashboard::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

void Dashboard::resizeEvent(QResizeEvent *event)
{
    QPixmap background(_pixmap);
    QPixmap resizeBackgroud=background.scaled(event->size());
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(resizeBackgroud));
    setPalette(palette);
}

void Dashboard::setBgPixmap(const QString &pixText)
{
    _pixText = pixText;

    _pixmap.load(_pixText);
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(_pixmap.scaled(width(), height())));
    setPalette(palette);

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
}

void Dashboard::errOut()
{
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, _ldialog->GetSystemInfo());

    while (!_finished)
        QApplication::processEvents();
    _finished = false;

    _Isheartbeat = true;
}

void Dashboard::refreshDesktop()
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

    qDebug() << "refresh desktop";
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
        qDebug() << "********" << g_myVappList.count();

        if (g_myVappList.count() == 0)
            return;

        #ifdef Q_WS_WIN
        QString iconDirPath = WIN_VAPP_IconPath ;
        #else
        QString iconDirPath =  xmlPath + "\\App Center\\Vicons";
        #endif

        QDir iconDir(iconDirPath);
        if(!iconDir.exists())
        {
            iconDir.mkdir(iconDirPath);
        }
        //store ico file locally
        for(int i = 0; i < g_myVappList.count(); i++)
        {
            QString iconPath = QString("%1%2.ico")
                    .arg(iconDirPath)
                    .arg(g_myVappList[i].id);
    //        qDebug()<<"iconPath="<<iconPath;

            //check if ico file is existed, or dont donwload

            QFile chkFile(iconPath);
            if(chkFile.exists())
            {
                chkFile.close();
                continue;
            }
            chkFile.close();

            //qDebug()<<"iconPath"<<iconPath;
            _commui->downloadIcon(QUrl(g_myVappList[i].icon), iconPath);
             while (!_finished)
                QApplication::processEvents();
            _finished = false;

            //ico 95 * 95
             QString newApp = iconPath;

             if (newApp.isEmpty())
                 return;

             QImage image = QImage(newApp).scaled(48, 48);
             QImage normal = QImage(":images/app_bg.png");

             for (int i = 0; i < normal.width(); i++) {
                 for (int j = 0; j < normal.height(); j++) {
                     QRgb pixel = normal.pixel(i,j);
                     int a = qAlpha(pixel);
                     QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                             qBlue(pixel), a * 0 / 255);
                     normal.setPixel(i, j, lightPixel);
                 }
             }

             QPainter pt1(&normal);
             pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
             pt1.drawImage(17, 8, image);
             pt1.end();
             QPixmap pix = QPixmap::fromImage(normal);
             pix.save(newApp, "ICO", -1);
        }

        modify();
    }

}

void Dashboard::timeOut()
{
    refreshDesktop();
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
                LocalAppList::getList()->delApp(g_RemoteappList[i].name);
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
                LocalApp *RemoteApp = new LocalApp();
                RemoteApp->setName(g_myVappList[i].name);
                RemoteApp->setId(g_myVappList[i].id);
                RemoteApp->setIcon(WIN_VAPP_IconPath + g_myVappList[i].id + ".ico");
                RemoteApp->setPage(vdesktop->count() - 1);
                RemoteApp->setIndex(-1);
                RemoteApp->setType(g_myVappList[i].type);
                RemoteApp->setIsRemote(true);
                LocalAppList::getList()->addRemoteApp(RemoteApp);

                vdesktop->addIcon(g_myVappList[i].name, WIN_VAPP_IconPath + g_myVappList[i].id + ".ico", \
                                  vdesktop->count() - 1, -1, VirtualDesktop::vappIcon);
            }
        }
    }

    g_RemoteappList.clear();

    for(int i = 0; i < g_myVappList.count(); i++)
    {
        QString iconPath = QString("%1%2.ico")
                .arg(iconDirPath)
                .arg(g_myVappList[i].id);

        g_RemoteappList.insert(i, g_myVappList[i]);
//        qDebug()<<"g_mylist:"<<g_myVappList[i].icon;
        g_RemoteappList[i].icon = iconPath;
//        qDebug()<<"g_Rmote:"<<g_RemoteappList.at(i).icon;
    }

}

void Dashboard::setVacServer()
{
    _vacServerWidget->setVisible(true);
}

void Dashboard::updateVacServer()
{
    _ldialog->updateVacServer();

//    _Isheartbeat = false;
    if (_commui->errID == "10000")
        _commui->logoff();
    else
        errOut();

}
