#ifndef DASHBOARD
#define DASHBOARD
#include <QWidget>
#include <QSize>
#include <QStackedLayout>
#include <QSystemTrayIcon>
#include <QDialog>
#include <QContextMenuEvent>
//vac
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QAction>
#include <QMenu>

//#include "virtualdesktop.h"
#include "commuinication.h"
#include "logindialog.h"
#include "vacwidget.h"
#include "skinwidget.h"
#include "movewidget.h"
#include "dirwidget.h"
#include "nodebutton.h"
#include "toolbarwidget.h"
#include "mask.h"

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
class IDesktopSettings;

extern QString WIN_LOCAL_IconPath;
extern QString WIN_VAPP_IconPath;
extern QString WIN_PAAS_IconPath;
extern QString WIN_TtempPath;
extern QString iniPath;

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

   QString GetSystemInfo();
   QString getLocalIcon(QString localPath);
   void setIcon(const QString &dirPath, const QString &iconPath);

   void getLocalIcon();
   void getVacIcon();
   void getPaasIcon(bool isLogin);
public slots:
    void initIconItem();
    void quit();
    void switchBetween();
    void outOfScreen();
    void inOfScreen();
    void iconActivated(QSystemTrayIcon::ActivationReason);
    void onShowVacDesktop();
    void onShowSkinDesktop();

    void setBgPixmap(const QString &pixText);
    void goPage(int page);

    //vac
    void onDone();
    void errOut();

    void onPaasDone();

    void upMove(int x, int y, int w, int h, int distance);
    void upBackMove(int x, int y, int w, int h, int distance);
    void downMove(int x, int y, int w, int h, int distance);
    void downBackMove(int x, int y, int w, int h, int distance);
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

    void updateNodes();

    void setMinMoveLabel(bool up);
    void getIn();
    void getOut();

    void vdesktopHideDirWidget();

    //menu
    void menuChanged(int value);
    void virtualDesktopAddDirItem();
    void hideMenuWidget();
    void refreshMenu();
    void showDirMenu(QPoint mPos, const QString &uniqueName);
    void showIconMenu(QPoint mPos, const QString &uniqueName);
    void showNormalMenu(QPoint mousePos);
    void menuDirOpen();
    void menuDirDel();
    void menuDirClear();
    void menuIconRun();
    void menuIconDel();


protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * event);
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

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
	//vac
	QMenu *menu;
	QPushButton *setPushButton;
    QPushButton *quitPushButton;
	QPropertyAnimation *_Topanimation;
	QAction *trayShowhideAction;

    QAction *menu_username;
    QAction *menu_refresh;

    QAction *menu_hideAction;
     //??//
    int _wid;
    QAction *changepwdAction;
    QAction *autologinAction;
    QString _sID;
    QString _smIP;
    commuinication *_commui;
    bool _finished;
    //paas
    PaasCommuinication *_paasCommui;
    bool _paasFinished;

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
    QVBoxLayout *layout;

    QPixmap _pixmap;
    QString _pixText;

    LoginDialog *_ldialog;

    VacShowWidget *_vacShowWidget;
    SkinShowWidget *_skinShowWidget;

    QPropertyAnimation *_animationUp;
    QPropertyAnimation *_animationDown;
    QPropertyAnimation *_animationDownMin;
    QPropertyAnimation *_animationUpMin;
    MoveWidget *_upMoveWidget;
    MoveWidget *_downMoveWidget;
    MoveMinWidget *_downMinW;

    DirShowWidget *_dirWidget;
    bool _animationUpFinished;
    bool _animationDownFinished;
    QLabel *_minDirLabel;
    QLabel *_minToolBarLabel;

    QPushButton* _backBtn;
    int _distance;
    int _minY;
    bool _minUpward;

    QNetworkAccessManager *_nam;
    PageNodes *_pageNodes;
    toolBarWidget *_toolBarWidget;

    bool _minMoveLabelUp;

    IDesktopSettings *_settings;

    Mask *_mask;

    //menu
    MenuWidget *_normalMenu;
    MenuWidget *_showIconMenu;
    MenuWidget *_createMenu;
    MenuWidget *_iconMenu;
    MenuWidget *_dustbinMenu;
    MenuWidget *_dirMenu;

    QPoint _mousePos;

    QString _uniqueName;

    LocalAppList *_local;

public:
    void setBgImage(QString url);

};
#endif
