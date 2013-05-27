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

extern QString VappServer;
extern QString VappUser;
extern QString VappPassword;

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
    :QWidget(parent, Qt::FramelessWindowHint | Qt::Tool), \
     _outOfScreen(false), _settingDialog(NULL)
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


    QDesktopWidget *d = QApplication::desktop();
    QRect r = d->availableGeometry();
    _width = r.width();
    _height = r.height();

    setGeoProper();

    vdesktop = new VirtualDesktop(QSize(_width, _height), this);
    indicator = new Indicator(vdesktop, this);
    indicator->move((_width - indicator->width())/2, _height - indicator->height() - 20);
    //indicator->setGeometry((_width - indicator->width())/2, _height - indicator->height() - 28 , 20, 80);
//    indicator->show();
    indicator->hide();

    switcher = new Switcher(this);
    switcher->setGeometry(_width - switcher->width(), (_height - switcher->height())/2, 20, 80);
    switcher->show();

    panel = new Panel(this);
    panel->setFixedSize(442, 63);
    //panel->setFixedSize(_width / 3, 40);
    panel->move((_width - panel->width()) / 2, _height - panel->height() - 10);
    panel->show();
    panel->animationHide();
    panel->animationShow();

    //vdesktop->setGeometry(QRect(0, 20, _width * vdesktop->count(), _height - indicator->height() - panel->height()));
    vdesktop->setGeometry(QRect(0, 0, _width * vdesktop->count(), _height - 40));

    _bsWidget = new BsWidget(_width, _height, this);
    //_bsWidget->setGeometry(0, 0, _width, _height);
    _bsWidget->move(0, 0);
    _bsWidget->setVisible(false);

    _perWidget = new PersonalizationWidget(this);
    _perWidget->setGeometry(QRect(0, 0, _width, _height));
    _perWidget->setVisible(false);

    quitAction = new QAction("退出", this);
    showAction = new QAction("显示", this);
    hideAction = new QAction("隐藏", this);
    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction(showAction);
    trayMenu->addAction(hideAction);
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
    connect(_refreshTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
    _refreshTimer->start(1000 * 20);

    _finished = false;
    _commui = new commuinication();
    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));

    connect(_perWidget, SIGNAL(setBgPixmap(const QString&)), this, SLOT(setBgPixmap(const QString&)));
    connect(panel, SIGNAL(quit()), this, SLOT(quit()));
    connect(panel, SIGNAL(desktop()), this, SLOT(onShowDesktop()));
    connect(panel, SIGNAL(showBsDesktop()), this, SLOT(onShowBsDesktop()));
    connect(panel, SIGNAL(showPerDesktop()), this, SLOT(onShowPerDesktop()));
    connect(panel, SIGNAL(pageChanged(int)), this, SLOT(goPage(int)));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(showAction, SIGNAL(triggered()), this, SLOT(show()));
    connect(hideAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(showAction, SIGNAL(triggered()), switcher, SLOT(show()));
    connect(hideAction, SIGNAL(triggered()), switcher, SLOT(hide()));
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(switcher, SIGNAL(activated()), this, SLOT(switchBetween()));
    connect(vdesktop, SIGNAL(toOrigin()), this, SLOT(switchBetween()));
    connect(vdesktop, SIGNAL(toOrigin()), switcher, SLOT(changed()));
    connect(panel, SIGNAL(setEqual(int)), vdesktop, SLOT(arrangeEqually(int)));
    connect(panel, SIGNAL(setMini()), vdesktop, SLOT(arrangeMinimum()));
//    connect(indicator, SIGNAL(iNeedMove()), this, SLOT(moveIndicator()));
    //connect(panel, SIGNAL(showDesktop()), this, SLOT(onShowDesktop()));
    //setGeoProper();

    // heart beat.5s timer
    heartbeat_timer = new QTimer(this);
    connect( heartbeat_timer, SIGNAL(timeout()), this, SLOT(heartbeat()));
    heartbeat_timer->start(5000);
    _retryTimes = 3;
    _Isheartbeat=true;
}

void Dashboard::onDone()
{
    //get the Application List;
    _finished = true;
}

void Dashboard::heartbeat()
{
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
    _perWidget->setVisible(false);
    _bsWidget->setVisible(false);

    vdesktop->setVisible(true);
    vdesktop->goPage(page);
}

void Dashboard::onShowDesktop()
{
    if (_outOfScreen)
        getIn();
}

void Dashboard::onShowBsDesktop()
{

    _perWidget->setVisible(false);

    vdesktop->setVisible(false);

    _bsWidget->setVisible(true);

}

void Dashboard::onShowPerDesktop()
{
    if (_bsWidget->isVisible())
        _bsWidget->setVisible(false);
    if (vdesktop->isVisible())
        vdesktop->setVisible(false);

    _perWidget->setVisible(true);

    /*
    _settingDialog = new IconArrangeDlg(vdesktop, NULL);
    _settingDialog->exec();
    delete _settingDialog;
    _settingDialog = NULL;
    */
}

void Dashboard::moveIndicator()
{
//    indicator->move((_width - indicator->width())/2, _height - indicator->height() - 20);
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
    panel->setWindowFlags(panel->windowFlags() & ~Qt::WindowStaysOnTopHint);
    panel->show();
    panel->setAutoHide(false);
    panel->animationShow();
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
    panel->setWindowFlags(panel->windowFlags() | Qt::WindowStaysOnTopHint);
    panel->show();
    panel->setAutoHide(true);
    panel->animationHide();
//    indicator->hide();
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _outOfScreen = !_outOfScreen;
    _animation->start();
}

void Dashboard::switchBetween()
{
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

        _Isheartbeat = false;
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

}

void Dashboard::errOut()
{
    _commui->login(VappServer, VappUser, VappPassword, _ldialog->GetSystemInfo());
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


    if(_commui->errID == "10000")
    {
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
