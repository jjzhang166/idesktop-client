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
//#include "bswidget.h"
#include "logindialog.h"
#include "vacserverwidget.h"
#include "vacwidget.h"
#include "skinwidget.h"
#include "movewidget.h"
#include "dirwidget.h"
#include "blurpicker.h"
#include "nodebutton.h"
#include "toolbarwidget.h"

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
    void outOfScreen();
    void inOfScreen();
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
    void upMove(int x, int y, int w, int h, int distance);
    void upBackMove(int x, int y, int w, int h, int distance);
    void downMove(int x, int y, int w, int h, int distance);
    void downBackMove(int x, int y, int w, int h, int distance);
//    void animationUpFinished();
//    void animationDownFinished();
    void animationFinished();
    void openMinWidget(int x, int y, int w, int h, int distance);
    void closeMinWidget(int x, int y, int w, int h, int distance);
    void upMinMove(int x, int y, int w, int h, int distance);
    void upMinBackMove(int x, int y, int w, int h, int distance);
    void animationMinDownFinished();

    void valueChanged(const QVariant &value);
    void desktopClicked();

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void desktopPageChanged(int page);

//    void onOutFinished(QNetworkReply *reply);
    void updateNodes();

    void setMinMoveLabel(bool up);
    void getIn();
    void getOut();

    void vdesktopHideDirWidget();

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * event);
    void paintEvent(QPaintEvent *event);
private:
    void refreshVapp();
    void modify();

    void refreshPaas();
    void paasModify();

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
//    void getIn();
//    void getOut();
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
    Switcher *_switcherLeft;
    Switcher *_switcherRight;
    Panel *panel;
//    Indicator *indicator;
    QVBoxLayout *layout;

    QPixmap _pixmap;
    QString _pixText;

//    PersonalizationWidget *_perWidget;
//    BsWidget *_bsWidget;
    QTimer *_refreshTimer;

    LoginDialog *_ldialog;
    VacServerWidget *_vacServerWidget;

//    LocalShowWidget *_localShowWidget;
    VacShowWidget *_vacShowWidget;
    SkinShowWidget *_skinShowWidget;

    QPropertyAnimation *_animationUp;
    QPropertyAnimation *_animationDown;
    QPropertyAnimation *_animationDownMin;
    QPropertyAnimation *_animationUpMin;
    MoveWidget *_upMoveWidget;
    MoveWidget *_downMoveWidget;
    MoveMinWidget *_downMinW;

//    MoveMinWidget *_minW;
    DirShowWidget *_dirWidget;
    bool _animationDownFinished;
    bool _animationUpFinished;
    QLabel *_minDirLabel;
    QLabel *_minToolBarLabel;

    QPushButton* _backBtn;
    int _distance;
    int _minY;
    bool _minUpward;
    BlurPicker *_blurPicker;

    QNetworkAccessManager *_nam;
    PageNodes *_pageNodes;
    toolBarWidget *_toolBarWidget;

    bool _minMoveLabelUp;
//miya add
public:
    void setBgImage(QString url);

};
#endif
