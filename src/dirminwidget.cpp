#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QPainter>
#include <cassert>

#include "dirminwidget.h"

#define ICONWIDTH 72            //72
#define ICONHEIGHT 72           //72
#define SELECTWIDTH 37
#define SELECTHEIGHT 37

#define FONTSIZE 10

#define ICONITEMWIDTH 143                                 //(ICONWIDTH + SELECTWIDTH/2 + 8)
#define ICONITEMHEIGHT 143                               //(ICONHEIGHT + 15 + FONTSIZE * 2)

#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30

#define LARGESIZE QSize(143, 143)       //72*72
#define MEDIUMSIZE QSize(119, 119)      //48*48
#define SMALLSIZE QSize(103, 103)       //32*32

#define ICONLSIZE QSize(72, 72)       //72*72
#define ICONMSIZE QSize(60, 60)      //48*48
#define ICONSSIZE QSize(52, 52)       //32*32

#define SMALL_L_SIZE QSize(42, 42)    //72*72
#define SMALL_M_SIZE QSize(36, 36)    //48*48
#define SMALL_S_SIZE QSize(30, 30)    //32*32


extern int ICON_TYPE;

IconMinItem::IconMinItem(QWidget *parent)
    : QWidget(parent)
{
    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setDuration(200);
    _animation->setEasingCurve(QEasingCurve::OutBack);
}

IconMinItem::~IconMinItem()
{

}

void IconMinItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

//    painter.drawPixmap(-1 * width() / 2, -1 * height() / 2,
//                       width() * 2, height() * 2, _pixmap.scaled(width() * 2, height() * 2,
//                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    painter.drawPixmap(0 , 0, _width, _height, _pixmap.scaled(_width, _height,
                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));


    QWidget::paintEvent(event);

}

void IconMinItem::setId(int id)
{
    _id = id;
}

void IconMinItem::setPage(int page)
{
    _page = page;
//    if (_saveDataBool)
//        _app->setPage(page);
}

void IconMinItem::setText(const QString &text)
{
    _text = text;
}

void IconMinItem::setIndex(int index)
{
    _index= index;
//    if (_saveDataBool)
//        _app->setIndex(index);
}
void IconMinItem::setHidden(bool hide)
{
//    _app->setHidden(hide);
}

void IconMinItem::setType(int type)
{
    _type = type;
}

void IconMinItem::setUrl(const QString &url)
{
    _url = url;
}

void IconMinItem::setIsRmote(bool isRemote)
{
    _isRemote = isRemote;
}

void IconMinItem::setPixmap(const QString &icon)
{
    _pixText = icon;
    _pixmap.load(icon);

}

void IconMinItem::setSLargeSize()
{
    _width = SMALL_L_SIZE.width();
    _height = SMALL_L_SIZE.height();

    setFixedSize(_width, _height);

    repaint();
}

void IconMinItem::setSMediumSize()
{
    _width = SMALL_M_SIZE.width();
    _height = SMALL_M_SIZE.height();

    setFixedSize(_width, _height);

    repaint();
}

void IconMinItem::setSSmallSize()
{
    _width = SMALL_S_SIZE.width();
    _height = SMALL_S_SIZE.height();

    setFixedSize(_width, _height);

    repaint();
}

void IconMinItem::animationMove(const QRect &start, const QRect &end)
{
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _animation->start();
}

DirMinWidget::DirMinWidget(QWidget *parent)
    : QWidget(parent)
    , _col(2)
    , _row(2)
    , _iconsPerPage(4)
    , _count(1)
    , _current(0)
    , _iconNum(0)
    , _dragEnable(true)
//    , _pageSize(_width, _height)
{
    setAcceptDrops(true);

    changeSize();
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
                    + (j % _col) * (_gridWidth + _spacing);
            int y = (j / _col) * (_gridHeight + _spacing);

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
    switch(ICON_TYPE)
    {
        case 0 :

            painter.drawPixmap(0, 0, ICONLSIZE.width(), ICONLSIZE.height()
                               , QPixmap(":/images/dir_add_icon.png").scaled(ICONLSIZE.width(), ICONLSIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

            break;

        case 1 :

            painter.drawPixmap(0, 0, ICONMSIZE.width(), ICONMSIZE.height()
                               , QPixmap(":/images/dir_add_icon.png").scaled(ICONMSIZE.width(), ICONMSIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            break;

        default:

            painter.drawPixmap(0, 0, ICONSSIZE.width(), ICONSSIZE.height()
                               , QPixmap(":/images/dir_add_icon.png").scaled(ICONSSIZE.width(), ICONSSIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            break;
    }

}

void DirMinWidget::dragEnterEvent(QDragEnterEvent *event)
{

    if (event->mimeData()->hasFormat("image/x-iconitem")) {
//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//           event->acceptProposedAction();
//        }

//        IconDesktopItem *icon = qobject_cast<IconDesktopItem*> (event->source());
//        if (icon && icon->type() != VirtualDesktop::dirIcon) {
                event->setDropAction(Qt::MoveAction);
                event->accept();
//       }

        emit iconEnter();
    } else {

        event->ignore();
    }
//        qDebug() << "DirMinWidget***************************************************dragEnter";

}

void DirMinWidget::dragMoveEvent(QDragMoveEvent *event)
{
//        qDebug() << "DirMinWidget*******dragMoveEvent******************";

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
        int page;
        int index;
        int type;
        dataStream >> nameText >> pixText >> page >> index >> urlText >> type;

//         pixText.replace(".png", ".ico");
//        qDebug() << pixText;

//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }

//    IconDesktopItem *icon = qobject_cast<IconDesktopItem*> (event->source());
//    if (icon && icon->type() != VirtualDesktop::dirIcon) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
//    }

    addIcon(nameText, pixText, -1, -1, urlText);
    emit iconDrop(nameText ,pixText, page, index, urlText, type);
    } else {
        event->ignore();
    }
}

void DirMinWidget::expand()
{
    _count++;
    changeSize();
    setFixedSize(_count * _width, _height);
    _pages.append(-(_count-1) * _width);
    QList<QRect> newGridList;
//    _gridWidth = ICONWIDTH + _spacing * 2;
//    _gridHeight = ICONHEIGHT + _spacing * 2;
    for (int i = 0; i < _col * _row; i++) {
        //vertical display
//        int x =  _pageSize.width() * (_count - 1)\
//                + (i / _row) * gridWidth;
//        int y = (i % _row) * gridHeight;

            //horizontal display
            int x =  _pageSize.width() * (_count - 1)\
                    + (i % _col) * (_gridWidth + _spacing);
            int y = (i / _col) * (_gridHeight + _spacing);
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
                icon->setGeometry(_gridTable[i][j].translated(_spacing / 2, 1));
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
    switch(ICON_TYPE)
    {
        case 0 :
            icon->setSLargeSize();
            break;

        case 1 :
            icon->setSMediumSize();
            break;

        default:
            icon->setSSmallSize();
            break;
    }

//    icon->setTimeLine(false);
//    icon->setPropertyAnimation(true);
//    icon->setRunAction(false);
//    icon->setDelAction(false);
//    icon->setClosePixBool(false);
//    icon->setSelectPixBool(false);
//    icon->setDoubleClickBool(false);
//    icon->setContextMenuBool(false);
//    icon->setTrembleBool(false);
//    icon->setDragEventBool(false);
//    icon->setEnterEventBool(false);
//    icon->setLeaveEventBool(false);
//    icon->setEqualIcon(false);

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
    icon->setText(text);
    icon->setPixmap(iconPath);

//    if (IconMinItem::large_size == ICON_TYPE)
//    {
//            icon->setSLargeSize();
//            icon->setGeometry(_gridTable[page][index].translated(-_spacing / 2, 0));
//    }
//    else if (IconMinItem::medium_size == ICON_TYPE)
//    {
//            icon->setSMediumSize();
//            icon->setGeometry(_gridTable[page][index].translated(-_spacing / 2, 0));
//    }
//    else
//    {
//            icon->setSSmallSize();
//            icon->setGeometry(_gridTable[page][index].translated(-_spacing / 2, 0));
//    }
    icon->setGeometry(_gridTable[page][index].translated(_spacing / 2, 1));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(url);
    _iconDict.insert(text, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

//    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));
//    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));

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

    moveBackIcons(p, s);

//    _iconNum--;

//    for(int i = p; i < _count; i++)
//    {
//        if (i == p)
//        {
//            int k;
//            for (k = s + 1; k < _nextIdx[p]; k++) {
//                QRect start = _gridTable[p][k].translated(_spacing, _spacing);
//                QRect end = _gridTable[p][k-1].translated(_spacing, _spacing);
//                _iconTable[p][k]->animationMove(start, end);
//                _iconTable[p][k]->setIndex(k-1);
//                _iconTable[p][k - 1] = _iconTable[p][k];
//            }
//        }
//        else {
//            for (int j = 0; j < _nextIdx[i]; j++)
//            {
//                if (j == 0)
//                {

//                    QRect start = _gridTable[i][j].translated(_spacing, _spacing);
//                    QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(_spacing, _spacing);
//                    _iconTable[i][j]->animationMove(start, end);
//                    _iconTable[i][j]->setPage(i-1);
//                    _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
//                    _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
//                }
//                else {

//                    QRect start = _gridTable[i][j].translated(_spacing, _spacing);
//                    QRect end = _gridTable[i][j-1].translated(_spacing, _spacing);
//                    _iconTable[i][j]->animationMove(start, end);
//                    _iconTable[i][j]->setIndex(j-1);
//                    _iconTable[i][j - 1] = _iconTable[i][j];
//                }
//            }
//        }
//    }

//    if (_nextIdx[_count - 1] == 0)
//    {
//        _iconTable[_count - 1 - 1][_nextIdx[_count - 1 - 1] - 1] = NULL;
//        _nextIdx[_count - 1 - 1]--;
//    }else
//    {
//        _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
//        _nextIdx[_count - 1]--;
//    }

//    if ((_count > 1) && (_iconNum == _iconsPerPage * (_count - 1)))
//        delPage(_count - 1);


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

void DirMinWidget::removeDirMinItem(const QString &text)
{
    //moveBackIcons(_iconDict.value(text)->page(), _iconDict.value(text)->index());
    delIcon(text);
}

void DirMinWidget::moveBackIcons(int page, int index)
{
    int p = page;
    int s = index;

    for(int i = p; i < _count; i++)
    {
       if (i == p)
       {
           int k;
           for (k = s + 1; k < _nextIdx[p]; k++) {
               QRect start = _gridTable[p][k].translated(_spacing / 2, 1);
               QRect end = _gridTable[p][k-1].translated(_spacing / 2, 1);
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

                   QRect start = _gridTable[i][j].translated(_spacing / 2, 1);
                   QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(_spacing / 2, 1);
                   _iconTable[i][j]->animationMove(start, end);
                   _iconTable[i][j]->setPage(i-1);
                   _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
                   _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
               }
               else {

                   QRect start = _gridTable[i][j].translated(_spacing / 2, 1);
                   QRect end = _gridTable[i][j-1].translated(_spacing / 2, 1);
                   _iconTable[i][j]->animationMove(start, end);
                   _iconTable[i][j]->setIndex(j-1);
                   _iconTable[i][j - 1] = _iconTable[i][j];
               }
           }
       }
    }

    _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
    _nextIdx[_count - 1]--;

    if (_nextIdx[_count - 1] == 0)
        delPage(_count - 1);

    _iconNum--;
}

void DirMinWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void DirMinWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
        emit mouseClicked();
}

void DirMinWidget::changeSize()
{
    switch(ICON_TYPE)
    {
        case 0 :

            _width = ICONLSIZE.width();
            _height = ICONLSIZE.height();
            _pageSize = QSize(_width, _height);

            _gridWidth = 21;
            _gridHeight = 21;

            _spacing = 5;

            break;

        case 1 :

            _width = ICONMSIZE.width();
            _height = ICONMSIZE.height();
            _pageSize = QSize(_width, _height);

            _gridWidth = 18;
            _gridHeight = 18;

            _spacing = 4;

            break;

        default:

            _width = ICONSSIZE.width();
            _height = ICONSSIZE.height();
            _pageSize = QSize(_width, _height);

            _gridWidth = 15;
            _gridHeight = 15;

            _spacing = 4;

            break;
    }
}

DirLineEdit::DirLineEdit(QString hint, QWidget *parent)
    : QLineEdit(parent), _hint(hint), _color(Qt::white)
    , _mousePress(false)
{
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:0px; color:white;");
    setAlignment(Qt::AlignCenter);
    setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));
//    resize(95, 15);

//    _editLeft = new QImage(":/images/dir_edit_left.png");
//    _editCenter = new QImage(":/images/dir_edit_center.png");
//    _editRight = new QImage(":/images/dir_edit_right.png");


//    setImgs(_editLeft, _editCenter, _editRight);

//    _pixmap.load(":images/login_input.png");

//    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(setHint(const QString&)));

}

void DirLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
    QPainter painter(this);
    painter.setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));

//    *_left = _left->scaled(11, 23);
//    painter.drawImage(QRect(0, 0, _left->width(), _left->height()), *_left);
//    *_right = _right->scaled(10, 23);
//    painter.drawImage(QRect(_width - _right->width(), 0, _right->width(), _right->height()), *_right);
//    *_center = _center->scaled(width() - 11 - 10, 23);
//    painter.drawImage(QRect(_left->width(), 0, _width - _left->width() - _right->width(), _center->height()), *_center);
    if (hasFocus())
        return;
    if (!text().isEmpty())
    {
        return;
    }

    QRect rt = rect();
    painter.drawText(rt.translated(0,1), Qt::AlignCenter | Qt::TextHideMnemonic, _hint);

}

void DirLineEdit::mousePressEvent(QMouseEvent *event)
{
//    emit focusIn();
//    _mousePress = true;
//            setEnabled(true);
            if (!hasFocus())
                setFocus();
//            emit focusIn();

    QLineEdit::mousePressEvent(event);
}

void DirLineEdit::focusInEvent(QFocusEvent *event)
{
//    if (!_mousePress)
//        clearFocus();
//    else
        emit focusIn();

    QLineEdit::focusInEvent(event);
}

void DirLineEdit::focusOutEvent(QFocusEvent *event)
{
    emit focusOut();
//    setEnabled(false);
//    _mousePress = false;

    QLineEdit::focusOutEvent(event);
}

void DirLineEdit::resizeEvent(QResizeEvent *event)
{
    _width = width();
    _height = height();

    QLineEdit::resizeEvent(event);
}

//void DirLineEdit::setImgs(QImage *strLeft, QImage *strCenter, QImage *strRight)
//{
//    _left = strLeft;
//    _center = strCenter;
//    _right = strRight;

//    repaint();
//}

DirMWidget::DirMWidget(QWidget *parent)
    : QWidget(parent)
{
//    setAcceptDrops(true);

    switch(ICON_TYPE)
    {
    case 0 :

        _width = ICONLSIZE.width();
        _height = ICONLSIZE.height();

        break;

    case 1 :

        _width = ICONMSIZE.width();
        _height = ICONMSIZE.height();

        break;

    default:

        _width = ICONSSIZE.width();
        _height = ICONSSIZE.height();

        break;
    }
    setFixedSize(_width, _height);

    _dirMinWidget = new DirMinWidget(this);
    _dirMinWidget->move(0, 0);
    _dirMinWidget->setVisible(true);

//    connect(_dirMinWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
    connect(_dirMinWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dirMinWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
    connect(_dirMinWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
    connect(_dirMinWidget, SIGNAL(iconDrop(const QString&, const QString&, int,
                                  int, const QString&, int)),
            this, SIGNAL(iconDrop(const QString&, const QString&, int,
                         int, const QString&, int)));
    connect(_dirMinWidget, SIGNAL(mouseClicked()), this, SIGNAL(mouseClicked()));

}

DirMWidget::~DirMWidget()
{

}

void DirMWidget::setMinDragEnable(bool dragEnable)
{
    if (dragEnable)
    {
        _dirMinWidget->setAcceptDrops(true);
        _dirMinWidget->setDragEnable(true);
    }
    else
    {
        _dirMinWidget->setAcceptDrops(false);
        _dirMinWidget->setDragEnable(false);
    }
}

void DirMWidget::removeDirMinItem(const QString &text)
{
    _dirMinWidget->removeDirMinItem(text);
}

void DirMWidget::addDirMinItem(const QString &text, const QString &icon, \
                   int page, int index, const QString &url)
{
    _dirMinWidget->addIcon(text, icon, page, index, url);
}

DirMinShowWidget::DirMinShowWidget(QWidget *parent)
    : QWidget(parent)
{

    setAcceptDrops(true);

    setFocusPolicy(Qt::ClickFocus);

    _dirMWidget = new DirMWidget(this);

    switch(ICON_TYPE)
    {
        case 0 :

            _width = LARGESIZE.width();
            _height = LARGESIZE.height();

            _iconWidth = ICONLSIZE.width();
            _iconHeight = ICONLSIZE.height();

            _dirMWidget->move(35, 36);
            break;

        case 1 :

            _width = MEDIUMSIZE.width();
            _height = MEDIUMSIZE.height();

            _iconWidth = ICONMSIZE.width();
            _iconHeight = ICONMSIZE.height();

            _dirMWidget->move(35 - 24 + 15, 35 - 24 + 15);
            break;

        default:

            _width = SMALLSIZE.width();
            _height = SMALLSIZE.height();

            _iconWidth = ICONSSIZE.width();
            _iconHeight = ICONSSIZE.height();

            _dirMWidget->move(35 - 32 + 22, 35 - 32 + 22);
            break;
    }

    setFixedSize(_width, _height);

//    _dirMWidget = new DirMWidget(this);
//    _dirMWidget->move(35, 36);          //((SELECTWIDTH / 2  + 8) / 2, 0)
    _dirMWidget->setVisible(true);

    _dirLineEdit = new DirLineEdit(QString(tr("ÊäÈëÎÄ¼þ¼ÐÃû³Æ")), this);
    _dirLineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));
    _dirLineEdit->setGeometry((_width - (72 + 37 / 2 + 8)) / 2, _height - 26, 72 + 37 / 2 + 8, 23);

//    _dirLineEdit->setTextMargins(3, 5, 2, 5);

//    _editLeft = new QImage(":/images/dir_edit_left.png");
//    _editCenter = new QImage(":/images/dir_edit_center.png");
//    _editRight = new QImage(":/images/dir_edit_right.png");

//    _editLeftNormal = new QImage("");
//    _editCenterNormal = new QImage("");
//    _editRightNormal = new QImage("");

    _editLeft = setTransparentPixmap(":/images/dir_edit_left.png");
    _editCenter = setTransparentPixmap(":/images/dir_edit_center.png");
    _editRight = setTransparentPixmap(":/images/dir_edit_right.png");
    _editLeftNormal.load("");
    _editCenterNormal.load("");
    _editRightNormal.load("");

    setImgs(_editLeftNormal, _editCenterNormal, _editRightNormal);

//    connect(_dirMWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
    connect(_dirMWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dirMWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
    connect(_dirMWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
//    connect(_dirMinWidget, SIGNAL(iconDrop(const QString&, const QString& ,const QString&)),
//            this, SLOT(iconDropEvent(const QString&, const QString& ,const QString&)));
//    connect(_dirMinWidget, SIGNAL(mouseClicked()), this, SLOT(mouseClicked()));
    connect(_dirMWidget, SIGNAL(iconDrop(const QString&, const QString&, int,
                                         int, const QString&, int)),
            this, SIGNAL(iconDrop(const QString&, const QString&, int,
                                  int, const QString&, int)));
    connect(_dirMWidget, SIGNAL(mouseClicked()), this, SIGNAL(openItem()));
    connect(_dirLineEdit, SIGNAL(focusIn()), this, SLOT(editFocusIn()));
    connect(_dirLineEdit, SIGNAL(focusOut()), this, SLOT(editFocusOut()));
}

DirMinShowWidget::~DirMinShowWidget()
{

}

void DirMinShowWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);

    switch(ICON_TYPE)
    {
        case 0 :

            painter.drawPixmap(0, 0, LARGESIZE.width(), LARGESIZE.height()
                               , QPixmap(":/images/icon_shadow.png").scaled(LARGESIZE.width(), LARGESIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

            break;

        case 1 :

            painter.drawPixmap(0, 0, MEDIUMSIZE.width(), MEDIUMSIZE.height()
                               , QPixmap(":/images/icon_shadow.png").scaled(MEDIUMSIZE.width(), MEDIUMSIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            break;

        default:

            painter.drawPixmap(0, 0, SMALLSIZE.width(), SMALLSIZE.height()
                               , QPixmap(":/images/icon_shadow.png").scaled(SMALLSIZE.width(), SMALLSIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            break;
    }
    //_width - (72 + 37 / 2 + 8)) / 2, _height - 26, 72 + 37 / 2 + 8, 23
//    painter.drawPixmap(0, 0, QPixmap(":/images/icon_shadow.png"));

    painter.drawPixmap((_width - (72 + 37 / 2 + 8)) / 2, _height - 26, \
                      _left.width(), _left.height(),
                      _left);
    painter.drawPixmap((_width + (72 + 37 / 2 + 8)) / 2 - _right.width(), _height - 26, \
                      _right.width(), _right.height(),
                      _right);
    painter.drawPixmap((_width - (72 + 37 / 2 + 8)) / 2 + _left.width(), _height - 26, \
                      (72 + 37 / 2 + 8) - _left.width() - _right.width(), _center.height(),
                      _center);
}

void DirMinShowWidget::mouseClicked()
{
//    emit openItem(_id, _page, _index);
    //emit openItem();
}

void DirMinShowWidget::iconDropEvent(const QString &text, const QString &iconPath, const QString &url)
{
    Q_UNUSED(text);
    Q_UNUSED(iconPath);
    Q_UNUSED(url);
    //emit iconDrop(_id, text, iconPath, url);
}

void DirMinShowWidget::editFocusIn()
{
    setImgs(_editLeft, _editCenter, _editRight);
}

void DirMinShowWidget::editFocusOut()
{
    QSqlQuery query(QSqlDatabase::database("local"));
    QString qstr = QString("update localapps set name=\'%1\' where id=\'%2\'").arg(_dirLineEdit->text()).arg(_id);
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return;
    }

    setImgs(_editLeftNormal, _editCenterNormal, _editRightNormal);
}

//void DirMinShowWidget::setImgs(QImage *strLeft, QImage *strCenter, QImage *strRight)
//{
//    _left = strLeft;
//    _center = strCenter;
//    _right = strRight;

//    repaint();
//}

void DirMinShowWidget::setImgs(QPixmap strLeft, QPixmap strCenter, QPixmap strRight)
{
    _left = strLeft;
    _center = strCenter;
    _right = strRight;

    repaint();
}

QPixmap DirMinShowWidget::setTransparentPixmap(const QString &pix)
{
    QImage normal = QImage(pix);

    for (int i = 0; i < normal.width(); i++) {
        for (int j = 0; j < normal.height(); j++) {
            QRgb pixel = normal.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel) * 1, qGreen(pixel) * 1, \
                                    qBlue(pixel) * 1, a * 100 / 255);
            normal.setPixel(i, j, lightPixel);
        }
    }
    return QPixmap::fromImage(normal);
}

void DirMinShowWidget::dragEnterEvent(QDragEnterEvent *event)
{
    Q_UNUSED(event);

    emit dragEnterMinWidget();
}

void DirMinShowWidget::setMinDragEnable(bool dragEnable)
{
    if (dragEnable)
    {
        _dirMWidget->setMinDragEnable(true);
        _dirMWidget->setAcceptDrops(false);
    }
    else
    {
        _dirMWidget->setMinDragEnable(false);
        _dirMWidget->setAcceptDrops(false);
    }

}

void DirMinShowWidget::removeDirMinItem(const QString &text)
{
    _dirMWidget->removeDirMinItem(text);
}

void DirMinShowWidget::addDirMinItem(const QString &text, const QString &icon, \
                   int page, int index, const QString &url)
{
    _dirMWidget->addDirMinItem(text, icon, page, index, url);
}

const QString & DirMinShowWidget::getDirText()
{
    _dirLineEdit->text();
}
