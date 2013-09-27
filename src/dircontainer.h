#ifndef DIRCONTAINER_H
#define DIRCONTAINER_H

#include <QtGui>
#include "indexedlist.h"

class LocalApp;
class DirIconWidget;
class AppIconWidget;
class DirContainer;

class Drawer : public QScrollArea
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
    friend class DirContainer;

    explicit Drawer(QWidget *parent = 0);
    void loadDir(LocalApp *dir);
    void clearDir();
    int dirId() const;
    const IndexedList& icons() const { return _items; }
    void addIcon(LocalApp* app);
    void insertIcon(int index, AppIconWidget *icon);
    void moveIconTo(IconWidget *icon, int index, bool animated = false, bool followCursor = false);

    QRect rectForIndex(int index) const;
    QPoint posForIndex(int index) const;
    AppIconWidget* iconAt(QPoint pos) const;
    int indexAt(QPoint pos) const;
    int cols() const;
    int rows() const;

    bool multiSelectionActivated() const { return _multiSelectionActivated; }
    QList<QVariant> toggledIcons() const;
    void clearToggles();

protected:
    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *);
    void dragEnterEvent(QDragEnterEvent *ev);
    void dragMoveEvent(QDragMoveEvent *ev);
    void dropEvent(QDropEvent *ev);
    void resizeEvent(QResizeEvent *);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *);
    void timerEvent(QTimerEvent *);

protected slots:
    void slotIconResized();
    void slotMoveIcons(const QSet<IndexedList::Change>& changes, IndexedList::ChangeReason);
    void removeIcon(IconWidget *icon);
    void slotAutoClose();
    void insertNewIcon(LocalApp *app);
    void delIcon(LocalApp *app);

private:
    QWidget *_content;
    IndexedList _items;
    LocalApp *_dir;
    QSize _gridSize;
    bool _multiSelectionActivated;

    //HACK: on windows, when QDrag is execing, events blocked when cursor moves
    //outside of target widget, so for workaround, I use a timeEvent to track
    //mouse position and check if it has move outside of widget for a period of time.
    // and then trigger auto auto close dir action.
    int _dragOutTimerId;
    bool _beginTracingDragOutTime;
    QTime _dragOutTime;

    bool containsIconFor(LocalApp *app);
    void handleMultiDrop(QDropEvent *ev);
    void reflow();
};

class DirectionIndicator : public QWidget
{
public:
    enum Direction {
        Up,
        Down
    };

    explicit DirectionIndicator(Direction direction, QWidget *parent);
    void setDirection(Direction dir);
    Direction direction() const { return _direction; }
    void setIndicatorPos(int pos);

    virtual QSize sizeHint() const;

private:
    Direction _direction;
    int _pos;
    QLabel *_cursor;

    QPixmap _upsideAnchorImage;
    QPixmap _downsideAnchorImage;
};

class DirContainer : public QWidget
{
    Q_OBJECT
public:
    friend class Drawer;

    explicit DirContainer(DirectionIndicator::Direction, DirIconWidget *parent = 0);
    
    /**
     * @brief popup open dir synchronously at pos
     * @param direct
     */
    void popup();
    void loadDir(LocalApp *dir);
    int dirId() const;
    void insertNewIcon(LocalApp *app);

signals:
    void iconItemNameChanged(const QString &, const QString &);

public slots:
    void animatedHide();
    void clearDir();
    /**
     * @brief deleteDir remove all icons and delete dir
     */
    void deleteDir();

protected:
    void paintEvent(QPaintEvent *);
    void focusOutEvent(QFocusEvent *);
    bool eventFilter(QObject *, QEvent *);

protected slots:
    void animatedShow();
    void closeDir();
    void slotUpdateMask(QVariant);
    void handleIconDeletion(IconWidget*);
    // right now, only when in trash dir, erasion is allowed
    void handleIconErasion(IconWidget*);
    void handleRestoreToDesktop(IconWidget*);
    void updateMateThumbs();

private:
    QLabel *_dirName;
    QPushButton *_pbClear;
    Drawer *_drawer;
    QLabel *_upsideMask;
    QLabel *_downsideMask;
    DirectionIndicator *_directionIndicator;

    DirIconWidget *_mate;

    void buildMask();
    void clearMask();

    /**
     * @brief translateToParent map _mate's pos into dir's parent pos
     * @return
     */
    QPoint translateToParent();
};

#endif // DIRCONTAINER_H
