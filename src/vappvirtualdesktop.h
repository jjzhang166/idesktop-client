#ifndef VIRTUALDESKTOP_H
#define VIRTUALDESKTOP_H
#include <QWidget>
#include <QMap>
#include <QProcess>
#include <QHBoxLayout>
#include "applist.h"
extern QString _icontext;
extern QString  WIN_IconPath;
extern QString  WIN_TtempPath;
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
#ifdef Q_WS_X11
    #include <QTcpServer>
#endif
class QPropertyAnimation;
class QTimeLine;
class QDrag;
class VappIconItem;
class QLabel;
class QPoint;
class QLibrary;
class QTcpServer;
class QTcpSocket;

#define SPACING 20
#define ICONWIDTH 48
#define ICONHEIGHT 48
//??//
extern bool _quit;
extern int _tray;
/* #############################################
 * Declaration of VappIconItem
 */

class VappIconItem : public QWidget
{
    Q_OBJECT
public:
    VappIconItem(const QString &text, QWidget *parent = NULL);
    ~VappIconItem();
    void setPixmap(const QString &icon);
    void setText(const QString &text);
    const QString & text();
    const QPixmap & pixmap();
    //const QPixmap & originPixmap();
    const QPixmap & grayPixmap();
    const QPixmap & darkPixmap();

    //bool isUserType();
    void mousePressEvent(QMouseEvent *event);
    void setToolTip(const QString &toolTip);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void animationMove(const QRect &start, const QRect &end);
    void setPage(int page);
    void setIndex(int index);
    void setHidden(bool hide);
    int page()
    {
        return _page;
    }
    int index()
    {
        return _index;
    }
signals:
    void clicked();
public slots:
    void startTremble(); 
    void stopTremble();
    void doTremble(qreal);
private:
    //LocalApp *_app;
    AppList *_app;
    //??//
    QString mytooltip;
    QDrag *_drag;
    QTimeLine *_timeline;
    QPoint dragStartPosition;
    QPropertyAnimation *_animation;
    int _textWidth;
    int _textWidth_firstrow;
    int _textWidth_secondrow;
    int _textHeight;
    int _page;
    int _index;

    QString _text;
    QString _texticon;
    QString _texticon_firstrow;
    QString _texticon_secondrow;
    int _trembling;
    QPixmap _pixmap;
    QPixmap _normalPixmap;
    QPixmap _grayPixmap;
    QPixmap _closePixmap;
    QPixmap _darkPixmap;
    //QPixmap _originPixmap;
};

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

class VappVirtualDesktop : public QWidget
{
    Q_OBJECT
public:
    VappVirtualDesktop(QSize pageSize, QWidget *parent = NULL);
    ~VappVirtualDesktop() {}
    void goNext();
    void goPrevious();
    //void setMargin(int top, int bottom, int left, int right);

    int addIcon(const QString &text, const QString &icon, int page, int index);
    VappIconItem *getIconByName(const QString &name);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void reload();
    void printDesktop();
    void expand();
    void delPage(int page);
    void movetoFist();
    void deleteAllVappIconItem();
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
    void runApp(const QString &text);
    void atExit();
	void tcpserverstart();
    void moveItem(VappIconItem *item, int page);
    void reloadApplist();
    int gridWidth;
    int gridHeight;

public slots:

    void onProcessFinished(int, QProcess::ExitStatus);
    void delIcon(const QString &text);
    int addIcon(const QString &text, const QString &icon);
    void updateClicked();
    void itemHeld();
    void dragRight();
    void dragLeft();
    void arrangeEqually(int pageCount);
    void arrangeMinimum();
    void acceptConnection();
    void tcpread();
    void tcprun(QString in);
    void runServerApp();



signals:
    void pageChanged(int i);
    void pageIncreased();
    void pageDecreased();
    void toOrigin();
    void trembleStarted();
    void trembleStoped();
    void reInitiate();
private:
    void uninstall(const QString &name);
    int getNearestIndex(const QRect &rect);
    bool largerThan(const QSize &a, const QSize &b);
    QPoint dragStartPosition;
    VappIconItem *_inDrag;
	QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;

    int flen;


#ifdef Q_WS_X11
    void deletelink();
    QList<QString> link;
    int linknum;
    QProcess *pro;
    bool _run;
#else
    QLibrary *mylib;   //éù?÷?ùó?μ?μ?dll???t
    QLibrary *mylib1;   //éù?÷?ùó?μ?μ?dll???t
    Dll_InitClass m_dllInitClass;    //??òy add() oˉêy
    Dll_CloseClass m_dllCloseClass;
    Dll_CloseAppAll m_dllCloseAppAll;
    Dll_StartAppEx m_dllStartAppEx;
#endif
    /*
    int _marginTop;
    int _marginBottom;
    int _marginLeft;
    int _marginRight;
    */
    //MovingDialog *_movingDialog;
    QString _appid;
    QRect cr;
    bool _inProgress;
    bool _vertical;
    bool _itemClicked;
    bool _itemHeld;
    bool _trembling;
    QDrag *drag;
    int m2v;
    int gm2v;
    int _traycou;
    commuinication _communi;
    QPropertyAnimation *_animation;
    VirutalAppList *_local;
public:
    int _current;
    int _count;
    QList<int> _pages;
    QList<QList<VappIconItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QMap<QString, VappIconItem*> _iconDict;
    int _width;
    int _height;
    int _col;
    int _row;
    int _iconsPerPage;

    QSize _pageSize;
    QTimer *dragLeftTimer;
    QTimer *dragRightTimer;
	QString appText;
    //QPoint getPosition(int x, int y);
};

/* ##############################################
 * Declaration of Indicator
 */

class VappIndicator : public QWidget
{
    Q_OBJECT
public:
    VappIndicator(VappVirtualDesktop *vdesk, QWidget *parent = NULL);
    ~VappIndicator() {}
    void mousePressEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
public slots:
    void setCurrent(int i);
    void increase();
    void decrease();
    void reset();
signals:
    void iNeedMove();
private:
    QHBoxLayout *layout;
    QList<QLabel*> _labels;
    VappVirtualDesktop *_vdesk;
    QSize _itemSize;
    int _currentindex;
};

#endif
