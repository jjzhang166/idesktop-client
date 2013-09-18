#include <QtGui>
#include "iconwidget.h"
#include "runapp.h"
#include "localapps.h"
#include "idesktopsettings.h"
#include "gridcontainer.h"
#include "dircontainer.h"
#include "contextmenuwidget.h"

QPixmap makeTranslucentPixmap(const QString &src)
{
    QImage img(src);
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            QRgb rgb = img.pixel(i, j);
            img.setPixel(i, j, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), 100));
        }
    }

    return QPixmap::fromImage(img);
}

LineEdit::LineEdit(QWidget *parent)
    : QLineEdit(parent)
{

    setStyleSheet(QString("QLineEdit {"
                          "border: 1px solid gray;"
                          "border-radius: 8px;"
                          "padding: 0 8px;"
                          "background: rgba(0, 0, 0, 100);"
                          "selection-background-color: darkgray;}"));

    setContextMenuPolicy(Qt::NoContextMenu);
    setTextMargins(10, 0, 0, 0);

    setFocusPolicy(Qt::ClickFocus);
    setAlignment(Qt::AlignCenter);
    setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));

    switch(IDesktopSettings::instance()->iconSize()) {
    case IconWidget::large_size: {
        _editLeft = makeTranslucentPixmap(":/images/dir_edit_left.png").scaled(11,23);
        _editCenter = makeTranslucentPixmap(":/images/dir_edit_center.png").scaled(1,23);
        _editRight = makeTranslucentPixmap(":/images/dir_edit_right.png").scaled(11,23);
        break;
    }
    case IconWidget::medium_size: {
        _editLeft = makeTranslucentPixmap(":/images/dir_edit_left.png").scaled(9,19);
        _editCenter = makeTranslucentPixmap(":/images/dir_edit_center.png").scaled(1,19);
        _editRight = makeTranslucentPixmap(":/images/dir_edit_right.png").scaled(9,19);
        break;
    }
    default: {
        _editLeft = makeTranslucentPixmap(":/images/dir_edit_left.png").scaled(8,16);
        _editCenter = makeTranslucentPixmap(":/images/dir_edit_center.png").scaled(1,16);
        _editRight = makeTranslucentPixmap(":/images/dir_edit_right.png").scaled(8,16);
        break;
    }
    }
}

QString LineEdit::myDisplayText() const
{
    QFontMetrics fm(font());
    QString elided = fm.elidedText(text(), Qt::ElideRight, width() - textMargins().left() - textMargins().right());
    return elided;
}

void LineEdit::paintEvent(QPaintEvent* ev)
{
    if (!hasFocus()) {
        QPainter painter(this);
//        painter.drawPixmap(_editLeft.rect(), _editLeft);
//        painter.drawPixmap(width() - _editRight.width(), 0, _editRight);
//        painter.drawPixmap(_editLeft.width(), 0,
//                           width()-_editLeft.width()-_editRight.width(), _editCenter.height(), _editCenter);

        QFontMetrics fm(font());
        painter.drawText(20, fm.ascent(), myDisplayText());
    } else {
        QLineEdit::paintEvent(ev);
    }
}

void LineEdit::focusInEvent(QFocusEvent *event)
{
    emit focusIn();
    QLineEdit::focusInEvent(event);
}

void LineEdit::focusOutEvent(QFocusEvent *event)
{
    emit focusOut();
    QLineEdit::focusOutEvent(event);
}

////////////////////////////////////////////////////////////////////////////////

IconWidget::IconWidget(QWidget *parent) :
    QWidget(parent)
{
    _isDown = false;
    _isToggled = false;
    _menu = NULL;

    _nameEdit = new LineEdit(this);
    _nameEdit->setFrame(false);
    _nameEdit->setReadOnly(true);
    setMouseTracking(true);

    init();
}

void IconWidget::setPixmap(const QString &icon)
{
    _icon = QIcon(QPixmap(icon));
}

void IconWidget::setText(const QString &text)
{
    _text = text;
    _nameEdit->setText(text);
}

void IconWidget::setToggled(bool val)
{
    _isToggled = val;
    if (_isToggled) {
        qDebug() << __PRETTY_FUNCTION__ << "icon" << _text << "toggled";
    }
    update();
}

void IconWidget::init()
{
    switch(IDesktopSettings::instance()->iconSize()) {
        case IconWidget::large_size :
            setLargeSize();
            break;

        case IconWidget::medium_size :
            setMediumSize();
            break;

        default:
            setSmallSize();
            break;
    }
    update();
}

void IconWidget::setLargeSize()
{
    _iconRect = QRect(37, 36, 72, 72);
    setFixedSize(LARGESIZE);
    _nameEdit->setGeometry(20, height() - 26, 110, 23);
}

void IconWidget::setMediumSize()
{
    _iconRect = QRect(32, 30, 60, 60);
    setFixedSize(MEDIUMSIZE);
    _nameEdit->setGeometry(20, height() - 24, 84, 18);
}

void IconWidget::setSmallSize()
{
    _iconRect = QRect(28, 24, 52, 52);
    setFixedSize(SMALLSIZE);
    _nameEdit->setGeometry(20, height() - 22, 65, 15);
}

const QString & IconWidget::text() const
{
    return _text;
}

void IconWidget::contextMenuEvent(QContextMenuEvent *ev)
{
    if (_menu && hitButton(ev->pos())) {
        _menu->move(ev->globalPos());
        _menu->setVisible(true);
        ev->accept();
    } else {
        QWidget::contextMenuEvent(ev);
    }
}

void IconWidget::mousePressEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);
    _isDown = true;
    update();
}

void IconWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    QWidget::mouseReleaseEvent(ev);
    _isDown = false;
    update();
}

void IconWidget::enterEvent(QEvent *ev)
{
    QWidget::enterEvent(ev);
    update();
}

void IconWidget::leaveEvent(QEvent *ev)
{
    QWidget::leaveEvent(ev);
    update();
}

void IconWidget::mouseMoveEvent(QMouseEvent *ev)
{
    QWidget::mouseMoveEvent(ev);
    update();
}

void IconWidget::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.setPen(Qt::red);
    painter.setFont(QFont(QString::fromLocal8Bit(" Microsoft YaHei"), FONTSIZE, QFont::Black));
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QIcon::Mode mode;
    if (isDown() || isToggled()) {
        mode = QIcon::Selected;
    } else if (hitButton(mapFromGlobal(QCursor::pos()))) {
        mode = QIcon::Disabled;
    } else {
        mode = QIcon::Normal;
    }

    icon().paint(&painter, 0, 0, width(), height(), Qt::AlignCenter, mode);
}

void IconWidget::sendRestoreToDesktop()
{
    emit restoreToDesktop(this);
}

void IconWidget::sendRequestErasion()
{
    emit requestErasion(this);
}

void IconWidget::sendRequestDeletion()
{
    emit requestDeletion(this);
}

bool IconWidget::hitButton(const QPoint &pos) const
{
    return _iconRect.contains(pos);
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

AppIconWidget::AppIconWidget(QWidget *parent)
    :IconWidget(parent), _app(0), _inDrag(false)
{
    _nameEdit->setReadOnly(false);
    connect(_nameEdit, SIGNAL(editingFinished()), this, SLOT(slotNameChanged()));
}

void AppIconWidget::setApp(LocalApp *app)
{
    _app = app;
    setupMenu();
}

void AppIconWidget::setPixmap(const QString &icon)
{
    if (!_app) {
        IconWidget::setPixmap(icon);
    } else {
        IconWidget::setPixmap(_app->icon());
    }
}

void AppIconWidget::slotNameChanged()
{
    qDebug() << __PRETTY_FUNCTION__ << _nameEdit->text();
    if (_nameEdit->text().isEmpty()) {
        _nameEdit->setText(text());
        return;
    }

    _text = _nameEdit->text();
    if (_app) {
        _app->setName(text());
        emit iconItemNameChanged(_app->uniqueName(), text());
    }
}

void AppIconWidget::setupMenu()
{
      if (_app->dirId() == 1000) {
        _menu = new MenuWidget(MenuWidget::dustbinMenu, this);
        connect(_menu, SIGNAL(restore()), this, SLOT(sendRestoreToDesktop()));
        connect(_menu, SIGNAL(del()), this, SLOT(sendRequestErasion()));

    } else {
        _menu = new MenuWidget(MenuWidget::iconMenu, this);
        connect(_menu, SIGNAL(del()), this, SLOT(sendRequestDeletion()));
        connect(_menu, SIGNAL(run()), this, SLOT(run()));
    }
    _menu->setVisible(false);

}

void AppIconWidget::setText(const QString &text)
{
    if (!_app) {
        IconWidget::setText(text);
    } else {
        IconWidget::setText(_app->name());
    }
}

void AppIconWidget::paintEvent(QPaintEvent *ev)
{
    IconWidget::paintEvent(ev);
}

void AppIconWidget::enterEvent(QEvent *ev)
{
    IconWidget::enterEvent(ev);
}

void AppIconWidget::leaveEvent(QEvent *ev)
{
    IconWidget::leaveEvent(ev);
    qDebug() << __PRETTY_FUNCTION__ << _text;
    _dragStartPos = QPoint();
}

void AppIconWidget::mouseMoveEvent(QMouseEvent *ev)
{
    IconWidget::mouseMoveEvent(ev);
    if (_dragStartPos.isNull())
        return;

    if ((ev->pos() - _dragStartPos).manhattanLength() < QApplication::startDragDistance())
        return;

    qDebug() << __PRETTY_FUNCTION__ << "drag started" << QTime::currentTime();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QMimeData *mimeData = new QMimeData;

    QDrag *drag = new QDrag(this);

    QVariant val = parentWidget()->property("multiSelectionActivated");
    bool supportMultiSelection = val.isValid() && val.value<bool>();
    QList<AppIconWidget*> items;
    if (supportMultiSelection) {
        const QList<QVariant>& objs = parent()->property("toggledIcons").toList();
        foreach(QVariant v, objs) {
            items.append(v.value<AppIconWidget*>());
        }

        QString mime_type("application/x-iconitems");

        foreach(AppIconWidget *w, items) {
            LocalApp *icon_app = w->app();
            dataStream << icon_app->uniqueName();
            if (w->inherits("DirIconWidget")) {
                //x-mixed-iconitems indicates that at least one dir icon is dragged
                mime_type = QString("application/x-mixed-iconitems");
            }
        }
        mimeData->setData(mime_type, itemData);

        drag->setMimeData(mimeData);
        QPixmap thumb = buildMultiDragPreview(items);
        drag->setPixmap(thumb);
        drag->setHotSpot(thumb.rect().center());

    } else {
        items.append(this);
        dataStream << _app->uniqueName() << _app->page() << _app->index();
        mimeData->setData("image/x-iconitem", itemData);
        drag->setMimeData(mimeData);
        QPixmap thumb = QPixmap::grabWidget(this);
        drag->setPixmap(thumb);
        drag->setHotSpot(thumb.rect().center());
    }

    _inDrag = true;
    Qt::DropAction action = drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
    qDebug() << __PRETTY_FUNCTION__ << "drop down and action is" << action;
    ev->accept();
    _inDrag = false;

    //HACK: there is no mouseRelease event received after drag dropped.
    //need to know why.
    QMouseEvent *mouseEv = new QMouseEvent(QEvent::MouseButtonRelease, ev->pos(),
                                           ev->button(), ev->buttons(), ev->modifiers());
    qApp->postEvent(this, mouseEv);

    if (action == Qt::MoveAction) {
        //ignore
    } else if (action == Qt::CopyAction) {
        //CopyAction: drag into a dir or toolbar
        //FIXME: if NO. of items is large, a lot of affectedIndexes will be trigger,
        //and a lot of animations will be concurrently executed.
        foreach(AppIconWidget *w, items) {
            emit w->DragOut(w);
        }

        if (supportMultiSelection)
            QMetaObject::invokeMethod(parent(), "clearToggles");
    }
}

QPixmap AppIconWidget::buildMultiDragPreview(const QList<AppIconWidget*> &items)
{
    int grids = qCeil(sqrt(items.size()));

    qDebug() << __PRETTY_FUNCTION__ << items.size() << "items" << grids << "grids";
    QImage img(grids*width(), grids*height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(QColor(0, 0, 0, 0));

    QPainter p(&img);
    for (int i = 0; i < items.size(); ++i) {
        int x = (i%grids)*width();
        int y = (i/grids)*height();
        QRect rect(x, y, width(), height());
        p.drawPixmap(rect, QPixmap::grabWidget(items[i]));
    }
    return QPixmap::fromImage(img.scaledToHeight(img.height()/2));
}

void AppIconWidget::mousePressEvent(QMouseEvent *ev)
{
    IconWidget::mousePressEvent(ev);
    if (ev->button() == Qt::LeftButton && (ev->modifiers() == Qt::NoModifier)) {
        if (this->hitButton(ev->pos())) {
            _dragStartPos = ev->pos();
        }
    }
}

void AppIconWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __PRETTY_FUNCTION__;
    IconWidget::mouseReleaseEvent(ev);
    _dragStartPos = QPoint();
}

void AppIconWidget::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if (!_app->url().isEmpty()) {
        qDebug() << __PRETTY_FUNCTION__ << "openUrl";
        QDesktopServices::openUrl(QUrl(_app->url()));

    } else if (ev->button() == Qt::LeftButton) {
        run();
    }

}

void AppIconWidget::run()
{
    RunApp::getRunApp()->runApp(_app->uniqueName());
}

////////////////////////////////////////////////////////////////////////////////

DirIconWidget::DirIconWidget(QWidget *parent)
    :AppIconWidget(parent), _thumb(0), _dir(0)
{
}

void DirIconWidget::setPixmap(const QString &icon)
{
    IconWidget::setPixmap(":/images/dir_add_icon.png");
    updateThumbs();
}

void DirIconWidget::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    QPainter painter(this);
    painter.setPen(Qt::red);
    painter.setFont(QFont(QString::fromLocal8Bit(" Microsoft YaHei"), FONTSIZE, QFont::Black));
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QIcon::Mode mode;
    if (isDown()) {
        mode = QIcon::Selected;
    } else if (hitButton(mapFromGlobal(QCursor::pos()))) {
        mode = QIcon::Disabled;
    } else {
        mode = QIcon::Normal;
    }
    icon().paint(&p, _iconRect, Qt::AlignCenter, mode);
}

#define SMALL_L_SIZE QSize(42, 42)    //72*72
#define SMALL_M_SIZE QSize(36, 36)    //48*48
#define SMALL_S_SIZE QSize(30, 30)    //32*32

void DirIconWidget::setupMenu()
{
    _menu = new MenuWidget(MenuWidget::dirMenu, this);
    _menu->setVisible(false);

    connect(_menu, SIGNAL(open()), this, SLOT(run()));
    connect(_menu, SIGNAL(clear()), this, SLOT(clearDir()));
    connect(_menu, SIGNAL(del()), this, SLOT(deleteDir()));
}

void DirIconWidget::setApp(LocalApp *app)
{
    AppIconWidget::setApp(app);
    updateThumbs();
}

void DirIconWidget::updateThumbs()
{
    if (!_app)
        return;

    if (_thumb) {
        delete _thumb;
        _thumb = NULL;
    }

    _thumb = new QWidget(this);

    switch(IDesktopSettings::instance()->iconSize()) {
    case 0 :
        _thumb->move(35, 36);
        break;

    case 1 :
        _thumb->move(35 - 24 + 15, 35 - 24 + 15);
        break;

    default:
        _thumb->move(35 - 32 + 22, 35 - 32 + 22);
        break;
    }

    _thumb->setGeometry(_iconRect);
    QGridLayout *lay = new QGridLayout;

    QSize thumb_size = SMALL_L_SIZE;
    if (IDesktopSettings::instance()->iconSize() == IconWidget::medium_size)
        thumb_size = SMALL_M_SIZE;
    else if (IDesktopSettings::instance()->iconSize() == IconWidget::small_size)
        thumb_size = SMALL_S_SIZE;

    lay->setSpacing(0);
    QList<LocalApp*> apps = LocalAppList::getList()->appsInDir(app()->id().toInt());
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            if (!apps.size())
                break;

            LocalApp *app = apps.takeFirst();
            QLabel *l = new QLabel(_thumb);
            l->show();
            l->setPixmap(QIcon(app->icon()).pixmap(thumb_size));
            lay->addWidget(l, r, c);
        }
    }

    _thumb->setLayout(lay);
    _thumb->show();
}

void DirIconWidget::init()
{
    AppIconWidget::init();
    updateThumbs();
}

void DirIconWidget::run()
{
    openDir();
}

void DirIconWidget::openDir()
{
    emit requestOpenDir(dir());
}

DirIconWidget::~DirIconWidget()
{
    qDebug() << __PRETTY_FUNCTION__ << "delete dir";
    delete _dir;
    delete _thumb;
}

void DirIconWidget::deleteDir()
{
    dir()->deleteDir();
    //dir itself will be delete when icon gets removed
    emit requestErasion(this);
}

void DirIconWidget::clearDir()
{
    dir()->clearDir();
}

DirContainer *DirIconWidget::dir()
{
    if (!_dir) {

        _dir = new DirContainer((app()->dirId() == -2?DirectionIndicator::Up:DirectionIndicator::Down), this);
        _dir->loadDir(app());
    }

    return _dir;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief TrashDirWidget
/// \param parent
///
TrashDirWidget::TrashDirWidget(QWidget *parent)
    :DirIconWidget(parent)
{
}

void TrashDirWidget::restoreIcon()
{
    Q_ASSERT(0);
}

void TrashDirWidget::paintEvent(QPaintEvent* ev)
{
    //TODO: draw hints about how many icons inside trash
    AppIconWidget::paintEvent(ev);
}

void TrashDirWidget::setPixmap(const QString &icon)
{
    AppIconWidget::setPixmap(icon);
}

void TrashDirWidget::setupMenu()
{
//    _menu = new MenuWidget(MenuWidget::dustbinMenu, this);
//    _menu->setVisible(false);

    //has no menu
}

void TrashDirWidget::setApp(LocalApp *app)
{
    AppIconWidget::setApp(app);
}

void TrashDirWidget::init()
{
    AppIconWidget::init();
}

void TrashDirWidget::openDir()
{
    emit requestOpenDir(dir());
}

DirContainer *TrashDirWidget::dir()
{
    if (!_dir) {
        _dir = new DirContainer(DirectionIndicator::Up, this);
        _dir->loadDir(app());
    }

    return _dir;
}
