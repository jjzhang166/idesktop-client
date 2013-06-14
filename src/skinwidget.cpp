#include <QPalette>
#include <QPen>
#include <QBrush>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "skinwidget.h"

#define INDICATOR_ITEMSIZE QSize(14, 14)
#define ICONWIDTH 96
#define ICONHEIGHT 96
#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30
#define FONTSIZE 10
#define APPICON   0
#define ADDICON 1
#define SPACING 30
#define ICONWIDTH 76
#define ICONHEIGHT 30
#define ICONNUM 30

SkinWidget::SkinWidget(QWidget *parent)
    : QWidget(parent)
{
    //resize(20, 20);

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    _pixWidget = new PixWidget(QSize(700, 484), this);
    _pixWidget->move(0,0);
    _pixWidget->setVisible(true);

    _scrollBar = new QScrollBar(this);

    _scrollBar->setStyleSheet("QScrollBar:vertical{width:8px;border: 0px solid gray;background:rgba(255,255,255,0);}\
                    QScrollBar::handle:vertical{ min-width: 8px;min-height: 249px; background-image:url(:/images/bs_scrollbar.png);\
                    background-position: left; background-repeat:none; border-style:flat;}\
                    QScrollBar::handle:vertical::disabled{background:#232329;}\
                    QScrollBar::handle:vertical:hover{background-image:url(:/images/bs_scrollbar.png);}\
                    QScrollBar::add-line:vertical{background-color: rgba(255,255,255,0);\
                                                  subcontrol-position: bottom; subcontrol-origin: margin;}\
                    QScrollBar::sub-line:vertical{background-color: rgba(255,255,255,0);\
                                                  subcontrol-position: top; subcontrol-origin: margin;}\
                    QScrollBar::add-page:vertical{background-color: rgba(255,255,255,0);}\
                    QScrollBar::sub-page:vertical{background-color: rgba(255,255,255,0);}\
                    QScrollBar::up-arrow:vertical{background-color: rgba(255,255,255,0);}\
                    QScrollBar::down-arrow:vertical{background-color: rgba(255,255,255,0);}");


    if (_pixWidget->count() > 1)
        _scrollBar->setVisible(true);

    _scrollBar->setSingleStep(_pixWidget->pageSize().height());
    _scrollBar->setPageStep(_pixWidget->pageSize().height());

    _animation = new QPropertyAnimation(_pixWidget, "geometry");

//    _rightTopPix.load(":/images/bs_rightbg_top.png");
//    _rightCenterPix.load(":/images/bs_rightbg_center.png");
//    _rightBottomPix.load(":/images/bs_rightbg_bottom.png");

    _bgPix.load(":/images/skin_bg.png");

    connect(_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
    connect(_pixWidget, SIGNAL(setBgPixmap(QString)), this, SIGNAL(setBgPixmap(QString)));
}

void SkinWidget::scrollBarValueChanged(int val)
{
    if (val % _pixWidget->pageSize().height() != 0)
    {
        return;
    }

    if (_animation->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animation->setDuration(80);
    _animation->setStartValue(QRect(0, _pixWidget->pos().y(), _pixWidget->pageSize().width(), _pixWidget->pageSize().height()));
    _animation->setEndValue(QRect(0, -1 * val, _pixWidget->pageSize().width(), _pixWidget->pageSize().height()));

    _animation->start();
}

void SkinWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    _width = width();
    _height = height();

    int w = 15;
    int x = width() - 28;
    x = x < 0 ? 0: x;
    int h = height();
    _scrollBar->setGeometry(x, 10, w, h - 20);

    _scrollBar->setRange(0, _pixWidget->count() * _pixWidget->pageSize().height() - h);
    update();
}

void SkinWidget::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
    _scrollBar->event(event);
}

void SkinWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    painter.drawPixmap(0, 0, _width, 9,\
//                       _rightTopPix.scaled(_width, 9));
//    painter.drawPixmap(0, 9, _width, _height - 18,\
//                       _rightCenterPix.scaled(_width, _height - 18));
//    painter.drawPixmap(0, _height - 9, _width, 9,\
//                       _rightBottomPix.scaled(_width, 9));
    painter.drawPixmap(0, 0, _width, _height, _bgPix.scaled(_width, _height));

//    painter.setPen(QPen(QBrush(Qt::white), 1, Qt::DashLine));
//    painter.drawLine(15, 40, _width - 25, 40);

    QWidget::paintEvent(event);
}

//
PixItem::PixItem(QWidget *parent)
    : QWidget(parent)
    , _selectPix(false)
{
    _color = QColor(Qt::white);
}

PixItem::~PixItem()
{

}

void PixItem::setPage(int page)
{
    _page = page;
//    _app->setPage(page);
}
void PixItem::setIndex(int index)
{
    _index= index;
//   _app->setIndex(index);
}

void PixItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.setPen(QPen(QBrush(_color), 3, Qt::SolidLine));
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

   painter.drawPixmap(2, 2, width() - 4, height() - 4, _pixmap.scaled(width() - 4, height() - 4));

    QWidget::paintEvent(event);
}

void PixItem::setPixmap(const QString &icon)
{
    _pixText = icon;
    _pixmap.load(_pixText);
}

void PixItem::mousePressEvent(QMouseEvent *event)
{

    emit mouseClicked(_pixText);

    if (!_selectPix)
    {
        _selectPix = true;
    }



    event->ignore();
}

void PixItem::mouseMoveEvent(QMouseEvent *event)
{

}

void PixItem::enterEvent(QEvent *event)
{

    Q_UNUSED(event);
}

void PixItem::leaveEvent(QEvent *event)
{

    Q_UNUSED(event);
}

const QPixmap & PixItem::pixmap()
{
    return _pixmap;
}

void PixItem::setPenColor(bool yellow)
{
    if (yellow)
    {
        _color = QColor(Qt::yellow);
    }
    else
    {
        _color = QColor(Qt::white);
    }
    update();
}

//
PixWidget::PixWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _localCount(0)
{

//    _count = 1;
    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = _pageSize.height();

    gridWidth = ICONWIDTH + SPACING * 2;
    gridHeight = ICONHEIGHT + SPACING * 2;
    _col = _width / (gridWidth + SPACING);
    _row = _height / (gridHeight + SPACING + 21);
    _iconsPerPage = _col * _row;
    _current  = 0;


    for (int i = 0; i < ICONNUM; i++)
    {
        _count = i / _iconsPerPage + 1;
    }

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _height));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {

            int x = (j % _col) * (gridWidth + SPACING);
            int y = _pageSize.height() * i \
                    + (j / _col) * (gridHeight + SPACING + 21) + 21;

            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);

    }

    for (int i = 0; i < _count; i++) {
        QList<PixItem*> newList;
        for (int j = 0; j < _col * _row; j++)
            newList.insert(j, NULL);
        _iconTable.insert(i, newList);
    }

    for (int i = 0; i < _count; i++)
        _nextIdx.insert(i, 0);

    setAcceptDrops(true);
    setAutoFillBackground(true);

    setFixedSize(_width, _height * _count);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);

    for (int i = 0; i < ICONNUM; i++) {
        addIcon( QString(":/images/wallpager/wp_%1.jpg").arg(i),
                -1, -1);
    }
}

PixWidget::~PixWidget()
{

}

int PixWidget::addIcon(const QString &iconPath, \
                       int page,int index)
{

    PixItem *icon = new PixItem(this);

    if (page == -1) {
        for (int i = 0; i < _count; i++) {
            if (_nextIdx[i] < _row * _col) {
                page = i;
                index = _nextIdx[i];
                break;
            }
        }
    } else {
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

    QSqlQuery query = QSqlDatabase::database("local").exec("select wallpaper from wallpapers where id=1;");
    query.next();
    _iconPath = query.value(0).toString();
    if (_iconPath == iconPath)
    {
        icon->setPenColor(true);
    }
    else
    {
        icon->setPenColor(false);
    }

    icon->setPixmap(iconPath);
    icon->setGeometry(_gridTable[page][index].translated(SPACING, SPACING));
    icon->setPage(page);
    icon->setIndex(index);
    _iconDict.insert("", icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

    connect(icon, SIGNAL(mouseClicked(const QString&)), this, SLOT(itemClicked(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));


     return page;
    //LocalAppList::getList()->save();
}

void PixWidget::itemClicked(const QString &pixText)
{

    QSqlQuery query = QSqlDatabase::database("local").exec("select wallpaper from wallpapers where id=1;");
    query.next();

    for (int i = 0; i < _count - 1; i++)
    {
        for (int j = 0; j < _nextIdx[i]; j++)
        {
            if (_iconTable[i][j]->getBgPix() == query.value(0).toString())
            {
                _iconTable[i][j]->setPenColor(false);
            }
        }
    }

    emit setBgPixmap(pixText);
}
