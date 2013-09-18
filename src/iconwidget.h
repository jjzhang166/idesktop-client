#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QtGui>
#include "dircontainer.h"

class LineEdit : public QLineEdit
{
    Q_OBJECT
//    Q_PROPERTY(QString displayText READ myDisplayText)
public:
    LineEdit(QWidget *parent = NULL);

    //HACK: override property accessor
    QString myDisplayText() const;

signals:
    void focusIn();
    void focusOut();

protected:
    QPixmap _editLeft;
    QPixmap _editCenter;
    QPixmap _editRight;

    virtual void paintEvent(QPaintEvent* event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
};

class MenuWidget;

// type-independent Icon class, used to represent icons for vacwidget,
// toolbar, virtualddesktop ....
class IconWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IconWidget(QWidget *parent = 0);
    
    /**
     * @brief The container_type enum
     * this is the virtual parent of icon, it indicates that this icon resides
     * in the hierachy of some container.
     */
    enum container_type {
        CT_Page, CT_Drawer, CT_Toolbar, CT_VacWidget
    };

    enum icon_size
    {
        large_size = 0, medium_size, small_size
    };

    enum status {
        normal = 1, hover = 2, selected = 3
    };

    enum icon_class {
        local = 0, vapp, paas
    };

    enum icon_style {
        localIcon = 0, vacIcon, paasIcon, dirIcon, dustbinIcon
    };

    virtual void setPixmap(const QString &icon);
    virtual void setText(const QString &text);
    virtual void setupMenu() = 0;
    virtual void init();

    const QString & text() const;
    QIcon icon() const { return _icon; }
    bool isDown() const { return _isDown; }

    bool isToggled() const { return _isToggled; }
    void setToggled(bool val);
    
    virtual bool hitButton(const QPoint &pos) const;

    void setContainerType(container_type ct) { _containerType = ct; }
    container_type containerType() const { return _containerType; }

    void setLargeSize();
    void setMediumSize();
    void setSmallSize();

signals:
    /**
     * @brief requestDeletion
     * deletion is actually del-to-trash, so is done by its container
     */
    void requestDeletion(IconWidget *self);
    void requestErasion(IconWidget *self);
    void restoreToDesktop(IconWidget *self);

protected slots:
    /**
     * @brief sendRequestDeletion deletion request is to move app into trash
     */
    void sendRequestDeletion();
    /**
     * @brief sendRequestErasion erasion request is to erase app completely
     */
    void sendRequestErasion();
    void sendRestoreToDesktop();

protected:
    QIcon _icon;

    bool _isDown;
    bool _isToggled;
    QString _text;
    QRect _iconRect;

    LineEdit *_nameEdit;

    MenuWidget *_menu;
    QMenu *_mainMenu;
    container_type _containerType;

    virtual void paintEvent(QPaintEvent *);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void contextMenuEvent(QContextMenuEvent *);

};

class LocalApp;

/**
 * @brief The AppIconWidget class
 * about drag and drop:
 *  draging between the same icon containers (e.g toolbarWidget, GridPage, DirContainer)
 *  is MoveAction, icon removing and deletion is handled by target container. Oppositely,
 *  if draging between different kinds of containers will be a CopyAction, and the source
 *  is responsible for removing draged-out icons.
 */
class AppIconWidget : public IconWidget
{
    Q_OBJECT
public:
    explicit AppIconWidget(QWidget *parent = 0);
    virtual void setApp(LocalApp *app);
    LocalApp *app() const { return _app; }
    virtual void setPixmap(const QString &icon);
    virtual void setText(const QString &text);
    virtual void setupMenu();
    bool isInDrag() const { return _inDrag; }

public slots:
    virtual void run();

protected slots:
    virtual void slotNameChanged();

signals:
    /**
     * @brief DragOut
     * notify icon container the icon has been dragged out, and needs to be removed
     * @param self
     */
    void DragOut(IconWidget *self);
    /**
     * @brief iconItemNameChanged alert vacwidget to update name (compatible signal
     * from old design)
     * @param uniqueName
     * @param name
     */
    void iconItemNameChanged(const QString &uniqueName, const QString &name);

protected:
    LocalApp *_app;
    QPoint _dragStartPos;
    bool _inDrag;

    virtual void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

    QPixmap buildMultiDragPreview(const QList<AppIconWidget*> &items);
};

Q_DECLARE_METATYPE(AppIconWidget*)

class DirContainer;
class DirIconWidget : public AppIconWidget
{
    Q_OBJECT
public:
    explicit DirIconWidget(QWidget *parent = 0);
    virtual ~DirIconWidget();

    virtual void setPixmap(const QString &icon);
    virtual void setApp(LocalApp *app);
    virtual void setupMenu();

    virtual void init();
    virtual DirContainer *dir();

public slots:
    virtual void run();
    void clearDir();
    void deleteDir();

signals:
    void requestOpenDir(DirContainer*);

    //signals used from dir container
    void restoreAppsToDesktop(const QList<LocalApp*>&);
    void requestMoveDirItemToTrash(LocalApp *);
    //completely remove app
    void requestEraseDirItem(LocalApp *);

protected:
    QWidget *_thumb;
    DirContainer *_dir;

    virtual void paintEvent(QPaintEvent *);
    void updateThumbs();
    virtual void openDir();
};

Q_DECLARE_METATYPE(DirIconWidget*)

class TrashDirWidget: public DirIconWidget
{
    Q_OBJECT
public:
    explicit TrashDirWidget(QWidget *parent = 0);
    virtual void setPixmap(const QString &icon);
    virtual void setApp(LocalApp *app);
    virtual void setupMenu();
    void init();
    virtual DirContainer *dir();

public slots:
    void restoreIcon();

protected:
    virtual void paintEvent(QPaintEvent* event);
    void openDir();
};

#endif // ICONWIDGET_H
