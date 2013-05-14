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

    _configMenu = new ConfigMenu(this);
    _configMenu->move(_width / 3 * 2 - 200, 40);
    _configMenu->setVisible(false);

    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT wallpaper FROM wallpapers where id=\'%1\';").arg(1));
    while (query.next())
        _pixmap = QPixmap(query.value(0).toString());

    connect(_configMenu, SIGNAL(setBgPixmap(const QString&)), this, SLOT(setBgPixmap(const QString&)));
    connect(_configMenu, SIGNAL(upLoad()), vdesktop, SLOT(upLoad()));
    connect(panel, SIGNAL(quit()), this, SLOT(quit()));
    connect(panel, SIGNAL(desktop()), this, SLOT(onShowDesktop()));
    connect(panel, SIGNAL(iconDialog()), this, SLOT(onIconDialog()));
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

    connect(vdesktop, SIGNAL(hideMenu()), this, SLOT(hideMenu()));
}

void Dashboard::goPage(int page)
{
    _configMenu->setVisible(false);
    vdesktop->goPage(page);
}

void Dashboard::hideMenu()
{
    _configMenu->setVisible(false);
}

void Dashboard::onShowDesktop()
{
    if (_outOfScreen)
        getIn();
}

void Dashboard::onIconDialog()
{
    _configMenu->setVisible(true);

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
