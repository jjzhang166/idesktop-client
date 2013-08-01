#ifndef VIRTUALDESKTOP_H
#define VIRTUALDESKTOP_H
#include <QWidget>
#include <QMap>
#include <QProcess>
#include <QTimer>
#include <QMenu>
#include <QAction>

#include "localapps.h"
#include "commuinication.h"
#include "arrangewidget.h"
#include "hovericonitem.h"
//#include "movewidget.h"
#include "iconitem.h"
#include "dirwidget.h"
#include "dirminwidget.h"
#include "contextmenuwidget.h"
#include "toolbarwidget.h"
#include "qitemmanager.h"
class ArrangeWidget;

//app
#ifdef Q_WS_WIN
#include <QLibrary>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <windows.h>
#include <shellapi.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QtNetwork>
#include <QTimer>
#include <QPoint>

typedef  unsigned long DWORD;

//typedef  wchar_t * LPCWSTR;
//#define WINAPI __stdcall
/*typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;
*/

extern QString _icontext;
typedef long (*Dll_InitClass)(LPCWSTR, LPCWSTR);
typedef void (*Dll_CloseClass)();
typedef void (*Dll_CloseAppAll)();
typedef long (*Dll_StartAppEx)(DWORD windowstyle,				//8
                DWORD printer,					//
                DWORD localime,					//
                DWORD AudioRedirectionMode,		//
                DWORD ColorDepth,				//
                DWORD WindowHandle,				//28
                DWORD AppStyle,					//32
                GUID* guidSessionKey, 			//?
                LPCWSTR szSessionId,	//		SM_Id,
                LPCWSTR szPassword,		//password
                LPCWSTR szUsername,		//username
                LPCWSTR szServerIp,		//server address
                LPCWSTR szAppname,		//application name to be open
                LPCWSTR szApppath,		//application location
                LPCWSTR szDomainname,	//
                LPCWSTR szDriverList,	//
                LPCWSTR szAppParam,
                DWORD RedirectPorts,
                DWORD RedirectPrinters,//1:local printer, 0: don't take
                BOOL  IsTray,
                DWORD DesktopHeight,
                DWORD DesktopWidth,
                DWORD RedirectClipboard,  //clipboard 1:enable 0: disable
                BOOL  IsSmoothFont, //1: enable 0:disable
                DWORD dwRDPPort,   //rdp port
                DWORD dwForbidApps
                );
#endif

class MovingDialog;
class QPropertyAnimation;
class QTimeLine;
class QDrag;
//class IconDesktopItem;
class QLabel;
class QPoint;

//class IconAddItem;
class HoverIconItem;

//#define SPACING 30
//#define ICONWIDTH 96
//#define ICONHEIGHT 96


/* ####################################################
 * The declaration of VirtualDesktop
 * TODO:
    void delIcon(QString &name);
    void setPageSize(QSize);
    void setIconSize(int x, int y);
    int getPage(int x, int y);
    void setSpacing(int);
    void setMargin(int top, int bottom, int left, int right);
    void pageIncrease();
    void pageDecrease();
 *
 */

class VirtualDesktop : public QWidget
{
    Q_OBJECT
public:

    enum iconStyle
    {
        localIcon = 0, vacIcon, paasIcon, dirIcon
    };

    VirtualDesktop(QSize pageSize, QWidget *parent = NULL);
    ~VirtualDesktop();
    void goNext();
    void goPrevious();
    //void setMargin(int top, int bottom, int left, int right);

    int addIcon(const QString &text, const QString &icon, \
                int page, int index, const QString &url, \
                int type = 0, int i = -1, const QString &uniqueName = "");
//    int showAddIcon(int page, int index);
    IconItem *getIconByName(const QString &uniqueName);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);	
    void keyReleaseEvent(QKeyEvent *);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void reload();
    void delPage(int page);
//    void deleteAllIconItem();
    void movetoFist();
    void reloadApplist();
    int iconCount()             { return _nextIdx[_current]; }
    int currentPage()           { return _current; }
//    void setCurrentPage(int current) { _current = current; }

    int count()                 { return _count; }
//    void setCount(int count)  {_count = count; }
    int iconsPerPage()          {  return _iconsPerPage; }

    QSize pageSize()            { return _pageSize; }

    bool trembleState()         { return _trembling; }
    bool dragEventState()       { return _dragEvent; }
    bool addAppState()          { return _addAppState; }
    bool iconDragState()        { return _iconDragEnter; }

    //void runApp(const QString &text);
    void atExit();
    void moveItem(IconItem *item, int page);

    int addDirIcon(int page, int index);
    void setDirHide();

    void setIconEnabled(bool enabled);

    void refresh(QSize size);

    void movetoFirst();
    void deleteAllIconItem();
    void reloadApplist(QSize size);

    void setIconMove(bool isIconMove) { _isIconMove = isIconMove; }

    void initIconItem();
    void dirMovingFinished();

    void toolBarAddDirShowWidget();

    void addShowWidget(const QString &text, int i, const QString &uniqueName);

public slots:
    void goPage(int page);

    void onProcessFinished(int, QProcess::ExitStatus);
    void delIcon(const QString &uniqueName);
//    void delIcon(IconItem *ic);
    int addIcon(const QString &text, const QString &icon,
                const QString &url, int type = 0,
                int i = -1, const QString &uniqueName = "");
    void updateClicked();
    void itemHeld();
    void dragRight();
    void dragLeft();
    void arrangeEqually(int pageCount);
    void arrangeMinimum();

    void appAdd();
    void appDelete();
    void appCancel();

    void runApp(const QString &uniqueName);
    void uninstall(const QString &uniqueName);

    void upLoad();

    void runServerApp();

    void arrangeWidgetMove(QPoint pos);

    //void updateIconTimeOut();

    void openDir(int id, int page, int index);
    void closeDir(int page, int index);
    void addDesktopApp(const QString &text, const QString &pix,
                       const QString &url, int type, const QString &uniqueName);

    void iconDragLeave();
    void iconDragEnter();
    void iconDragMove();
    void iconDragDrop(int id, const QString &text, const QString &iconPath,
                      int page, int index,
                      const QString &url, int type, const QString &uniqueName);

    void hideDirWidget(const QString &uniqueName, int dirId);
    //normalMenu

    void menuChanged(int value);
    void hideMenuWidget();

    void setLargeIcon();
    void setMediumIcon();
    void setSmallIcon();

    void addDirItem();

    void showIconContextMenu(bool,QPoint pos, QPoint mPos, const QString &uniqueName);
    void iconMenuRunClicked();
    void iconMenuDelClicked();

    void refreshMenu();
    void dirWidgetDelIcon(int id, const QString &uniqueName);

    void moveWidgetDrop(IconItem *iconItem);

    void vacWidgetDelIcon(const QString &uniqueName);

    void iconMoveOtherWidget(const QString &uniqueName);

    void toolBarOpenDir(int id, int posX, int width);
    void toolBarCloseDir(int id, int posX, int width);
    void toolBarIconToDustbin(const QString &text,
                              const QString &iconPath, int page,
                              int index, const QString &url, int type, const QString &uniqueName);
    void toolBarIconToDir(int id, const QString &text,
                          const QString &iconPath, int page,
                          int index, const QString &url, int type, const QString &uniqueName);

    void changedDirWidgetTitle(int i, const QString &text);

signals:
    void pageChanged(int i);
    void pageIncreased();
    void pageDecreased();
    void toOrigin();
    void trembleStarted();
    void trembleStoped();
    void reInitiate();

//    void bgMove(int x, int y);
//    void sendUrl(const QString &url);

//    void setDirIcon(const QString &text, const QString &iconPath, const QString &url);

    void upMove(int x, int y, int w, int h, int distance);
    void upBackMove(int x, int y, int w, int h, int distance);

    void desktopOpenMove(int x, int y, int w, int h, int distance);
    void desktopCloseMove(int x, int y, int w, int h, int distance);

    void openMinWidget(int mx, int my, int mw, int mh, int distance);
    void closeMinWidget(int mx, int my, int mw, int mh, int distance);

    void upMinMove(int mx, int my, int mw, int mh, int distance);
    void upMinBackMove(int mx, int my, int mw, int mh, int distance);

    int rowsNum(int row);

    void desktopClicked();

    //normalMenu
    void changeSkin();
    void desktopTheme();
    void del();
//    void refresh();
    //showIconMenu
    void largeIcon();
    void mediumIcon();
    void smallIcon();
    //createMenu
    void createDir();
    void createLink();
    void createDOC();
    void createEXCEL();
    void createPPT();

    void desktopDelIcon(const QString &uniqueName);
    void delToolBarWidgetIcon(const QString &uniqueName);
    void delToolBarIcon(const QString &uniqueName);
    //miya add
    void hideDesktop();
    void addDesktopLink();

    void refreshVac();

    void setMinMoveLabel(bool up);

    void toolBarRemoveDirMinItem(const QString &uniqueName = "", int dirId = -1);

    void iconItemNameChanged(const QString &uniqueName, const QString &name);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void printDesktop();
    //void uninstall(const QString &name);
    void expand();
    //void delPage(int page);
    int getNearestIndex(const QRect &rect);
    bool largerThan(const QSize &a, const QSize &b);
    QPoint dragStartPosition;
    IconItem *_inDrag;
    /*
    int _marginTop;
    int _marginBottom;
    int _marginLeft;
    int _marginRight;
    */
    MovingDialog *_movingDialog;
    bool _inProgress;
    bool _vertical;
    bool _itemClicked;
    bool _itemHeld;
    bool _trembling;
    QDrag *drag;
    int m2v;
    int gm2v;
    QPropertyAnimation *_animation;
    LocalAppList *_local;
//    IconAddItem *g_addIcon;

    QMenu *_menu;
    QAction *_addAction;
    QAction *_deleteAction;
    QAction *_cancelAction;

//    BsWidget *_bsWidget;
//    ManageWidget *_manageWidget;
    //app
    commuinication _communi;
    QString appText;
    QString _appid;

    int gridWidth;
    int gridHeight;

    //int _vappCount;
    //int _count;
    bool _dragEvent;
    bool _addAppState;

    ArrangeWidget *_arrangeWidget;

    QPropertyAnimation *_animationScreen;
    bool _animationScreenDown;
//    MoveWidget *_mW;
    QDesktopWidget *_desktopWidget;
    QRect _desktopRect;
    QString _url;

    //dirWidget
    bool _iconDragEnter;

    DirMinShowWidget *_dirMinShowWidget;

    QList<QString> _dirMinList;
    QList<DirShowWidget *> _dirShowWidgetList;

    DirShowWidget *_dirShowWidget;
    DirShowWidget *_dustbinDirShowWidget;
    int _dirId;

    void moveBackIcons(int page, int index);
    int _iconNum;

    int _upDistance;
    int _distance;

    int _dirPage;
    int _dirIndex;
    bool _openDir;
    bool _openToolBar;

    int _iconSize;
    int _iconHSpacing;
    int _iconVSpacing;

    void changeSpacing();

    bool _dragEnterMinWidget;

    QString _currentIconItem;
    QString _currentUniqueName;

    MenuWidget *_iconMenu;
//DirWidget object to this
    bool _isIconMove;
    bool _isDirWidgetObject;
    QPoint _dragStartPosition;

    IconItem *_dragItem;

    int _localIconNum;

    bool _dirMovingFinished;

    int _toolBarClickedPosX;
    int _toolBarIconItemWidth;

public:
    int _current;
    QList<QList<IconItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    int _count;
    QList<int> _pages;
    QList<int> _nextIdx;
    QMap<QString, IconItem*> _iconDict;
    int _width;
    int _height;
    int _col;
    int _row;
    int _iconsPerPage;
    QSize _pageSize;
    QTimer *dragLeftTimer;
    QTimer *dragRightTimer;
    //QPoint getPosition(int x, int y);

    QLibrary *mylib;
    QLibrary *mylib1;
    Dll_InitClass m_dllInitClass;
    Dll_CloseClass m_dllCloseClass;
    Dll_CloseAppAll m_dllCloseAppAll;
    Dll_StartAppEx m_dllStartAppEx;

    //QTimer *_updateIconTimer;

    MenuWidget *_normalMenu;
    MenuWidget *_showIconMenu;
    MenuWidget *_createMenu;

    QPoint _mousePos;
    int _menuValue;

//    QList<IconItem*> _iconLists;
//    QList<IconItem*> _tempLists;



};

/* #############################################
 * Declaration of IconItem
 */

//class IconDesktopItem : public IconItem
//{
//    Q_OBJECT
//public:
//    IconDesktopItem(QWidget *parent = 0);
//    ~IconDesktopItem();

//    void addMinWidget();
//    QString getDirText()    { _dirMinShowWidget->getDirText(); }

//signals:
//    void iconLeave();
//    void iconEnter();
//    void iconMove();
//    void openDir(int id, int page, int index);
//    void iconDrop(int id, const QString &text, const QString &iconPath, const QString &url);
//    void dragEnterMinWidget();

//public slots:

//    void openDirWidget();
//    void iconDropEvent(const QString &text, const QString &iconPath, const QString &url);

//protected:

//private:
////    LocalApp *_app;

//    DirMinShowWidget *_dirMinShowWidget;
//};

/* #############################################
 * Declaration of IconAddItem
 */
#if 0
class IconAddItem : public QWidget
{
    Q_OBJECT
public:
	IconAddItem(const QString &text, QWidget *parent = NULL);
    ~IconAddItem();
    void setPixmap(const QString &icon);
    void setText(const QString &text);
    const QString & text();
    const QPixmap & pixmap();
    const QPixmap & originPixmap();
    const QPixmap & grayPixmap();
    const QPixmap & darkPixmap();

    bool isUserType();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void animationMove(const QRect &start, const QRect &end);
    void setPage(int page);
    void setIndex(int index);
    void setHidden(bool hide);
    //virtual void mouseDoubleClickEvent(QMouseEvent *event);//重载鼠标双击事件函数 从QWidget继承
    int _icontype;/*添加图标*/
    int page()
    {
        return _page;
    }
    int index()
    {
        return _index;
    }

    void setGrayPixmap()
    {
        _pixmap = _grayPixmap;
    }

    void setNormalPixmap()
    {
        _pixmap = _normalPixmap;
    }

signals:
    //void clicked();
    void addApp();

public slots:
    void startTremble(); 
    void stopTremble();
    void doTremble(qreal);
    //void addApp();
private:
    LocalApp *_app;
    QDrag *_drag;
    QTimeLine *_timeline;
    QPoint dragStartPosition;
    QPropertyAnimation *_animation;
    int _textWidth;        
    QString _text;    
    int _textHeight;
    int _page;
    int _index;
    int _trembling;
    QPixmap _pixmap;
    QPixmap _normalPixmap;
    QPixmap _grayPixmap;
    QPixmap _closePixmap;
    QPixmap _darkPixmap;
    QPixmap _originPixmap;

    //HoverIconItem *_hoverAddItem;
};

/* ##############################################
 * Declaration of Indicator
 */
#endif

class Indicator : public QWidget
{
    Q_OBJECT
public:
    Indicator(VirtualDesktop *vdesk, QWidget *parent = NULL);
    ~Indicator() {}
public slots:
    void setCurrent(int i);
    void increase();
    void decrease();
    void reset();
signals:
    void iNeedMove();
private:
    QList<QLabel*> _labels;
    VirtualDesktop *_vdesk;
    QSize _itemSize;
};


#endif
