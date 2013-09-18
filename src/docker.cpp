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

    setFocusPolicy(Qt::ClickFocus);
//    setWindowFlags(Qt::FramelessWindowHint);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255,0));
    setPalette(pal);

    setAcceptDrops(true);
    setAutoFillBackground(true);

    _gridSize = QSize(SMALLSIZE.width() + HSPACING, SMALLSIZE.height() + VSPACING + BOTTOMSPACING);

    setFixedSize(parent->width(), _gridSize.height());

    connect(&_items, SIGNAL(affectedIndices(const QSet<IndexedList::Change>&)),
            this, SLOT(slotMoveIcons(QSet<IndexedList::Change>,IndexedList::ChangeReason)));
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
    reflow();
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
}

void Docker::slotMoveIcons(const QSet<IndexedList::Change>& changes, IndexedList::ChangeReason reason)
{
    qDebug() << __PRETTY_FUNCTION__ << changes;

    QList<AppIconWidget*> affected;
    QSet<IndexedList::Change>::const_iterator i = changes.constBegin();
    while (i != changes.constEnd()) {
        AppIconWidget *icon = qobject_cast<AppIconWidget*>(_items.at(i->second));
        icon->app()->setIndex(i->second);
        affected.append(icon);
        ++i;
    }

    reflow();
//    batchResetIcons(affected);
}

void Docker::addIcon(LocalApp* app)
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

    icon->init();
    icon->setSmallSize();
    icon->setApp(app);
    icon->setText(app->name());
    icon->setPixmap(app->icon());

    insertIcon(app->index(), icon);
}

void Docker::insertIcon(int index, AppIconWidget *icon)
{
    icon->setContainerType(IconWidget::CT_Toolbar);
    connect(icon, SIGNAL(DragOut(IconWidget*)), this, SLOT(removeIcon(IconWidget*)),
            Qt::UniqueConnection);

    connect(icon, SIGNAL(requestErasion(IconWidget*)), this,
            SLOT(handleIconErasion(IconWidget*)));
    connect(icon, SIGNAL(requestDeletion(IconWidget*)), this,
            SLOT(handleIconDeletion(IconWidget*)));
    connect(icon, SIGNAL(iconItemNameChanged(const QString &, const QString &)),
            this, SIGNAL(iconItemNameChanged(const QString &, const QString &)));

    _items.insert(index, icon);
    moveIconTo(icon, index);
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

void Docker::moveIconTo(IconWidget *icon, int index, bool animated)
{
    if (animated) {
        QPropertyAnimation *pa = new QPropertyAnimation(icon, "geometry");
        pa->setStartValue(icon->geometry());
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
    int y = height() - _gridSize.height();

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

void Docker::reflow(bool animated)
{
    if (animated) {
        QParallelAnimationGroup *pag = new QParallelAnimationGroup(this);
        for (int i = 0; i < icons().size(); ++i) {
            AppIconWidget *icon = qobject_cast<AppIconWidget*>(icons().at(i));
            QPropertyAnimation *pa = new QPropertyAnimation(icon, "geometry");
            pa->setStartValue(icon->geometry());
            pa->setEndValue(rectForIndex(icon->app()->index()));
            pag->addAnimation(pa);
        }

        connect(pag, SIGNAL(finished()), this, SLOT(slotOnMovingIconsFinished()));
        pag->start(QAbstractAnimation::DeleteWhenStopped);

    } else {
        for (int i = 0; i < _items.size(); ++i) {
            AppIconWidget *icon = qobject_cast<AppIconWidget*>(_items.at(i));
            moveIconTo(icon, icon->app()->index(), animated);
        }
    }
}

void Docker::mousePressEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);
}

void Docker::mouseReleaseEvent(QMouseEvent *ev)
{
    QWidget::mouseReleaseEvent(ev);
}

void Docker::dragEnterEvent(QDragEnterEvent *ev)
{
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
}

void Docker::updatePreviewing(int newIndex)
{

}

void Docker::removeIcon(IconWidget *icon)
{
    _items.remove(icon);
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
        moveIconTo(icon_widget, newIndex, true);

    } else {
        // drag from outside, GridPage or toolbar
        LocalApp *app = icon_widget->app();
        app->setIndex(newIndex);
        app->setDirId(-2);
        addIcon(app);
        reflow();
    }

    ev->accept();
}

void Docker::handleMultiDrop(QDropEvent *ev)
{
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

        while (!dataStream.atEnd()) {
            QString uniqueName;
            dataStream >> uniqueName;

            LocalApp *app = LocalAppList::getList()->getAppByUniqueName(uniqueName);
            qDebug() << __PRETTY_FUNCTION__ << app->name() << uniqueName;
            app->setIndex(this->icons().size());
            app->setPage(0);
            app->setDirId(-2);
            addIcon(app);
        }
        reflow();
    }

    ev->accept();
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
