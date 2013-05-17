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
#include "../commuinication.h"
#include "iconarrangedlg.h"
//#include "configmenu.h"
#include "personalizationwidget.h"
#include "bswidget.h"

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
    ~Dashboard() {}
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
    void onShowPerDesktop();
    void onShowDesktop();
    void onShowBsDesktop();
    //void onShowStore();

    void setBgPixmap(const QString &pixText);
    void goPage(int page);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * event);
private:
	//vac 
	int _retryTimes;
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
    bool _finished;
    QTimer *heartbeat_timer;  //heart beat.
    bool  Isheartbeat;    // check if send the heart beat packet
	 //??//
    TopWidget *topWidget;
    QPropertyAnimation *_animation;
    QSystemTrayIcon *tray;
    QAction *quitAction;
    QAction *hideAction;
    QAction *showAction;
    VirtualDesktop *vdesktop;
    Switcher *switcher;
    Panel *panel;
    Indicator *indicator;
    QVBoxLayout *layout;
    IconArrangeDlg *_settingDialog;

    QPixmap _pixmap;
    QString _pixText;

    PersonalizationWidget *_perWidget;
    BsWidget *_bsWidget;
};
#endif
