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

#include "commuinication.h"
#include "logindialog.h"
#include "vacwidget.h"
#include "skinwidget.h"
#include "movewidget.h"
#include "nodebutton.h"
#include "mask.h"
#include "paascommuinication.h"

class Panel;
class Switcher;
class DirContainer;
class Docker;
class GridContainer;
class QVBoxLayout;
class AppWidget;
class QAction;
class IDesktopSettings;
class MenuWidget;

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

   void getLocalIcon(bool isLogin);
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

    //vac
    void onDone();
    void errOut();
    void onPaasDone();
    void desktopClicked();

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void desktopPageChanged(int page);

    void updateNodes();

    void getIn();
    void getOut();

    void vdesktopHideDirWidget();

    //menu
    void menuChanged(int value);
    void virtualDesktopAddDirItem();
    void hideMenuWidget();
    void refreshMenu();
    void showNormalMenu(QPoint mousePos);

    void moveAppToTrash(LocalApp *app);
    void handleOpenDir(DirContainer *dirContainer);
    void handleEraseApp(LocalApp*);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * event);
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

private:
    void refreshLocal();
    void localModify();

    void refreshVapp();
    void modify();

    void refreshPaas();
    void paasModify();
    void loadPixmaps();
    void getDbIcon();

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
    QPropertyAnimation *_animation;
    QSystemTrayIcon *tray;
    QAction *quitAction;
    QAction *hideAction;
    QAction *showAction;
    QAction *_setVacServer;
    GridContainer *vdesktop;
    Switcher *_switcherLeft;
    Switcher *_switcherRight;
    Panel *panel;
    QVBoxLayout *layout;

    QPixmap _pixmap;
    QString _pixText;

    LoginDialog *_ldialog;

    VacShowWidget *_vacShowWidget;
    SkinShowWidget *_skinShowWidget;

    QPushButton* _backBtn;

    QNetworkAccessManager *_nam;
    QNetworkAccessManager *_namLogout;
    PageNodes *_pageNodes;
    Docker *_docker;

    IDesktopSettings *_settings;

    Mask *_mask;

    //menu
    MenuWidget *_normalMenu;
    MenuWidget *_showIconMenu;
    MenuWidget *_createMenu;

    QSize _normalMenuSize;
    QSize _showIconMenuSize;
    QSize _createMenuSize;

    int _normalMenuX;
    int _normalMenuY;
    int _showIconMenuX;
    int _showIconMenuY;
    int _createMenuX;
    int _createMenuY;

    QPoint _mousePos;

    QString _uniqueName;

    LocalAppList *_local;

public:
    void setBgImage(QString url);

};
#endif
