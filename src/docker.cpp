#include <QtGui>
#include "docker.h"
#include "localapps.h"
#include "idesktopsettings.h"
#include "iconwidget.h"
#include "indexedlist.h"

Docker::Docker(QWidget *parent)
    :QWidget(parent)
{
    _trashIcon = NULL;
    _multiSelectionActivated = false;

    setFocusPolicy(Qt::ClickFocus);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255,0));
    setPalette(pal);

    setAcceptDrops(true);
    setAutoFillBackground(true);

    _gridSize = QSize(SMALLSIZE.width(), SMALLSIZE.height() + VSPACING + BOTTOMSPACING);
    setFixedSize(parent->width(), _gridSize.height());

    connect(&_items, SIGNAL(affectedIndices(QSet<IndexedList::Change>,IndexedList::ChangeReason)),
            this, SLOT(slotMoveIcons(QSet<IndexedList::Change>,IndexedList::ChangeReason)));

    _openHoverOnDirTimer = new QTimer(this);
    _openHoverOnDirTimer->setSingleShot(true);
    connect(_openHoverOnDirTimer, SIGNAL(timeout()), this, SLOT(slotOpenHoverOnDir()));

    this->setProperty("multidragDelegate", QVariant::fromValue((QObject*)this));

    LocalAppList *localapps = LocalAppList::getList();
    connect(localapps, SIGNAL(appRemoved(LocalApp*)), this, SLOT(delIcon(LocalApp*)));
}

void Docker::paintEvent(QPaintEvent *ev)
{
    QWidget::paintEvent(ev);
    QPainter painter(this);
    int gridWidth = _gridSize.width(), nr_icons = icons().size();

    painter.drawPixmap((width() - gridWidth * nr_icons) / 2 - 40, height() - 70, 40, 70
                       , QPixmap(":images/toolbar_left.png").scaled(40, 70, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter.drawPixmap((width() - gridWidth * nr_icons) / 2, height() - 70, gridWidth * nr_icons , 70
                       , QPixmap(":images/toolbar_center.png").scaled(gridWidth * nr_icons , 70, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter.drawPixmap((width() - gridWidth * nr_icons) / 2 + nr_icons * gridWidth, height() - 70, 40, 70
                       , QPixmap(":images/toolbar_right.png").scaled(40, 70, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void Docker::init()
{
    QList<LocalApp*> apps = LocalAppList::getList()->appsInDir(-2);
    foreach(LocalApp *app, apps) {
        addIcon(app);
    }
    reflow(true);
    update();
}

void Docker::batchResetIcons(const QList<AppIconWidget*> &icons)
{
    QParallelAnimationGroup *pag = new QParallelAnimationGroup(this);
    foreach(AppIconWidget *icon, icons) {
        QPropertyAnimation *pa = new QPropertyAnimation(icon, "geometry");
        pa->setStartValue(icon->geometry());
        pa->setEndValue(rectForIndex(icon->app()->index()));
        pag->addAnimation(pa);
    }

    connect(pag, SIGNAL(finished()), this, SLOT(slotOnMovingIconsFinished()));
    pag->start(QAbstractAnimation::DeleteWhenStopped);
}

void Docker::slotOnMovingIconsFinished()
{
    update();

    if (_redundantApps.size() > 0)
        redundantApps(_redundantApps);
}

void Docker::slotMoveIcons(const QSet<IndexedList::Change>& changes, IndexedList::ChangeReason reason)
{
    qDebug() << __PRETTY_FUNCTION__ << changes;

    QList<AppIconWidget*> affected;
    QSet<IndexedList::Change>::const_iterator i = changes.constBegin();
    while (i != changes.constEnd()) {
        AppIconWidget *icon = qobject_cast<AppIconWidget*>(_items.at(i->second));
        //HACK: when a multidrag happened, some icons contained in toggledIcons has
        //been lost connection with the apps
        if (icon->app()) {
            icon->app()->setIndex(i->second);
        } else {
            qDebug() << __PRETTY_FUNCTION__ << "icon disconnected";
        }
        affected.append(icon);
        ++i;
    }

    if (reason == IndexedList::MoveAround) {
        batchResetIcons(affected);
    } else {
        reflow(true);
    }
}

void Docker::addIcon(LocalApp* app, bool draggedIn)
{
    if (containsIconFor(app)) {
        return;
    }

    int type = app->type().toInt();
    AppIconWidget *icon = NULL;
    switch (type) {
    case IconWidget::dustbinIcon:
        icon = new TrashDirWidget(this);
        _trashIcon = qobject_cast<TrashDirWidget*>(icon);
        qDebug() << __PRETTY_FUNCTION__ << "create trash";

        Q_ASSERT(_trashIcon);
        break;

    case IconWidget::dirIcon:
        icon = new DirIconWidget(this); break;

    default:
        icon = new AppIconWidget(this); break;
    }

    if (type >= IconWidget::dirIcon) {
        connect(icon, SIGNAL(requestOpenDir(DirContainer*)), this,
                SIGNAL(requestOpenDir(DirContainer*)));
        connect(icon, SIGNAL(restoreAppsToDesktop(QList<LocalApp*>)), this,
                SIGNAL(restoreAppsToDesktop(QList<LocalApp*>)));
        connect(icon, SIGNAL(requestEraseDirItem(LocalApp*)),
                this, SIGNAL(requestEraseApp(LocalApp*)));

        if (type == IconWidget::dirIcon) {
            connect(icon, SIGNAL(requestMoveDirItemToTrash(LocalApp*)), this,
                    SIGNAL(moveAppToTrash(LocalApp*)));
        }
    }

//    icon->init();
    icon->setSmallSize();
    icon->setLockSize(true);
    icon->setApp(app);
    icon->setText(app->name());
    icon->setPixmap(app->icon());

    if (draggedIn)
        icon->setProperty("draggedIn", draggedIn);
    insertIcon(app->index(), icon);
}

void Docker::insertIcon(int index, AppIconWidget *icon)
{
    icon->setContainerType(IconWidget::CT_Toolbar);
    connect(icon, SIGNAL(DragOut(IconWidget*)), this, SLOT(removeIcon(IconWidget*)),
            Qt::QueuedConnection);

    connect(icon, SIGNAL(requestErasion(IconWidget*)), this,
            SLOT(handleIconErasion(IconWidget*)));
    connect(icon, SIGNAL(requestDeletion(IconWidget*)), this,
            SLOT(handleIconDeletion(IconWidget*)));
    connect(icon, SIGNAL(iconItemNameChanged(const QString &, const QString &)),
            this, SIGNAL(iconItemNameChanged(const QString &, const QString &)));

    _items.insert(index, icon);
    icon->show();
}

bool Docker::containsIconFor(LocalApp *app)
{
    for (int i = 0; i < _items.size(); ++i) {
        if (qobject_cast<AppIconWidget*>(_items.at(i))->app() == app) {
            qDebug() << __PRETTY_FUNCTION__ << "icon for app " << app->name() << " exists";
            return true;
        }
    }

    return false;
}

void Docker::moveIconTo(IconWidget *icon, int index, bool animated, bool followCursor)
{
    if (animated) {
        QPropertyAnimation *pa = new QPropertyAnimation(icon, "geometry");
        if (followCursor) {
            QRect geo = icon->geometry();
            geo.moveCenter(mapFromGlobal(QCursor::pos()));
            pa->setStartValue(geo);
        } else {
            pa->setStartValue(icon->geometry());
        }
        pa->setEndValue(rectForIndex(index));
        pa->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        icon->move(posForIndex(index));
    }
}

QRect Docker::rectForIndex(int index) const
{
    return QRect(posForIndex(index), _gridSize);
}

QPoint Docker::posForIndex(int index) const
{
    int x = (width() - _gridSize.width() * icons().size()) / 2 + index * _gridSize.width();
    int y = height() - _gridSize.height() + 14; // + 14 is abitrary, just make icon lower

    qDebug() << __PRETTY_FUNCTION__ << "index" << index << "x " << x << " y " << y;
    return QPoint(x, y);
}

int Docker::suggestIndex(const QPoint &pos)
{
    int w = _gridSize.width(), xs = (width() - w * icons().size()) / 2 - w/2;
    int suggest = qMin(qMax((pos.x() - xs) / w, 0), cols());
    qDebug() << __PRETTY_FUNCTION__ << suggest;
    return suggest;
}

/**
 * @brief Docker::iconAt
 * @param pos pos is relative to Docker
 * @return
 */
AppIconWidget* Docker::iconAt(QPoint pos) const
{
    for (int i = 0; i < _items.size(); ++i) {
        IconWidget *icon = _items.at(i);
        if (icon->geometry().contains(pos)) {
            if (icon->hitButton(icon->mapFromParent(pos))) {
                return qobject_cast<AppIconWidget*>(icon);
            }
            break;
        }
    }

    return NULL;
}

int Docker::cols() const
{
    return (width() - ICONHSPACING) / _gridSize.width();
}

// rows should be at least one
int Docker::rows() const
{
    return 1;
}

void Docker::queuedReflow()
{
    qDebug() << __PRETTY_FUNCTION__;
    reflow(true);
}

void Docker::reflow(bool animated)
{
    qDebug() << __PRETTY_FUNCTION__ << animated;
    if (_multiSelectionActivated) {
        //disable reflow during multi-draging
        return;
    }

    if (animated) {
        QParallelAnimationGroup *pag = new QParallelAnimationGroup(this);
        for (int i = 0; i < icons().size(); ++i) {
            AppIconWidget *icon = qobject_cast<AppIconWidget*>(icons().at(i));
            if (!icon->app()) {
                icon->hide();
                continue;
            }
            QPropertyAnimation *pa = new QPropertyAnimation(icon, "geometry");
            if (icon->property("draggedIn").toBool()) {
                QRect geo = icon->geometry();
                geo.moveCenter(mapFromGlobal(QCursor::pos()));
                pa->setStartValue(geo);
                icon->setProperty("draggedIn", QVariant::Invalid);
            } else {
                pa->setStartValue(icon->geometry());
            }
            pa->setEndValue(rectForIndex(icon->app()->index()));
            pag->addAnimation(pa);
        }

        connect(pag, SIGNAL(finished()), this, SLOT(slotOnMovingIconsFinished()));
        pag->start(QAbstractAnimation::DeleteWhenStopped);

    } else {
        for (int i = 0; i < _items.size(); ++i) {
            AppIconWidget *icon = qobject_cast<AppIconWidget*>(_items.at(i));
            if (!icon->app()) {
                icon->hide();
                continue;
            }
            moveIconTo(icon, icon->app()->index(), animated);
        }
    }
}

void Docker::mousePressEvent(QMouseEvent *ev)
{
    emit mousePress();
	
    if ((ev->modifiers() & Qt::ControlModifier) && ev->button() == Qt::LeftButton) {
        _multiSelectionActivated = true;
    }

    IconWidget *icon = this->iconAt(ev->pos());
    if (icon && _multiSelectionActivated && !icon->inherits("TrashDirWidget")) {
        icon->setToggled(true);
        return;
    }

    //or else
    clearToggles();
   QWidget::mousePressEvent(ev);
}

void Docker::mouseReleaseEvent(QMouseEvent *ev)
{
    QWidget::mouseReleaseEvent(ev);
}

void Docker::dragEnterEvent(QDragEnterEvent *ev)
{
//    if (cols() <= icons().size()) {
//        qDebug() << __PRETTY_FUNCTION__ << "docker's full, disable drag in";
//        ev->ignore();
//        return;
//    }
    qDebug() << __PRETTY_FUNCTION__ << ev->mimeData()->formats();
    QSet<QString> accept_formats;
    accept_formats << "image/x-iconitem" << "application/x-iconitems"
                   <<  "application/x-mixed-iconitems";
    bool support = ev->mimeData()->formats().toSet().intersect(accept_formats).size() > 0;

    if (ev->source()->inherits("AppIconWidget") && support) {
        IconWidget *icon = qobject_cast<IconWidget*>(ev->source());
        if (icon->containerType() != IconWidget::CT_Toolbar) {
            ev->setDropAction(Qt::CopyAction);
            ev->accept();
            qDebug() << __PRETTY_FUNCTION__ << " accept copy action";

        } else {
            ev->acceptProposedAction();
            qDebug() << __PRETTY_FUNCTION__ << " accept move action";
        }

    } else {
        ev->ignore();
    }
}

void Docker::leaveEvent(QEvent *ev)
{
    qDebug() << __PRETTY_FUNCTION__ << QTime::currentTime();
}

void Docker::dragMoveEvent(QDragMoveEvent *ev)
{
    QWidget::dragMoveEvent(ev);

    IconWidget *w = iconAt(ev->pos());
    if (w) {
        if (w->inherits("DirIconWidget")) {
            //x-mixed-iconitems indicates that at least one dir icon is dragged
            if (ev->mimeData()->hasFormat("application/x-mixed-iconitems")
                    || ev->source()->inherits("DirIconWidget")) {
                _openHoverOnDirTimer->stop();
                qDebug() << __PRETTY_FUNCTION__ << "disable hover record";
                return;
            }

            if (!_openHoverOnDirTimer->isActive()) {
                qDebug() << __PRETTY_FUNCTION__ << "start hover record";
                _openHoverOnDirTimer->start(1000);
            }
            return;
        }
    }

    _openHoverOnDirTimer->stop();
}

void Docker::delIcon(LocalApp *app)
{
    if (app->dirId() != -2)
        return;

    qDebug() << __PRETTY_FUNCTION__ << "app " << app->name();
    int idx = app->index();
    IconWidget *icon = icons().at(idx);
    removeIcon(icon);
}

void Docker::removeIcon(IconWidget *icon)
{
    if(_items.indexOf(icon) == -1)
        return;

    _items.remove(icon);
    qobject_cast<AppIconWidget*>(icon)->setApp(0);
    icon->deleteLater();
}

void Docker::dropEvent(QDropEvent *ev)
{
    qDebug() << __PRETTY_FUNCTION__ << QTime::currentTime();
    AppIconWidget *icon_widget = qobject_cast<AppIconWidget*>(ev->source());
    if (!icon_widget) {
        ev->ignore();
        return;
    }

    if (ev->mimeData()->hasFormat("application/x-iconitems")
            || ev->mimeData()->hasFormat("application/x-mixed-iconitems")) {
        handleMultiDrop(ev);
        return;
    }

    QByteArray data = ev->mimeData()->data("image/x-iconitem");
    QDataStream dataStream(&data, QIODevice::ReadOnly);

    QString uniqueName;
    int page, index;
    dataStream >> uniqueName >> page >> index;
    qDebug() << __PRETTY_FUNCTION__ << uniqueName << page << index;

    //TODO: since position of icons changes when insert/delete new icon, this is tricky
    int newIndex = suggestIndex(ev->pos());
    if (newIndex < 0) newIndex = 0;
    if (icon_widget->containerType() == IconWidget::CT_Toolbar) {
        if (icon_widget->app()->index() == newIndex) {
            ev->ignore();
            return;
        }

        if (newIndex >= icons().size()) {
            newIndex = icons().size()-1;
        }

        qDebug() << __PRETTY_FUNCTION__ << "move icon from" << index << "to" << newIndex;
        LocalApp *app = icon_widget->app();
        Q_ASSERT(index == app->index());
        app->setIndex(newIndex);
        _items.moveAround(index, newIndex);
        moveIconTo(icon_widget, newIndex, true, true);

    } else {
        newIndex = qMin(newIndex, icons().size());
        // drag from outside, GridPage or toolbar
        LocalApp *app = icon_widget->app();

        if (_redundantApps.size() > 0)
            _redundantApps.clear();

        if (cols() <= icons().size()) {
            _redundantApps.append(app);
        }

        app->setIndex(newIndex);
        app->setDirId(-2);
        addIcon(app, true);

        reflow(true);
    }

    ev->accept();
}

void Docker::handleMultiDrop(QDropEvent *ev)
{
    int newIndex = qMin(suggestIndex(ev->pos()), icons().size());
    if (newIndex == -1) newIndex = icons().size();

    AppIconWidget *icon_widget = qobject_cast<AppIconWidget*>(ev->source());
    if (icon_widget->containerType() == IconWidget::CT_Toolbar) {
        ev->ignore();
        return;

    } else {
        QByteArray data;
        if (ev->mimeData()->hasFormat("application/x-iconitems")) {
            data = ev->mimeData()->data("application/x-iconitems");
        } else {
            data = ev->mimeData()->data("application/x-mixed-iconitems");
        }
        QDataStream dataStream(&data, QIODevice::ReadOnly);

        if (_redundantApps.size() > 0)
            _redundantApps.clear();

        while (!dataStream.atEnd()) {

            QString uniqueName;
            dataStream >> uniqueName;

            LocalApp *app = LocalAppList::getList()->getAppByUniqueName(uniqueName);

            if (cols() <= icons().size()) {
                _redundantApps.append(app);
            }

            qDebug() << __PRETTY_FUNCTION__ << app->name() << "insert at" << icons().size();
            app->setIndex(newIndex++);
            app->setPage(0);
            app->setDirId(-2);
            addIcon(app, true);

        }
        reflow(true);
    }

    ev->accept();
}

void Docker::redundantApps(const QList<LocalApp*> &apps)
{
    for (int i = 0; i < apps.size(); i++)
    {
        int idx = apps.at(i)->index();
        IconWidget *icon = icons().at(idx);
        removeIcon(icon);
    }

    emit restoreAppsToDesktop(apps);

    _redundantApps.clear();
}

void Docker::handleIconErasion(IconWidget *icon)
{
    LocalApp * app = qobject_cast<AppIconWidget*>(icon)->app();
    emit requestEraseApp(app);
    removeIcon(icon);
}

void Docker::handleIconDeletion(IconWidget *icon)
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_ASSERT(!icon->inherits("DirIconWidget"));
    AppIconWidget *app_icon = qobject_cast<AppIconWidget*>(icon);
    LocalApp *app = app_icon->app();

    //TODO: do animation
    emit moveAppToTrash(app);
    removeIcon(icon);
}

void Docker::slotOpenHoverOnDir()
{
    IconWidget *icon = iconAt(mapFromGlobal(QCursor::pos()));
    //when timed out, mouse may have been moved around.
    if (icon && icon->inherits("DirIconWidget")) {
        qobject_cast<AppIconWidget*>(icon)->run();
    }
}

QList<QVariant> Docker::toggledIcons() const
{
    QList<QVariant> toggles;
    for (int i = 0; i < icons().size(); ++i) {
        if (_items.at(i)->isToggled()) {
            toggles.append(QVariant::fromValue((AppIconWidget*)_items.at(i)));
        }
    }
    return toggles;
}

void Docker::clearToggles()
{
    if (!_multiSelectionActivated)
        return;

    _multiSelectionActivated = false;
    for (int i = 0; i < icons().size(); ++i) {
        if (_items.at(i)->isToggled()) {
            _items.at(i)->setToggled(false);
        }
    }
    QTimer::singleShot(0, this, SLOT(queuedReflow()));
}
