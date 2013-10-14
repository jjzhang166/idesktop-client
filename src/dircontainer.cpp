#include <QtGui>
#include "dircontainer.h"
#include "localapps.h"
#include "idesktopsettings.h"
#include "iconwidget.h"

Drawer::Drawer(QWidget *parent)
    :QScrollArea(parent), _content(new QWidget(this)), _dir(0), _multiSelectionActivated(false)
{
    _dragOutTimerId = -1;
    _beginTracingDragOutTime = false;

//    setMouseTracking(true);
    setAcceptDrops(true);
    QString style =
            "QScrollArea { background: transparent; } "
            "QScrollBar:vertical {"
            "     border: 1px solid gray;"
            "     background: transparent;"
            "     width: 15px;"
            " }"
            ""
            " QScrollBar::handle:vertical {"
            "     background: #7f7f7f;"
            "     border-radius: 10px;"
            " }"
            ""
            " QScrollBar::add-line:vertical {"
            "     background: transparent;"
            " }"
            ""
            " QScrollBar::sub-line:vertical {"
            "     background: transparent;"
            " }"
            ""
            " QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
            "     background: none;"
            " }";
    setStyleSheet(style);

    setAlignment(Qt::AlignCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QSize maxgrid = LARGESIZE + QSize(HSPACING<<1, VSPACING + BOTTOMSPACING);
    setMaximumHeight(maxgrid.height()*3);
    setMinimumHeight(maxgrid.height());

    _content->installEventFilter(this);
    _content->setMinimumHeight(minimumHeight());
    QPalette pal = _content->palette();
    pal.setColor(QPalette::Background, Qt::transparent);
    _content->setPalette(pal);
    setWidget(_content);

    connect(IDesktopSettings::instance(), SIGNAL(iconSizeUpdated(IconWidget::icon_size,IconWidget::icon_size)),
            this, SLOT(slotIconResized()));

    slotIconResized();

    connect(&_items, SIGNAL(affectedIndices(QSet<IndexedList::Change>,IndexedList::ChangeReason)),
            this, SLOT(slotMoveIcons(QSet<IndexedList::Change>,IndexedList::ChangeReason)));

    _content->setProperty("multidragDelegate", QVariant::fromValue((QObject*)this));

    LocalAppList *localapps = LocalAppList::getList();
    connect(localapps, SIGNAL(appRemoved(LocalApp*)), this, SLOT(delIcon(LocalApp*)));
}

void Drawer::slotAutoClose()
{
    qDebug() << __PRETTY_FUNCTION__;
    DirContainer *cnt = qobject_cast<DirContainer*>(parent());
    cnt->animatedHide();
}

void Drawer::resizeEvent(QResizeEvent *ev)
{
    if (ev->size().width() != ev->oldSize().width())
        slotIconResized();
}

bool Drawer::eventFilter(QObject *obj, QEvent *ev)
{
    return QScrollArea::eventFilter(obj, ev);
}

void Drawer::paintEvent(QPaintEvent *ev)
{
    QWidget::paintEvent(ev);
}

int Drawer::dirId() const
{
    if (!_dir) return -1;
    return _dir->id().toInt();
}

void Drawer::clearDir()
{
    for (int i = 0; i < _items.size(); ++i) {
        _items.at(i)->deleteLater();
    }
    _items.clear();
    QTimer::singleShot(0, parent(), SLOT(updateMateThumbs()));
}

void Drawer::loadDir(LocalApp *dir)
{
    _dir = dir;
    QList<LocalApp*> apps = LocalAppList::getList()->appsInDir(dir->id().toInt());
    foreach(LocalApp *app, apps) {
        addIcon(app);
    }
}

void Drawer::slotMoveIcons(const QSet<IndexedList::Change>& changes, IndexedList::ChangeReason reason)
{
    qDebug() << __PRETTY_FUNCTION__ << changes;
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
        moveIconTo(icon, i->second, true);
        ++i;
    }
    _content->resize(qMax(_gridSize.width(), width()), _gridSize.height()*rows());
    qDebug() << __PRETTY_FUNCTION__ << "content resize to" << _content->size();
    qDebug() << __PRETTY_FUNCTION__ << "viewport size" << viewport()->rect();
    update();
}

void Drawer::insertNewIcon(LocalApp *app)
{
    app->setIndex(this->icons().size());
    app->setPage(0);
    app->setDirId(dirId());
    addIcon(app);
}

void Drawer::delIcon(LocalApp *app)
{
    if (app->dirId() < 0)
        return;

    qDebug() << __PRETTY_FUNCTION__ << "app " << app->name();
    int idx = app->index();
    IconWidget *icon = icons().at(idx);
    removeIcon(icon);
}

void Drawer::removeIcon(IconWidget *icon)
{
    _items.remove(icon);
    qobject_cast<AppIconWidget*>(icon)->setApp(0);
    icon->deleteLater();
    QTimer::singleShot(0, parent(), SLOT(updateMateThumbs()));
}

void Drawer::addIcon(LocalApp* app)
{
    if (containsIconFor(app)) {
        return;
    }

    int type = app->type().toInt();
    Q_ASSERT(type != IconWidget::dirIcon);
    AppIconWidget *icon = new AppIconWidget(this->widget());

    icon->init();
    icon->setApp(app);
    icon->setText(app->name());
    icon->setPixmap(app->icon());

    insertIcon(app->index(), icon);
}

void Drawer::insertIcon(int index, AppIconWidget *icon)
{
    icon->setContainerType(IconWidget::CT_Drawer);
    connect(icon, SIGNAL(DragOut(IconWidget*)), this, SLOT(removeIcon(IconWidget*)), Qt::QueuedConnection);

    connect(icon, SIGNAL(restoreToDesktop(IconWidget*)), parent(), SLOT(handleRestoreToDesktop(IconWidget*)));
    connect(icon, SIGNAL(requestDeletion(IconWidget*)), parent(), SLOT(handleIconDeletion(IconWidget*)));
    connect(icon, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , parent(), SIGNAL(iconItemNameChanged(const QString &, const QString &)));

//    if (this->dirId() == 1000)
    connect(icon, SIGNAL(requestErasion(IconWidget*)), parent(), SLOT(handleIconErasion(IconWidget*)));

    _items.insert(index, icon);
    moveIconTo(icon, index);
    icon->show();
}

bool Drawer::containsIconFor(LocalApp *app)
{
    for (int i = 0; i < _items.size(); ++i) {
        if (qobject_cast<AppIconWidget*>(_items.at(i))->app() == app) {
            qDebug() << __PRETTY_FUNCTION__ << "icon for app " << app->name() << " exists";
            return true;
        }
    }

    return false;
}

void Drawer::moveIconTo(IconWidget *icon, int index, bool animated, bool followCursor)
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

QRect Drawer::rectForIndex(int index) const
{
    return QRect(posForIndex(index), _gridSize);
}

QPoint Drawer::posForIndex(int index) const
{
    int x = (index % cols()) * _gridSize.width();
    int y = (index / cols()) * _gridSize.height();
    qDebug() << __PRETTY_FUNCTION__ << "index" << index << "cols()" << cols() << "x " << x << " y " << y;
    return QPoint(x, y);
}

/**
 * @brief Drawer::iconAt
 * @param pos pos is relative to Drawer
 * @return
 */
AppIconWidget* Drawer::iconAt(QPoint pos) const
{
    pos = _content->mapFromGlobal(mapToGlobal(pos));
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

int Drawer::indexAt(QPoint pos) const
{
    pos = _content->mapFromGlobal(this->mapToGlobal(pos));
    if (pos.x() < 0 || pos.x() > cols()*_gridSize.width())
        return -1;

    if (pos.y() < 0 || pos.y() > rows()*_gridSize.height())
        return -1;

    int col = pos.x()/_gridSize.width();
    int row = pos.y()/_gridSize.height();
    qDebug() << __PRETTY_FUNCTION__ << "(" << col << "," << row << ")";
    return row * cols() + col;
}

int Drawer::cols() const
{
    return _content->width() / _gridSize.width();
}

// rows should be at least one
int Drawer::rows() const
{
    return qMax(qCeil(_items.size() / (qreal)cols()), 1);
}

void Drawer::reflow()
{
    for (int i = 0; i < _items.size(); ++i) {
        AppIconWidget *icon = qobject_cast<AppIconWidget*>(_items.at(i));
        icon->init();
        moveIconTo(icon, icon->app()->index(), true, true);
    }
}

void Drawer::slotIconResized()
{
    QSize newSize;
    switch(IDesktopSettings::instance()->iconSize()) {
        case IconWidget::large_size: newSize = LARGESIZE; break;
        case IconWidget::medium_size: newSize = MEDIUMSIZE; break;
        default: newSize = SMALLSIZE; break;
    }
    _gridSize = newSize + QSize(HSPACING<<1, VSPACING + BOTTOMSPACING);
    // make sure cols() at least great than 0
    _content->resize(qMax(_gridSize.width(), width()), _gridSize.height()*rows());
    qDebug() << __PRETTY_FUNCTION__ << "content resize to" << _content->size();
    reflow();
}

void Drawer::mousePressEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);
    if ((ev->modifiers() & Qt::ControlModifier) && ev->button() == Qt::LeftButton) {
        _multiSelectionActivated = true;
    }

    IconWidget *icon = this->iconAt(ev->pos());
    if (icon) {
        if (_multiSelectionActivated)
            icon->setToggled(true);
    } else {
        clearToggles();
    }
}

void Drawer::mouseReleaseEvent(QMouseEvent *ev)
{
    QScrollArea::mouseReleaseEvent(ev);
}

void Drawer::dragEnterEvent(QDragEnterEvent *ev)
{
    if (_dragOutTimerId != -1) {
        killTimer(_dragOutTimerId);
    }
    _dragOutTimerId = startTimer(100);

    if (ev->source()->inherits("AppIconWidget")
            && (ev->mimeData()->hasFormat("image/x-iconitem")
                || ev->mimeData()->hasFormat("application/x-iconitems"))) {

        IconWidget *icon = qobject_cast<IconWidget*>(ev->source());
        if (icon->containerType() != IconWidget::CT_Drawer) {
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

void Drawer::timerEvent(QTimerEvent *ev)
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    qDebug() << __PRETTY_FUNCTION__ << rect() << pos << _dragOutTime << QTime::currentTime();
    if (!this->rect().contains(pos)) {
        if (_beginTracingDragOutTime) {
            if (_dragOutTime.msecsTo(QTime::currentTime()) > 1000) {
                _beginTracingDragOutTime = false;
                killTimer(_dragOutTimerId);
                _dragOutTimerId = -1;
                slotAutoClose();
            }
        } else {
            _beginTracingDragOutTime = true;
            _dragOutTime = QTime::currentTime();
        }
    } else {
        _beginTracingDragOutTime = false;
    }
}

void Drawer::leaveEvent(QEvent *ev)
{
    qDebug() << __PRETTY_FUNCTION__ << QTime::currentTime();
}

void Drawer::dragMoveEvent(QDragMoveEvent *ev)
{
    QScrollArea::dragMoveEvent(ev);
}

void Drawer::dropEvent(QDropEvent *ev)
{
    qDebug() << __PRETTY_FUNCTION__ << QTime::currentTime();
    if (_dragOutTimerId != -1) {
        killTimer(_dragOutTimerId);
        _dragOutTimerId = -1;
    }

    AppIconWidget *icon_widget = qobject_cast<AppIconWidget*>(ev->source());
    if (!icon_widget) {
        ev->ignore();
        return;
    }

    if (ev->mimeData()->hasFormat("application/x-iconitems")) {
        handleMultiDrop(ev);
        return;
    }

    int newIndex = indexAt(ev->pos());
    if (newIndex == -1) newIndex = icons().size();
    qDebug() << __PRETTY_FUNCTION__ << "indexAt -> " << newIndex;


    QByteArray data = ev->mimeData()->data("image/x-iconitem");
    QDataStream dataStream(&data, QIODevice::ReadOnly);

    QString uniqueName;
    int page, index;
    dataStream >> uniqueName >> page >> index;
    qDebug() << __PRETTY_FUNCTION__ << uniqueName << page << index;

    if (icon_widget->containerType() == IconWidget::CT_Drawer) {
        if (icon_widget->parent() == this->_content) {
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
            //TODO:
            Q_ASSERT(0);
            ev->ignore();
        }

    } else {
        // drag from outside, GridPage or toolbar
        LocalApp *app = icon_widget->app();
        newIndex = qMin(newIndex, icons().size());
        app->setIndex(newIndex);
        app->setPage(0);
        app->setDirId(dirId());
        addIcon(app);
        DirContainer *cnt = qobject_cast<DirContainer*>(parent());
        cnt->_mate->init();
    }

    ev->accept();
}

void Drawer::handleMultiDrop(QDropEvent *ev)
{
    AppIconWidget *icon_widget = qobject_cast<AppIconWidget*>(ev->source());
    if (icon_widget->containerType() == IconWidget::CT_Drawer) {
        if (icon_widget->parent() == this->_content) {
            clearToggles();
            ev->ignore();
            return;

        } else {
            //move from dir to another dir
            Q_ASSERT(0);

            QByteArray data = ev->mimeData()->data("application/x-iconitems");
            QDataStream dataStream(&data, QIODevice::ReadOnly);

            while (!dataStream.atEnd()) {
                QString uniqueName;
                dataStream >> uniqueName;
                LocalApp *app = LocalAppList::getList()->getAppByUniqueName(uniqueName);
                app->setIndex(this->icons().size());
                app->setPage(0);
                addIcon(app);
            }

            //TODO: how to remove from original container?
        }

    } else {
        QByteArray data = ev->mimeData()->data("application/x-iconitems");
        QDataStream dataStream(&data, QIODevice::ReadOnly);

        int newIndex = qMin(indexAt(ev->pos()), icons().size());
        if (newIndex == -1) newIndex = this->icons().size();

        DirContainer *cnt = qobject_cast<DirContainer*>(parent());
        while (!dataStream.atEnd()) {
            QString uniqueName;
            dataStream >> uniqueName;

            LocalApp *app = LocalAppList::getList()->getAppByUniqueName(uniqueName);
            qDebug() << __PRETTY_FUNCTION__ << app->name() << uniqueName;
            app->setIndex(newIndex++);
            app->setPage(0);
            app->setDirId(dirId());
            addIcon(app);
        }
        cnt->_mate->init();
    }

    ev->accept();
}

QList<QVariant> Drawer::toggledIcons() const
{
    QList<QVariant> toggles;
    for (int i = 0; i < icons().size(); ++i) {
        if (_items.at(i)->isToggled()) {
            toggles.append(QVariant::fromValue((AppIconWidget*)_items.at(i)));
        }
    }
    return toggles;
}

void Drawer::clearToggles()
{
    _multiSelectionActivated = false;
    for (int i = 0; i < icons().size(); ++i) {
        if (_items.at(i)->isToggled()) {
            _items.at(i)->setToggled(false);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////


DirectionIndicator::DirectionIndicator(Direction direction, QWidget *parent)
    :QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _upsideAnchorImage = QPixmap(":/images/min_mask_icon.png");
    _downsideAnchorImage = QPixmap(":/images/min_toolbar_mask_icon.png");
    _cursor = new QLabel(this);
    _cursor->show();
    _cursor->setFrameStyle(QFrame::NoFrame);

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0,0,0,0));
    setPalette(pal);

    setDirection(direction);
}

void DirectionIndicator::setDirection(DirectionIndicator::Direction dir)
{
    _direction = dir;
    if (dir == Down) {
        _cursor->setPixmap(_upsideAnchorImage);
    } else {
        _cursor->setPixmap(_downsideAnchorImage);
    }
    _cursor->resize(_cursor->pixmap()->size());
    setMinimumHeight(_cursor->height());
}

QSize DirectionIndicator::sizeHint() const
{
    return QSize(width(), _cursor->height());
}

void DirectionIndicator::setIndicatorPos(int pos)
{
    _pos = pos;
    _cursor->move(pos-_cursor->width()/2, 0);
}

////////////////////////////////////////////////////////////////////////////////

DirContainer::DirContainer(DirectionIndicator::Direction direction, DirIconWidget *parent)
    : QWidget(0), _mate(parent)
{
    _animatedHiding = false;
    setContextMenuPolicy(Qt::PreventContextMenu);
    setFocusPolicy(Qt::ClickFocus);
    resize(qApp->desktop()->width(), height());

    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);

    _directionIndicator = new DirectionIndicator(direction, this);
    if (direction == DirectionIndicator::Down)
        vl->addWidget(_directionIndicator);

    QHBoxLayout *hl = new QHBoxLayout();
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    _dirName = new QLabel(this);
    _dirName->setFrameStyle(QFrame::NoFrame);
    QFont font(QString::fromLocal8Bit("Microsoft YaHei"), 14, QFont::Normal);
    _dirName->setStyleSheet("color:white");
    _dirName->setFont(font);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(_dirName);
    effect->setBlurRadius(8);
    effect->setColor(QColor(0, 200, 0));
    effect->setOffset(-1,1);
    _dirName->setGraphicsEffect(effect);
    hl->addWidget(_dirName);

    hl->addSpacerItem(new QSpacerItem(40, 40, QSizePolicy::Expanding));
    _pbClear = new QPushButton(tr("Çå¿Õ"), this);
    connect(_pbClear, SIGNAL(clicked()), this, SLOT(clearDir()));
    hl->addWidget(_pbClear);

    vl->addLayout(hl);

    _drawer = new Drawer(this);
    _drawer->setFrameStyle(QFrame::NoFrame);
    vl->addWidget(_drawer);

    if (direction == DirectionIndicator::Up)
        vl->addWidget(_directionIndicator);

    setLayout(vl);
    hide();
}

QPoint DirContainer::translateToParent()
{
    qDebug() << __PRETTY_FUNCTION__ << _mate->pos() << parentWidget()->mapFromGlobal(_mate->mapToGlobal(QPoint(0, 0)));
    return parentWidget()->mapFromGlobal(_mate->mapToGlobal(QPoint(0, 0)));
}

void DirContainer::popup()
{
    if (_directionIndicator->direction() == DirectionIndicator::Down) {
        move(0, translateToParent().y() + _mate->height());
    } else {
        move(0, translateToParent().y());
    }
    resize(width(), 0);

    _directionIndicator->setIndicatorPos(_mate->pos().x()+_mate->width()/2);

    buildMask();
    QTimer::singleShot(0, this, SLOT(animatedShow()));

}

void DirContainer::closeDir()
{
    _drawer->clearToggles();
    hide();
    clearMask();
    _animatedHiding = false;
}

void DirContainer::animatedHide()
{
    if (_animatedHiding)
        return;

    _animatedHiding = true;
    qDebug() << __PRETTY_FUNCTION__;
    QPropertyAnimation *pa = new QPropertyAnimation(this, "geometry");
    pa->setDuration(500);
    QRect geo = this->geometry();
    pa->setStartValue(geo);

    if (_directionIndicator->direction() == DirectionIndicator::Down) {
        geo.setTop(translateToParent().y() + _mate->height());
    } else {
        geo.setTop(translateToParent().y());
    }
    geo.setHeight(0);
    pa->setEndValue(geo);

    connect(pa, SIGNAL(valueChanged(QVariant)), this, SLOT(slotUpdateMask(QVariant)));
    connect(pa, SIGNAL(finished()), this, SLOT(closeDir()));
    pa->start(QAbstractAnimation::DeleteWhenStopped);
}

void DirContainer::animatedShow()
{
    show();
    raise();

    QPropertyAnimation *pa = new QPropertyAnimation(this, "geometry");
    pa->setDuration(500);
    QRect geo = this->geometry();
    pa->setStartValue(geo);

    if (_directionIndicator->direction() == DirectionIndicator::Down) {
        geo.setHeight(_drawer->maximumHeight());
        if (geo.bottom() >= parentWidget()->height()) {
            geo.moveBottom(parentWidget()->height()-1);
        }
    } else {
        geo.setHeight(_drawer->maximumHeight());
        geo.moveTop(geo.top() - geo.height());
    }
    pa->setEndValue(geo);

    connect(pa, SIGNAL(valueChanged(QVariant)), this, SLOT(slotUpdateMask(QVariant)));
    pa->start(QAbstractAnimation::DeleteWhenStopped);
}

void DirContainer::slotUpdateMask(QVariant v)
{
    QRect geo = v.value<QRect>();
    if (_directionIndicator->direction() == DirectionIndicator::Down) {
        QRect r = _upsideMask->geometry();
        r.moveBottom(geo.y()+_directionIndicator->sizeHint().height());
        _upsideMask->setGeometry(r);

        r = _downsideMask->geometry();
        r.moveTop(geo.bottom());
        _downsideMask->setGeometry(r);

    } else {
        QRect r = _upsideMask->geometry();
        r.moveBottom(geo.y());
        _upsideMask->setGeometry(r);

        //_downsideMask keeps still
    }
}

void DirContainer::buildMask()
{
    QPixmap pm = QPixmap::grabWidget(parentWidget());
    //grayish the mask
    QImage img = pm.toImage();
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            int val = qGray(img.pixel(i, j));
            img.setPixel(i, j, qRgba(val, val, val, qAlpha(img.pixel(i, j))));
        }
    }

    QPainter p(&pm);
    p.drawImage(0, 0, img);
    //highlight current dir icon
    p.drawPixmap(translateToParent(), QPixmap::grabWidget(_mate));

    int splity;
    if (_directionIndicator->direction() == DirectionIndicator::Down) {
       splity = geometry().y() + _directionIndicator->sizeHint().height();
    } else {
       splity = geometry().y() - _directionIndicator->sizeHint().height();
    }

    _upsideMask = new QLabel(parentWidget());
    _upsideMask->setContextMenuPolicy(Qt::PreventContextMenu);
    _upsideMask->setPixmap(pm.copy(0, 0, pm.width(), splity));

    _downsideMask = new QLabel(parentWidget());
    _downsideMask->setContextMenuPolicy(Qt::PreventContextMenu);
    _downsideMask->setPixmap(pm.copy(0, splity+1, pm.width(), pm.height()-splity));

    _upsideMask->move(0, 0);
    _downsideMask->move(0, splity+1);


    qDebug() << __PRETTY_FUNCTION__ << _upsideMask->geometry();
    _upsideMask->installEventFilter(this);
    _downsideMask->installEventFilter(this);
    _upsideMask->show();
    _downsideMask->show();
    _upsideMask->raise();
    _downsideMask->raise();
}

void DirContainer::clearMask()
{
    _upsideMask->hide();
    _downsideMask->hide();
    _upsideMask->setParent(0);
    _downsideMask->setParent(0);
    delete _upsideMask;
    delete _downsideMask;
}

bool DirContainer::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == _upsideMask || obj == _downsideMask) {
        if (ev->type() == QEvent::MouseButtonPress) {
            QTimer::singleShot(0, this, SLOT(animatedHide()));
            return true;
        }
    }

    return QWidget::eventFilter(obj, ev);
}

void DirContainer::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    QRect r = rect();
    if (_directionIndicator->direction() == DirectionIndicator::Down) {
        r.setTop(_directionIndicator->height());
    } else {
        r.setBottom(r.bottom()-_directionIndicator->height());
    }
    painter.drawPixmap(
        r, QPixmap(":images/dir_bg.png").scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void DirContainer::focusOutEvent(QFocusEvent *ev)
{
    animatedHide();
    QWidget::focusOutEvent(ev);
}

int DirContainer::dirId() const
{
    return _drawer->dirId();
}

void DirContainer::loadDir(LocalApp *dir)
{
    _dirName->setText(dir->name());
    _drawer->loadDir(dir);
}

void DirContainer::clearDir()
{
    const IndexedList& icons = _drawer->icons();
    QList<LocalApp*> apps;
    for (int i = 0; i < icons.size(); ++i) {
        apps.append(qobject_cast<AppIconWidget*>(icons.at(i))->app());
    }

    _drawer->clearDir();
    QMetaObject::invokeMethod(_mate, "restoreAppsToDesktop", Q_ARG(QList<LocalApp*>, apps));
}

void DirContainer::deleteDir()
{
    const IndexedList& icons = _drawer->icons();
    for (int i = 0; i < icons.size(); ++i) {
        LocalApp *app = qobject_cast<AppIconWidget*>(icons.at(i))->app();
        QMetaObject::invokeMethod(_mate, "requestEraseDirItem", Q_ARG(LocalApp*, app));
    }

    _drawer->clearDir();
}

void DirContainer::handleIconErasion(IconWidget *icon)
{
    LocalApp * app = qobject_cast<AppIconWidget*>(icon)->app();
    QMetaObject::invokeMethod(_mate, "requestEraseDirItem", Q_ARG(LocalApp*, app));
}

void DirContainer::handleRestoreToDesktop(IconWidget *icon)
{
    QList<LocalApp*> apps;
    apps.append(qobject_cast<AppIconWidget*>(icon)->app());
    _drawer->removeIcon(icon);
    QMetaObject::invokeMethod(_mate, "restoreAppsToDesktop", Q_ARG(QList<LocalApp*>, apps));
}

void DirContainer::handleIconDeletion(IconWidget *icon)
{
    LocalApp * app = qobject_cast<AppIconWidget*>(icon)->app();
    //move into trash
    _drawer->removeIcon(icon);
    QMetaObject::invokeMethod(_mate, "requestMoveDirItemToTrash", Q_ARG(LocalApp*, app));
}

void DirContainer::insertNewIcon(LocalApp *app)
{
    _drawer->insertNewIcon(app);
    _mate->init();
}

void DirContainer::updateMateThumbs()
{
    qDebug() << __PRETTY_FUNCTION__;
    _mate->init();
}
