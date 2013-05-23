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
#include "logindialog.h"
#include "config.h"

extern QString xmlPath;
extern QString iconDirPath;
extern QString WIN_VAPP_IconPath;
extern QString WIN_TtempPath;

extern QList<APP_LIST> g_myVappList;
extern QList<APP_LIST> g_RemoteappList;

Dashboard::Dashboard(QWidget *parent)
    :QWidget(parent, Qt::FramelessWindowHint | Qt::Tool), \
     _outOfScreen(false), _settingDialog(NULL)
{
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

    LoginDialog *ldialog = new LoginDialog(this);
    if (!ldialog->exec())
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

    quitAction = new QAction("ÍË³ö", this);
    showAction = new QAction("ÏÔÊ¾", this);
    hideAction = new QAction("Òþ²Ø", this);
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

}

void Dashboard::onDone()
{
    //get the Application List;
    _finished = true;
}

void Dashboard::goPage(int page)
{
//    if (page < 2)
//    {
//        _perWidget->setVisible(false);
//        vdesktop->setVisible(true);
//    }

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
    vdesktop->atExit();

    _commui->logoff();
    while (!_finished)
    {
        QApplication::processEvents();
    }

    qApp->quit();
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

    //delete old page.
    vdesktop->movetoFist();
    vdesktop->deleteAllIconItem();

    int count = vdesktop->_count;
    for(int i = count; i > 1; i--)
    {
        vdesktop->delPage(i-1);
    }

    qDebug()<<"clear over.."<<endl;

    g_myVappList.clear();
    g_RemoteappList.clear();

    //get vapp list
    _commui->getAppList();
    while (!_finished)
        QApplication::processEvents();
    _finished = false;

    g_myVappList = _commui->getList();
    qDebug()<<"g_myList.count()="<<g_myVappList.count();
    //qDebug()<<"g_myList:"<<g_myVappList[0].name;

    #ifdef Q_WS_WIN
    QString iconDirPath = WIN_VAPP_IconPath ;
    #else
    QString iconDirPath =  xmlPath + "\\App Center\\Vicons";
    #endif

    //qDebug()<<"############## WIN_IconPath"<<WIN_IconPath;
    //qDebug()<<"############## iconDirPath"<<iconDirPath;

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
        //qDebug()<<"iconPath="<<iconPath;
        g_RemoteappList.insert(i, g_myVappList[i]);
        qDebug()<<"g_mylist:"<<g_myVappList[i].icon;
        g_RemoteappList[i].icon = iconPath;
        qDebug()<<"g_Rmote:"<<g_RemoteappList.at(i).icon;
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

    LocalAppList::getList()->updateAppList();
     //after we get the new app list,we can add it to the page
     //first we must clear some value which in the vdesktop;
//     qDebug()<<"get the new appp list the count is:"<<g_myVappList.count();

     vdesktop->_pages.clear();
     vdesktop->_iconTable.clear();
     vdesktop->_nextIdx.clear();
     vdesktop->_iconDict.clear();
     vdesktop->_gridTable.clear();

//     vdesktop->_count = 1;
//     qDebug()<<"the new page count is"<<vdesktop->_count;

//     //before we add the icon,we need download the icon from the server.
//
    vdesktop->reloadApplist();
}

void Dashboard::timeOut()
{
    refreshDesktop();
}
