#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include <QWidget>

#include "iconitem.h"
#include "dirwidget.h"
#include "dirminwidget.h"

class toolBarWidget : public QWidget
{
    Q_OBJECT
    
public:

    enum iconStyle
    {
        localIcon = 0, vacIcon, paasIcon, dirIcon, dustbinIcon
    };

    explicit toolBarWidget(QSize pageSize, QWidget *parent = 0);
    ~toolBarWidget();


    int addIcon(const QString &text, const QString &icon,
                int page, int index, const QString &url,
                int type, int id, const QString &uniqueName);

    IconItem *getIconByName(const QString &uniqueName);

//    void mousePressEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void keyPressEvent(QKeyEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);

    int iconCount()             { return _nextIdx[_current]; }
    int currentPage()           { return _current; }

    int count()                 { return _count; }
    int iconsPerPage()          {  return _iconsPerPage; }

    QSize pageSize()            { return _pageSize; }

    bool trembleState()         { return _trembling; }
    bool dragEventState()       { return _dragEvent; }
    bool addAppState()          { return _addAppState; }
    bool iconDragState()        { return _iconDragEnter; }

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

    void setShowType();

    void test();
public slots:
//    void goPage(int page);

//    void onProcessFinished(int, QProcess::ExitStatus);
    void delIcon(const QString &uniqueName);
////    void delIcon(IconItem *ic);
   int addIcon(const QString &text, const QString &icon
               , const QString &url, int type
               , int id, const QString &uniqueName);
    void updateClicked();

//    void arrangeEqually(int pageCount);
//    void arrangeMinimum();



//    void runApp(const QString &text);

//    void upLoad();

//    void runServerApp();

//    void arrangeWidgetMove(QPoint pos);

//    void openDir(int id, int page, int index);
//    void closeDir(int page, int index);
//    void addDesktopApp(const QString &text, const QString &pix,
//                       const QString &url, int type,
//                       const QString &uniqueName);

    void iconDragLeave();
    void iconDragEnter();
    void iconDragMove();
    void iconDragDrop(int id, const QString &text, const QString &iconPath,
                      int page, int index,
                      const QString &url, int type, const QString &uniqueName);

    void hideDirWidget();
//    //normalMenu

//    void menuChanged(int value);
//    void hideMenuWidget();

    void setLargeIcon();
    void setMediumIcon();
    void setSmallIcon();

//    void addDirItem();

//   void showIconContextMenu(QPoint pos, QPoint mPos, const QString &text);
//    void iconMenuRunClicked();
//    void iconMenuDelClicked();

   void refreshMenu();
    void dirWidgetDelIcon(int id, const QString &uniqueName);

//    void moveWidgetDrop(IconItem *iconItem);

    void moveOutIcon(const QString &uniqueName);

    void openDir(int id, int page, int index);

    void toolBarRemoveDirMinItem(const QString &uniqueName, int dirId);

    void runApp(const QString &uniqueName);

    void toolBarRefreshDirMinWidget(const QString &uniqueName);

    void iconItemDragOut();

signals:
    void pageChanged(int i);
    void pageIncreased();
    void pageDecreased();
    void toOrigin();
    void trembleStarted();
    void trembleStoped();
    void reInitiate();

    void toolBarAddDirSW(int id);

//    void bgMove(int x, int y);

//    void setDirIcon(const QString &text, const QString &iconPath, const QString &url);

    int rowsNum(int row);

    void toolBarClicked();

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

    void iconDropToolWidget(const QString &uniqueName);

    void toolOpenDir(int x, int width, int index);
    void toolCloseDir(int page, int index);

    void toolBarIconToDustbin(const QString &text,
                              const QString &iconPath, int page,
                              int index, const QString &url,
                              int type, const QString &uniqueName);
    void toolBarIconToDir(int id, const QString &text,
                          const QString &iconPath, int page,
                          int index, const QString &url,
                          int type, const QString &uniqueName);

    void iconItemNameChanged(const QString &uniqueName, const QString &name);
    void changedDirWidgetTitle(int i, const QString &text);

    void toolBarDirWidgetRefresh(int i);

protected:
//    void contextMenuEvent(QContextMenuEvent *event);

private:
    void printDesktop();

    int getNearestIndex(const QRect &rect);
    bool largerThan(const QSize &a, const QSize &b);
    QPoint dragStartPosition;
    IconItem *_inDrag;

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


//    QMenu *_menu;
//    QAction *_addAction;
//    QAction *_deleteAction;
//    QAction *_cancelAction;

    //app
//    commuinication _communi;
    QString appText;
    QString _appid;

    int gridWidth;
    int gridHeight;

    //int _vappCount;
    //int _count;
    bool _dragEvent;
    bool _addAppState;


    QPropertyAnimation *_animationScreen;
    bool _animationScreenDown;
//    MoveWidget *_mW;
    QDesktopWidget *_desktopWidget;
    QRect _desktopRect;
    QString _url;

    //dirWidget
    bool _iconDragEnter;

//    DirMinShowWidget *_dirMinShowWidget;

    QList<QString> _dirMinList;
//    QList<DirShowWidget *> _dirList;

    DirShowWidget *_dirShowWidget;
    int _dirId;

    void moveBackIcons(int page, int index);
    int _iconNum;

    int _upDistance;
    int _distance;

    int _dirPage;
    int _dirIndex;
    bool _openDir;

    int _iconSize;
    int _iconHSpacing;
    int _iconVSpacing;

    void changeSpacing();

    bool _dragEnterMinWidget;

    QString _currentIconItem;

    MenuWidget *_iconMenu;
//    DirWidget object to this
    bool _isIconMove;
    bool _isOtherWidgetObject;
    QPoint _dragStartPosition;

    IconItem *_dragItem;

    int _localIconNum;

    bool _dirMovingFinished;

    int _MaxNum;

    QList<IconItem*> _iconItemLists;
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

//    QLibrary *mylib;
//    QLibrary *mylib1;
//    Dll_InitClass m_dllInitClass;
//    Dll_CloseClass m_dllCloseClass;
//    Dll_CloseAppAll m_dllCloseAppAll;
//    Dll_StartAppEx m_dllStartAppEx;

    //QTimer *_updateIconTimer;

//    MenuWidget *_normalMenu;
//    MenuWidget *_showIconMenu;
//    MenuWidget *_createMenu;

    QPoint _mousePos;
    int _menuValue;

//    QList<IconItem*> _iconLists;
//    QList<IconItem*> _tempLists;

    int _toolBarDirIndex;

    bool _refreshToolBar;

    QString _currentUniqueName;

};

#endif // TOOLBARWIDGET_H

