#include <QPalette>
#include <QPainter>
#include <QSettings>
#include <QApplication>
#include <QMenu>
#include <QFileInfoList>
#include <QLibrary>
#include <windows.h>
#include <QTimer>
#include <QImage>
#include <stdio.h>
#include <QtDebug>
#include <QTextCodec>
#include <QDir>
#include <QEventLoop>
#include <QEvent>
#include <QWebSettings>
#include <cassert>

//#include <QtScript/QScriptValue>
//#include <QtScript/QScriptEngine>
//#include <QtScript/QScriptValueIterator>

#include "vacwidget.h"
#include "config.h"
#include "commuinication.h"

//#define KEY "\\Windows\\CurrentVersion\\App Paths\\"
//#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"
//#define PAASURL "http://192.168.30.128/dongfang.php"
extern QString WIN_LOCAL_IconPath;
extern QString WIN_VAPP_IconPath;
extern QString WIN_PAAS_IconPath;
extern QList<LOCAL_LIST> g_RemotelocalList;
extern QList<APP_LIST> g_RemoteappList;
extern QList<PAAS_LIST> g_RemotepaasList;
extern QList<APP_LIST> g_myVappList;
extern QList<PAAS_LIST> g_myPaasList;

extern int ICON_TYPE;

//#define ICONWIDTH 72
//#define ICONHEIGHT 72
//#define SELECTWIDTH 37
//#define SELECTHEIGHT 37

//#define SPACING 30
//#define FONTSIZE 10

//#define ICONITEMWIDTH (ICONWIDTH + SELECTWIDTH/2 + 8)
//#define ICONITEMHEIGHT (ICONHEIGHT + 15 + FONTSIZE * 2)

//#define CLOSEWIDTH 30
//#define CLOSEHEIGHT 30

//#define APPICON   0
//#define ADDICON 1
//#define HSPACING (40 - (SELECTWIDTH / 2 + 8) / 2)
//#define VSPACING 25

#define ICONNUM 17

#define ICONWIDTH 143
#define ICONHEIGHT 143
#define SELECTWIDTH 37
#define SELECTHEIGHT 37

#define SPACING 30
#define FONTSIZE 10

#define ICONITEMWIDTH 143
#define ICONITEMHEIGHT 143

#define LARGESIZE QSize(143, 143)       //72*72
#define MEDIUMSIZE QSize(119, 119)      //48*48
#define SMALLSIZE QSize(103, 103)       //32*32

#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30

#define APPICON 0
#define HSPACING 4
#define VSPACING 0
#define BOTTOMSPACING 4

#define ICONHSPACING 0 // 70 -40
#define ICONVSPACING (45 - 36)

VacShowWidget::VacShowWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
{
//    QTextCodec *codec = QTextCodec::codecForName("utf-8"); //System
//    QTextCodec::setCodecForCStrings(codec);

    _bgPix.load(":/images/widget_bg.png");

    _width = pageSize.width();
    _height = pageSize.height();

    resize(_width, _height);

    _vacScrollBarWidget = new VacScrollBarWidget(QSize(pageSize.width(), 464), this);
    _vacScrollBarWidget->move(13,50);
    _vacScrollBarWidget->setVisible(true);

    _closePix.load(":images/widget_close_normal.png");
    _closeHoverPix.load(":images/widget_close_hover.png");
    _closeBtn = new DynamicButton(_closePix, _closeHoverPix, this);
    _closeBtn->setGeometry(_width - 8 -25 - 3, 28, 8, 8);

    connect(_closeBtn, SIGNAL(clicked()), this, SIGNAL(vacCloseBtnClicked()));

    connect(_vacScrollBarWidget, SIGNAL(addApp(const QString&,const QString&, const QString&, int)),
            this, SIGNAL(addApp(const QString&,const QString&, const QString&, int )));
    connect(_vacScrollBarWidget, SIGNAL(delItem(const QString &)), this, SIGNAL(delItem(const QString &)));
}

VacShowWidget::~VacShowWidget()
{

}

void VacShowWidget::showApp(bool localApp)
{
    _vacScrollBarWidget->showApp(localApp);
}

int VacShowWidget::addIcon(QString text, const QString &icon, int page, int index, const QString &url, int type)
{
    _vacScrollBarWidget->addIcon(text, icon, page, index, url, type);
}

void VacShowWidget::delIcon(const QString &text)
{
    _vacScrollBarWidget->delIcon(text);
}


void VacShowWidget::largeIcon()
{
    _vacScrollBarWidget->largeIcon();
}

void VacShowWidget::mediumIcon()
{
    _vacScrollBarWidget->mediumIcon();
}

void VacShowWidget::smallIcon()
{
    _vacScrollBarWidget->smallIcon();
}

void VacShowWidget::desktopDelIcon(const QString &text)
{
    _vacScrollBarWidget->desktopDelIcon(text);
}

void VacShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, _width, _height, _bgPix.scaled(_width, _height));
    painter.drawPixmap(18, 25, QPixmap(":images/panel_software_normal.png"));

    QWidget::paintEvent(event);
}

VacScrollBarWidget::VacScrollBarWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _oldPagePos(0)
    , _newPagePos(0)
{
//    QTextCodec *codec = QTextCodec::codecForName("utf-8"); //System
//    QTextCodec::setCodecForCStrings(codec);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0xdd));
    setPalette(pal);

    _width = pageSize.width();
    _height = pageSize.height();

    resize(_width, _height);

    _vacWidget = new VacWidget(pageSize, this);
    _vacWidget->setGeometry(0,0, _width - 5, _height - 1);
    _vacWidget->setVisible(true);

    _scrollBar = new QScrollBar(this);

    _scrollBar->setStyleSheet("QScrollBar:vertical{width:8px;border: 0px solid gray;background:rgba(255,255,255,0);}\
                    QScrollBar::handle:vertical{ min-width: 8px;min-height: 251px; background-image:url(:/images/widget_scrollbar.png);\
                    background-position: left; background-repeat:none; border-style:flat;}\
                    QScrollBar::handle:vertical::disabled{background:#232329;}\
                    QScrollBar::handle:vertical:hover{background-image:url(:/images/widget_scrollbar.png);}\
                    QScrollBar::add-line:vertical{background-color: rgba(255,255,255,0);height:13px;\
                                                  subcontrol-position: bottom; subcontrol-origin: margin;}\
                    QScrollBar::sub-line:vertical{background-color: rgba(255,255,255,0);height:13px;\
                                                  subcontrol-position: top; subcontrol-origin: margin;}\
                    QScrollBar::add-page:vertical{background-color: rgba(255,255,255,0);}\
                    QScrollBar::sub-page:vertical{background-color: rgba(255,255,255,0);}\
                    QScrollBar::up-arrow:vertical{subcontrol-origin: margin; background-color: rgba(255,255,255,0);height:13px;}\
                    QScrollBar::down-arrow:vertical{background-color: rgba(255,255,255,0);height:13px;}");


    _scrollBar->setSingleStep(_vacWidget->pageSize().height());
    _scrollBar->setPageStep(_vacWidget->pageSize().height());

    if (_vacWidget->count() > 1)
        _scrollBar->setVisible(true);
    else
        _scrollBar->setVisible(false);

    _animation = new QPropertyAnimation(_vacWidget, "geometry");

    connect(_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
    connect(_vacWidget, SIGNAL(addApp(const QString&,const QString&, const QString&, int)),
            this, SIGNAL(addApp(const QString&,const QString&, const QString&, int )));
    connect(_vacWidget, SIGNAL(pageIncreased()), this, SLOT(vacWidgetCountChanged()));
    connect(_vacWidget, SIGNAL(pageDecreased()), this, SLOT(vacWidgetCountChanged()));
    connect(_vacWidget, SIGNAL(delItem(const QString &)), this, SIGNAL(delItem(const QString &)));



}

VacScrollBarWidget::~VacScrollBarWidget()
{

}

void VacScrollBarWidget::scrollBarValueChanged(int val)
{
    _oldPagePos = _newPagePos;
    _newPagePos = val / _height;
    if (_oldPagePos == _newPagePos)
    {
        return;
    }

    if (_animation->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animation->setDuration(10);
    _animation->setStartValue(QRect(0, _vacWidget->pos().y(), _vacWidget->pageSize().width(), _vacWidget->pageSize().height()));
    _animation->setEndValue(QRect(0, -1 * (_newPagePos*_height), _vacWidget->pageSize().width(), _vacWidget->pageSize().height()));

    _animation->start();
}

//void VacScrollBarWidget::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);;
//    painter.drawPixmap(0, 0, _width, _height,\
//                       _bgPix.scaled(_width, _height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

//    QWidget::paintEvent(event);
//}

void VacScrollBarWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    _width = width();
    _height = height();

    int w = 15;
    int x = width() - 48;
    x = x < 0 ? 0: x;
    int h = height();
    _scrollBar->setGeometry(x, 15, w, h - 30);

    _scrollBar->setRange(0, _vacWidget->count() * _vacWidget->pageSize().height() - h);

    update();
}

void VacScrollBarWidget::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
    _scrollBar->event(event);
}

void VacScrollBarWidget::showApp(bool localApp)
{
    if (localApp)
        _vacWidget->showApp(true);
    else
        _vacWidget->showApp(false);
}

int VacScrollBarWidget::addIcon(QString text, const QString &icon, int page, int index, const QString &url, int type)
{
    _vacWidget->addIcon(text, icon, page, index, url, type);
}

void VacScrollBarWidget::delIcon(const QString &text)
{
    _vacWidget->delIcon(text);
}

void VacScrollBarWidget::vacWidgetCountChanged()
{
    _scrollBar->setSingleStep(_vacWidget->pageSize().height());
    _scrollBar->setPageStep(_vacWidget->pageSize().height());

    if (_vacWidget->count() > 1)
        _scrollBar->setVisible(true);
    else
        _scrollBar->setVisible(false);

    _scrollBar->setRange(0, _vacWidget->count() * _vacWidget->pageSize().height() - _height);
}

void VacScrollBarWidget::largeIcon()
{
    _vacWidget->largeIcon();
}

void VacScrollBarWidget::mediumIcon()
{
    _vacWidget->mediumIcon();
}

void VacScrollBarWidget::smallIcon()
{
    _vacWidget->smallIcon();
}

void VacScrollBarWidget::desktopDelIcon(const QString &text)
{
    _vacWidget->unChecked(text);
}

//
//VacItem::VacItem(const QString &text, QWidget *parent)
//    : QWidget(parent)
//    , _text(text)
//    , _trembling(0)
//    ,_equal(false)
//{
////    int width, height;
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
///*
////    _texticon = _text;
//    _texticon_firstrow = _texticon;
//    _texticon_secondrow = _texticon;
//    _texticon_thirdrow = _texticon;

//    int i;
//    for (i = 0; i < _textWidth; i++)
//    {
//        _textWidth_firstrow = fm.width(_texticon_firstrow);

//        if (_textWidth_firstrow > width - 20)
//        {
//            _texticon_firstrow.chop(1);
//        }
//        else{
//            //_texticon.chop(1);
//            //_texticon.append("...");
//            break;
//        }
//    }
//    _texticon_secondrow = _texticon.right(_texticon.length() - _texticon_firstrow.length());

//    _textWidth_secondrow = fm.width(_texticon_secondrow);
//    for (i = 0; i < _textWidth_secondrow; i++)
//    {
//        _textWidth_secondrow = fm.width(_texticon_secondrow);

//        if (_textWidth_secondrow > width - 20)
//        {
//            _texticon_secondrow.chop(1);
//        }
//        else{
//            //_texticon.chop(1);
//            //_texticon.append("...");
//            break;
//        }
//    }
//    _texticon_thirdrow = _texticon.right(_texticon.length() - _texticon_firstrow.length() - _texticon_secondrow.length());

//    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
//    if (_textWidth_thirdrow > width)
//    {
//        for (i = 0; i < _textWidth_thirdrow; i++)
//        {
//            _textWidth_thirdrow = fm.width(_texticon_thirdrow);
//            if (_textWidth_thirdrow > width - 20)
//            {
//                _texticon_thirdrow.chop(1);
//            }else{
//                _texticon_thirdrow.chop(3);
//                _texticon_thirdrow.append("...");
//                break;
//            }
//        }
//    }

//    _textHeight = fm.height();
//*/
//    int i;

//    if (_textWidth > _width)
//    {
//        for (i = 0; i < _textWidth; i++)
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

////    _app = LocalAppList::getList()->getAppByName(_text);
//    _animation = new QPropertyAnimation(this, "geometry");
//    _animation->setDuration(200);
//    _animation->setEasingCurve(QEasingCurve::OutBack);

////    _timeline = new QTimeLine(5000, this);
////    _timeline->setLoopCount(0);
////    _timeline->setUpdateInterval(65);

//    _icontype = APPICON;

////    _hoverVacItem = new HoverIconItem(this->width(), this->height(), this);
////    _hoverVacItem->setVisible(false);

////    _openAction = new QAction(tr("运行"), this);
////    connect(_openAction, SIGNAL(triggered()), this, SLOT(openClicked()));

////    if (_style == VirtualDesktop::localIcon)
////    {
////        _delAction = new QAction(tr("删除"), this);
////        connect(_delAction, SIGNAL(triggered()), this, SLOT(delClicked()));
////    }

////    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(doTremble(qreal)));
////    if (parent) {
////        if (_style == VirtualDesktop::localIcon)
////        {
////            connect(static_cast<VacWidget*>(parent), \
////                    SIGNAL(trembleStarted()), \
////                    this, SLOT(startTremble()));
////            connect(static_cast<VacWidget*>(parent), \
////                    SIGNAL(trembleStoped()), \
////                    this, SLOT(stopTremble()));
////        }
////    }
//}

//VacItem::~VacItem()
//{
////    delete _openAction;
////    delete _hoverVacItem;

////    delete _timeline;
////    delete _animation;
//}

//void VacItem::setUrl(const QString &url)
//{
//    _url = url;
//}

//void VacItem::setPage(int page)
//{
//    _page = page;
////    _app->setPage(page);
//}
//void VacItem::setIndex(int index)
//{
//    _index= index;
////   _app->setIndex(index);
//}
//void VacItem::setHidden(bool hide)
//{
//    Q_UNUSED(hide);
////    _app->setHidden(hide);
//}

//void VacItem::animationMove(const QRect &start, const QRect &end)
//{
//    _animation->setStartValue(start);
//    _animation->setEndValue(end);
//    _animation->start();
//}

////void VacItem::doTremble(qreal value)
////{
////    Q_UNUSED(value);

////#define LEFT -1
////#define RIGHT 1
////    static int direction = LEFT;
////    if (_trembling == -1) {
////        _trembling = 0;
////        direction = RIGHT;
////    }
////    else if (_trembling == 0) {
////        if (direction == LEFT) {
////            _trembling = -1;
////        } else {
////            _trembling = 1;
////        }
////    }
////    else {
////        _trembling = 0;
////        direction = LEFT;
////    }
////    repaint();
////#undef LEFT
////#undef RIGHT
////}

////void VacItem::startTremble()
////{
////    _timeline->start();
////    _pixmap = _closePixmap;
////}

////void VacItem::stopTremble()
////{
////    _timeline->stop();
////    _trembling = 0;
////    _pixmap = _normalPixmap;
////    repaint();
////}

//void VacItem::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);
////    if (_trembling)
////        painter.translate(width()/2, height()/2);
////    painter.rotate(2*_trembling);
////    if (_trembling)
////        painter.translate(-1*width()/2, -1*height()/2);

//    painter.drawPixmap(3, 0, ICONWIDTH, ICONHEIGHT,_pixmap);

//    painter.drawPixmap(_width - ICONWIDTH / 4 - SELECTWIDTH / 2, height() - 25 - SELECTHEIGHT, _selectPixmap);

//    QWidget::paintEvent(event);
//}

//void VacItem::mousePressEvent(QMouseEvent *event)
//{
//    qDebug()  << event->pos();
//    if (event->button() == Qt::LeftButton) {
//        if (QRect(35, 36, 72, 72 + 35).contains(event->pos())) {

//            if (!_equal)
//            {
//                _selectPixmap.load(":/images/select_icon.png");
//                update();
//                _equal = true;
//                emit addVacApp(_text, _pix, _url);
//            }
//        }
//    }
//    event->ignore();

//}

//void VacItem::mouseDoubleClickEvent(QMouseEvent *event)
//{
//    if ((event->button() == Qt::LeftButton))
//    {
////        openClicked();
//    }
//}

//void VacItem::mouseMoveEvent(QMouseEvent *event)
//{
//    if (!(event->buttons() & Qt::LeftButton))
//        return;
//    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
//        return;
//    }
////    _drag = new QDrag(this);
////    QMimeData *mimeData = new QMimeData;
////    mimeData->setText(_text);
////    _drag->setMimeData(mimeData);
////    _drag->setPixmap(grayPixmap());
////    _drag->setHotSpot(event->pos());
////    gap = _drag->hotSpot();
////    _drag->exec(Qt::MoveAction);
//}

////void VacItem::contextMenuEvent(QContextMenuEvent *event)
////{
////    Q_UNUSED(event);


////    QCursor cur=this->cursor();
////    QMenu *iconItemcontextMenu = new QMenu(this);

////    iconItemcontextMenu->addAction(_openAction);

//////    if (_style == VirtualDesktop::localIcon)
//////    {
////        iconItemcontextMenu->addAction(_delAction);
//////    }

////    iconItemcontextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);

////    iconItemcontextMenu->exec(cur.pos());

////    return;
////}


//void VacItem::enterEvent(QEvent *event)
//{
//    _pixmap = darkPixmap();
////    _hoverIconItem->setVisible(true);
//    repaint();

//    Q_UNUSED(event);
//}

//void VacItem::leaveEvent(QEvent *event)
//{

////    if (_timeline->state() == QTimeLine::Running)
////        _pixmap = _closePixmap;
////    else
//        _pixmap = _normalPixmap;

////    _hoverIconItem->setVisible(false);
//    repaint();

//    Q_UNUSED(event);
//}

//const QPixmap& VacItem::originPixmap()
//{
//    return _originPixmap;
//}

//void VacItem::setPixmap(const QString &icon)
//{

//    _pix = icon;
////    int begin;
//    QString text = _text;
//    if (isUserType())
//        text = _text.right(_text.length() - 1);
//    _originPixmap = QPixmap(icon);
//    QImage image = QImage(icon);
//    QImage normal = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);

//    //    image.scaled(ICONWIDTH, ICONHEIGHT);
//    //    normal.scaled(ICONWIDTH, ICONHEIGHT);

//    QPainter pt1(&normal);
//    pt1.setPen(Qt::white);
//    //pt1.setFont(QFont("", FONTSIZE, QFont::Black));
//    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
//    pt1.setCompositionMode(QPainter::CompositionMode_Source);
//    pt1.fillRect(normal.rect(), Qt::transparent);
////    pt1.drawImage((SELECTWIDTH / 2  + 8) / 2, 0, image);
//    pt1.drawImage(0, 0, image);

//        QFont font("", FONTSIZE, QFont::Normal);
////        QFont font(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal);

//        QFontMetrics fm(font);
//        _textWidth_firstrow = fm.width(_texticon);
//        pt1.drawText( QRect((_width - _textWidth_firstrow) / 2,  ICONHEIGHT - FONTSIZE * 2,\
//                            _textWidth_firstrow, _textHeight), Qt::TextSingleLine, _texticon);

//    //    QFontMetrics fm(font);
//    //    _textWidth_firstrow = fm.width(_texticon_firstrow);
//    //    QString tx = _texticon_firstrow;
//    //    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_firstrow) / 2 , height() - _textHeight * 3 - 8, _textWidth_firstrow, _textHeight), Qt::TextSingleLine, tx);
//    //    _textWidth_secondrow = fm.width(_texticon_secondrow);
//    //    tx = _texticon_secondrow;
//    //    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_secondrow) / 2, height() - _textHeight * 2 - 8, _textWidth_secondrow, _textHeight), Qt::TextSingleLine, tx);
//    //    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
//    //    tx = _texticon_thirdrow;
//    //    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_thirdrow) / 2 , height() - _textHeight - 8, _textWidth_thirdrow, _textHeight), Qt::TextSingleLine, tx);
//        pt1.end();

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

//void VacItem::setText(const QString &text)
//{
//    _text = text;
//}

//bool VacItem::isUserType()
//{
//    if (_text.startsWith("/"))
//        return true;
//    return false;
//}

//const QString & VacItem::text()
//{
//    return _text;
//}

//const QPixmap & VacItem::pixmap()
//{
//    return _pixmap;
//}

//const QPixmap & VacItem::grayPixmap()
//{
//    return _grayPixmap;
//}

//const QPixmap & VacItem::darkPixmap()
//{
////    if (_timeline->state() == QTimeLine::Running)
////        return _closePixmap;
//    return _darkPixmap;
//}

//void VacItem::openClicked()
//{
//    emit runItem(_text);
//}

//void VacItem::delClicked()
//{
//    emit delItem(_text);
//}

//void VacItem::setEqualIcon(bool equal)
//{
//    if (equal)
//    {
//        _equal = true;
//        _selectPixmap.load(":/images/select_icon.png");
//    }
//    else
//    {
//        _equal = false;
//        _selectPixmap.load("");
//    }

//    repaint();
//}

//
VacWidget::VacWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _localCount(0)
    , _iconSize(ICON_TYPE)
{
    _count = 1;
    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = _pageSize.height();

    changeSpacing();

//    gridWidth = ICONITEMWIDTH + HSPACING * 2;
//    gridHeight = ICONITEMWIDTH + BOTTOMSPACING;

    _col = (_width - _iconHSpacing) / gridWidth;
    _row = (_height - _iconVSpacing) / gridHeight;
    _iconsPerPage = _col * _row;
    _current  = 0;


//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;

//        if(_local->at(i)->isRemote())
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

    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setEasingCurve(QEasingCurve::OutBack);

    setAcceptDrops(true);
    setAutoFillBackground(true);

    setFixedSize(_width, _height * _count);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);

//    _pFinished = false;
//    _commui = new commuinication();

//    _vFinished = false;
//    _paasCommui = new PaasCommuinication();

//    connect(_commui, SIGNAL(done()), this, SLOT(onVacDone()));
//    connect(_paasCommui, SIGNAL(done()), this, SLOT(onPaasDone()));

//    for (int i = 0; i < _local->count(); i++) {
//        if (_local->at(i)->hidden())
//            continue;

//        if(_local->at(i)->isRemote())
//        {
//            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
//                    _local->at(i)->page(), -1);
//        }
//    }




//    for (int i = 0; i < ICONNUM; i++) {
//        addIcon("", QString(":images/custom/z_%1.png").arg(i),
//                -1, i);
//    }
    getLocalIcon();
    getVacIcon();
    getPaasIcon();
}

VacWidget::~VacWidget()
{

}

int VacWidget::addIcon(QString text, \
                       const QString &iconPath, \
                       int page, \
                       int index,
                       const QString &url,
                       int type)
{
    int expandPageCount;
    int iconNum = g_RemotelocalList.count() + g_myPaasList.count() + g_myVappList.count();
    if (iconNum % _iconsPerPage == 0)
        expandPageCount = iconNum / _iconsPerPage;
    else
        expandPageCount = iconNum / _iconsPerPage + 1;

    if (expandPageCount > _count)
        expand();

    _url = url;

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
    icon->setRunAction(false);
    icon->setDelAction(false);
    icon->setClosePixBool(false);
    icon->setSelectPixBool(true);
    icon->setDoubleClickBool(false);
    icon->setContextMenuBool(false);
    icon->setTrembleBool(false);
    icon->setDragEventBool(false);
    icon->setAcceptDrops(false);
    icon->setEnterEventBool(true);
    icon->setLeaveEventBool(true);


    if (LocalAppList::getList()->getAppByName(text))
    {
        icon->setEqualIcon(true);
    }
    else
    {
        icon->setEqualIcon(false);
    }

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

//    for (int i = 0; i < ICONNUM; i++)
//    {
//        if (_local->at(i)->name() == text)
//        {
//            icon->setEqualIcon(true);
//        }
//        else
//            icon->setEqualIcon(false);
//    }

    icon->setPixmap(iconPath,text);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(_url);
    icon->setType(type);
    icon->setDirId(0);
    _iconDict.insert(text, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

    connect(icon, SIGNAL(addApp(const QString&, const QString&, const QString&, int)),
            this, SIGNAL(addApp(const QString&,const QString&, const QString&, int)));

//    if (text == "XX的虚拟机" || text == "IE " || text == "WPP" || text == " Word")
//    {
//        qDebug() << text << iconPath << _url;
//        icon->setEqualIcon(true);
//        emit addVacApp(text, iconPath, _url);
//    }

//    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
    connect(icon, SIGNAL(delItem(const QString&)), this, SIGNAL(delItem(const QString&)));


     return page;
    //LocalAppList::getList()->save();
}

void VacWidget::delIcon(const QString &text)
{
//    qDebug() << "texttexttexttexttexttexttext" << text;

    IconItem *icon = _iconDict.take(text);
    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();
//    qDebug() << "texttexttexttexttexttexttext" << p;
//    qDebug() << "texttexttexttexttexttexttext" << s;
    delete _iconTable[p][s];

//    moveBackIcons(p, s);
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

//    if (_local->count() == 0) {
//        appCancel();
//    }

//    bool trembling = true;

//    for (int i = 0; i < _local->count(); i++)
//    {
//        if (!_local->at(i)->isRemote())
//        {
//            trembling = false;
//            break;
//        }
//    }

//    if (trembling) {
//        appCancel();
//    }

}

void VacWidget::moveBackIcons(int page, int index)
{
//    int p = page;
//    int s = index;

//    for(int i = p; i < _count; i++)
//    {
//        if (i == p)
//        {
//            int k;
//            for (k = s + 1; k < _nextIdx[p]; k++) {
//                QRect start = _gridTable[p][k].translated(HSPACING, VSPACING);
//                QRect end = _gridTable[p][k-1].translated(HSPACING, VSPACING);
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
//                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
//                    QRect end = _gridTable[i-1][_nextIdx[i-1] - 1].translated(HSPACING, VSPACING);
//                    _iconTable[i][j]->animationMove(start, end);
//                    _iconTable[i][j]->setPage(i-1);
//                    _iconTable[i][j]->setIndex(_nextIdx[i-1] - 1);
//                    _iconTable[i-1][_nextIdx[i-1] - 1] = _iconTable[i][j];
//                }
//                else {

//                    QRect start = _gridTable[i][j].translated(HSPACING, VSPACING);
//                    QRect end = _gridTable[i][j-1].translated(HSPACING, VSPACING);
//                    _iconTable[i][j]->animationMove(start, end);
//                    _iconTable[i][j]->setIndex(j-1);
//                    _iconTable[i][j - 1] = _iconTable[i][j];
//                }
//            }
//        }
//    }

//    _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
//    _nextIdx[_count - 1]--;

//    if (_nextIdx[_count - 1] == 0)
//        delPage(_count - 1);

}

void VacWidget::showApp(bool localApp)
{
        Q_UNUSED(localApp);
}


/*
QIcon VacWidget::icon(const QUrl &url)
{
    qDebug() << "url" << url;
    QIcon icon = QWebSettings::iconForUrl(url);
    if (!icon.isNull()) {
        return icon.pixmap(16, 16);
        }
    if (icon.isNull()) {
        QPixmap pixmap = QWebSettings::webGraphic(QWebSettings::DefaultFrameIconGraphic);
        if (pixmap.isNull()) {
            pixmap = QPixmap(QLatin1String(":/images/icon.png"));
            QWebSettings::setWebGraphic(QWebSettings::DefaultFrameIconGraphic, pixmap);
        }
        return pixmap;
    }
    return icon;
}
*/

QString VacWidget::getAppImage(QString appPath)
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

         QString path(WIN_PAAS_IconPath);

         QPixmap newicon =  picon.scaled(59, 59, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
         newicon.save(path, "ICO",-1);
         return path;
    }
    return "";
}

//void VacWidget::addVacApp(QString appPath)
//{
//    QString newApp = getAppImage(appPath);

//    if (newApp.isEmpty())
//        return;


//    QImage image = QImage(newApp).scaled(72, 72);
//    QImage normal = QImage(":images/icon_shadow.png");
    //setImgOpacity(normal, 0);

//    for (int i = 0; i < normal.width(); i++) {
//        for (int j = 0; j < normal.height(); j++) {
//            QRgb pixel = normal.pixel(i,j);
//            int a = qAlpha(pixel);
//            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
//                                    qBlue(pixel), a * 117 / 255);
//            normal.setPixel(i, j, lightPixel);
//        }
//    }

//    QPainter pt1(&normal);
//    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
//    pt1.drawImage(35, 36, image);
//    pt1.end();
//    QPixmap pix = QPixmap::fromImage(normal);
//    pix.save(newApp, "PNG", -1);//

//    QFileInfo info = QFileInfo(appPath);
//    LocalApp *app = new LocalApp();
//    app->setName("/" + info.baseName());
//    app->setIcon(newApp);
//    app->setExecname(appPath);
//    if (LocalAppList::getList()->getAppByName(app->name())) {
//        QApplication::processEvents();
//        AppMessageBox box(false, NULL);
//        box.setText("    已添加该图标");
//        box.exec();
//    } else {
//        addApp(app);
//    }
//    QImage image = QImage(newApp).scaled(59, 59);
//    QImage normal = QImage(":images/icon_shadow.png").scaled(143, 143);
//    QImage middle = QImage(":images/icon_middle_shadow.png").scaled(72, 72);

//    QPainter pt1(&normal);
//    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
//    pt1.drawImage(QRect(35, 36, 72, 72), middle);
//    pt1.drawImage(QRect(35 + 7, 36 + 3, 59, 59), image);
//    pt1.end();
//    QPixmap pix = QPixmap::fromImage(normal);
//    pix.save(newApp, "ICO", -1);
//}

void VacWidget::expand()
{
    _count++;
    setFixedSize(_pageSize.width(), _count * _height);
    _pages.append(-(_count-1) * _height);
    QList<QRect> newGridList;

    changeSpacing();

    for (int i = 0; i < _col * _row; i++) {
        //vertical display
//        int x =  _pageSize.width() * (_count - 1)\
//                + (i / _row) * gridWidth;
//        int y = (i % _row) * gridHeight;

            //horizontal display
            int x =  (i % _col) * gridWidth + _iconHSpacing;
            int y = _height * (_count - 1) \
                    + (i / _col) * gridHeight + _iconVSpacing;
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

void VacWidget::delPage(int page)
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
                icon->setGeometry(_gridTable[i][j].translated(HSPACING, VSPACING));
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
//        if (_inDrag)
//            _inDrag->setPage(_inDrag->page() - 1);
//        emit pageChanged(_current);
    }
    setFixedSize(_pageSize.width(), _count * _pageSize.height());
    move(_pages[_current], y());
    emit pageDecreased();
}

void VacWidget::getPaasIcon()
{
//    qDebug() << "!#@$!@#$@#^%*(&)(*%^$#%@#$!@#$#@!$#";
//    _paasList.clear();

//    QString::SectionFlag flag = QString::SectionSkipEmpty;
//    QString url;

//    //get vapp list
//    _paasCommui->getAppList();
//    while (!_pFinished)
//        QApplication::processEvents();
//    _pFinished = false;

//    _paasList = _paasCommui->getList();
//    qDebug() << "********" << _paasList.count();

//    if (_paasList.count() == 0)
//        return;


//    QString iconDirPath = WIN_PAAS_IconPath ;


//    QDir iconDir(iconDirPath);
//    if(!iconDir.exists())
//    {
//        iconDir.mkdir(iconDirPath);
//    }
//    //store ico file locally
//    for(int i = 0; i < _paasList.count(); i++)
//    {
//        QString iconPath = QString("%1%2.ico")
//                .arg(iconDirPath)
//                .arg(_paasList[i].cnName);
////        qDebug()<<"iconPath="<<iconPath;

//        //check if ico file is existed, or dont donwload

//        QFile chkFile(iconPath);
//        if(chkFile.exists())
//        {
//            chkFile.close();
//            continue;
//        }
//        chkFile.close();

//        //qDebug()<<"iconPath"<<iconPath;
//        if (_paasList[i].logoURL.isEmpty())
//        {
//            url = _paasList.at(i).urls.section('/', 1, 1, flag);
//            url = QString("http://" + url + "/Favicon.ico");

//            _paasCommui->downloadIcon(QUrl(url), iconPath);
//            while (!_pFinished)
//            {
//               QApplication::processEvents();
//            }
//           _pFinished = false;
//        }
//        else
//        {
//            _paasCommui->downloadIcon(QUrl(_paasList[i].logoURL), iconPath);
//            while (!_pFinished)
//                QApplication::processEvents();
//            _pFinished = false;
//        }

//         QString newApp = iconPath;

//         if (newApp.isEmpty())
//             return;

//        QImage image = QImage(newApp).scaled(59, 59);
//        QImage normal = QImage(":images/icon_shadow.png").scaled(143, 143);
//        QImage middle = QImage(":images/icon_middle_shadow.png").scaled(72, 72);

//        QPainter pt1(&normal);
//        pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
//        pt1.drawImage(QRect(35, 36, 72, 72), middle);
//        pt1.drawImage(QRect(35 + 7, 36 + 3, 59, 59), image);

//        pt1.end();
//        QPixmap pix = QPixmap::fromImage(normal);
//        pix.save(newApp, "ICO", -1);

//    }

    for(int i = 0; i < g_RemotepaasList.count(); i++)
    {
//        addIcon(g_RemotepaasList.at(i).cnName, WIN_PAAS_IconPath + g_RemotepaasList.at(i).cnName +".png",
//                -1, i, g_RemotepaasList.at(i).urls);
        addIcon(g_RemotepaasList.at(i).cnName, g_RemotepaasList.at(i).iconPath,
                -1, i, g_RemotepaasList.at(i).urls, paasIcon);
    }

}

void VacWidget::getVacIcon()
{
    for(int i = 0; i < g_RemoteappList.count(); i++)
    {
        addIcon(g_RemoteappList[i].name, WIN_VAPP_IconPath + g_RemoteappList[i].id + ".png", \
                                - 1, -1, QString(""), vacIcon);
    }
}

void VacWidget::getLocalIcon()
{
    for(int i = 0; i < g_RemotelocalList.count(); i++)
    {
        addIcon(g_RemotelocalList[i].name, g_RemotelocalList[i].iconPath, \
                                - 1, -1, QString(""), localIcon);
    }
}

void VacWidget::largeIcon()
{
    _iconSize = IconItem::large_size;
    refresh(LARGESIZE);
}

void VacWidget::mediumIcon()
{
    _iconSize = IconItem::medium_size;
    refresh(MEDIUMSIZE);
}

void VacWidget::smallIcon()
{
    _iconSize = IconItem::small_size;
    refresh(SMALLSIZE);
}

void VacWidget::refresh(QSize size)
{
    movetoFirst();
    deleteAllIconItem();
    for(int i = _count; i > 1; i--)
    {
        delPage(i-1);
    }

    _pages.clear();
    _iconTable.clear();
    _nextIdx.clear();
    _iconDict.clear();
    _gridTable.clear();

    reloadApplist(size);

    getLocalIcon();
    getVacIcon();
    getPaasIcon();
}

void VacWidget::movetoFirst()
{
    _current = 0;
    move(_pages[_current], y());
//    emit pageChanged(_current);

}

void VacWidget::reloadApplist(QSize size)
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

    qDebug()<<" VacWidget reload all.";
}

void VacWidget::deleteAllIconItem()
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
}

void VacWidget::changeSpacing()
{
    switch(_iconSize)
    {
        case IconItem::large_size :

            _iconHSpacing = ICONHSPACING + 25;
            _iconVSpacing = ICONVSPACING;

            gridWidth = LARGESIZE.width() + HSPACING * 2;
            gridHeight = LARGESIZE.height() + VSPACING + BOTTOMSPACING;

            break;

        case IconItem::medium_size :

            _iconHSpacing = ICONHSPACING / 3 * 2 + 19;
            _iconVSpacing = ICONVSPACING * 1.5;

            gridWidth = MEDIUMSIZE.width() + HSPACING * 2;
            gridHeight = MEDIUMSIZE.height() + BOTTOMSPACING * 5;
            break;

        default:

            _iconHSpacing = ICONHSPACING / 3 + 14;
            _iconVSpacing = ICONVSPACING * 1.5;

            gridWidth = SMALLSIZE.width() + HSPACING * 2;
            gridHeight = SMALLSIZE.height() + BOTTOMSPACING;
            break;
    }
}

void VacWidget::unChecked(const QString &text)
{
    qDebug() << "void VacWidget::unChecked(const QString &text) : " <<text;

    if (!_iconDict.value(text))
        return;

    _iconDict.value(text)->setEqualIcon(false);
}
