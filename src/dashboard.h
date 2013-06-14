#ifndef MAINWINDOW
#define MAINWINDOW
#include <QWidget>
#include <QSize>
#include <QStackedLayout>
#include <QSystemTrayIcon>
#include <QDialog>
//vac
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QAction>
#include <QMenu>

#include "commuinication.h"
//#include "personalizationwidget.h"
#include "bswidget.h"
#include "logindialog.h"
#include "vacserverwidget.h"
#include "localwidget.h"
#include "vacwidget.h"
#include "skinwidget.h"
#include "movewidget.h"
#include "dirwidget.h"

class QVBoxLayout;
class AppWidget;
class QAction;
class VirtualDesktop;
class QPropertyAnimation;
//vac
class TopWidget;
class Switcher;
class Panel;
class Indicator;

class Dashboard : public QWidget
{
    Q_OBJECT
public:
    Dashboard(QWidget * parent = NULL);
    ~Dashboard() { }//delete _ldialog; }
    QSize size;
//vac
   void setInfo(QString smIP, QString sID)
    {
        _smIP = smIP;
        _sID = sID;
    }
public slots:
    void quit();
    void switchBetween();
    void iconActivated(QSystemTrayIcon::ActivationReason);
    void moveIndicator();
	//vac
//	void fadeaway();
//	void changepwd();
//	void autologin();
//	void showhide();
//	void onDone();
//	void setmenu();
//	void menuhide();
//	void RefreshMenu();
//	void heartbeat();
    //void MenuTest();
	//vac end
//    void onShowSwitcherDesktop();
    void onShowVacDesktop();
    void onShowLocalDesktop();
    void onShowDirDesktop();
    void onShowPerDesktop();
    //void onShowStore();

    void setBgPixmap(const QString &pixText);
    void goPage(int page);

    void timeOut();
    //vac
    void heartbeat();
    void onDone();
    void errOut();

    void setVacServer();
    void updateVacServer();

    void showBs(const QString &url);
    void goDesktop();
    void desktopOpenMove(int x, int y, int w, int h);
    void desktopCloseMove(int x, int y, int w, int h);
    void scrFinished();
    void  openMinWidget(int x, int y, int w, int h);
    void closeMinWidget(int x, int y, int w, int h);
    void scrMinFinished();

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * event);
private:
    void refreshDesktop();
    void modify();
private:
	//vac 
    int ind_height;
    int _width;
    int _height;
    bool _outOfScreen;
	//vac
	bool _showHide;
    void setGeoProper();
	//vac
	QMenu *menu;
	QPushButton *setPushButton;
    QPushButton *quitPushButton;
	QPropertyAnimation *_Topanimation;
	QAction *trayShowhideAction;

    QAction *menu_username;
    QAction *menu_refresh;

    QAction *menu_hideAction;
    void getIn();
    void getOut();
     //??//
    int _wid;
    QAction *changepwdAction;
    QAction *autologinAction;
    QString _sID;
    QString _smIP;
    commuinication *_commui;
    QTimer *heartbeat_timer;  //heart beat.
    bool  _Isheartbeat;    // check if send the heart beat packet
    int _retryTimes;
    bool _finished;
	 //??//
    TopWidget *topWidget;
    QPropertyAnimation *_animation;
    QSystemTrayIcon *tray;
    QAction *quitAction;
    QAction *hideAction;
    QAction *showAction;
    QAction *_setVacServer;
    VirtualDesktop *vdesktop;
    Switcher *switcher;
    Panel *panel;
    Indicator *indicator;
    QVBoxLayout *layout;

    QPixmap _pixmap;
    QString _pixText;

//    PersonalizationWidget *_perWidget;
    BsWidget *_bsWidget;
    QTimer *_refreshTimer;

    LoginDialog *_ldialog;
    VacServerWidget *_vacServerWidget;

    LocalShowWidget *_localShowWidget;
    VacShowWidget *_vacShowWidget;
    SkinWidget *_skinShowWidget;

    QPropertyAnimation *_animationScreen;
    QPropertyAnimation *_animationMinScreen;
    MoveWidget *_mW;
    MoveMinWidget *_minW;
    DirShowWidget *_dirWidget;
    bool _animationFinished;
    QLabel *_minLabel;

    QPushButton* _backBtn;
};
#endif
