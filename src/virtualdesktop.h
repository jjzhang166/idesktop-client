#ifndef VIRTUALDESKTOP_H
#define VIRTUALDESKTOP_H
#include <QWidget>
#include <QMap>
#include <QProcess>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include "localapps.h"
#include "bswidget.h"
#include "../commuinication.h"
#include "managewidget.h"
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
#include "commuinication.h"
//#include "../myapplication.h"
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
class IconItem;
class QLabel;
class QPoint;

class IconAddItem;
class HoverIconItem;

#define SPACING 30 
#define ICONWIDTH 96
#define ICONHEIGHT 96


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

    enum iconStyle{
        localIcon, vappIcon
    };

    VirtualDesktop(QSize pageSize, QWidget *parent = NULL);
    ~VirtualDesktop();
    void goNext();
    void goPrevious();
    //void setMargin(int top, int bottom, int left, int right);

    int addIcon(const QString &text, const QString &icon, \
                int page, int index, const iconStyle &iSt = localIcon);
    int showAddIcon(const QString &text, const QString &icon, int page, int index);
    IconItem *getIconByName(const QString &name);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void reload();
    int iconCount() {
        return _nextIdx[_current];
    }
    int currentPage() {
        return _current;
    }
    int count() {
        return _count;
    }
    QSize pageSize() {
        return _pageSize;
    }
    //void runApp(const QString &text);
    void atExit();
    void moveItem(IconItem *item, int page);

public slots:
    void goPage(int page);

    void onProcessFinished(int, QProcess::ExitStatus);
    void delIcon(const QString &text);
    int addIcon(const QString &text, const QString &icon, const iconStyle &iSt = localIcon);
    void updateClicked();
    void itemHeld();
    void dragRight();
    void dragLeft();
    void arrangeEqually(int pageCount);
    void arrangeMinimum();

    void appAdd();
    void appDelete();
    void appCancel();

    void runApp(const QString &text);
    void uninstall(const QString &name);

    void upLoad();

    void runServerApp();

    //void updateIconTimeOut();

signals:
    void pageChanged(int i);
    void pageIncreased();
    void pageDecreased();
    void toOrigin();
    void trembleStarted();
    void trembleStoped();
    void reInitiate();

    void hideMenu();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void printDesktop();
    //void uninstall(const QString &name);
    void expand();
    void delPage(int page);
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
    IconAddItem *g_addIcon;

    QMenu *_menu;
    QAction *_addAction;
    QAction *_deleteAction;
    QAction *_cancelAction;

    BsWidget *_bsWidget;
    ManageWidget *_manageWidget;
    //app
    commuinication _communi;
    QString appText;
    QString _appid;

    //int _vappCount;

public:
    int _current;
    int _count;
    QList<int> _pages;
    QList<QList<IconItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
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

};

/* #############################################
 * Declaration of IconItem
 */

class IconItem : public QWidget
{
    Q_OBJECT
public:
    IconItem(const QString &text, const VirtualDesktop::iconStyle &iSt = VirtualDesktop::localIcon, QWidget *parent = NULL);
    ~IconItem();
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

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void animationMove(const QRect &start, const QRect &end);
    void setPage(int page);
    void setIndex(int index);
    void setHidden(bool hide);
    void setIsRmote(bool isRemote);
    int page()
    {
        return _page;
    }
    int index()
    {
        return _index;
    }
    bool isRmote()
    {
        return _isRemote;
    }
    int _icontype;/*程序图标*/    
signals:
    void clicked();
    void runItem(const QString &text);
    void delItem(const QString &text);

public slots:
    void startTremble(); 
    void stopTremble();
    void doTremble(qreal);
    void addApp();

    void openClicked();
    void delClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

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
    bool _isRemote;
    QPixmap _pixmap;
    QPixmap _normalPixmap;
    QPixmap _grayPixmap;
    QPixmap _closePixmap;
    QPixmap _darkPixmap;
    QPixmap _originPixmap;

    QAction *_openAction;
    QAction *_delAction;
    HoverIconItem *_hoverIconItem;

    const VirtualDesktop::iconStyle &_style;
};

/* #############################################
 * Declaration of IconAddItem
 */
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
signals:
    //void clicked();
    void hideMenu();

public slots:
    void startTremble(); 
    void stopTremble();
    void doTremble(qreal);
    void addApp();
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

    HoverIconItem *_hoverAddItem;
};

/* ##############################################
 * Declaration of Indicator
 */

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

/* ##############################################
 * Mouse Enter IconItem
 */

class HoverIconItem : public QWidget
{
    Q_OBJECT
public:
    HoverIconItem(int width, int height, QWidget *parent = 0);
    ~HoverIconItem(){}

protected:
    void paintEvent(QPaintEvent *event);

private:
    int _width;
    int _height;

};

#endif
