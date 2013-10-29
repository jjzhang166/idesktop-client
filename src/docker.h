#ifndef DOCKER_H
#define DOCKER_H

#include <QWidget>
#include "indexedlist.h"

class LocalApp;
class DirIconWidget;
class AppIconWidget;
class TrashDirWidget;
class IconWidget;
class DirContainer;

class Docker : public QWidget
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
    explicit Docker(QWidget *parent = 0);
    void init();

    const IndexedList& icons() const { return _items; }
    /**
     * @brief addIcon
     * @param app
     * @param draggedIn this used to indicate if the icon is created by drag from outside
     */
    void addIcon(LocalApp* app, bool draggedIn = false);
    void insertIcon(int index, AppIconWidget *icon);
    void moveIconTo(IconWidget *icon, int index, bool animated = false, bool followCursor = false);

    QRect rectForIndex(int index) const;
    QPoint posForIndex(int index) const;
    AppIconWidget* iconAt(QPoint pos) const;
    int cols() const;
    inline int rows() const;

    bool multiSelectionActivated() const { return _multiSelectionActivated; }
    QList<QVariant> toggledIcons() const;

    //trash is special
    TrashDirWidget *trash() const { return _trashIcon; }

public slots:
    void clearToggles();

signals:
    // relay signals from icons
    void restoreAppsToDesktop(const QList<LocalApp*>&);
    void moveAppToTrash(LocalApp *app);
    void requestOpenDir(DirContainer*);
    void iconItemNameChanged(const QString &, const QString &);

    void requestEraseApp(LocalApp*);

    void mousePress();

protected:
    void paintEvent(QPaintEvent *);
    void dragEnterEvent(QDragEnterEvent *ev);
    void dragMoveEvent(QDragMoveEvent *ev);
    void dropEvent(QDropEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *);

protected slots:
    void slotMoveIcons(const QSet<IndexedList::Change>& changes, IndexedList::ChangeReason);
    void removeIcon(IconWidget *icon);
    void slotOnMovingIconsFinished();
    void handleIconDeletion(IconWidget *icon);
    void handleIconErasion(IconWidget*);
    void slotOpenHoverOnDir();
    void delIcon(LocalApp *app);
    void queuedReflow();

private:
    IndexedList _items;
    QSize _gridSize;
    QTimer *_openHoverOnDirTimer;
    bool _multiSelectionActivated;

    TrashDirWidget *_trashIcon;

    QList<LocalApp*> _redundantApps;

    bool containsIconFor(LocalApp *app);
    void handleMultiDrop(QDropEvent *ev);
    void reflow(bool animated = false);
    int suggestIndex(const QPoint &pos);
    void batchResetIcons(const QList<AppIconWidget*> &icons);

    void redundantApps(const QList<LocalApp*> &apps);

};

#endif // DOCKER_H
