#include <QtGui>

#include "config.h"
#include "contextmenuwidget.h"
#include "appmessagebox.h"
#include "gridcontainer.h"
#include "idesktopsettings.h"
#include "iconwidget.h"
#include "localapps.h"
#include "dircontainer.h"
#include "indexedlist.h"
#include "swipegesture.h"
#include "docker.h"

GridPage::GridPage(QSize pageSize, GridContainer *container)
    :QWidget(container), _container(container), _pageSize(pageSize), _id(-1),
      _multiSelectionActivated(false)
{
    setFocusPolicy(Qt::ClickFocus);
    _autoMoveAroundTimer = new QTimer(this);
    _autoMoveAroundTimer->setSingleShot(true);
    connect(_autoMoveAroundTimer, SIGNAL(timeout()), this, SLOT(emitAutoPageTransition()));

    _openHoverOnDirTimer = new QTimer(this);
    _openHoverOnDirTimer->setSingleShot(true);
    connect(_openHoverOnDirTimer, SIGNAL(timeout()), this, SLOT(slotOpenHoverOnDir()));

    connect(&_items, SIGNAL(affectedIndices(QSet<IndexedList::Change>,IndexedList::ChangeReason)),
            this, SLOT(slotMoveIcons(QSet<IndexedList::Change>,IndexedList::ChangeReason)));
    setFixedSize(pageSize);
    setAcceptDrops(true);
}

void GridPage::emitAutoPageTransition()
{
    qDebug() << __PRETTY_FUNCTION__ << "dir " << _autoMoveAroundDirection;
    emit requestAutoPageTransition(_autoMoveAroundDirection);
}

void GridPage::setPageIndex(int idx, bool animated)
{
    bool init = false;
    if (_id == -1) { // initialize id
        init = true;
    }

    if (_id != idx) { // page id changed

        _id = idx;
        //TODO: emit pageMoved
        QPoint newPos(_container->pageOffset(index()), 0);
        if (animated) {
            QPropertyAnimation *pa = new QPropertyAnimation(this, "pos");
            pa->setEasingCurve(QEasingCurve::OutQuint);
            pa->setStartValue(pos());
            pa->setEndValue(newPos);
            pa->start(QAbstractAnimation::DeleteWhenStopped);
        } else {
            move(newPos);
        }

        if (!init) {
            for (int i = 0; i < _items.size(); ++i) {
                AppIconWidget *app_icon = qobject_cast<AppIconWidget*>(_items.at(i));
                app_icon->app()->setPage(_id);
            }
            //TODO: should create a transaction to make sure data and db are consistent.
            //TODO: save to db
        }
    }
}

bool GridPage::eventFilter(QObject *obj, QEvent *ev)
{
    return QWidget::eventFilter(obj, ev);
}

void GridPage::insertIcon(int index, IconWidget *icon)
{
    icon->setContainerType(IconWidget::CT_Page);
    icon->installEventFilter(this);
    connect(icon, SIGNAL(DragOut(IconWidget*)), this, SLOT(removeIcon(IconWidget*)), Qt::QueuedConnection);

    connect(icon, SIGNAL(requestErasion(IconWidget*)), this,
            SLOT(handleIconErasion(IconWidget*)));
    connect(icon, SIGNAL(requestDeletion(IconWidget*)), this, SLOT(handleIconDeletion(IconWidget*)));
    connect(icon, SIGNAL(iconItemNameChanged(const QString &, const QString &)),
            _container, SIGNAL(iconItemNameChanged(const QString &, const QString &)));

    qDebug() << __PRETTY_FUNCTION__ << "page" << _id << "index" << index;
    _items.insert(index, icon);
    moveIconTo(icon, index, true, true);
    icon->show();
}

void GridPage::slotIconResized()
{
    for (int i = 0; i < _items.size(); ++i) {
        AppIconWidget *icon = qobject_cast<AppIconWidget*>(_items.at(i));
        icon->init();
        moveIconTo(icon, icon->app()->index(), true);
    }
}

void GridPage::slotOpenHoverOnDir()
{
    IconWidget *icon = iconAt(mapFromGlobal(QCursor::pos()));
    //when timed out, mouse may have been moved around.
    if (icon && icon->inherits("DirIconWidget")) {
        qobject_cast<AppIconWidget*>(icon)->run();
    }
}

void GridPage::slotMoveIcons(const QSet<IndexedList::Change>& changes, IndexedList::ChangeReason reason)
{
    qDebug() << __PRETTY_FUNCTION__ << changes;
    QSet<IndexedList::Change>::const_iterator i = changes.constBegin();
    while (i != changes.constEnd()) {
        AppIconWidget *icon = qobject_cast<AppIconWidget*>(_items.at(i->second));
        //FIXME: only when not after a dragout happened
        if (icon->app()) {
            icon->app()->setIndex(i->second);
        } else {
            qDebug() << __PRETTY_FUNCTION__ << "icon disconnected";
        }
        moveIconTo(icon, i->second, !_container->inBatchMode());
        ++i;
    }
    update();
}

QRect GridPage::rectForIndex(int index) const
{
    return QRect(posForIndex(index), _container->gridSize());
}

QPoint GridPage::posForIndex(int index) const
{
    int x = (index % _container->cols()) * _container->gridSize().width() + ICONHSPACING;
    int y = (index / _container->cols()) * _container->gridSize().height() + ICONVSPACING;
    qDebug() << __PRETTY_FUNCTION__ << "x " << x << " y " << y;
    return QPoint(x, y);
}

IconWidget* GridPage::iconAt(QPoint pos) const
{
    for (int i = 0; i < _items.size(); ++i) {
        if (_items.at(i)->geometry().contains(pos)) {
            IconWidget *icon = _items.at(i);
            if (icon->hitButton(icon->mapFromParent(pos))) {
                return icon;
            }
            break;
        }
    }

    return NULL;
}

int GridPage::indexAt(QPoint pos) const
{
    GridContainer *c = _container;
    QSize grid_size = c->gridSize();
    if (pos.x() < ICONHSPACING || pos.x() > ICONHSPACING + c->cols()*grid_size.width())
        return -1;

    if (pos.y() < ICONVSPACING || pos.y() > ICONVSPACING + c->rows()*grid_size.height())
        return -1;

    int col = (pos.x() - ICONHSPACING)/grid_size.width();
    int row = (pos.y() - ICONVSPACING)/grid_size.height();
    qDebug() << "(" << col << "," << row << ")";
    return row * c->cols() + col;
}

void GridPage::moveIconTo(IconWidget *icon, int index, bool animated, bool followCursor)
{
    if (animated) {
        QPropertyAnimation *pa = new QPropertyAnimation(icon, "geometry");
        if (followCursor) {
            QRect geo = icon->geometry();
            geo.setTopLeft(mapFromGlobal(QCursor::pos()));
            pa->setStartValue(geo);
        } else {
            pa->setStartValue(icon->geometry());
        }
        pa->setEndValue(rectForIndex(index));
        pa->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        icon->setGeometry(rectForIndex(index));
    }
}

void GridPage::dragEnterEvent(QDragEnterEvent *ev)
{
    qDebug() << __PRETTY_FUNCTION__ << ev->mimeData()->formats();
    QSet<QString> accept_formats;
    accept_formats << "image/x-iconitem" << "application/x-iconitems"
                   <<  "application/x-mixed-iconitems";
    bool support = ev->mimeData()->formats().toSet().intersect(accept_formats).size() > 0;

    if (ev->source()->inherits("AppIconWidget") && support) {
        AppIconWidget *icon_widget = qobject_cast<AppIconWidget*>(ev->source());
        if (icon_widget->containerType() != IconWidget::CT_Page) {
            ev->setDropAction(Qt::CopyAction);
            ev->accept();
            qDebug() << __PRETTY_FUNCTION__ << "page " << _id << " accept copy action";

        } else {
            ev->acceptProposedAction();
            qDebug() << __PRETTY_FUNCTION__ << "page " << _id << " accept move action";
        }

    } else {
        ev->ignore();
    }
}

void GridPage::leaveEvent(QEvent *ev)
{
    if (_autoMoveAroundTimer->isActive())
        _autoMoveAroundTimer->stop();
    QWidget::leaveEvent(ev);
}

void GridPage::dragMoveEvent(QDragMoveEvent *ev)
{
    int threshold = 50;
    QRect rBound(width()-threshold, 0, threshold, height()), lBound(0, 0, threshold, height());
    //TODO: highlight drag zone
    if (rBound.contains(ev->pos())) {
        _autoMoveAroundDirection = RIGHT;
        if (!_autoMoveAroundTimer->isActive())
            _autoMoveAroundTimer->start(600);

    } else if (lBound.contains(ev->pos())) {
        _autoMoveAroundDirection = LEFT;
        if (!_autoMoveAroundTimer->isActive())
            _autoMoveAroundTimer->start(600);

    } else {
        if (_autoMoveAroundTimer->isActive())
            _autoMoveAroundTimer->stop();
    }

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

void GridPage::removeIcon(IconWidget *icon)
{
    _items.remove(icon);
    icon->deleteLater();
}

void GridPage::handleIconErasion(IconWidget *icon)
{
    qDebug() << __PRETTY_FUNCTION__ ;
    AppIconWidget *app_icon = qobject_cast<AppIconWidget*>(icon);
    LocalApp *app = app_icon->app();

    //TODO: do animation
    emit _container->requestEraseApp(app);
    //icon will be removed by appRemoved signal
}

void GridPage::handleIconDeletion(IconWidget *icon)
{
    qDebug() << __PRETTY_FUNCTION__ ;
    // since dir cannot be delete to trash, dir won't send delete signal
    Q_ASSERT(!icon->inherits("DirIconWidget"));

    LocalApp *app = qobject_cast<AppIconWidget*>(icon)->app();
    //TODO: do animation
    emit _container->moveAppToTrash(app);

    removeIcon(icon);
    if (this->isEmpty()) {
        _container->removeEmptyPage(this->index());
    }
}

void GridPage::dropEvent(QDropEvent *ev)
{
    _autoMoveAroundTimer->stop();

    if (!ev->source())
        return;

    enum DragAction action = MOVE_AROUND_IN_PAGE;
    AppIconWidget *icon_widget = qobject_cast<AppIconWidget*>(ev->source());
    if (icon_widget->containerType() == IconWidget::CT_Page) {
        if (icon_widget->parent() == this) {
            action = MOVE_AROUND_IN_PAGE;
        } else
            action = MOVE_BETWEEN_PAGES;
    } else {
        action = DRAG_FROM_OUTSIDE;
    }

    qDebug() << __PRETTY_FUNCTION__ << "drag action " << action;
    int newIndex = indexAt(ev->pos());
    if (newIndex == -1) {
        ev->ignore();
        return;
    }

    if (ev->mimeData()->hasFormat("application/x-iconitems")
            || ev->mimeData()->hasFormat("application/x-mixed-iconitems")) {
        handleMultiDrop(ev, action);
        return;
    }

    QByteArray data = ev->mimeData()->data("image/x-iconitem");
    QDataStream dataStream(&data, QIODevice::ReadOnly);

    AppIconWidget *icon = qobject_cast<AppIconWidget*>(ev->source());
    QString uniqueName;
    int page, index;
    dataStream >> uniqueName >> page >> index;
    qDebug() << __PRETTY_FUNCTION__ << uniqueName << page << index;

    if (action == DRAG_FROM_OUTSIDE) {
        LocalApp *app = icon->app();

        if (newIndex > icons().size()) {
            newIndex = icons().size();
        }
        int newPage = this->index();
        app->setIndex(newIndex);
        app->setPage(newPage);
        app->setDirId(-1);
        _container->addIcon(app);

    } else {
        if (icon_widget->app()->index() == newIndex) {
            ev->ignore();
            return;
        }

        if (action == MOVE_AROUND_IN_PAGE) {

            if (newIndex >= icons().size()) {
                newIndex = icons().size()-1;
            }

            LocalApp *app = icon->app();
            app->setIndex(newIndex);
            _items.moveAround(index, newIndex);
            moveIconTo(icon, newIndex, true, true);

        } else {
            GridPage *orig_page = qobject_cast<GridPage*>(icon->parentWidget());
            LocalApp *app = icon->app();
            orig_page->removeIcon(icon);

            if (newIndex > icons().size()) {
                newIndex = icons().size();
            }
            int newPage = this->index();
            app->setIndex(newIndex);
            app->setPage(newPage);
            _container->addIcon(app);

            if (orig_page->isEmpty()) {
                _container->removeEmptyPage(orig_page->index());
            }
        }
    }
    ev->accept();
    QTimer::singleShot(0, this, SLOT(afterDrop()));
}

void GridPage::handleMultiDrop(QDropEvent *ev, DragAction action)
{
    if (action != MOVE_BETWEEN_PAGES) {
        //TODO: move from a Dir
        qDebug() << __PRETTY_FUNCTION__ << "impossible to handle now";
        ev->ignore();
        return;
    }

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
        qDebug() << __PRETTY_FUNCTION__ << uniqueName;

    }


    //FIXME: this is tricky way to do multidrop, cause it's not general enough
    //and dataStream is useless
    AppIconWidget *icon = qobject_cast<AppIconWidget*>(ev->source());
    GridPage *orig_page = qobject_cast<GridPage*>(icon->parent());

    int newIndex = qMin(indexAt(ev->pos()), this->icons().size());
    if (newIndex == -1) newIndex = this->icons().size();

    const QList<QVariant>& objs = orig_page->toggledIcons();
    foreach(QVariant v, objs) {
        _container->moveIconToPageAt(v.value<AppIconWidget*>(), this, newIndex++);
    }
    ev->accept();

    if (orig_page->isEmpty()) {
        _container->removeEmptyPage(orig_page->index());
    }
    QTimer::singleShot(0, this, SLOT(afterDrop()));
}

void GridPage::afterDrop()
{
    if (this->icons().size() > _container->iconsPerPage()) {
        qDebug() << __PRETTY_FUNCTION__ << "expanding page";
        _container->expandPage(this);
    }
}

QList<QVariant> GridPage::toggledIcons() const
{
    QList<QVariant> toggles;
    for (int i = 0; i < icons().size(); ++i) {
        if (_items.at(i)->isToggled()) {
            toggles.append(QVariant::fromValue((AppIconWidget*)_items.at(i)));
        }
    }
    return toggles;
}

void GridPage::clearToggles()
{
    _multiSelectionActivated = false;
    for (int i = 0; i < icons().size(); ++i) {
        if (_items.at(i)->isToggled()) {
            _items.at(i)->setToggled(false);
        }
    }
}

void GridPage::mousePressEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);
    if ((ev->modifiers() & Qt::ControlModifier) && ev->button() == Qt::LeftButton) {
        _multiSelectionActivated = true;
    }

    IconWidget *icon = this->iconAt(ev->pos());
    if (icon && _multiSelectionActivated) {
        icon->setToggled(true);
        return;
    }

    //or else
    clearToggles();
}

void GridPage::mouseReleaseEvent(QMouseEvent *ev)
{
    QWidget::mouseReleaseEvent(ev);
}


////////////////////////////////////////////////////////////////////////////////
/// \brief GridContainer::GridContainer
/// \param size
/// \param parent
///
GridContainer::GridContainer(QSize size, QWidget *parent) :
    QWidget(parent), _pageSize(size), _currentId(-1), _inBatchMode(0)
{
    setContextMenuPolicy(Qt::NoContextMenu);

    _settings = IDesktopSettings::instance();

    connect(_settings, SIGNAL(iconSizeUpdated(IconWidget::icon_size, IconWidget::icon_size)),
            this, SLOT(slotIconResized()));

#ifdef Q_WS_WIN
    mylib= new QLibrary("DllClientEngineMain.dll");   //
    if (mylib->load())              //check whether load successfully
    {
        //QMessageBox::information(NULL,"OK","DLL load is OK!");

        m_dllInitClass=(Dll_InitClass)mylib->resolve("DllCEM_InitClass");    //get function add()
        m_dllCloseClass=(Dll_CloseClass)mylib->resolve("DllCEM_CloseClass");
        m_dllCloseAppAll=(Dll_CloseAppAll)mylib->resolve("DllCEM_closeAppAll");
        m_dllStartAppEx=(Dll_StartAppEx)mylib->resolve("DllCEM_startAppEx");
    }
    else
        QMessageBox::information(NULL,"NO","DllClientEngineMain is not loaded!");
#endif

    slotIconResized();

    // default settings, should read from db
    LocalAppList* localapps = LocalAppList::getList();
    int visibleApps = 0;
    for (int i = 0; i < localapps->count(); i++) {
        if (localapps->at(i)->hidden())
            continue;

        if (localapps->at(i)->dirId() == -1) {
            visibleApps++;
        }
    }

    int pages = LocalAppList::getList()->getPages();
    for (int page = 0; page < pages; page++) {
        addPage(new GridPage(_pageSize, this));
    }

    if (pages == 0) {
        addPage(new GridPage(_pageSize, this));
    }

    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,0,255,0));
    setPalette(pal);

    SwipeGesture *sg_recognizer = new SwipeGesture;
    _mySwipeGestureId = QGestureRecognizer::registerRecognizer(sg_recognizer);
    grabGesture(Qt::SwipeGesture);

    connect(localapps, SIGNAL(appAdded(LocalApp*)), this, SLOT(insertNewIcon(LocalApp*)));
    connect(localapps, SIGNAL(appRemoved(LocalApp*)), this, SLOT(delIcon(LocalApp*)));
}

int GridContainer::cols() const
{
    return (_pageSize.width() - ICONHSPACING) / _gridSize.width();
}

int GridContainer::rows() const
{
    return (_pageSize.height() - ICONVSPACING - (SMALLSIZE.height() + VSPACING + BOTTOMSPACING)) / _gridSize.height();
}

void GridContainer::initIcons()
{
    LocalAppList* localapps = LocalAppList::getList();
    int visibleApps = 0;
    for (int i = 0; i < localapps->count(); i++) {
        LocalApp* app = localapps->at(i);
        if (app->hidden())
            continue;

        if (app->dirId() == -1) { // app in the grid
            visibleApps++;
            addIcon(app);
        }
    }

    qDebug() << __PRETTY_FUNCTION__ << " add " << visibleApps << " app icons";
    moveToPage(0);
}

void GridContainer::slotIconResized()
{
    QSize newSize;
    switch(_settings->iconSize()) {
        case IconWidget::large_size: newSize = LARGESIZE; break;
        case IconWidget::medium_size: newSize = MEDIUMSIZE; break;
        default: newSize = SMALLSIZE; break;
    }
    _gridSize = newSize + QSize(HSPACING<<1, VSPACING + BOTTOMSPACING);

    // copy list, _pages will change when insert new pages
    QList<GridPage*> orig_pages = _pages;
    //TODO: when icon larger, may need to expand new pages
    foreach (GridPage *page, orig_pages) {
        if (page->icons().size() <= iconsPerPage()) {
            continue;
        }

        expandPage(page);
    }
    emit iconResized();
}

void GridContainer::expandPage(GridPage *page)
{
    IndexedList& items = page->icons();
    // asume this value won't change during processing
    int icons_per_page = iconsPerPage();
    int nr_new_page = qCeil(items.size()/(qreal)icons_per_page);
    qDebug() << __PRETTY_FUNCTION__ << items.size() << icons_per_page;
    qDebug() << __PRETTY_FUNCTION__ << "need to split page" << page->index()
             << "into " << nr_new_page << "pages";

    beginBatchModification();

    while (nr_new_page > 1) {
        GridPage *new_page = new GridPage(_pageSize, this);
        insertPageAfter(page->index(), new_page);

        QSet<AppIconWidget*> icons;
        int start_idx = icons_per_page, end_idx = qMin(icons_per_page*2, items.size());
        for (int idx = start_idx; idx < end_idx; ++idx) {
            icons.insert(static_cast<AppIconWidget*>(items.at(idx)));
        }

        moveIconsToPage(icons, new_page);
        nr_new_page = qFloor((qreal)items.size()/icons_per_page);
    }

    endBatchModification();

}

int GridContainer::pageOffset(int n) const
{
    return n * _pageSize.width();
}

void GridContainer::moveIconsToPage(const QSet<AppIconWidget*> &icons, GridPage *newPage)
{
    foreach(AppIconWidget *icon, icons) {
        //TODO: this may overflow if newPage already has iconsPerPage() of icons
        moveIconToPageAt(icon, newPage, newPage->icons().size());
    }
}

void GridContainer::moveIconToPageAt(AppIconWidget* icon, GridPage *newPage, int newIndex)
{
    LocalApp *app = icon->app();
    GridPage *page = qobject_cast<GridPage*>(icon->parent());
    page->removeIcon(icon);

    app->setIndex(newIndex);
    app->setPage(newPage->index());
    addIcon(app);
}

void GridContainer::insertPageAfter(int pageIndex, GridPage *newPage)
{
    _pages.insert(pageIndex+1, newPage);
    newPage->setPageIndex(pageIndex);
    connect(newPage, SIGNAL(requestAutoPageTransition(GridPage::Direction)),
            this, SLOT(autoPageTransition(GridPage::Direction)));
    connect(this, SIGNAL(iconResized()), newPage, SLOT(slotIconResized()));
    newPage->show();

    for (int idx = pageIndex+1; idx < this->pageCount(); ++idx) {
        _pages[idx]->setPageIndex(idx, !inBatchMode());
    }

    resize(_pageSize.width()*pageCount(), _pageSize.height());
    emit pageIncreased();
}

void GridContainer::removeEmptyPage(int index)
{
    if (pageCount() == 1) {
        qDebug() << __PRETTY_FUNCTION__ << "only one page left, cancel action";
        return;
    }

    GridPage *page = this->page(index);
    Q_ASSERT(page->icons().size() == 0);

    //recalculate page index and all apps indexes and page id
    if (index == pageCount()-1) {
        if (currentId() == index)
            moveToPage(currentId()-1);
        _pages.removeAt(index);

    } else {
        _pages.removeAt(index);
        for (int idx = index; idx < this->pageCount(); ++idx) {
            _pages[idx]->setPageIndex(idx, true);
        }
        moveToPage(currentId());
        if (currentId() < index)
            moveToPage(currentId());
        else
            moveToPage(currentId()-1);
    }

    // shrink size of container
    resize(_pageSize.width()*pageCount(), _pageSize.height());

    emit pageDecreased();
    page->deleteLater();
}

void GridContainer::delIcon(LocalApp *app)
{
    if (app->dirId() != -1)
        return;

    int pg = app->page();
    int idx = app->index();

    GridPage *page = this->page(pg);
    Q_ASSERT(page != NULL);

    qDebug() << __PRETTY_FUNCTION__ << "page " << pg << "index" << idx;
    IconWidget *icon = page->icons().at(idx);
    page->removeIcon(icon);
    if (page->isEmpty()) {
        removeEmptyPage(pg);
    }
}

void GridContainer::insertNewIcon(LocalApp *app)
{
    // newly created local app
    Q_ASSERT(app->page() == -1);
    int pgId = currentId();
    GridPage *page = this->page(pgId);
    while (page && page->icons().size() >= iconsPerPage()) {
        page = this->page(++pgId);
    }

    if (pgId == this->pageCount()) {
        expand();
        page = this->page(pgId);
    }

    if (currentId() != pgId) {
        moveToPage(pgId);
    }
    app->setPage(pgId);
    app->setIndex(page->icons().size());
    qDebug() << __PRETTY_FUNCTION__ << "newly created local app" << pgId << app->index();
    addIcon(app);
}

void GridContainer::addIcon(LocalApp* app)
{
    if (containsIconFor(app)) {
        return;
    }

    GridPage *page = this->page(app->page());
    int type = app->type().toInt();
    AppIconWidget *icon = NULL;
    switch (type) {
    case IconWidget::dirIcon:
        icon = new DirIconWidget(page);  break;
    default:
        icon = new AppIconWidget(page); break;
    }

    if (type == IconWidget::dirIcon) {
        connect(icon, SIGNAL(requestOpenDir(DirContainer*)), this,
                SIGNAL(requestOpenDir(DirContainer*)));
        connect(icon, SIGNAL(restoreAppsToDesktop(QList<LocalApp*>)), this,
                SLOT(handleRestoreAppsToDesktop(QList<LocalApp*>)));
        connect(icon, SIGNAL(requestMoveDirItemToTrash(LocalApp*)), this,
                SIGNAL(moveAppToTrash(LocalApp*)));
        connect(icon, SIGNAL(requestEraseDirItem(LocalApp*)),
                this, SIGNAL(requestEraseApp(LocalApp*)));
    }

    icon->init();
    icon->setApp(app);
    icon->setText(app->name());
    icon->setPixmap(app->icon());

    page->insertIcon(app->index(), icon);
}

bool GridContainer::containsIconFor(LocalApp *app)
{
    foreach (GridPage *page, _pages) {
        const IndexedList& items = page->icons();
        for (int i = 0; i < items.size(); ++i) {
            if (qobject_cast<AppIconWidget*>(items.at(i))->app() == app) {
                qDebug() << __PRETTY_FUNCTION__ << "icon for app " << app->name() << " exists";
                return true;
            }
        }
    }

    return false;
}

void GridContainer::expand()
{
    qDebug() << __PRETTY_FUNCTION__;
    addPage(new GridPage(_pageSize, this));
    emit pageIncreased();
}


GridPage *GridContainer::page(int id) const
{
    if (id >= 0 && id < _pages.size())
        return _pages[id];
    return NULL;
}

void GridContainer::addPage(GridPage *page)
{
    _pages.append(page);
    connect(page, SIGNAL(requestAutoPageTransition(GridPage::Direction)),
            this, SLOT(autoPageTransition(GridPage::Direction)));
    connect(this, SIGNAL(iconResized()), page, SLOT(slotIconResized()));

    page->setPageIndex(_pages.size()-1);
    page->show();
    //TODO: emit or change size
    resize(_pageSize.width()*pageCount(), _pageSize.height());
}

void GridContainer::autoPageTransition(GridPage::Direction dir)
{
    if (dir == GridPage::LEFT) {
        if (currentId() == 0)
            return; // ignore, do not jump to origin desktop

        moveToPage(currentId()-1);
    } else if (dir == GridPage::RIGHT) {
        if (currentId() == pageCount()-1) {
            //Only alow one empty page to be created
            if (!this->page(currentId())->isEmpty())
                expand();
        }

        moveToPage(currentId()+1);
    }
}

QList<GridPage*> GridContainer::pages() const
{
    return _pages;
}

int GridContainer::pageCount() const
{
    return _pages.size();
}

void GridContainer::mousePressEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);
    emit desktopClicked();
}

void GridContainer::mouseMoveEvent(QMouseEvent *ev)
{
    QWidget::mouseMoveEvent(ev);
}

void GridContainer::mouseReleaseEvent(QMouseEvent *ev)
{
    QWidget::mouseReleaseEvent(ev);
}

bool GridContainer::event(QEvent *ev)
{
    if (ev->type() == QEvent::Gesture) {
        qDebug() << "capture gesture";
        return handleGesture(static_cast<QGestureEvent*>(ev));
    }

    return QWidget::event(ev);
}

bool GridContainer::handleGesture(QGestureEvent *gev)
{
    QGesture *gest = NULL;
    if ((gest = gev->gesture(Qt::SwipeGesture)) && gest->state() == Qt::GestureFinished) {
        QSwipeGesture *swipe = static_cast<QSwipeGesture*>(gest);
        QPoint pos = swipe->property("startPoint").value<QPointF>().toPoint();

        qDebug() << "startpoint" << pos;
        GridPage *page = this->page(_currentId);
        if (page && page->iconAt(page->mapFromParent(pos))) {
            qDebug() << __PRETTY_FUNCTION__ << page->mapFromParent(pos) << "over a iconwidget, disable gesture";
            return true;
        }

        qreal angle = swipe->swipeAngle();
        qDebug() << "swipe gesture angle " << angle;
         if ((0 <= angle && angle <= 45) || (315 <= angle && angle <= 360)) {
             //swipe right
             moveToPage(_currentId-1);

         } else  if (135 <= angle && angle <= 225) {
             //swipe left
             moveToPage(_currentId+1);
         }

        gev->accept();
    }
    return true;
}

void GridContainer::moveToPage(int id)
{
    qDebug() << __PRETTY_FUNCTION__ << "current id " << _currentId << "new id" << id;
    if (id < 0 && _currentId == 0) {
        emit toOrigin();
        return;
    }

    if (id < 0 || id >= pageCount())
        return;

    QPropertyAnimation *pa = new QPropertyAnimation(this, "pos");
    pa->setEasingCurve(QEasingCurve::OutQuint);
    pa->setDuration(600);
    pa->setStartValue(pos());
    pa->setEndValue(QPoint(-pageOffset(id), y()));
    pa->start(QAbstractAnimation::DeleteWhenStopped);

    _currentId = id;
    emit pageChanged(id);
}

void GridContainer::addDirItem()
{
    QUuid uuid = QUuid::createUuid();
    QString strUuid = uuid.toString();

    strUuid.replace("{", "");
    strUuid.replace("}", "");

    addDesktopApp(QString(tr("新建文件夹")), "", "", 3, "3_" + strUuid);
}

void GridContainer::setLargeIcon()
{
    if (_settings->iconSize() == IconWidget::large_size)
        return;

    _settings->setIconSize(IconWidget::large_size);

    refresh(LARGESIZE);

    emit largeIcon();
}

void GridContainer::setMediumIcon()
{
    if (_settings->iconSize() == IconWidget::medium_size)
        return;

    _settings->setIconSize(IconWidget::medium_size);

    refresh(MEDIUMSIZE);

    emit mediumIcon();
}

void GridContainer::setSmallIcon()
{
    if (_settings->iconSize() == IconWidget::small_size)
        return;

    _settings->setIconSize(IconWidget::small_size);

    refresh(SMALLSIZE);

    emit smallIcon();

}

void GridContainer::refresh(QSize size)
{
    moveToPage(0);
    // delete all
}

void GridContainer::appAdd()
{
    emit hideMenu();
    QString path = QFileDialog::getOpenFileName(this, tr("创建快捷方式"),
                                                Config::get("CommonProgramDir"),
                                                tr("app (*.*)"));   //tr("app (*.lnk *.exe)"));
    if (path.isEmpty()) {
        return;
    }

    path.replace('/','\\');
    LocalAppList::getList()->addLocalApp(path);
}

void GridContainer::handleRestoreAppsToDesktop(QList<LocalApp*> apps)
{
    emit hideMenu();
    foreach(LocalApp *app, apps) {
        app->setDirId(-1);
        app->setPage(-1);
        app->setIndex(-1);
        qDebug() << __PRETTY_FUNCTION__ << app->name();
        insertNewIcon(app);
    }
}

void GridContainer::addDesktopApp(const QString &text, const QString &pix, const QString &url, int type, const QString &uniqueName)
{
    if (localIcon == type) {
            LocalApp *app = new LocalApp();
            app->setName(text);
            app->setIcon(pix);
            app->setType(QString("%1").arg(type));
            app->setUrl(url);
            app->setPage(-1);
            app->setIndex(-1);
            app->setDirId(-1);
            app->setId("111");
            app->setUniqueName(uniqueName);

            QList<LOCAL_LIST>& remoteLocalList = _settings->remoteLocalList();
            for (int i = 0; i < remoteLocalList.count(); i++) {
                if("0_" + remoteLocalList.at(i).uniqueName == uniqueName) {
                    app->setExecname(remoteLocalList.at(i).execname);
                    break;
                }
            }

            if (LocalAppList::getList()->getAppByUniqueName(uniqueName)) {
                QApplication::processEvents();
                AppMessageBox box(false, NULL);
                box.setText("已添加该图标");
                box.exec();
            } else {
                LocalAppList::getList()->addApp(app);
            }

    } else if (vacIcon == type) {
        LocalApp *remoteApp = new LocalApp();

        QList<APP_LIST>& myVappList = _settings->vappList();
        for (int i = 0; i < myVappList.count(); i++) {
            if(myVappList.at(i).name == text) {
                remoteApp->setName(myVappList[i].name);
                remoteApp->setId(myVappList[i].id);
                remoteApp->setIcon(pix);
                remoteApp->setPage(-1);
                remoteApp->setIndex(-1);
                remoteApp->setType(QString("%1").arg(type));
                remoteApp->setIsRemote(true);
                remoteApp->setUrl(url);
                remoteApp->setDirId(-1);
                remoteApp->setId("111");
                remoteApp->setUniqueName(uniqueName);
                break;
            }
        }

        //FIXME: why not emit signal inside addRemoteApp?
        LocalAppList::getList()->addRemoteApp(remoteApp);

    } else if (paasIcon == type) {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(-1);
        app->setIndex(-1);
        app->setExecname(url);
        app->setDirId(-1);
        app->setId("111");
        app->setUniqueName(uniqueName);


        if (LocalAppList::getList()->getAppByUniqueName(uniqueName)) {
            QApplication::processEvents();
            AppMessageBox box(false, NULL);
            box.setText("已添加该图标");
            box.exec();
        } else {
            LocalAppList::getList()->addApp(app);
        }

    } else if (dirIcon == type) {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(-1);
        app->setIndex(-1);
        app->setExecname("");
        app->setDirId(-1);
        app->setId(QString("%1").arg(LocalAppList::getList()->dirCounts()));
        app->setUniqueName(uniqueName);

        LocalAppList::getList()->addApp(app);

    }
}

void GridContainer::vacWidgetDelIcon(const QString &uniqueName)
{
    Q_ASSERT(0);
}

void GridContainer::atExit()
{
    LocalAppList::getList()->save();
    m_dllCloseClass();
}

void GridContainer::beginBatchModification()
{
   _inBatchMode++;
}

void GridContainer::endBatchModification()
{
    _inBatchMode--;
}


void GridContainer::contextMenuEvent(QContextMenuEvent *ev)
{
    QWidget::contextMenuEvent(ev);
}


//void GridContainer::dustbinMenuRestoreIcon(IconItem *iconItem)
//{
//    if (!iconItem)
//        return;

//    _dustbinDirShowWidget->restoreIcon(iconItem);
//}

//void GridContainer::dustbinMenuDelIcon(const QString &uniqueName)
//{
//    _dustbinDirShowWidget->delIcon(uniqueName);
//}
