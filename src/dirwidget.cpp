#include <QAction>
#include <QPainter>
#include <QApplication>

#include <QtDebug>

#include <QPalette>
#include <QSettings>
#include <QApplication>
#include <QMenu>
//#include <QDir>
#include <QFileInfoList>
#include <QLibrary>
#include <windows.h>
#include <stdio.h>

#include <QtDebug>
#include <QDrag>

#include "DirWidget.h"
#include "config.h"
#include "dirwidget.h"

//#define KEY "\\Windows\\CurrentVersion\\App Paths\\"
#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"

//#define CLOSEWIDTH 30
//#define CLOSEHEIGHT 30
//#define FONTSIZE 10
//#define APPICON   0
//#define ADDICON 1
//#define SPACING 30
//#define ICONWIDTH 96
//#define ICONHEIGHT 96
//#define ICONNUM 3

#define ICONWIDTH 143
#define ICONHEIGHT 143
#define SELECTWIDTH 37
#define SELECTHEIGHT 37

#define SPACING 30
#define FONTSIZE 10

#define ICONITEMWIDTH 143
#define ICONITEMHEIGHT 143

#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30

#define APPICON 0
#define HSPACING 4
#define VSPACING 0
#define BOTTOMSPACING 14

#define LARGESIZE QSize(143, 143)     //72*72
#define MEDIUMSIZE QSize(119, 119)     //48*48
#define SMALLSIZE QSize(103, 103)   //32*32

#define ICONHSPACING (88 - 40)
#define ICONVSPACING (36 - 36)

extern int ICON_TYPE;

static QPoint gap;

//******/////
//
DirShowWidget::DirShowWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _id(0)
{
//    _rightTopPix.load(":/images/bs_rightbg_top.png");
//    _rightCenterPix.load(":/images/bs_rightbg_center.png");
//    _rightBottomPix.load(":/images/bs_rightbg_bottom.png");

    _width = pageSize.width();

    _dirWidget = new DirWidget(pageSize, this);
    _dirWidget->setGeometry(0,0, _width - 5, _height);
//    _dirWidget->setMaxPage(_maxRow);
    _dirWidget->setVisible(true);

    _height = _dirWidget->height();
//    _bgHeight = (ICONITEMWIDTH + VSPACING * 2) * _maxRow;

    resize(_width, _height);

//    _scrollBar = new QScrollBar(this);

//    _scrollBar->setStyleSheet("QScrollBar:vertical{width:8px;border: 0px solid gray;background:rgba(255,255,255,0);}\
//                    QScrollBar::handle:vertical{ min-width: 8px;min-height: 249px; background-image:url(:/images/bs_scrollbar.png);\
//                    background-position: left; background-repeat:none; border-style:flat;}\
//                    QScrollBar::handle:vertical::disabled{background:#232329;}\
//                    QScrollBar::handle:vertical:hover{background-image:url(:/images/bs_scrollbar.png);}\
//                    QScrollBar::add-line:vertical{background-color: rgba(255,255,255,0);\
//                                                  subcontrol-position: bottom; subcontrol-origin: margin;}\
//                    QScrollBar::sub-line:vertical{background-color: rgba(255,255,255,0);\
//                                                  subcontrol-position: top; subcontrol-origin: margin;}\
//                    QScrollBar::add-page:vertical{background-color: rgba(255,255,255,0);}\
//                    QScrollBar::sub-page:vertical{background-color: rgba(255,255,255,0);}\
//                    QScrollBar::up-arrow:vertical{background-color: rgba(255,255,255,0);}\
//                    QScrollBar::down-arrow:vertical{background-color: rgba(255,255,255,0);}");


//    _scrollBar->setSingleStep(_dirWidget->pageSize().height());
//    _scrollBar->setPageStep(_dirWidget->pageSize().height());

//    if (_dirWidget->count() > 1)
//        _scrollBar->setVisible(true);
//    else
//        _scrollBar->setVisible(false);

//    _animation = new QPropertyAnimation(_dirWidget, "geometry");

//    connect(_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
 //   connect(_dirWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
    connect(_dirWidget, SIGNAL(pageIncreased()), this, SLOT(setSize()));
    connect(_dirWidget, SIGNAL(pageDecreased()), this, SLOT(setSize()));
}

DirShowWidget::~DirShowWidget()
{

}

void DirShowWidget::removeIcon(const QString &text)
{
    _dirWidget->removeIcon(text);
}

//void DirShowWidget::scrollBarValueChanged(int val)
//{
//    if (val % _dirWidget->pageSize().height() != 0)
//    {
//        return;
//    }

//    if (_animation->state() == QAbstractAnimation::Running)
//    {
//        return;
//    }

//    _animation->setDuration(80);
//    _animation->setStartValue(QRect(0, _dirWidget->pos().y(), _dirWidget->pageSize().width(), _dirWidget->pageSize().height()));
//    _animation->setEndValue(QRect(0, -1 * val, _dirWidget->pageSize().width(), _dirWidget->pageSize().height()));

//    _animation->start();
//}

void DirShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    painter.drawPixmap(0, 0, _width, 9,
//                       _rightTopPix.scaled(_width, 9));
//    painter.drawPixmap(0, 9, _width, _height - 18,
//                       _rightCenterPix.scaled(_width, _height - 18));
//    painter.drawPixmap(0, _height - 9, _width, 9,
//                       _rightBottomPix.scaled(_width, 9));

    painter.drawPixmap(0, 0, _width, _height, QPixmap(":images/dir_bg.png").scaled(_width, _height, \
                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QWidget::paintEvent(event);
}

void DirShowWidget::mousePressEvent(QMouseEvent *)
{

}

//void DirShowWidget::resizeEvent(QResizeEvent *event)
//{
//    Q_UNUSED(event);
//    _width = width();
//    _height = height();

//    if (_dirWidget->count() > 1)
//        _scrollBar->setVisible(true);
//    else
//        _scrollBar->setVisible(false);

//    int w = 15;
//    int x = width() - w;
//    x = x < 0 ? 0: x;
//    int h = height();
//    _scrollBar->setGeometry(x, 8, w, h - 16);

//    _scrollBar->setRange(0, _dirWidget->count() * _dirWidget->pageSize().height() - h);

//    update();
//}

void DirShowWidget::setSize()
{
    _width = _dirWidget->width();
    _height = _dirWidget->height();

    setFixedSize(_width, _height);

    _dirWidget->setGeometry(0,0, _width, _height);

    update();
}

//void DirShowWidget::wheelEvent(QWheelEvent *event)
//{
//    Q_UNUSED(event);
//    _scrollBar->event(event);
//}

void DirShowWidget::showApp(bool localApp)
{
    if (localApp)
        _dirWidget->showApp(true);
    else
        _dirWidget->showApp(false);
}

void DirShowWidget::addDirIcon(const QString &text, const QString &iconPath, const QString &url)
{
    _dirWidget->setUrl(url);
    _dirWidget->addIcon(text, iconPath, -1, -1);
}

void DirShowWidget::setMaxRow(int row)
{
    _dirWidget->setMaxPage(row - 1);
}

int DirShowWidget::getRow()
{
    return _height / _dirWidget->getIconHeight();
}

void DirShowWidget::largeIcon()
{
    _dirWidget->largeIcon();
}

void DirShowWidget::mediumIcon()
{
    _dirWidget->mediumIcon();
}

void DirShowWidget::smallIcon()
{
    _dirWidget->smallIcon();
}

int DirShowWidget::iconCount()
{
    _iconCount = _dirWidget->iconCount();
    return _iconCount;
}
int DirShowWidget::currentPage()
{
    _currentPage = _dirWidget->currentPage();
    return _currentPage;
}
int DirShowWidget::count()
{
    _count = _dirWidget->count();
    return _count;
}

void DirShowWidget::setId(int id)
{
    _id = id;
    _dirWidget->setId(_id);
}

//
//DirItem::DirItem(const QString &text, QWidget *parent)
//    : QWidget(parent)
//    , _text(text)
//    , _trembling(0)
//    ,_equal(false)
//{
//    QFontMetrics fm(QFont("", FONTSIZE, QFont::Black));

//    _width = ICONITEMWIDTH;
//    _height = ICONITEMHEIGHT;
//    setFixedSize(_width, _height);

//    _textWidth = fm.width(_text);

//    if (_text.startsWith("/")) {
//        _texticon = _text.right(_text.length() - 1);
//        _textWidth = fm.width(_text.right(_text.length() - 1));
//    }
//    else
//        _texticon = _text;

//    if (_textWidth > _width)
//    {
//        for (int i = 0; i < _textWidth; i++)
//        {
//            _textWidth_firstrow = fm.width(_texticon);
//            if (_textWidth_firstrow > _width)
//            {
//                _texticon.chop(1);
//            }else{
//                _texticon.chop(3);
//                _texticon.append("...");
//                break;
//            }
//        }
//    }

//    _textHeight = fm.height();

////    _texticon = _text;
////    _texticon_firstrow = _texticon;
////    _texticon_secondrow = _texticon;
////    _texticon_thirdrow = _texticon;

////    for (int i = 0; i < _textWidth; i++)
////    {
////        _textWidth_firstrow = fm.width(_texticon_firstrow);

////        if (_textWidth_firstrow > width - 20)
////        {
////            _texticon_firstrow.chop(1);
////        }
////        else{
////            //_texticon.chop(1);
////            //_texticon.append("...");
////            break;
////        }
////    }
////    _texticon_secondrow = _texticon.right(_texticon.length() - _texticon_firstrow.length());

////    _textWidth_secondrow = fm.width(_texticon_secondrow);
////    for (i = 0; i < _textWidth_secondrow; i++)
////    {
////        _textWidth_secondrow = fm.width(_texticon_secondrow);

////        if (_textWidth_secondrow > width - 20)
////        {
////            _texticon_secondrow.chop(1);
////        }
////        else{
////            //_texticon.chop(1);
////            //_texticon.append("...");
////            break;
////        }
////    }
////    _texticon_thirdrow = _texticon.right(_texticon.length() - _texticon_firstrow.length() - _texticon_secondrow.length());

////    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
////    if (_textWidth_thirdrow > width)
////    {
////        for (i = 0; i < _textWidth_thirdrow; i++)
////        {
////            _textWidth_thirdrow = fm.width(_texticon_thirdrow);
////            if (_textWidth_thirdrow > width - 20)
////            {
////                _texticon_thirdrow.chop(1);
////            }else{
////                _texticon_thirdrow.chop(3);
////                _texticon_thirdrow.append("...");
////                break;
////            }
////        }
////    }

////    _textHeight = fm.height();

////    _app = LocalAppList::getList()->getAppByName(_text);
//    _animation = new QPropertyAnimation(this, "geometry");
//    _animation->setDuration(200);
//    _animation->setEasingCurve(QEasingCurve::OutBack);

//    _timeline = new QTimeLine(5000, this);
//    _timeline->setLoopCount(0);
//    _timeline->setUpdateInterval(65);

//    _icontype = APPICON;

////    _hoverDirItem = new HoverIconItem(this->width(), this->height(), this);
////    _hoverDirItem->setVisible(false);

////    _openAction = new QAction(tr("ÔËÐÐ"), this);
////    connect(_openAction, SIGNAL(triggered()), this, SLOT(openClicked()));

////    if (_style == VirtualDesktop::DirIcon)
////    {
////        _delAction = new QAction(tr("É¾³ý"), this);
////        connect(_delAction, SIGNAL(triggered()), this, SLOT(delClicked()));
////    }

//    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(doTremble(qreal)));
////    if (parent) {
////        if (_style == VirtualDesktop::DirIcon)
////        {
////            connect(static_cast<DirWidget*>(parent), \
////                    SIGNAL(trembleStarted()), \
////                    this, SLOT(startTremble()));
////            connect(static_cast<DirWidget*>(parent), \
////                    SIGNAL(trembleStoped()), \
////                    this, SLOT(stopTremble()));
////        }
////    }
//}

//DirItem::~DirItem()
//{
////    delete _openAction;
////    delete _hoverDirItem;

////    delete _timeline;
////    delete _animation;
//}

//void DirItem::setPage(int page)
//{
//    _page = page;
////    _app->setPage(page);
//}
//void DirItem::setIndex(int index)
//{
//    _index= index;
////   _app->setIndex(index);
//}
//void DirItem::setHidden(bool hide)
//{
////    _app->setHidden(hide);
//}

//void DirItem::animationMove(const QRect &start, const QRect &end)
//{
//    _animation->setStartValue(start);
//    _animation->setEndValue(end);
//    _animation->start();
//}

//void DirItem::doTremble(qreal value)
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

////void DirItem::startTremble()
////{
////    _timeline->start();
////    _pixmap = _closePixmap;
////}

////void DirItem::stopTremble()
////{
////    _timeline->stop();
////    _trembling = 0;
////    _pixmap = _normalPixmap;
////    repaint();
////}

//void DirItem::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);
////    if (_trembling)
////        painter.translate(width()/2, height()/2);
////    painter.rotate(2*_trembling);
////    if (_trembling)
////        painter.translate(-1*width()/2, -1*height()/2);

//    painter.drawPixmap(3, 0, _pixmap);

////    painter.drawPixmap(width() - 50, height() - 80, _closePixmap);

//    QWidget::paintEvent(event);
//}

//void DirItem::mousePressEvent(QMouseEvent *event)
//{
////    if (_timeline->state() == QTimeLine::Running) {
////        if (QRect(0, 5, 30, 35).contains(event->pos())) {
////            delClicked();
////            return;
////        }
////    }

////    if (event->button() == Qt::LeftButton && \
////            geometry().contains(event->pos())) {
////        dragStartPosition = event->pos();
////    }
////    emit sendUrl(_url);
////    event->ignore();

//}

//void DirItem::mouseDoubleClickEvent(QMouseEvent *event)
//{
////    if ((event->button() == Qt::LeftButton))
////    {
////        emit sendUrl(_url);
////    }
//    if (!_url.isEmpty())
//        emit sendUrl(_url);
//}

//void DirItem::mouseMoveEvent(QMouseEvent *event)
//{
////    if (!(event->buttons() & Qt::LeftButton))
////        return;
////    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
////        return;
////    }
////    _drag = new QDrag(this);
////    QMimeData *mimeData = new QMimeData;
////    mimeData->setText(_text);
////    _drag->setMimeData(mimeData);
////    _drag->setPixmap(grayPixmap());
////    _drag->setHotSpot(event->pos());
////    gap = _drag->hotSpot();
////    _drag->exec(Qt::MoveAction);
//}

////void DirItem::contextMenuEvent(QContextMenuEvent *event)
////{
////    Q_UNUSED(event);


////    QCursor cur=this->cursor();
////    QMenu *iconItemcontextMenu = new QMenu(this);

////    iconItemcontextMenu->addAction(_openAction);

//////    if (_style == VirtualDesktop::DirIcon)
//////    {
////        iconItemcontextMenu->addAction(_delAction);
//////    }

////    iconItemcontextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);

////    iconItemcontextMenu->exec(cur.pos());

////    return;
////}


//void DirItem::enterEvent(QEvent *event)
//{
//    _pixmap = darkPixmap();
////    _hoverIconItem->setVisible(true);
//    repaint();

//    Q_UNUSED(event);
//}

//void DirItem::leaveEvent(QEvent *event)
//{

////    if (_timeline->state() == QTimeLine::Running)
////        _pixmap = _closePixmap;
////    else
//        _pixmap = _normalPixmap;

////    _hoverIconItem->setVisible(false);
//    repaint();

//    Q_UNUSED(event);
//}

//const QPixmap& DirItem::originPixmap()
//{
//    return _originPixmap;
//}

//void DirItem::setPixmap(const QString &icon)
//{
////    int begin;
//    QString text = _text;
//    if (isUserType())
//        text = _text.right(_text.length() - 1);
//    _originPixmap = QPixmap(icon);
////    QImage image = QImage(icon).scaled(ICONWIDTH, ICONHEIGHT);
//    QImage image = QImage(icon);
//    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);
//    QPainter pt1(&normal);
//    pt1.setPen(Qt::white);
//    //pt1.setFont(QFont("", FONTSIZE, QFont::Black));
//    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
//    pt1.setCompositionMode(QPainter::CompositionMode_Source);
//    pt1.fillRect(normal.rect(), Qt::transparent);
////    pt1.drawImage((SELECTWIDTH / 2  + 8) / 2, 0, image);
//    pt1.drawImage(0, 0, image);

//    //QFont font("", FONTSIZE, QFont::Normal);
////    QFont font(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal);
////    QFontMetrics fm(font);
////    _textWidth_firstrow = fm.width(_texticon_firstrow);
////    QString tx = _texticon_firstrow;
////    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_firstrow) / 2, height() - _textHeight * 3 - 8, _textWidth_firstrow, _textHeight), Qt::TextSingleLine, tx);
////    _textWidth_secondrow = fm.width(_texticon_secondrow);
////    tx = _texticon_secondrow;
////    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_secondrow) / 2, height() - _textHeight * 2 - 8, _textWidth_secondrow, _textHeight), Qt::TextSingleLine, tx);
////    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
////    tx = _texticon_thirdrow;
////    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_thirdrow) / 2, height() - _textHeight - 8, _textWidth_thirdrow, _textHeight), Qt::TextSingleLine, tx);
////    pt1.end();
//    QFont font("", FONTSIZE, QFont::Normal);
////    QFont font(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal);

//    QFontMetrics fm(font);
//    _textWidth_firstrow = fm.width(_texticon);
////    QString tx = _texticon;
//    pt1.drawText( QRect((_width - _textWidth_firstrow) / 2,  ICONHEIGHT - FONTSIZE * 2,\
//                        _textWidth_firstrow, _textHeight), Qt::TextSingleLine, _texticon);
//    pt1.end();

//    QImage light = QImage(width(), height(), QImage::Format_ARGB32);
//    QImage dark =  QImage(width(), height(), QImage::Format_ARGB32);

//    for (int i = 0; i < width(); i++) {
//        for (int j = 0; j < height(); j++) {
//            QRgb pixel = normal.pixel(i,j);
//            int a = qAlpha(pixel);
//            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
//                                    qBlue(pixel), a/2);
//            light.setPixel(i, j, lightPixel);


//            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
//                                   qBlue(pixel)*0.8, a);
//            dark.setPixel(i, j, darkPixel);
//        }
//    }
//    QPainter pt2(&dark);
//    pt2.setPen(Qt::white);
//    pt2.setFont(QFont("", FONTSIZE, QFont::Black));
//    pt2.setRenderHint(QPainter::HighQualityAntialiasing);

//    pt2.end();
//    _grayPixmap = QPixmap::fromImage(light).scaled(width() * 1.0, height() * 1.0);
//    _darkPixmap = QPixmap::fromImage(dark);
//    _normalPixmap = QPixmap::fromImage(normal);
//    _pixmap = _normalPixmap;
//    QImage resultImage(width(), height(),  \
//                       QImage::Format_ARGB32_Premultiplied);

//    QPainter painter(&resultImage);
//    painter.setCompositionMode(QPainter::CompositionMode_Source);
//    painter.fillRect(resultImage.rect(), Qt::transparent);
//    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
//    painter.drawImage(0, 0, normal);

////    QImage closeImage(":images/close_icon.png");
////    painter.drawImage(width() - 20, height() -50, closeImage);

//    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
//    painter.end();
//    //_closePixmap = QPixmap::fromImage(resultImage);

//}

//void DirItem::setText(const QString &text)
//{
//    _text = text;
//}

//bool DirItem::isUserType()
//{
//    if (_text.startsWith("/"))
//        return true;
//    return false;
//}

//const QString & DirItem::text()
//{
//    return _text;
//}

//const QPixmap & DirItem::pixmap()
//{
//    return _pixmap;
//}

//const QPixmap & DirItem::grayPixmap()
//{
//    return _grayPixmap;
//}

//const QPixmap & DirItem::darkPixmap()
//{
////    if (_timeline->state() == QTimeLine::Running)
////        return _closePixmap;
//    return _darkPixmap;
//}

//void DirItem::openClicked()
//{
//    emit runItem(_text);
//}

//void DirItem::delClicked()
//{
//    emit delItem(_text);
//}

//void DirItem::setEqualIcon(bool equal)
//{
//    if (equal)
//        _closePixmap.load(":/images/close_icon.png");
//    else
//        _closePixmap.load("");

//    update();
//}

//
DirWidget::DirWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _localCount(0)
    , _iconNum(0)
    , _url("")
    , _iconSize(ICON_TYPE)
    , _dragEvent(false)
{
    _count = 1;
    _pageSize = pageSize;
    _width = _pageSize.width();
//    _height = ICONITEMWIDTH + VSPACING + BOTTOMSPACING;

    changeSpacing();
//    gridWidth = ICONITEMWIDTH + HSPACING * 2;
//    gridHeight = ICONITEMWIDTH + VSPACING + BOTTOMSPACING;

    _col = (_width - ICONHSPACING) / gridWidth;
    _row = (_height - ICONVSPACING) / gridHeight;

    _iconsPerPage = _col * _row;
    _current  = 0;

//    _local = LocalAppList::getList();

    ////test   start  Classes //URLInfoAbout //InstallLocation //DisplayIcon
//    QSettings reg(QSettings::NativeFormat, \
//            QSettings::SystemScope, "Microsoft", KEY);
//    for (int i = 0; i < reg.childGroups().count(); i++) {
//        QSettings tmp(QSettings::NativeFormat, \
//            QSettings::SystemScope, "Microsoft", \
//            KEY + reg.childGroups().at(i));

////        addIcon(tmp.value("").toString(), addLocalApp(tmp.value("").toString()), -1, -1);



//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;

//        if(!_local->at(i)->isRemote())
//        {
//            _localCount++;
//        }
//    }


//    for (int i = 0; i < ICONNUM; i++)
//    {
//        _count = i / _iconsPerPage + 1;
//    }

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _height));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {

            int x = (j % _col) * gridWidth  + ICONHSPACING;
            int y = _height * i \
                    + (j / _col) * gridHeight + ICONVSPACING;

            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);

    }

    for (int i = 0; i < _count; i++) {
        QList<IconItem*> newList;
        for (int j = 0; j < _col * _row; j++)
            newList.insert(j, NULL);
        _iconTable.insert(i, newList);
    }

    for (int i = 0; i < _count; i++)
        _nextIdx.insert(i, 0);

    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setEasingCurve(QEasingCurve::OutBack);

    setAcceptDrops(true);
    setAutoFillBackground(true);

    setFixedSize(_width, _height * _count);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);

//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;

//        if(!_local->at(i)->isRemote())
//        {
//            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                    _local->at(i)->page(), -1);
//        }
//    }

//    for (int i = 0; i < reg.childGroups().count(); i++) {
//        QSettings tmp(QSettings::NativeFormat, \
//            QSettings::SystemScope, "Microsoft", \
//            KEY + reg.childGroups().at(i));

////        addIcon(tmp.value("").toString(), addLocalApp(tmp.value("").toString()), -1, -1);
//    }

//    for (int i = 0; i < ICONNUM; i++) {
//        addIcon("", QString(":images/custom/z_%1.png").arg(i + 15),
//                -1, i);
//    }
}

DirWidget::~DirWidget()
{

}

int DirWidget::addIcon(QString text,
                       const QString &iconPath,
                       int page,
                       int index)
{

    int expandPageCount = _iconNum / _iconsPerPage + 1;

    if (expandPageCount > _count)
        expand();

    IconItem *icon = new IconItem(this);
    switch(_iconSize)
    {
        case IconItem::large_size :
            icon->setLargeSize();
            break;

        case IconItem::medium_size :
            icon->setMediumSize();
            break;

        default:
            icon->setSmallSize();
            break;
    }

    icon->setText(text);
    icon->setTimeLine(false);
    icon->setPropertyAnimation(true);
    icon->setRunAction(true);
    icon->setDelAction(true);
    icon->setClosePixBool(false);
    icon->setSelectPixBool(false);
    icon->setEqualIcon(false);
    icon->setDoubleClickBool(true);
    icon->setContextMenuBool(true);
    icon->setTrembleBool(false);
    icon->setDragEventBool(true);
    icon->setEnterEventBool(false);
    icon->setLeaveEventBool(false);
    icon->setSaveDataBool(true);    //  true


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

//    for (int i = 0; i < _local->count(); i++)
//    {
//        if (_local->at(i)->name() == text)
//        {
//            icon->setEqualIcon(true);
//        }
//        else
//            icon->setEqualIcon(false);
//    }
//    qDebug() << _nextIdx[page] << page;
    icon->setPixmap(iconPath);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(_url);
    icon->setDirId(_id);
    icon->setId(_id);
    _iconDict.insert(text, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

    _iconNum ++;
//    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
//   connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(delIcon(const QString&)));

    _url = QString("");
     return page;
    //LocalAppList::getList()->save();
}

void DirWidget::expand()
{
//    qDebug() << "asdlkfjalksjdflkajsdflkjal;sdjf;lkajsdfljasldkfjalk;sjdflasdfas";
    _count++;
    setFixedSize(_pageSize.width(), _count * _height);
    _pages.append(-(_count-1) * _height);
    QList<QRect> newGridList;
//    _gridWidth = ICONWIDTH + SPACING * 2;
//    _gridHeight = ICONHEIGHT + SPACING * 2;

//    gridWidth = ICONITEMWIDTH + HSPACING * 2;
//    gridHeight = ICONITEMWIDTH + VSPACING + BOTTOMSPACING;
    changeSpacing();

    for (int i = 0; i < _col * _row; i++) {
        //vertical display
//        int x =  _pageSize.width() * (_count - 1)\
//                + (i / _row) * gridWidth;
//        int y = (i % _row) * gridHeight;

            //horizontal display
            int x =  (i % _col) * gridWidth + ICONHSPACING;
            int y = _height * (_count - 1) \
                    + (i / _col) * gridHeight + ICONVSPACING;
        newGridList.insert(i, \
                           QRect(x, y, gridWidth, gridHeight));
    }
    _gridTable.append(newGridList);
    QList<IconItem*> newIconList;
    for (int i = 0; i < _col * _row; i++)
        newIconList.insert(i, NULL);
    _iconTable.append(newIconList);
    _nextIdx.append(0);
    emit pageIncreased();
}

void DirWidget::delPage(int page)
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
            IconItem *icon = _iconTable[i][j];
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

void DirWidget::delIcon(const QString &text)
{
    qDebug() << text;
    IconItem *icon = _iconDict.take(text);
    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();
    //icon->setPage(-1);
    //icon->setIndex(-1);
    delete _iconTable[p][s];

    moveBackIcons(p, s);
}


void DirWidget::removeIcon(const QString &text)
{
    moveBackIcons(_iconDict.value(text)->page(), _iconDict.value(text)->index());

    _iconDict.take(text);
}

void DirWidget::moveBackIcons(int page, int index)
{

    int p = page;
    int s = index;

    for(int i = p; i < _count; i++)
    {
        if (i == p)
        {
            int k;
            for (k = s + 1; k < _nextIdx[p]; k++) {
                QRect start = _gridTable[p][k].translated(HSPACING, VSPACING);
                QRect end = _gridTable[p][k-1].translated(HSPACING, VSPACING);
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
                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
                    QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(HSPACING, VSPACING);
                    _iconTable[i][j]->animationMove(start, end);
                    _iconTable[i][j]->setPage(i-1);
                    _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
                    _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
                }
                else {

                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
                    QRect end = _gridTable[i][j-1].translated(HSPACING, VSPACING);
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

QString DirWidget::getAppImage(QString appPath)
{
    typedef HICON (*tempFuc)(CONST TCHAR *filePath);
    QLibrary myLib;
    #ifdef DEBUG
        myLib.setFileName("IconGetD.dll");
    #else
        myLib.setFileName("IconGet.dll");
    #endif

    tempFuc myFunction = (tempFuc) myLib.resolve("getJumbIcon");
    if (myFunction) {
         HICON jumbIcon = myFunction((CONST TCHAR *)appPath.utf16 ());

         QPixmap picon = QPixmap::fromWinHICON(jumbIcon);

         QFileInfo info = QFileInfo(appPath);
         QString path(Config::get("IconDir"));
         path = path + "\\" + "USER_ADDED_" + info.baseName();
         path += ".png";
         QPixmap newicon =  picon.scaled(48,48,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
         newicon.save(path, "PNG",-1);
         return path;
    }
    return "";
}

QString DirWidget::addLocalApp(QString appPath)
{
//    qDebug() <<" appPath "<< appPath;
    QString newApp = getAppImage(appPath);

    if (newApp.isEmpty())
        return "";

    QImage image = QImage(newApp).scaled(48, 48);
    QImage normal = QImage(":images/app_bg.png");
    //setImgOpacity(normal, 0);

    for (int i = 0; i < normal.width(); i++) {
        for (int j = 0; j < normal.height(); j++) {
            QRgb pixel = normal.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                    qBlue(pixel), a * 0 / 255);
            normal.setPixel(i, j, lightPixel);
        }
    }

    QPainter pt1(&normal);
    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
    pt1.drawImage(17, 8, image);
    pt1.end();
    QPixmap pix = QPixmap::fromImage(normal);
    pix.save(newApp, "PNG", -1);

    return newApp;
}

void DirWidget::showApp(bool localApp)
{
//    if (localApp)
//    {
//        for (int i = 0; i < _local->count(); i++) {
//            if (_local->at(i)->hidden())
//                continue;

//            if(!_local->at(i)->isRemote())
//            {
//                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                        _local->at(i)->page(), -1);
//            }
//        }
//    }
//    else
//    {
//        for (int i = 0; i < _local->count(); i++) {
//            if (_local->at(i)->hidden())
//                continue;

//            if(_local->at(i)->isRemote())
//            {
//                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                        _local->at(i)->page(), -1);
//            }
//        }
//    }
}
void DirWidget::largeIcon()
{
    _iconSize = IconItem::large_size;
    refresh(LARGESIZE);
}

void DirWidget::mediumIcon()
{
    _iconSize = IconItem::medium_size;
    refresh(MEDIUMSIZE);
}

void DirWidget::smallIcon()
{
    _iconSize = IconItem::small_size;
    refresh(SMALLSIZE);
}

void DirWidget::refresh(QSize size)
{
    movetoFirst();
    deleteAllIconItem();
    reloadApplist(size);

//    getLocalIcon();
//    getVacIcon();
//    getPaasIcon();
}

void DirWidget::movetoFirst()
{
    _current = 0;
    move(_pages[_current], y());
//    emit pageChanged(_current);

}

void DirWidget::reloadApplist(QSize size)
{
    _count = 1;

//    gridWidth = size.width() + HSPACING * 2;
//    gridHeight = size.height() + BOTTOMSPACING;

    changeSpacing();

    _col = (_width - _iconHSpacing) / gridWidth;
    _row = (_height - _iconVSpacing) / gridHeight;
    _iconsPerPage = _col * _row;
    _current  = 0;

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _height));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {

            int x = (j % _col) * gridWidth +  _iconHSpacing;
            int y = _pageSize.height() * i \
                    + (j / _col) * gridHeight + _iconVSpacing;

            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);

    }

    for (int i = 0; i < _count; i++) {
        QList<IconItem*> newList;
        for (int j = 0; j < _col * _row; j++)
            newList.insert(j, NULL);
        _iconTable.insert(i, newList);
    }

    for (int i = 0; i < _count; i++)
        _nextIdx.insert(i, 0);

    qDebug()<<"reload all.";
}

void DirWidget::deleteAllIconItem()
{
//    _local->save();

    for (int i=0;i<_count;i++)
    {
        for(int j=0;j<_iconsPerPage;j++) // also you can use _nextIdx[i]
        {
            if(_iconTable[i][j]!=NULL)
            {
                delete _iconTable[i][j];
                _iconTable[i][j]=NULL;
            }
        }
        _nextIdx[i]=0;
    }

    _pages.clear();
    _iconTable.clear();
    _nextIdx.clear();
    _iconDict.clear();
    _gridTable.clear();
}

void DirWidget::changeSpacing()
{
    switch(_iconSize)
    {
        case IconItem::large_size :
            _height = LARGESIZE.height() + VSPACING + BOTTOMSPACING;

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = LARGESIZE.width() + HSPACING * 2;
            gridHeight = LARGESIZE.height() + VSPACING + BOTTOMSPACING;

            break;

        case IconItem::medium_size :

            _height = MEDIUMSIZE.height() + VSPACING + BOTTOMSPACING;

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = MEDIUMSIZE.width() + HSPACING * 2;
            gridHeight = MEDIUMSIZE.height() + BOTTOMSPACING;
            break;

        default:

            _height = SMALLSIZE.height() + VSPACING + BOTTOMSPACING;

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = SMALLSIZE.width() + HSPACING * 2;
            gridHeight = SMALLSIZE.height() + BOTTOMSPACING;
            break;
    }
}

void DirWidget::dragEnterEvent(QDragEnterEvent *event)
{
    _dragEvent = true;

     IconItem *icon = qobject_cast<IconItem*> (event->source());
    _inDrag = icon;
    icon->hide();
    event->accept();
}


void DirWidget::dragMoveEvent(QDragMoveEvent *event)
{
    _dragEvent = true;

    event->accept();

    int p = _inDrag->page();
    int s = _inDrag->index();
    /* The virtual desktop can only expand to right */
    /* If the page is the last page and there is only one icon
     * on this page, this icon can't be moved to add a page
     */

    /* move to the right page */
    if (QRect(_pageSize.width() - 20, 0, 20, _pageSize.height())\
            .contains(mapToGlobal(event->pos()))) {
        if (p == (_count-1) && _iconTable[p][1] == NULL)
            return;
        //dragRightTimer->start();
        //QTimer::singleShot(500, this, SLOT(dragRight()));
    }
    /* move to the left page */
    else if (QRect(0, 0, 20, _pageSize.height())\
             .contains(mapToGlobal(event->pos()))) {
        if (_current == 0)
            return;
        //dragLeftTimer->start();
    }
    /* move in this page */
    else {
        QRect moving(event->pos() - gap, _inDrag->size() * 1.0);
        int index = getNearestIndex(moving);
        if (index == -1 || s == index)
            return;
#if 0
        if ((ADDICON == _iconTable[_current][index]->_icontype)||(ADDICON == _iconTable[_current][s]->_icontype))
        {
            return;
        }
#endif
        index = index > _nextIdx[_current] - 1 ? _nextIdx[_current] - 1: index;
        if (s > index) {
            for (int i = s; i > index; i--) {
                QPoint end = _gridTable[_current][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_current][i-1]->move(end);
                _iconTable[_current][i] = _iconTable[_current][i-1];
                _iconTable[_current][i]->setIndex(i);
            }
        }
        else {
            for (int i = s; i < index; i++) {
                QPoint end = _gridTable[_current][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_current][i+1]->move(end);
                _iconTable[_current][i] = _iconTable[_current][i+1];
                _iconTable[_current][i]->setIndex(i);
            }
        }
        //}
        _iconTable[_current][index] = _inDrag;
        _inDrag->setPage(_current);
        _inDrag->setIndex(index);
    }

}

void DirWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    _dragEvent = false;

    Q_UNUSED(event);
}

void DirWidget::dropEvent(QDropEvent *event)
{

    event->accept();


    if (!_inDrag)
        return;

    int p = _inDrag->page();
    int s = _inDrag->index();

    QPoint end = _gridTable[p][s].translated(HSPACING, VSPACING).topLeft();

    _iconTable[p][s] = _inDrag;
    _inDrag->move(end);

    _inDrag->show();

    _inDrag=NULL;

    _dragEvent = false;
}

int DirWidget::getNearestIndex(const QRect &rect)
{
    int page = rect.x()/_pageSize.width();
    int index = -1;
    QRect inter;
    for (int i = 0; i < _row * _col; i++) {
        inter = rect.intersected(_gridTable[page][i]);
        int area = abs(inter.width() * inter.height());
        if (area > 500) {
            index = i;
            break;
        }
    }
    return index;
}

void DirWidget::mousePressEvent(QMouseEvent *)
{

}

void DirWidget::mouseMoveEvent(QMouseEvent *)
{

}

void DirWidget::mouseReleaseEvent(QMouseEvent *)
{

}
