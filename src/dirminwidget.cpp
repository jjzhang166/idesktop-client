#include "dirminwidget.h"

#include <QtDebug>
#include <QPainter>
#include <cassert>

#include "virtualdesktop.h"
#define SPACING 10
#define ICONWIDTH 96
#define ICONHEIGHT 96


IconMinItem::IconMinItem(QWidget *parent)
    : QWidget(parent)
//    , _trembling(0)
{

    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setDuration(200);
    _animation->setEasingCurve(QEasingCurve::OutBack);

//    _timeline = new QTimeLine(5000, this);
//    _timeline->setLoopCount(0);
//    _timeline->setUpdateInterval(65);

//    _icontype = APPICON;

//    if (parent) {
//        if (_style == VirtualDesktop::localIcon)
//        {
//            connect(static_cast<VirtualDesktop*>(parent), \
//                    SIGNAL(trembleStarted()), \
//                    this, SLOT(startTremble()));
//            connect(static_cast<VirtualDesktop*>(parent), \
//                    SIGNAL(trembleStoped()), \
//                    this, SLOT(stopTremble()));
//        }
//    }
}

IconMinItem::~IconMinItem()
{
//    delete _timeline;
    delete _animation;
}

void IconMinItem::setPage(int page)
{
    _page = page;
//    _app->setPage(page);
}
void IconMinItem::setIndex(int index)
{
    _index= index;
//   _app->setIndex(index);
}
//void IconMinItem::setHidden(bool hide)
//{
//    _app->setHidden(hide);
//}

void IconMinItem::animationMove(const QRect &start, const QRect &end)
{
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _animation->start();
}

//void IconMinItem::doTremble(qreal value)
//{
//    Q_UNUSED(value);

//#define LEFT -1
//#define RIGHT 1
//    static int direction = LEFT;
//    if (_trembling == -1) {
//        _trembling = 0;
//        direction = RIGHT;
//    }
//    else if (_trembling == 0) {
//        if (direction == LEFT) {
//            _trembling = -1;
//        } else {
//            _trembling = 1;
//        }
//    }
//    else {
//        _trembling = 0;
//        direction = LEFT;
//    }
//    repaint();
//#undef LEFT
//#undef RIGHT
//}

//void IconMinItem::startTremble()
//{
//    _timeline->start();
//    _pixmap = _closePixmap;
//}

//void IconMinItem::stopTremble()
//{
//    _timeline->stop();
//    _trembling = 0;
//    _pixmap = _normalPixmap;
//    repaint();
//}

void IconMinItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
//    if (_trembling)
//        painter.translate(width()/2, height()/2);
//    painter.rotate(2*_trembling);
//    if (_trembling)
//        painter.translate(-1*width()/2, -1*height()/2);

    painter.drawPixmap(0, 0, width(), height(), _pixmap.scaled(width(), height()));
    QWidget::paintEvent(event);
}

void IconMinItem::setPixmap(const QString &icon)
{
    _pixText = icon;
    _pixmap = icon;

}

void IconMinItem::setText(const QString &text)
{
    _text = text;
}

const QString & IconMinItem::text()
{
    return _text;
}

const QPixmap & IconMinItem::pixmap()
{
    return _pixmap;
}

void IconMinItem::setUrl(const QString &url)
{
    _url = url;
}


DirMinWidget::DirMinWidget(QWidget *parent)
    : QWidget(parent)
    , _width(74)
    , _height(74)
    , _gridWidth(21)
    , _gridHeight(21)
    , _col(2)
    , _row(2)
    , _iconsPerPage(4)
    , _count(1)
    , _current(0)
    , _iconNum(0)
    , _pageSize(_width, _height)
{
    setAcceptDrops(true);

//    _local = LocalAppList::getList();

//    for (int i = 0; i < _local->count(); i++)
//    {
//        _count = i / _iconsPerPage + 1;
//    }

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j % _col) * (_gridWidth + SPACING);
            int y = (j / _col) * (_gridHeight + SPACING);

            newList.insert(j, \
                           QRect(x, y, _gridWidth, _gridHeight));
        }
        _gridTable.insert(i, newList);

    }


    for (int i = 0; i < _count; i++) {
        QList<IconMinItem*> newList;
        for (int j = 0; j < _col * _row; j++)
            newList.insert(j, NULL);
        _iconTable.insert(i, newList);
    }

    for (int i = 0; i < _count; i++)
        _nextIdx.insert(i, 0);

//    _animation = new QPropertyAnimation(this, "geometry");
//    _animation->setEasingCurve(QEasingCurve::OutBack);

    setAutoFillBackground(true);
    setFixedSize(_width * _count, _height);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255,0));
    setPalette(pal);


//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;

//        if(_local->at(i)->isRemote())
//        {
//            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                    _local->at(i)->page(), _local->at(i)->index(), vappIcon);
//        }else{
//            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                    _local->at(i)->page(), _local->at(i)->index(), localIcon);
//        }
//    }

//    connect(_local, SIGNAL(appAdded(const QString&, const QString&)), this, SLOT(addIcon(const QString&, const QString&)));
//    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));


}

DirMinWidget::~DirMinWidget()
{

}

//int DirsWidget::addDirIcon(const QString &text, const QString &iconPath, int page, int index)
//{

//}

void DirMinWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,72, 72, QPixmap(":/images/dir_add_icon.png"));
}

void DirMinWidget::dragEnterEvent(QDragEnterEvent *event)
{
    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (icon && icon->getStyle() == VirtualDesktop::dirIcon)
        return;

    if (event->mimeData()->hasFormat("image/x-iconitem")) {
//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }

//        IconItem *icon = qobject_cast<IconItem*> (event->source());
//        if (icon && icon->getStyle() != VirtualDesktop::dirIcon) {
//            qDebug() << "***************************************************";
                event->setDropAction(Qt::MoveAction);
                event->accept();
//       }
    } else {
        event->ignore();
    }
    emit iconEnter();
//    qDebug() << "dragEnterEvent******************";
}

void DirMinWidget::dragMoveEvent(QDragMoveEvent *event)
{
//qDebug() << "dragMoveEvent******************";

//        qDebug() << "dragMoveEvent******************";

    event->setDropAction(Qt::MoveAction);
    event->accept();


    emit iconMove();
}

void DirMinWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit iconLeave();
}

void DirMinWidget::dropEvent(QDropEvent *event)
{

    if (event->mimeData()->hasFormat("image/x-iconitem")) {
        QByteArray itemData = event->mimeData()->data("image/x-iconitem");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString nameText;
        QString pixText;
        QString urlText;
        dataStream >> nameText >> pixText >> urlText;

        qDebug() << nameText;
        qDebug() << pixText;
        qDebug() << urlText;

         pixText.replace(".png", ".ico");
        qDebug() << pixText;

//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }

    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (icon && icon->getStyle() != VirtualDesktop::dirIcon) {
        qDebug() << "***************************************************";
            event->setDropAction(Qt::MoveAction);
            event->accept();
    }

    addIcon(nameText, pixText, -1, -1, urlText);
    emit iconDrop(nameText ,pixText, urlText);
    } else {
        event->ignore();
    }
}

void DirMinWidget::expand()
{
    _count++;
    setFixedSize(_count * _pageSize.width(), _pageSize.height());
    _pages.append(-(_count-1) * _pageSize.width());
    QList<QRect> newGridList;
//    _gridWidth = ICONWIDTH + SPACING * 2;
//    _gridHeight = ICONHEIGHT + SPACING * 2;
    for (int i = 0; i < _col * _row; i++) {
        //vertical display
//        int x =  _pageSize.width() * (_count - 1)\
//                + (i / _row) * gridWidth;
//        int y = (i % _row) * gridHeight;

            //horizontal display
            int x =  _pageSize.width() * (_count - 1)\
                    + (i % _col) * (_gridWidth + SPACING);
            int y = (i / _col) * (_gridHeight + SPACING);
        newGridList.insert(i, \
                           QRect(x, y, _gridWidth, _gridHeight));
    }
    _gridTable.append(newGridList);
    QList<IconMinItem*> newIconList;
    for (int i = 0; i < _col * _row; i++)
        newIconList.insert(i, NULL);
    _iconTable.append(newIconList);
    _nextIdx.append(0);
    emit(pageIncreased());
}

void DirMinWidget::delPage(int page)
{
    if (_count == 1)
        return;
    if (_iconTable[page][0] != NULL) {
        assert("Error while delete page");
    }
    /* start from page, end before the last page
     * move the icon backward
     */
    for (int i = page; i < (_count - 1); i++) {
        for (int j = 0; j < _col * _row; j++) {
            _iconTable[i][j] = _iconTable[i+1][j];
            IconMinItem *icon = _iconTable[i][j];
            if (icon != NULL) {
                icon->setPage(i);
                icon->setIndex(j);
                icon->setGeometry(_gridTable[i][j].translated(SPACING, SPACING));
            }
        }
    }
    _iconTable.removeLast();
    _gridTable.removeLast();
    _pages.removeLast();
    _nextIdx.removeAt(page);
    _count--;
    if (_current > page) {
        _current--;
        if (_inDrag)
            _inDrag->setPage(_inDrag->page() - 1);
        emit pageChanged(_current);
    }
    setFixedSize( _count * _pageSize.width(), _pageSize.height());
    move(_pages[_current], y());
    emit pageDecreased();
}

int DirMinWidget::addIcon(const QString &text, \
                            const QString &iconPath, \
                            int page, \
                            int index,\
                            const QString &url)
{
    int expandPageCount = _iconNum / _iconsPerPage + 1;
    if (expandPageCount > _count)
        expand();


    IconMinItem *icon = new IconMinItem(this);
    //icon->setText(text);
    /* TODO deal with the icon is full */
    if (page == -1) {
        for (int i = 0; i < _count; i++) {
            if (_nextIdx[i] < _row * _col) {
                page = i;
                index = _nextIdx[i];
                break;
            }
        }
    } else {
        if (index == -1) {
            if (_nextIdx[page] < _iconsPerPage)
            {
                index = _nextIdx[page];
            }
            else {
                for (int i = 0; i < _count; i++) {
                    if (_nextIdx[i] < _row * _col) {
                        page = i;
                        index = _nextIdx[i];
                        move(_pages[page], y());
                        break;
                    }
                }
            }
        }
    }

    icon->setPixmap(iconPath);
    icon->setGeometry(_gridTable[page][index].translated(SPACING, SPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(url);
    _iconDict.insert(text, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();
    qDebug() << icon->index();
    qDebug() << icon->page();
    qDebug() << icon->getUrl();

//    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));
    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));

    _iconNum++;

     return page;
}

void DirMinWidget::delIcon(const QString &text)
{

    IconMinItem *icon = _iconDict.take(text);
    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();
    //icon->setPage(-1);
    //icon->setIndex(-1);
    delete _iconTable[p][s];

    _iconNum--;

    for(int i = p; i < _count; i++)
    {
        if (i == p)
        {
            int k;
            for (k = s + 1; k < _nextIdx[p]; k++) {
                QRect start = _gridTable[p][k].translated(SPACING, SPACING);
                QRect end = _gridTable[p][k-1].translated(SPACING, SPACING);
                _iconTable[p][k]->animationMove(start, end);
                _iconTable[p][k]->setIndex(k-1);
                _iconTable[p][k - 1] = _iconTable[p][k];
            }
        }
        else {
            for (int j = 0; j < _nextIdx[i]; j++)
            {
                if (j == 0)
                {

                    QRect start = _gridTable[i][j].translated(SPACING, SPACING);
                    QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(SPACING, SPACING);
                    _iconTable[i][j]->animationMove(start, end);
                    _iconTable[i][j]->setPage(i-1);
                    _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
                    _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
                }
                else {

                    QRect start = _gridTable[i][j].translated(SPACING, SPACING);
                    QRect end = _gridTable[i][j-1].translated(SPACING, SPACING);
                    _iconTable[i][j]->animationMove(start, end);
                    _iconTable[i][j]->setIndex(j-1);
                    _iconTable[i][j - 1] = _iconTable[i][j];
                }
            }
        }
    }

    if (_nextIdx[_count - 1] == 0)
    {
        _iconTable[_count - 1 - 1][_nextIdx[_count - 1 - 1] - 1] = NULL;
        _nextIdx[_count - 1 - 1]--;
    }else
    {
        _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
        _nextIdx[_count - 1]--;
    }

    if ((_count > 1) && (_iconNum == _iconsPerPage * (_count - 1)))
        delPage(_count - 1);


//    if (_iconNum == 0) {
//        appCancel();
//    }

//    bool trembling = true;

//    for (int i = 0; i < _iconNum; i++)
//    {
//        if (!_iconNum->isRemote())
//        {
//            trembling = false;
//            break;
//        }
//    }

//    if (trembling) {
//        appCancel();
//    }
}

void DirMinWidget::mousePressEvent(QMouseEvent *)
{
    emit mouseClicked();
}

DirLineEdit::DirLineEdit(QString hint, QWidget *parent)
    : QLineEdit(parent), _hint(hint), _color(Qt::white)
{

    setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:5px;");
    resize(95, 15);

    _editLeft = new QImage(":/images/dir_edit_left.png");
    _editCenter = new QImage(":/images/dir_edit_center.png");
    _editRight = new QImage(":/images/dir_edit_right.png");


    setImgs(_editLeft, _editCenter, _editRight);

//    _pixmap.load(":images/login_input.png");

    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(setHint(const QString&)));

}

void DirLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
    QPainter painter(this);

    *_left = _left->scaled(11, 15);
    painter.drawImage(QRect(0, 0, 11, 15), *_left);
    *_right = _right->scaled(10, 15);
    painter.drawImage(QRect(width() - 10, 0, 10, 15), *_right);
    *_center = _center->scaled(width() - 11 - 10, 15);
    painter.drawImage(QRect(11, 0, 95 - 11 - 10, 15), *_center);
    //if (hasFocus())
    //    return;
    if (!text().isEmpty())
    {
        QRect rt = rect();
    //    painter.setPen(Qt::gray);
        painter.setPen(_color);
        painter.drawText(rt.translated(23,1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextHideMnemonic, _hint);
        return;
    }

    QRect rt = rect();
//    painter.setPen(Qt::gray);
    painter.setPen(_color);
    painter.drawText(rt.translated(3,1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextHideMnemonic, _hint);
}

void DirLineEdit::focusInEvent(QFocusEvent *event)
{
    _color = QColor(Qt::white);
    setImgs(_editLeft, _editCenter, _editRight);

    QLineEdit::focusInEvent(event);
}

void DirLineEdit::focusOutEvent(QFocusEvent *event)
{
    _color = QColor(Qt::white);
    setImgs(_editLeft, _editCenter, _editRight);//

    QLineEdit::focusOutEvent(event);
}

void DirLineEdit::setImgs(QImage *strLeft, QImage *strCenter, QImage *strRight)
{
    _left = strLeft;
    _center = strCenter;
    _right = strRight;

    repaint();
}

DirMinShowWidget::DirMinShowWidget(QWidget *parent)
    : QWidget(parent)
{
    resize(95, 95);
    _dirMinWidget = new DirMinWidget(this);
    _dirMinWidget->move(11, 0);
    _dirMinWidget->setVisible(true);

    _dirLineEdit = new DirLineEdit(QString("输入文件夹名称"), this);
    _dirLineEdit->move(0, height() - 15);

    connect(_dirMinWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
    connect(_dirMinWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dirMinWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
    connect(_dirMinWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
//    connect(_dirMinWidget, SIGNAL(iconDrop(const QString&, const QString& ,const QString&)),
//            this, SLOT(iconDropEvent(const QString&, const QString& ,const QString&)));
//    connect(_dirMinWidget, SIGNAL(mouseClicked()), this, SLOT(mouseClicked()));
    connect(_dirMinWidget, SIGNAL(iconDrop(const QString&, const QString& ,const QString&)),
            this, SIGNAL(iconDrop(const QString&, const QString& ,const QString&)));
    connect(_dirMinWidget, SIGNAL(mouseClicked()), this, SIGNAL(openItem()));
}

DirMinShowWidget::~DirMinShowWidget()
{

}

void DirMinShowWidget::mouseClicked()
{
//    emit openItem(_id, _page, _index);
    //emit openItem();
}

void DirMinShowWidget::iconDropEvent(const QString &text, const QString &iconPath, const QString &url)
{
    //emit iconDrop(_id, text, iconPath, url);
}
