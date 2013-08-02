#ifndef DIRWIDGET_H
#define DIRWIDGET_H

#include <QWidget>
#include <QDrag>
#include <QTimeLine>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QMenu>
#include <QScrollBar>
#include <cassert>
using namespace std;

#include "hovericonitem.h"
#include "localapps.h"
#include "iconitem.h"
#include "contextmenuwidget.h"
#include "commuinication.h"
#include "switcher.h"

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
#include <QLabel>

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

//class DirItem;
class DirWidget;

class DirShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirShowWidget(QSize pageSize, QWidget *parent = 0);
    ~DirShowWidget();

    void showApp(bool localApp);

    void setId(int id);
    int id()                        { return _id; }

    void setMaxRow(int row);
    int getHeight()                 { return _height; }
    int getRow();
    int iconCount();
    int currentPage();
    int count();

    void removeIcon(const QString &uniqueName);

    void showTitle(QString text);
    void showClearBtn();

    void setUniqueName(const QString &uniqueName);
    QString uniqueName()             { return _uniqueName; }

signals:
    void dirWidgetDragLeave(const QString &uniqueName, int dirId);
    void dirWidgetDelIcon(int id, const QString &uniqueName);
    void delApp(const QString &uniqueName);
    void iconItemNameChanged(const QString &uniqueName, const QString &name);

public slots:
//    void scrollBarValueChanged(int val);
    void addDirIcon(const QString &text, const QString &iconPath,
                    int page, int index, const QString &url, int type, const QString &uniqueName);
    void setSize();

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void clearAllIcon();

    void dirWidgetLeave(const QString &uniqueName);

protected:
//    void resizeEvent(QResizeEvent *event);
//    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *);

private:
    int _width;
    int _height;

    QPixmap _rightTopPix;
    QPixmap _rightCenterPix;
    QPixmap _rightBottomPix;

    DirWidget *_dirWidget;

    QScrollBar *_scrollBar;
    QPropertyAnimation *_animation;

    int _id;
    int _bgHeight;

    int _iconCount;
    int _currentPage;
    int _count;

    QLabel *_titleLabel;
    Switcher *_clearBtn;

    QString _uniqueName;
    QString _text;

};

class DirWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirWidget(QSize pageSize, QWidget *parent = 0);
    ~DirWidget();


    int addIcon(const QString &text, const QString &iconPath,
                int page, int index,
                const QString &url, int type, const QString &uniqueName);

    QString addLocalApp(QString appPath);
    QString getAppImage(QString appPath);

    void showApp(bool localApp);

    int iconCount()                     { return _nextIdx[_current]; }
    int currentPage()                   { return _current; }
    int count()                         { return _count; }
    int iconsPerPage()                  { return _iconsPerPage; }
    QSize pageSize()                    { return _pageSize; }
    int row()                           { return _row; }
    int getIconHeight()                 { return gridHeight; }
    void setUrl(const QString &url)     { _url = url; }

    void setId(int id)                  { _id = id; }
    int id()                            { return _id; }

    int getHeight()                    { return _count * _pageSize.height(); }

    void expand();
    void delPage(int page);

    void setMaxPage(int page)   { _maxPage = page; }

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void refresh(QSize size);

    void movetoFirst();
    void deleteAllIconItem();
    void reloadApplist(QSize size);
    int getNearestIndex(const QRect &rect);

    void removeIcon(const QString &uniqueName);
    void moveBackIcons(int page, int index);

//    void initIconItem();
    void clearAllIcon();

    void setUniqueName(const QString &uniqueName);
    QString uniqueName()             { return _uniqueName; }

signals:
//    void sendUrl(const QString &url);
    void pageChanged(int i);
//    void sizeChanged();
    void pageIncreased();
    void pageDecreased();

    void dirWidgetDragLeave(const QString &uniqueName);
    void dirWidgetDelIcon(int id, const QString &uniqueName);
    void iconItemNameChanged(const QString &uniqueName, const QString &name);

public slots:
    void showIconContextMenu(bool visiable, QPoint pos, QPoint mPos, const QString &uniqueName);
    void iconMenuRunClicked();
    void iconMenuDelClicked();
    void runApp(const QString &uniqueName);
    void delIcon(const QString &uniqueName);

protected:

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

private:


//    LocalAppList *_local;

    QSize _pageSize;

    int _col;
    int _row;

    int gridWidth;
    int gridHeight;

    int _width;
    int _height;

    int _current;
    int _count;

    int _localCount;

    QList<int> _pages;
    QList<QList<IconItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QList<QString> _clearNames;
    QMap<QString, IconItem*> _iconDict;

    int _iconsPerPage;

    QPropertyAnimation *_animation;
    QString _url;

    int _iconNum;
    IconItem *_inDrag;

    int _maxPage;

    int _iconSize;
    int _iconHSpacing;
    int _iconVSpacing;

    void changeSpacing();

    bool _dragEvent;
    int m2v;
    int gm2v;

    int _id;

    LocalAppList *_local;
    MenuWidget *_iconMenu;

    //app
//    commuinication _communi;
//    QString appText;
//    QString _appid;
    QString _currentIconItem;

//    Dll_InitClass m_dllInitClass;
//    Dll_CloseClass m_dllCloseClass;
//    Dll_CloseAppAll m_dllCloseAppAll;
//    Dll_StartAppEx m_dllStartAppEx;

    QString _uniqueName;

};

#endif // DIRWIDGET_H
