#ifndef GRIDCONTAINER_H
#define GRIDCONTAINER_H

#include <QtGui>
#include "indexedlist.h"
#include "dircontainer.h"

class IDesktopSettings;
class LocalApp;

class GridContainer;
class IconWidget;
class AppIconWidget;
class MenuWidget;
class Docker;

//app
#ifdef Q_WS_WIN
#include <QLibrary>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <windows.h>
#include <shellapi.h>

typedef  unsigned long DWORD;

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

/**
 * @brief The DragAction enum
 */
enum DragAction {
    MOVE_AROUND_IN_PAGE,  // move icon around inside page
    MOVE_BETWEEN_PAGES,   // move icon from one page into another page
    DRAG_FROM_OUTSIDE     // drag an icon from outside page
};

/**
 * @brief The GridPage class
 */
class GridPage: public QWidget
{
    Q_OBJECT
    /**
     * two properties below is protocol for multi-draging icons. AppIconWidget
     * need these to inspect if multi-draging is activated in the container
     * and what are the companies.
     */
    Q_PROPERTY(bool multiSelectionActivated READ multiSelectionActivated)
    Q_PROPERTY(QList<QVariant> toggledIcons READ toggledIcons)

public:
    enum Direction {
        LEFT, RIGHT
    };

    explicit GridPage(QSize pageSize, GridContainer *container);
    const IndexedList& icons() const { return _items; }
    IndexedList& icons() { return _items; }
    void insertIcon(int index, IconWidget *icon);
    bool isEmpty() const { return icons().size() == 0; }
    void moveIconTo(IconWidget *icon, int index, bool animated = false, bool followCursor = false);
    void setPageIndex(int idx, bool animated = false);
    int index() const { return _id; }
    QPoint posForIndex(int index) const;
    QRect rectForIndex(int index) const;
    QList<QRect> rectsForIcons() const;
    int indexAt(QPoint pos) const;
    IconWidget* iconAt(QPoint pos) const;

    bool multiSelectionActivated() const { return _multiSelectionActivated; }
    QList<QVariant> toggledIcons() const;

public slots:
    void slotIconResized();
    void removeIcon(IconWidget *icon);
    void clearToggles();
    void dimPage(bool dim = false);

protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);
    void leaveEvent(QEvent *);

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    bool eventFilter(QObject *, QEvent *);

    void handleMultiDrop(QDropEvent *ev, DragAction action);

private slots:
    void slotMoveIcons(const QSet<IndexedList::Change>& changes, IndexedList::ChangeReason reason);
    void emitAutoPageTransition();
    void slotOpenHoverOnDir();
    void handleIconDeletion(IconWidget *icon);
    void handleIconErasion(IconWidget*);

    void afterDrop();

signals:
    void requestAutoPageTransition(GridPage::Direction dir);

    void mousePress();

private:
    GridContainer *_container;
    IndexedList _items;
    QSize _pageSize;
    int _id; // page id
    bool _multiSelectionActivated;

    QTimer *_autoMoveAroundTimer;
    Direction _autoMoveAroundDirection;

    //track dir icon which a dragging icon is over
    IconWidget *_lastHoverOnDirWidget;
    QTimer *_openHoverOnDirTimer;

    QPixmap buildMultiDragPreview(const QList<AppIconWidget*> &items);
};

/**
 * @brief The GridContainer class
 */
class GridContainer : public QWidget
{
    Q_OBJECT
public:
    enum icon_typle
    {
        localIcon = 0, vacIcon, paasIcon, dirIcon
    };

    friend class GridPage;
    explicit GridContainer(QSize size, QWidget *parent = 0);

    int cols() const;
    int rows() const;
    int iconsPerPage() const { return cols() * rows(); }
    int pageOffset(int n) const;
    QSize gridSize() const { return _gridSize; }

    GridPage *page(int id) const;
    int currentId() const { return _currentId; }
    void addPage(GridPage *page);
    QList<GridPage*> pages() const;
    int pageCount() const;

    void atExit();

    // used to optimize animations: when a lot of modifications happened,
    // delay animations until all changes finished.
    void beginBatchModification();
    void endBatchModification();
    bool inBatchMode() const { return _inBatchMode > 0; }

    void moveIconsToPage(const QSet<AppIconWidget*> &icons, GridPage *newPage);
    void moveIconToPageAt(AppIconWidget* icon, GridPage *newPage, int newIndex);
    void insertPageAfter(int pageIndex, GridPage *newPage);

    void deleteEmptyPages();

    QString getIconName(const QString &iconPath);
signals:
    void pageChanged(int newId);
    void toOrigin();

    void largeIcon();
    void mediumIcon();
    void smallIcon();
    void iconItemNameChanged(const QString &, const QString &);

    //relay signals
    void moveAppToTrash(LocalApp *app);
    void requestOpenDir(DirContainer*);
    void requestEraseApp(LocalApp *app);

    void pageIncreased();
    void pageDecreased();
    void delToolBarIcon(const QString &uniqueName);

    // for GridPage
    void iconResized();

    //menu
    void vdesktopNormalMenu(QPoint mousePos);
    void desktopClicked();

    void mousePress();

public slots:
    void slotIconResized();
    void initIcons();
    void expand();
    void moveToPage(int id);
    /**
     * @brief autoPageTransition this is different from moveToPage, it'll create
     * page on-the-fly if needed.
     * @param dir
     */
    void autoPageTransition(GridPage::Direction dir);

    void setLargeIcon();
    void setMediumIcon();
    void setSmallIcon();

    void refresh(QSize size);

    void addDirItem();
    void appAdd();
    void handleRestoreAppsToDesktop(QList<LocalApp*>);

    void vacWidgetDelIcon(const QString &uniqueName);
    void addDesktopApp(const QString &text, const QString &pix,
                       const QString &url, int type, const QString &uniqueName);

    void clearToggles();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    bool event(QEvent *);
    virtual bool handleGesture(QGestureEvent *gev);
    void contextMenuEvent(QContextMenuEvent *event);

protected slots:
    void insertNewIcon(LocalApp*);
    void delIcon(LocalApp*);

private:
    QList<GridPage*> _pages;
    int _currentId;

    IDesktopSettings *_settings;

    QSize _gridSize; // icon occupation
    QSize _pageSize;

    int _mySwipeGestureId;

    QPoint _mousePos;

    int _inBatchMode;

    QLibrary *mylib;
    QLibrary *mylib1;
    Dll_InitClass m_dllInitClass;
    Dll_CloseClass m_dllCloseClass;
    Dll_CloseAppAll m_dllCloseAppAll;
    Dll_StartAppEx m_dllStartAppEx;

    void addIcon(LocalApp *app);
    bool containsIconFor(LocalApp *app);
    void removeEmptyPage(int index);
    void expandPage(GridPage *page);
};

#endif // GRIDCONTAINER_H
