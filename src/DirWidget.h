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

#include "localapps.h"
#include "iconitem.h"
#include "contextmenuwidget.h"
#include "commuinication.h"
#include "switcher.h"
#include "struct.h"

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

extern QList<TEMP_LIST> dirWidget_FirstLists;

typedef  unsigned long DWORD;

extern QString _icontext;

//class DirItem;
class DirWidget;

class DirShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirShowWidget(int id, QSize pageSize, QWidget *parent = 0);
    ~DirShowWidget();

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
    void getFirstIconItem();
    void initIconItem();

    int getDustBinNewHeight();

signals:
    void dirWidgetDragLeave(const QString &uniqueName, int dirId);
    void dirWidgetDelIcon(int id, const QString &uniqueName);
    void delApp(const QString &uniqueName);
    void iconItemNameChanged(const QString &uniqueName, const QString &name);
    void refreshDirMinWidget(int id);
    void dustbinRestore(IconItem *iconItem);

public slots:
    void scrollBarValueChanged(int val);
    void addDirIcon(const QString &text, const QString &iconPath,
                    int page, int index, const QString &url, int type, const QString &uniqueName);
    void setSize();

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void clearAllIcon();

    void dirWidgetLeave(const QString &uniqueName);

    void getParentGlobalPos();

    void setClearBtnEnabled(bool enabled);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    int _width;
    int _height;
    int _oldPagePos;
    int _newPagePos;

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

    int _maxPage;

};

class DirWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirWidget(int id, QSize pageSize, QWidget *parent = 0);
    ~DirWidget();


    int addIcon(const QString &text, const QString &iconPath,
                int page, int index,
                const QString &url, int type, const QString &uniqueName);

    QString addLocalApp(QString appPath);
    QString getAppImage(QString appPath);

    int iconCount()                     { return _nextIdx[_current]; }
    int currentPage()                   { return _current; }
    int count()                         { return _count; }
    int iconsPerPage()                  { return _iconsPerPage; }
    QSize pageSize()                    { return _pageSize; }
    int row()                           { return _row; }
    int getIconHeight()                 { return gridHeight; }
    void setUrl(const QString &url)     { _url = url; }

    void setId(int id);
    int id()                            { return _id; }

    int getHeight()                    { return _height; }

    void expand();
    void delPage(int page);

    void setMaxPage(int page);

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
    void clearAllIcon();

    void setUniqueName(const QString &uniqueName);
    QString uniqueName()             { return _uniqueName; }

    void getFirstIconItem();

    void dragDown(QPoint pos);
    void dragUp(QPoint pos);

    void setGlobalPos(QRect r);

    void initIconItem();

    int getIconNum()               { return _iconNum; }


signals:

    void pageChanged(int i);
    void pageIncreased();
    void pageDecreased();

    void dirWidgetDragLeave(const QString &uniqueName);
    void dirWidgetDelIcon(int id, const QString &uniqueName);
    void iconItemNameChanged(const QString &uniqueName, const QString &name);
    void refreshDirMinWidget(int id);

    void getParentGlobalPos();
    void heightChanged();

    void setClearBtnEnabled(bool enabled);
    void dustbinRestore(IconItem *iconItem);

public slots:
    void showIconContextMenu(bool visiable, QPoint pos, QPoint mPos, IconItem *iconItem);
    void iconMenuRunClicked();
    void menuDelClicked();
    void runApp(const QString &uniqueName);
    void delIcon(const QString &uniqueName);
    void dustbinMenuRestoreClicked();

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
    MenuWidget *_dustbinMenu;

    QString _currentUniqueName;
    IconItem *_currentIconItem;

    QString _uniqueName;

    int _currentPage;
    bool _dragDown;
    bool _dragUp;

    QRect _parentRect;

    QList<TEMP_LIST> _dirTempLists;
};

#endif // DIRWIDGET_H
