#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QPainter>
#include <cassert>
#include <QTimer>
#include <QToolTip>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
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

class IconItem;
extern int ICON_TYPE;
extern QList<TEMP_LIST> dirWidget_FirstLists;

IconMinItem::IconMinItem(QWidget *parent)
    : QWidget(parent)
    , _page(0)
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

//    switch(ICON_TYPE)
//    {
//        case 0 :
//            painter.drawPixmap(_width - 36 - 26, 36, 10, 10
//                               , _iconClassPixmap.scaled(10, 10
//                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//            break;

//        case 1 :

//            painter.drawPixmap(_width - 29 - 27, 29 + 1,10, 10
//                               , _iconClassPixmap.scaled(10, 10
//                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//            break;

//        default:

//            painter.drawPixmap(_width - 25 - 27, 25 + 1, 10, 10
//                               ,_iconClassPixmap.scaled(10, 10
//                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//            break;
//    }


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

void IconMinItem::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;
}

//void IconMinItem::setIconClass(int iconClass)
//{
//    if (iconClass == 1)
//    {
//        _iconClassPixmap.load(":/images/app_normal.png");
//    }
//    else if (iconClass == 2)
//    {
//        _iconClassPixmap.load(":/images/paas_normal.png");
//    }
//    else
//    {
//        _iconClassPixmap.load("");
//    }

//    update();
//}

DirMinWidget::DirMinWidget(int type, QWidget *parent)
    : QWidget(parent)
    , _col(2)
    , _row(2)
    , _iconsPerPage(4)
    , _count(1)
    , _current(0)
    , _iconNum(0)
    , _dragEnable(true)
    , _type(type)
//    , _isMouseMove(false)
//    , _pageSize(_width, _height)
{
//    setAcceptDrops(true);

    changeSize();

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

    setAutoFillBackground(true);
    setFixedSize(_width * _count, _height);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255,0));
    setPalette(pal);

}

DirMinWidget::~DirMinWidget()
{

}

void DirMinWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    switch(_type)   //ICON_TYPE
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

int DirMinWidget::addIcon(const QString &text,
                          const QString &iconPath,
                          int page,
                          int index,
                          const QString &url,
                          const QString &uniqueName)
{
    if (_iconDict.value(uniqueName))
        return -3;

    qDebug() << " dirMinWidget---->addIcon text" << text;
//    qDebug() <<"DirMinWidget::addIcon:: iconNum"<<_iconNum;
    if (_iconNum == 4)
        return -3;

    int expandPageCount = _iconNum / _iconsPerPage + 1;
    if (expandPageCount > _count)
        expand();

    IconMinItem *icon = new IconMinItem(this);

    switch(_type)   //ICON_TYPE
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
//                        move(_pages[page], y());
                        break;
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < _count; i++) {
                if (_nextIdx[i] < _row * _col) {
                    page = i;
                    index = _nextIdx[i];
//                    move(_pages[page], y());
                    break;
                }
            }
        }
    }
    icon->setText(text);
    icon->setPixmap(iconPath);
//    icon->setIconClass(type);

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
    icon->setUniqueName(uniqueName);
    _iconDict.insert(uniqueName, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

//    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));
//    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));

    ++_iconNum;
//    qDebug() <<"DirMinWidget::addIcon:: iconNum"<<_iconNum;

     return page;
}

void DirMinWidget::delIcon(const QString &uniqueName)
{
    if (!_iconDict.value(uniqueName))
        return;

    IconMinItem *icon = _iconDict.take(uniqueName);
    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();
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

void DirMinWidget::removeDirMinItem(const QString &uniqueName)
{
    //moveBackIcons(_iconDict.value(text)->page(), _iconDict.value(text)->index());

    if (!_iconDict.value(uniqueName))
    {
        qDebug() << uniqueName << " isn't exist!!!";
        return;
    }

    delIcon(uniqueName);
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

void DirMinWidget::changeSize()
{
    switch(_type)   //ICON_TYPE
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

void DirMinWidget::setDragEnable(bool dragEnable)
{
    _dragEnable = dragEnable;
}

int DirMinWidget::getIconNum()
{
    return _iconNum;
}

void DirMinWidget::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;
}

void DirMinWidget::refresh()
{
    qDebug() << "1111111111111111111111111111111111111111111111111111111";

    for (int i = 0; i < 1; i++)
    {
        for(int j = 4;j > 0; j--)
        {
            if(_iconTable[i][j - 1] != NULL)
            {
                qDebug() << "_iconTable[i][j]->uniqueName()" << _iconTable[i][j - 1]->uniqueName();
                delIcon(_iconTable[i][j - 1]->uniqueName());
            }
        }
    }

    update();

    for (int i = 0; i < dirWidget_FirstLists.count(); i++)
    {
        qDebug() << "dirWidget_FirstLists.at(i).name" <<dirWidget_FirstLists.at(i).name
                 <<dirWidget_FirstLists.count();
        addIcon(dirWidget_FirstLists.at(i).name
                , dirWidget_FirstLists.at(i).iconPath
                , -1, -1
                , dirWidget_FirstLists.at(i).url
                , dirWidget_FirstLists.at(i).uniqueName);
    }

}

DirLineEdit::DirLineEdit(QString hint, QWidget *parent)
    : QLineEdit(parent), _hint(hint), _color(Qt::white)
    , _mousePress(false)
{
    setContextMenuPolicy(Qt::NoContextMenu);

    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:0px; color:white;" \
                  "selection-background-color: lightGray;");
    setAlignment(Qt::AlignCenter);
    setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));
//    resize(95, 15);

//    _editLeft = new QImage(":/images/dir_edit_left.png");
//    _editCenter = new QImage(":/images/dir_edit_center.png");
//    _editRight = new QImage(":/images/dir_edit_right.png");


//    setImgs(_editLeft, _editCenter, _editRight);

//    _pixmap.load(":images/login_input.png");

//    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(setHint(const QString&)));
//    qApp->setStyleSheet(" QToolTip { background-color:lightGray; border: 2px;padding: 1px; border-radius: 3px; color: white;}");
}

void DirLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
    QPainter painter(this);
    if (hasFocus())
        return;
    if (!text().isEmpty())
    {
        return;
    }

    QRect rt = rect();
    painter.drawText(rt.translated(0,1), Qt::AlignCenter | Qt::TextHideMnemonic, _hint);

}
void DirLineEdit::enterEvent(QEvent *event)
{
//    if(this->text().length() != _fullName.length())
//       QToolTip::showText( QCursor::pos(), _fullName);
    QWidget::enterEvent(event);
}

void DirLineEdit::mousePressEvent(QMouseEvent *event)
{
    if (!hasFocus())
        setFocus();

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

DirMWidget::DirMWidget(int type, QWidget *parent)
    : QWidget(parent)
    , _type(type)
{
//    setAcceptDrops(true);

    switch(_type)   //ICON_TYPE
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

    _dirMinWidget = new DirMinWidget(_type, this);
    _dirMinWidget->move(0, 0);
    _dirMinWidget->setVisible(true);

//    connect(_dirMinWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
    connect(_dirMinWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dirMinWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
    connect(_dirMinWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
    connect(_dirMinWidget, SIGNAL(iconDrop(const QString&, const QString&, int,
                                  int, const QString&, int, const QString &)),
            this, SIGNAL(iconDrop(const QString&, const QString&, int,
                         int, const QString&, int, const QString &)));
    connect(_dirMinWidget, SIGNAL(mouseClicked()), this, SIGNAL(mouseClicked()));

}

DirMWidget::~DirMWidget()
{

}

void DirMWidget::setMinDragEnable(bool dragEnable)
{
//    if (dragEnable)
//    {
//        _dirMinWidget->setAcceptDrops(true);
//        _dirMinWidget->setDragEnable(true);
//    }
//    else
//    {
//        _dirMinWidget->setAcceptDrops(false);
//        _dirMinWidget->setDragEnable(false);
//    }
}

void DirMWidget::removeDirMinItem(const QString &uniqueName)
{
    _dirMinWidget->removeDirMinItem(uniqueName);
}

void DirMWidget::addDirMinItem(const QString &text, const QString &icon, \
                   int page, int index, const QString &url, const QString &uniqueName)
{
    _dirMinWidget->addIcon(text, icon, page, index, url, uniqueName);
}

int DirMWidget::getMinIconNum()
{
    return _dirMinWidget->getIconNum();
}

void DirMWidget::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;
    _dirMinWidget->setUniqueName(uniqueName);
}
void DirMWidget::refresh()
{
    _dirMinWidget->refresh();
}

DirMinShowWidget::DirMinShowWidget(int type, QWidget *parent)
    : QWidget(parent)
    , _isMouseMove(false)
    , _isMousePress(false)
    , _skipMouseMove(false)
    , _type(type)
{

    setAcceptDrops(true);

    setFocusPolicy(Qt::ClickFocus);

    _dirMWidget = new DirMWidget(_type, this);

    switch(_type)       //ICON_TYPE
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

    _dirMWidget->setVisible(true);
    _dirLineEdit = new DirLineEdit(QString(tr("ÊäÈëÎÄ¼þ¼ÐÃû³Æ")), this);
    _dirLineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(_dirLineEdit);
        effect->setBlurRadius(8);
        effect->setColor(QColor(0, 0, 0));
        effect->setOffset(-1,1);
        _dirLineEdit->setGraphicsEffect(effect);
    _dirLineEdit->setGeometry((_width - (72 + 37 / 2 + 8)) / 2, _height - 26, 72 + 37 / 2 + 8, 23);

//    _dirLineEdit->setTextMargins(3, 5, 2, 5);

    _editLeftNormal =  QPixmap("");
    _editCenterNormal =  QPixmap("");
    _editRightNormal =  QPixmap("");

    _local = LocalAppList::getList();

    setImgs(_editLeftNormal, _editCenterNormal, _editRightNormal);

//    connect(_dirMWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
    connect(_dirMWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dirMWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
    connect(_dirMWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
//    connect(_dirMinWidget, SIGNAL(iconDrop(const QString&, const QString& ,const QString&)),
//            this, SLOT(iconDropEvent(const QString&, const QString& ,const QString&)));
//    connect(_dirMinWidget, SIGNAL(mouseClicked()), this, SLOT(mouseClicked()));
    connect(_dirMWidget, SIGNAL(iconDrop(const QString&, const QString&, int,
                                         int, const QString&, int, const QString&)),
            this, SIGNAL(iconDrop(const QString&, const QString&, int,
                                  int, const QString&, int, const QString&)));
    connect(_dirMWidget, SIGNAL(mouseClicked()), this, SIGNAL(openItem()));
    connect(_dirLineEdit, SIGNAL(focusIn()), this, SLOT(editFocusIn()));
    connect(_dirLineEdit, SIGNAL(focusOut()), this, SLOT(editFocusOut()));
    connect(_dirLineEdit, SIGNAL(textChanged(QString)), this ,SIGNAL(lineEditValueChange(QString)));
}

DirMinShowWidget::~DirMinShowWidget()
{

}
void DirMinShowWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    _height = height();
    _width = width();
    switch(_type)   //ICON_TYPE
    {
    case 0:
        _dirLineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));
        _dirLineEdit->setGeometry((_width - (72 + 37 / 2 + 8)) / 2, _height - 26, 72 + 37 / 2 + 8, 23);
        _editLeft = setTransparentPixmap(":/images/dir_edit_left.png").scaled(11,23);
        _editCenter = setTransparentPixmap(":/images/dir_edit_center.png").scaled(11,23);
        _editRight = setTransparentPixmap(":/images/dir_edit_right.png").scaled(11,23);
        break;
    case 1:
        _dirLineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE - 1, QFont::Normal));
        _dirLineEdit->setGeometry((_width - (60 + 30 / 2 + 22)) / 2, _height - 23, 60 + 30 / 2 + 22, 19 );
        _editLeft = setTransparentPixmap(":/images/dir_edit_left.png").scaled(9,19);
        _editCenter = setTransparentPixmap(":/images/dir_edit_center.png").scaled(9,19);
        _editRight = setTransparentPixmap(":/images/dir_edit_right.png").scaled(9,19);
        break;
    case 2:
        _dirLineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE - 2, QFont::Normal));
        _dirLineEdit->setGeometry((_width - (51 + 26 / 2 + 16)) / 2, _height - 18, 51 + 26 / 2 + 16, 16 );
        _editLeft = setTransparentPixmap(":/images/dir_edit_left.png").scaled(8,16);
        _editCenter = setTransparentPixmap(":/images/dir_edit_center.png").scaled(8,16);
        _editRight = setTransparentPixmap(":/images/dir_edit_right.png").scaled(8,16);
        break;
    }
}
void DirMinShowWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);

    switch(_type)   //ICON_TYPE
    {
        case 0 :

            painter.drawPixmap(0, 0, LARGESIZE.width(), LARGESIZE.height()
                               , QPixmap(":/images/icon_shadow.png").scaled(LARGESIZE.width(), LARGESIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.drawPixmap((_width - (72 + 37 / 2 + 8)) / 2, _height - 26, \
                              _left.width(), _left.height(),
                              _left);
            painter.drawPixmap((_width + (72 + 37 / 2 + 8)) / 2 - _right.width(), _height - 26, \
                              _right.width(), _right.height(),
                              _right);
            painter.drawPixmap((_width - (72 + 37 / 2 + 8)) / 2 + _left.width(), _height - 26, \
                              (72 + 37 / 2 + 8) - _left.width() - _right.width(), _center.height(),
                              _center);

            break;

        case 1 :

            painter.drawPixmap(0, 0, MEDIUMSIZE.width(), MEDIUMSIZE.height()
                               , QPixmap(":/images/icon_shadow.png").scaled(MEDIUMSIZE.width(), MEDIUMSIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.drawPixmap((_width - (60 + 30 / 2 + 16)) / 2, _height - 23, \
                              _left.width(), _left.height() ,
                              _left);
            painter.drawPixmap((_width + (60 + 30 / 2 + 16)) / 2 - _right.width(), _height - 23, \
                              _right.width(), _right.height(),
                              _right);
            painter.drawPixmap((_width - (60 + 30 / 2 + 16)) / 2 + _left.width(), _height - 23, \
                              (60 + 30 / 2 + 16) - _left.width() - _right.width(), _center.height(),
                              _center);
            break;

        default:

            painter.drawPixmap(0, 0, SMALLSIZE.width(), SMALLSIZE.height()
                               , QPixmap(":/images/icon_shadow.png").scaled(SMALLSIZE.width(), SMALLSIZE.height()
                               , Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.drawPixmap((_width - (51 + 26 / 2 + 16)) / 2, _height - 18, \
                              _left.width(), _left.height(),
                              _left);
            painter.drawPixmap((_width + (51 + 26 / 2 + 16)) / 2 - _right.width(), _height - 18,\
                                      _right.width(), _right.height(),
                                      _right);
            painter.drawPixmap((_width - (51 + 26 / 2 + 16)) / 2 + _left.width(), _height - 18, \
                              (51 + 26 / 2 + 16) - _left.width() - _right.width(), _center.height(),
                              _center);
            break;
    }
    //_width - (72 + 37 / 2 + 8)) / 2, _height - 26, 72 + 37 / 2 + 8, 23
//    painter.drawPixmap(0, 0, QPixmap(":/images/icon_shadow.png"));
}

void DirMinShowWidget::mouseClicked()
{
//    emit openItem(_id, _page, _index);
    //emit openItem();
}

void DirMinShowWidget::iconDropEvent(const QString &text, const QString &iconPath, const QString &url, const QString &uniqueName)
{
    Q_UNUSED(text);
    Q_UNUSED(iconPath);
    Q_UNUSED(url);
    Q_UNUSED(uniqueName);
    //emit iconDrop(_id, text, iconPath, url);
}

void DirMinShowWidget::editFocusIn()
{
    setImgs(_editLeft, _editCenter, _editRight);
}

void DirMinShowWidget::editFocusOut()
{
//    QSqlQuery query(QSqlDatabase::database("local"));
//    QString qstr = QString("update localapps set name=\'%1\' where uniquename=\'%2\'").arg(_dirLineEdit->text()).arg(_uniqueName);
//    if(!query.exec(qstr)) {
//        qDebug() <<"query failed";
//        return;
//    }
    LocalApp * _app = LocalAppList::getList()->getAppByUniqueName(_uniqueName);

    if (!_dirLineEdit->text().isEmpty())
    {
        _app->setName(_dirLineEdit->text());
        emit dirMinLineEditFocusOut(_id, _dirLineEdit->text());
    }
    else
    {
        _app->setName(tr("ÐÂ½¨ÎÄ¼þ¼Ð"));
        emit dirMinLineEditFocusOut(_id, tr("ÐÂ½¨ÎÄ¼þ¼Ð"));
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

void DirMinShowWidget::setMinDragEnable(bool dragEnable)
{
//    if (dragEnable)
//    {
//        _dirMWidget->setMinDragEnable(true);
//        _dirMWidget->setAcceptDrops(false);
//    }
//    else
//    {
//        _dirMWidget->setMinDragEnable(false);
//        _dirMWidget->setAcceptDrops(false);
//    }

}

void DirMinShowWidget::removeDirMinItem(const QString &uniqueName)
{
    _dirMWidget->removeDirMinItem(uniqueName);
}

void DirMinShowWidget::addDirMinItem(const QString &text, const QString &icon, \
                   int page, int index, const QString &url, const QString &uniqueName)
{
    _dirMWidget->addDirMinItem(text, icon, page, index, url, uniqueName);
}

//void DirMinShowWidget::dragEnterEvent(QDragEnterEvent *event)
//{
//    if (event->source() == this)
//        return;

//    emit dragEnterMinWidget();
//}

void DirMinShowWidget::dragEnterEvent(QDragEnterEvent *event)
{

    void *source = (DirMinShowWidget*)(event->source());
    void *dst = this->parent();
    if(source == dst)
    {
        event->ignore();
        return;
    }

    if ((event->source() != this) && (event->mimeData()->hasFormat("image/x-iconitem"))) {
//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//           event->acceptProposedAction();
//        }
        emit iconEnter();
//        IconDesktopItem *icon = qobject_cast<IconDesktopItem*> (event->source());
//        if (icon && icon->type() != VirtualDesktop::dirIcon) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
//       }
//        emit dragEnterMinWidget();
} else {

    event->ignore();
}
//        qDebug() << "DirMinWidget******************DirMinShowWidget********************************dragEnter";

}

void DirMinShowWidget::dragMoveEvent(QDragMoveEvent *event)
{
//        qDebug() << "DirMinWidget*******dragMoveEvent**********DirMinShowWidget********";

    event->setDropAction(Qt::MoveAction);
    event->accept();


    emit iconMove();
}

void DirMinShowWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit iconLeave();
}

void DirMinShowWidget::dropEvent(QDropEvent *event)
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
        int id;
        QString uniqueName;
        dataStream >> nameText >> pixText >> page >> index >> urlText >> type >> id >> uniqueName;
        qDebug() << "id"<< id;
        qDebug() << "_id" << _id;

        if (id == _id)
            event->ignore();


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
    if (_dirMWidget->getMinIconNum() < 4)
        _dirMWidget->addDirMinItem(nameText, pixText, -1, -1, urlText, uniqueName);

    emit iconDrop(nameText ,pixText, page, index, urlText, type, uniqueName);
    } else {
        event->ignore();
    }
}

const QString & DirMinShowWidget::getDirText()
{
    _dirLineEdit->text();
}

int DirMinShowWidget::getMinIconNum()
{
    return _dirMWidget->getMinIconNum();
}

void DirMinShowWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*
    if (_isMousePress)
        return;
 */
   if(_skipMouseMove)
        return;

    qDebug() << "void DirMinWidget::mouseMoveEvent(QMouseEvent *event)";
    _isMouseMove = true;

    event->ignore();
}

void DirMinShowWidget::mousePressEvent(QMouseEvent *event)
{
//    QWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        _isMousePress = true;
        _skipMouseMove=false;
        QTimer::singleShot(300, this, SLOT(mouseStatus()));
    }

}

void DirMinShowWidget::mouseStatus()
{
//    _isMousePress = true;

    if (_isMouseMove)
    {
        _isMouseMove = false;
        return;
    }
    else
    {
        emit openItem();
        _skipMouseMove = true;
    }
}

void DirMinShowWidget::mouseReleaseEvent(QMouseEvent *)
{
    _isMousePress = false;
    _skipMouseMove=false;
}

void DirMinShowWidget::setId(int id)
{
    _id = id;

//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;

//        if (_local->at(i)->dirId() == _id)
//        {

//            addDirMinItem(_local->at(i)->name(), _local->at(i)->icon(),
//                          _local->at(i)->page(), _local->at(i)->index(),
//                          _local->at(i)->url(), _local->at(i)->uniqueName());


//        }
//    }
}

void DirMinShowWidget::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;
    _dirMWidget->setUniqueName(uniqueName);
}

void DirMinShowWidget::setText(const QString text)
{
    _dirLineEdit->setText(text);
}

void DirMinShowWidget::refresh()
{
    _dirMWidget->refresh();
}
