#include <QtGui>
#include <QtSql/QtSql>

#include <iostream>

#ifdef LINUX_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif
#include <windows.h>
#include <shldisp.h>

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

extern QString xmlPath;
extern QString WIN_VAPP_IconPath;
extern QString WIN_PAAS_IconPath;
extern QString WIN_TtempPath;

extern QString USERNAME;

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

    _ldialog = new LoginDialog(this);
    if (!_ldialog->exec())
        exit(1);

    QDesktopWidget *d = QApplication::desktop();
    QRect r = d->availableGeometry();
    _width = r.width();
    _height = r.height();

    setGeometry(0, 0, _width, _height);

    vdesktop = new VirtualDesktop(QSize(_width, _height), this);

    panel = new Panel(this);
    panel->setGeometry(0, 0, _width, 28);
    panel->setVisible(true);
    panel->animationHide();

    _switcherLeft = new Switcher(this);
    _switcherLeft->setPixmap(QString(":images/win_normal.png"));
    _switcherLeft->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);
    _switcherLeft->setGeometry(_width / 2 - _switcherLeft->width(), 0, _switcherLeft->width(), _switcherLeft->height());
    _switcherLeft->show();
    _switcherLeft->activateWindow();

    _switcherRight = new Switcher(this);
    _switcherRight->setPixmap(QString(":images/isoft_normal.png"));
    _switcherRight->setWindowFlags(_switcherLeft->windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);
    _switcherRight->setGeometry(_switcherLeft->pos().x() + _switcherLeft->width(), 0, _switcherRight->width(), _switcherRight->height());
    _switcherRight->show();
    _switcherRight->activateWindow();

    vdesktop->setGeometry(QRect(0, 0, _width * vdesktop->count(), _height));

    _toolBarWidget = new toolBarWidget(QSize(_width, 103 + 0 + 24), this);
    _toolBarWidget->move(0,_height - (103 + 0 + 24));//
    _toolBarWidget->raise();
    _toolBarWidget->show();

    _pageNodes = new PageNodes(this);

    _pageNodes->update(vdesktop->count(), vdesktop->currentPage());
    _pageNodes->move((_width - _pageNodes->width()) / 2, _height - _pageNodes->height() - 100 - 35);
    _pageNodes->setVisible(true);

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
    connect(_upMoveWidget, SIGNAL(mousePress()), this, SLOT(vdesktopHideDirWidget()));
    connect(_downMoveWidget, SIGNAL(mousePress()), this, SLOT(vdesktopHideDirWidget()));

    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));
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
    connect(_ldialog, SIGNAL(dVacServer()), this, SLOT(setVacServer()));
    connect(_ldialog, SIGNAL(dActivated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(vdesktop, SIGNAL(toOrigin()), this, SLOT(switchBetween()));
    connect(vdesktop, SIGNAL(largeIcon()), this, SLOT(largeIcon()));
    connect(vdesktop, SIGNAL(mediumIcon()), this, SLOT(mediumIcon()));
    connect(vdesktop, SIGNAL(smallIcon()), this, SLOT(smallIcon()));
    connect(vdesktop, SIGNAL(desktopDelIcon(const QString &))
            , _vacShowWidget, SLOT(desktopDelIcon(const QString &)));
    connect(vdesktop, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , _vacShowWidget, SLOT(iconItemNameChanged(const QString &, const QString &)));
    connect(vdesktop, SIGNAL(pageChanged(int)), this, SLOT(desktopPageChanged(int)));
    connect(vdesktop, SIGNAL(pageIncreased()), this, SLOT(updateNodes()));
    connect(vdesktop, SIGNAL(pageDecreased()), this, SLOT(updateNodes()));
    connect(vdesktop, SIGNAL(refreshVac()), this, SLOT(timeOut()));
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

    _toolBarWidget->initIconItem();

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
    vdesktop->setIconEnabled(true);
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
        }

        _animationUpFinished = false;

        _pageNodes->setVisible(true);

        vdesktop->setIconEnabled(true);
        vdesktop->setIconMove(true);
        _toolBarWidget->setVisible(true);

        _downMoveWidget->setVisible(false);

        vdesktop->dirMovingFinished();
    }
}

void Dashboard::downMove(int x, int y, int w, int h, int distance)
{
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
    _skinShowWidget->setVisible(false);
    _vacShowWidget->setVisible(!_vacShowWidget->isVisible());
}

void Dashboard::onShowSkinDesktop()
{
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(!_skinShowWidget->isVisible());
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
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _outOfScreen = !_outOfScreen;
    _animation->start();
}

void Dashboard::switchBetween()
{
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
    _vacShowWidget->setVisible(false);
    _skinShowWidget->setVisible(false);

    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();

    AppMessageBox box(true, NULL);
    box.setText("是否确定退出？");
    if (box.exec()) {

        QString quitUrl = Config::get("Server") + ":8080/idesktop/logout.action";

        QString data = "username=" + USERNAME;

        _nam->post(QNetworkRequest(QUrl(quitUrl)), data.toAscii());

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
//    qDebug() << "Dashboard::setBgImage(QString url)" << url;
    QString path = QCoreApplication::applicationDirPath();
    url.replace(QString(":"), QString(""));
    qDebug() << url;
    path += url;
    path.replace(QString("/"), QString("\\"));

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
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, _ldialog->GetSystemInfo());

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

        _ldialog->setIcon(WIN_VAPP_IconPath, tempPath);
    }

    _settings->setVappList(myVappList);
    modify();
}

void Dashboard::timeOut()
{
    refreshVapp();
    refreshPaas();
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

    _ldialog->getPaas(false);

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
//                LocalAppList::getList()->delApp(remoteAppList[i].name);
                _vacShowWidget->delIcon("2_" + remotePaasList[i].name);
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
