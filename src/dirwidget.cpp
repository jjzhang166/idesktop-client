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

#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30
#define FONTSIZE 10
#define APPICON   0
#define ADDICON 1
#define SPACING 30
#define ICONWIDTH 96
#define ICONHEIGHT 96
#define ICONNUM 3

//QPoint gap;

//DirAddItem::DirAddItem(const QString &text, QWidget *parent)
//    : QWidget(parent), _text(text), _trembling(0)
//{
//    setAcceptDrops(true);

//    int width, height;
//    QFontMetrics fm(QFont("", FONTSIZE, QFont::Black));

//    width = ICONWIDTH + CLOSEWIDTH/2 + 8;
//    height = ICONHEIGHT + CLOSEHEIGHT/2 + fm.height() * 2;
//    setFixedSize(width, height);
//    _textWidth = fm.width(_text);

//    _texticon = _text;

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
////    qDebug() << "secondrow : " << _texticon_thirdrow;

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

////    _app = LocalAppList::getList()->getAppByName(_text);
//    _animation = new QPropertyAnimation(this, "geometry");
//    _animation->setDuration(200);
//    _animation->setEasingCurve(QEasingCurve::OutBack);

//    _timeline = new QTimeLine(5000, this);
//    _timeline->setLoopCount(0);
//    _timeline->setUpdateInterval(65);

//    _icontype = APPICON;

////    _hoverDirAddItem = new HoverIconItem(this->width(), this->height(), this);
////    _hoverDirAddItem->setVisible(false);

//    _pixmap.load(":images/dir_add_icon.png");

//    _openAction = new QAction(tr("打开"), this);
//    connect(_openAction, SIGNAL(triggered()), this, SLOT(openClicked()));
//    _delAction = new QAction(tr("删除"), this);
//    connect(_delAction, SIGNAL(triggered()), this, SLOT(delClicked()));


//    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(doTremble(qreal)));
////    if (parent) {

////            connect(static_cast<VirtualDesktop*>(parent), \
////                    SIGNAL(trembleStarted()), \
////                    this, SLOT(startTremble()));
////            connect(static_cast<VirtualDesktop*>(parent), \
////                    SIGNAL(trembleStoped()), \
////                    this, SLOT(stopTremble()));
////    }
//}

//DirAddItem::~DirAddItem()
//{
//    //if (_style == VirtualDesktop::DirIcon)
//        //delete _delAction;

////    delete _openAction;
////    delete _hoverDirAddItem;

////    delete _timeline;
////    delete _animation;
//}

//void DirAddItem::setPage(int page)
//{
//    _page = page;
////    _app->setPage(page);
//}
//void DirAddItem::setIndex(int index)
//{
//    _index= index;
////   _app->setIndex(index);
//}
//void DirAddItem::setHidden(bool hide)
//{
////    _app->setHidden(hide);
//}

//void DirAddItem::animationMove(const QRect &start, const QRect &end)
//{
//    _animation->setStartValue(start);
//    _animation->setEndValue(end);
//    _animation->start();
//}

//void DirAddItem::doTremble(qreal value)
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

//void DirAddItem::startTremble()
//{
//    _timeline->start();
//    _pixmap = _closePixmap;
//}

//void DirAddItem::stopTremble()
//{
//    _timeline->stop();
//    _trembling = 0;
//    _pixmap = _normalPixmap;
//    repaint();
//}

//void DirAddItem::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);
////    if (_trembling)
////        painter.translate(width()/2, height()/2);
////    painter.rotate(2*_trembling);
////    if (_trembling)
////        painter.translate(-1*width()/2, -1*height()/2);

//    painter.drawPixmap(3, 0, _pixmap);

//    QFont font(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal);
//    QFontMetrics fm(font);
//    _textWidth_firstrow = fm.width(_texticon_firstrow);
//    QString tx = _texticon_firstrow;
//    painter.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_firstrow) / 2, height() - _textHeight * 3 - 8, _textWidth_firstrow, _textHeight), Qt::TextSingleLine, tx);
//    _textWidth_secondrow = fm.width(_texticon_secondrow);
//    tx = _texticon_secondrow;
//    painter.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_secondrow) / 2, height() - _textHeight * 2 - 8, _textWidth_secondrow, _textHeight), Qt::TextSingleLine, tx);
//    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
//    tx = _texticon_thirdrow;
//    painter.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_thirdrow) / 2, height() - _textHeight - 8, _textWidth_thirdrow, _textHeight), Qt::TextSingleLine, tx);

//    QWidget::paintEvent(event);
//}

//void DirAddItem::mousePressEvent(QMouseEvent *event)
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

//    emit openItem(page(), index());

////    event->ignore();

//}

//void DirAddItem::mouseDoubleClickEvent(QMouseEvent *event)
//{
////    if ((event->button() == Qt::LeftButton) && (_timeline->state() != QTimeLine::Running))
////    {
////        openClicked();
////    }
////    emit openItem(page(), index());
//}

//void DirAddItem::mouseMoveEvent(QMouseEvent *event)
//{
////    if (!(event->buttons() & Qt::LeftButton))
////        return;
////    if ((event->pos() - dragStartPosition).manhattanLength() \
////            < QApplication::startDragDistance()) {
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

//void DirAddItem::contextMenuEvent(QContextMenuEvent *event)
//{
//    Q_UNUSED(event);


//    QCursor cur=this->cursor();
//    QMenu *DirAddItemcontextMenu = new QMenu(this);

//    DirAddItemcontextMenu->addAction(_openAction);
//    DirAddItemcontextMenu->addAction(_delAction);


//    DirAddItemcontextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);

//    DirAddItemcontextMenu->exec(cur.pos());

//    return;
//}


//void DirAddItem::enterEvent(QEvent *event)
//{
//    _pixmap = darkPixmap();
//////    _hoverIconItem->setVisible(true);

//    repaint();

//    Q_UNUSED(event);
//}

//void DirAddItem::leaveEvent(QEvent *event)
//{

//    if (_timeline->state() == QTimeLine::Running)
//        _pixmap = _closePixmap;
//    else
//        _pixmap = _normalPixmap;

////    _hoverIconItem->setVisible(false);
//    repaint();

//    Q_UNUSED(event);
//}

//const QPixmap& DirAddItem::originPixmap()
//{
//    return _originPixmap;
//}

//void DirAddItem::setPixmap(const QString &icon)
//{
////    _pixmap.load(icon);
////    update();
//    int begin;

//    QString text = _text;
//    if (isUserType())
//        text = _text.right(_text.length() - 1);
//    _originPixmap = QPixmap(icon);
//    QImage image = QImage(icon).scaled(ICONWIDTH, ICONHEIGHT);
//    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);
//    QPainter pt1(&normal);
//    pt1.setPen(Qt::white);
//    //pt1.setFont(QFont("", FONTSIZE, QFont::Black));
//    pt1.setFont(QFont("", FONTSIZE, QFont::Normal));
//    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
//    pt1.setCompositionMode(QPainter::CompositionMode_Source);
//    pt1.fillRect(normal.rect(), Qt::transparent);
//    pt1.drawImage(CLOSEWIDTH/2 - 3, CLOSEHEIGHT/2, image);

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
//    /*
//    pt2.drawText( QRect(begin, ICONHEIGHT + CLOSEHEIGHT/2 , \
//                        width(), _textHeight), Qt::TextSingleLine, \
//                  text);
//    */
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
////    painter.drawImage(0, 5, closeImage);

//    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
//    painter.end();
//    _closePixmap = QPixmap::fromImage(resultImage);

//}

//void DirAddItem::setText(const QString &text)
//{
//    _text = text;
//}

//bool DirAddItem::isUserType()
//{
//    if (_text.startsWith("/"))
//        return true;
//    return false;
//}

//const QString & DirAddItem::text()
//{
//    return _text;
//}

//const QPixmap & DirAddItem::pixmap()
//{
//    return _pixmap;
//}

//const QPixmap & DirAddItem::grayPixmap()
//{
//    return _grayPixmap;
//}

//const QPixmap & DirAddItem::darkPixmap()
//{
//    if (_timeline->state() == QTimeLine::Running)
//        return _closePixmap;
//    return _darkPixmap;
//}

//void DirAddItem::addApp()
//{

//}

//void DirAddItem::openClicked()
//{
//    emit openItem(page(), index());
//}

//void DirAddItem::delClicked()
//{
//    emit delItem(_text);
//}
///*****
//void DirAddItem::dragEnterEvent(QDragEnterEvent* event)
//{
//    qDebug() << "****************************************";
//  　 event->accept( QTextDrag::canDecode(event)||QImageDrag::canDecode(event));
//}//当拖动在这个窗口部件上被放下，这个事件处理器被调用

//void DirAddItem::dropEvent(QDropEvent* event)  {
//    qDebug() << "132123132132132132132132132132132132132132131321";
//    QImage image;
//    QString text;
//    if ( QDrag::decode(event, image) )
//    {//解码图片
//        insertImageAt(image, event->pos());　//在窗口部件中插入图片
//    }
//    else if ( QTextDrag::decode(event, text) )
//    {
//        insertTextAt(text, event->pos());
//    }
//}



//******/////
//
DirShowWidget::DirShowWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
{
//    _rightTopPix.load(":/images/bs_rightbg_top.png");
//    _rightCenterPix.load(":/images/bs_rightbg_center.png");
//    _rightBottomPix.load(":/images/bs_rightbg_bottom.png");

    _width = pageSize.width();
    _height = pageSize.height();

    resize(_width, _height);

    _dirWidget = new DirWidget(pageSize, this);
    _dirWidget->setGeometry(0,0, _width - 5, _height - 1);
    _dirWidget->setVisible(true);

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


    _scrollBar->setSingleStep(_dirWidget->pageSize().height());
    _scrollBar->setPageStep(_dirWidget->pageSize().height());

    if (_dirWidget->count() > 1)
        _scrollBar->setVisible(true);
    else
        _scrollBar->setVisible(false);

    _animation = new QPropertyAnimation(_dirWidget, "geometry");

    connect(_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
    connect(_dirWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));

}

DirShowWidget::~DirShowWidget()
{

}

void DirShowWidget::scrollBarValueChanged(int val)
{
    if (val % _dirWidget->pageSize().height() != 0)
    {
        return;
    }

    if (_animation->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animation->setDuration(80);
    _animation->setStartValue(QRect(0, _dirWidget->pos().y(), _dirWidget->pageSize().width(), _dirWidget->pageSize().height()));
    _animation->setEndValue(QRect(0, -1 * val, _dirWidget->pageSize().width(), _dirWidget->pageSize().height()));

    _animation->start();
}

void DirShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    painter.drawPixmap(0, 0, _width, 9,\
//                       _rightTopPix.scaled(_width, 9));
//    painter.drawPixmap(0, 9, _width, _height - 18,\
//                       _rightCenterPix.scaled(_width, _height - 18));
//    painter.drawPixmap(0, _height - 9, _width, 9,\
//                       _rightBottomPix.scaled(_width, 9));

    painter.drawPixmap(0, 0, _width, _height, QPixmap(":images/dir_bg.png").scaled(_width, _height, \
                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QWidget::paintEvent(event);
}

void DirShowWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    _width = width();
    _height = height();

    if (_dirWidget->count() > 1)
        _scrollBar->setVisible(true);
    else
        _scrollBar->setVisible(false);

    int w = 15;
    int x = width() - w;
    x = x < 0 ? 0: x;
    int h = height();
    _scrollBar->setGeometry(x, 8, w, h - 16);

    _scrollBar->setRange(0, _dirWidget->count() * _dirWidget->pageSize().height() - h);

    update();
}

void DirShowWidget::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
    _scrollBar->event(event);
}

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


//
DirItem::DirItem(const QString &text, QWidget *parent)
    : QWidget(parent)
    , _text(text)
    , _trembling(0)
    ,_equal(false)
{
    int width, height;
    QFontMetrics fm(QFont("", FONTSIZE, QFont::Black));

    width = ICONWIDTH + CLOSEWIDTH/2 + 8;
    height = ICONHEIGHT + CLOSEHEIGHT/2 + fm.height() * 2;
    setFixedSize(width, height);
    _textWidth = fm.width(_text);

    if (_text.startsWith("/")) {
        _texticon = _text.right(_text.length() - 1);
        _textWidth = fm.width(_text.right(_text.length() - 1));
    }
    else
        _texticon = _text;

//    _texticon = _text;
    _texticon_firstrow = _texticon;
    _texticon_secondrow = _texticon;
    _texticon_thirdrow = _texticon;

    int i;
    for (i = 0; i < _textWidth; i++)
    {
        _textWidth_firstrow = fm.width(_texticon_firstrow);

        if (_textWidth_firstrow > width - 20)
        {
            _texticon_firstrow.chop(1);
        }
        else{
            //_texticon.chop(1);
            //_texticon.append("...");
            break;
        }
    }
    _texticon_secondrow = _texticon.right(_texticon.length() - _texticon_firstrow.length());

    _textWidth_secondrow = fm.width(_texticon_secondrow);
    for (i = 0; i < _textWidth_secondrow; i++)
    {
        _textWidth_secondrow = fm.width(_texticon_secondrow);

        if (_textWidth_secondrow > width - 20)
        {
            _texticon_secondrow.chop(1);
        }
        else{
            //_texticon.chop(1);
            //_texticon.append("...");
            break;
        }
    }
    _texticon_thirdrow = _texticon.right(_texticon.length() - _texticon_firstrow.length() - _texticon_secondrow.length());

    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
    if (_textWidth_thirdrow > width)
    {
        for (i = 0; i < _textWidth_thirdrow; i++)
        {
            _textWidth_thirdrow = fm.width(_texticon_thirdrow);
            if (_textWidth_thirdrow > width - 20)
            {
                _texticon_thirdrow.chop(1);
            }else{
                _texticon_thirdrow.chop(3);
                _texticon_thirdrow.append("...");
                break;
            }
        }
    }

    _textHeight = fm.height();

//    _app = LocalAppList::getList()->getAppByName(_text);
//    _animation = new QPropertyAnimation(this, "geometry");
//    _animation->setDuration(200);
//    _animation->setEasingCurve(QEasingCurve::OutBack);

//    _timeline = new QTimeLine(5000, this);
//    _timeline->setLoopCount(0);
//    _timeline->setUpdateInterval(65);

    _icontype = APPICON;

//    _hoverDirItem = new HoverIconItem(this->width(), this->height(), this);
//    _hoverDirItem->setVisible(false);

//    _openAction = new QAction(tr("运行"), this);
//    connect(_openAction, SIGNAL(triggered()), this, SLOT(openClicked()));

//    if (_style == VirtualDesktop::DirIcon)
//    {
//        _delAction = new QAction(tr("删除"), this);
//        connect(_delAction, SIGNAL(triggered()), this, SLOT(delClicked()));
//    }

//    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(doTremble(qreal)));
//    if (parent) {
//        if (_style == VirtualDesktop::DirIcon)
//        {
//            connect(static_cast<DirWidget*>(parent), \
//                    SIGNAL(trembleStarted()), \
//                    this, SLOT(startTremble()));
//            connect(static_cast<DirWidget*>(parent), \
//                    SIGNAL(trembleStoped()), \
//                    this, SLOT(stopTremble()));
//        }
//    }
}

DirItem::~DirItem()
{
//    delete _openAction;
//    delete _hoverDirItem;

//    delete _timeline;
//    delete _animation;
}

void DirItem::setPage(int page)
{
    _page = page;
//    _app->setPage(page);
}
void DirItem::setIndex(int index)
{
    _index= index;
//   _app->setIndex(index);
}
void DirItem::setHidden(bool hide)
{
//    _app->setHidden(hide);
}

void DirItem::animationMove(const QRect &start, const QRect &end)
{
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _animation->start();
}

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

//void DirItem::startTremble()
//{
//    _timeline->start();
//    _pixmap = _closePixmap;
//}

//void DirItem::stopTremble()
//{
//    _timeline->stop();
//    _trembling = 0;
//    _pixmap = _normalPixmap;
//    repaint();
//}

void DirItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
//    if (_trembling)
//        painter.translate(width()/2, height()/2);
//    painter.rotate(2*_trembling);
//    if (_trembling)
//        painter.translate(-1*width()/2, -1*height()/2);

    painter.drawPixmap(3, 0, _pixmap);

//    painter.drawPixmap(width() - 50, height() - 80, _closePixmap);

    QWidget::paintEvent(event);
}

void DirItem::mousePressEvent(QMouseEvent *event)
{
//    if (_timeline->state() == QTimeLine::Running) {
//        if (QRect(0, 5, 30, 35).contains(event->pos())) {
//            delClicked();
//            return;
//        }
//    }

//    if (event->button() == Qt::LeftButton && \
//            geometry().contains(event->pos())) {
//        dragStartPosition = event->pos();
//    }
//    emit sendUrl(_url);
//    event->ignore();

}

void DirItem::mouseDoubleClickEvent(QMouseEvent *event)
{
//    if ((event->button() == Qt::LeftButton))
//    {
//        emit sendUrl(_url);
//    }
    if (!_url.isEmpty())
        emit sendUrl(_url);
}

void DirItem::mouseMoveEvent(QMouseEvent *event)
{
//    if (!(event->buttons() & Qt::LeftButton))
//        return;
//    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
//        return;
//    }
//    _drag = new QDrag(this);
//    QMimeData *mimeData = new QMimeData;
//    mimeData->setText(_text);
//    _drag->setMimeData(mimeData);
//    _drag->setPixmap(grayPixmap());
//    _drag->setHotSpot(event->pos());
//    gap = _drag->hotSpot();
//    _drag->exec(Qt::MoveAction);
}

//void DirItem::contextMenuEvent(QContextMenuEvent *event)
//{
//    Q_UNUSED(event);


//    QCursor cur=this->cursor();
//    QMenu *iconItemcontextMenu = new QMenu(this);

//    iconItemcontextMenu->addAction(_openAction);

////    if (_style == VirtualDesktop::DirIcon)
////    {
//        iconItemcontextMenu->addAction(_delAction);
////    }

//    iconItemcontextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);

//    iconItemcontextMenu->exec(cur.pos());

//    return;
//}


void DirItem::enterEvent(QEvent *event)
{
    _pixmap = darkPixmap();
//    _hoverIconItem->setVisible(true);
    repaint();

    Q_UNUSED(event);
}

void DirItem::leaveEvent(QEvent *event)
{

//    if (_timeline->state() == QTimeLine::Running)
//        _pixmap = _closePixmap;
//    else
        _pixmap = _normalPixmap;

//    _hoverIconItem->setVisible(false);
    repaint();

    Q_UNUSED(event);
}

const QPixmap& DirItem::originPixmap()
{
    return _originPixmap;
}

void DirItem::setPixmap(const QString &icon)
{
//    int begin;
    QString text = _text;
    if (isUserType())
        text = _text.right(_text.length() - 1);
    _originPixmap = QPixmap(icon);
    QImage image = QImage(icon).scaled(ICONWIDTH, ICONHEIGHT);
    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);
    QPainter pt1(&normal);
    pt1.setPen(Qt::white);
    //pt1.setFont(QFont("", FONTSIZE, QFont::Black));
    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
    pt1.setCompositionMode(QPainter::CompositionMode_Source);
    pt1.fillRect(normal.rect(), Qt::transparent);
    pt1.drawImage(CLOSEWIDTH/2 - 3, CLOSEHEIGHT/2, image);

    //QFont font("", FONTSIZE, QFont::Normal);
    QFont font(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal);
    QFontMetrics fm(font);
    _textWidth_firstrow = fm.width(_texticon_firstrow);
    QString tx = _texticon_firstrow;
    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_firstrow) / 2, height() - _textHeight * 3 - 8, _textWidth_firstrow, _textHeight), Qt::TextSingleLine, tx);
    _textWidth_secondrow = fm.width(_texticon_secondrow);
    tx = _texticon_secondrow;
    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_secondrow) / 2, height() - _textHeight * 2 - 8, _textWidth_secondrow, _textHeight), Qt::TextSingleLine, tx);
    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
    tx = _texticon_thirdrow;
    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_thirdrow) / 2, height() - _textHeight - 8, _textWidth_thirdrow, _textHeight), Qt::TextSingleLine, tx);
    pt1.end();

    QImage light = QImage(width(), height(), QImage::Format_ARGB32);
    QImage dark =  QImage(width(), height(), QImage::Format_ARGB32);

    for (int i = 0; i < width(); i++) {
        for (int j = 0; j < height(); j++) {
            QRgb pixel = normal.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                    qBlue(pixel), a/2);
            light.setPixel(i, j, lightPixel);


            QRgb darkPixel = qRgba(qRed(pixel)*0.8, qGreen(pixel)*0.8, \
                                   qBlue(pixel)*0.8, a);
            dark.setPixel(i, j, darkPixel);
        }
    }
    QPainter pt2(&dark);
    pt2.setPen(Qt::white);
    pt2.setFont(QFont("", FONTSIZE, QFont::Black));
    pt2.setRenderHint(QPainter::HighQualityAntialiasing);

    pt2.end();
    _grayPixmap = QPixmap::fromImage(light).scaled(width() * 1.0, height() * 1.0);
    _darkPixmap = QPixmap::fromImage(dark);
    _normalPixmap = QPixmap::fromImage(normal);
    _pixmap = _normalPixmap;
    QImage resultImage(width(), height(),  \
                       QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(resultImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, normal);

//    QImage closeImage(":images/close_icon.png");
//    painter.drawImage(width() - 20, height() -50, closeImage);

    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.end();
    //_closePixmap = QPixmap::fromImage(resultImage);

}

void DirItem::setText(const QString &text)
{
    _text = text;
}

bool DirItem::isUserType()
{
    if (_text.startsWith("/"))
        return true;
    return false;
}

const QString & DirItem::text()
{
    return _text;
}

const QPixmap & DirItem::pixmap()
{
    return _pixmap;
}

const QPixmap & DirItem::grayPixmap()
{
    return _grayPixmap;
}

const QPixmap & DirItem::darkPixmap()
{
//    if (_timeline->state() == QTimeLine::Running)
//        return _closePixmap;
    return _darkPixmap;
}

void DirItem::openClicked()
{
    emit runItem(_text);
}

void DirItem::delClicked()
{
    emit delItem(_text);
}

void DirItem::setEqualIcon(bool equal)
{
    if (equal)
        _closePixmap.load(":/images/close_icon.png");
    else
        _closePixmap.load("");

    update();
}

//
DirWidget::DirWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _localCount(0)
    , _iconNum(0)
{

    _count = 1;
    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = _pageSize.height();

    gridWidth = ICONWIDTH + SPACING * 2 - 20;
    gridHeight = ICONHEIGHT + SPACING * 2 - 20;
    _col = _width / gridWidth;
    _row = _height / gridHeight;
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
//        qDebug() << "*****************************************" << tmp.value("").toString()<< "****************";
////        addIcon(tmp.value("").toString(), addLocalApp(tmp.value("").toString()), -1, -1);

//    qDebug() <<"**********************************************************"<<reg.childGroups().count();

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

            int x = (j % _col) * gridWidth;
            int y = _pageSize.height() * i \
                    + (j / _col) * gridHeight;

            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);

    }

    for (int i = 0; i < _count; i++) {
        QList<DirItem*> newList;
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
//        qDebug() << "*****************************************" << tmp.value("").toString()<< "****************";
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

int DirWidget::addIcon(QString text, \
                            const QString &iconPath, \
                            int page, \
                            int index)
{

    int expandPageCount = _iconNum / _iconsPerPage + 1;
    if (expandPageCount > _count)
        expand();

    DirItem *icon = new DirItem(text, this);

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

    icon->setPixmap(iconPath);
    icon->setGeometry(_gridTable[page][index].translated(SPACING, SPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(_url);
    _iconDict.insert(text, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

    _iconNum ++;
    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
//    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));


     return page;
    //LocalAppList::getList()->save();
}

void DirWidget::expand()
{
    qDebug() << "asdlkfjalksjdflkajsdflkjal;sdjf;lkajsdfljasldkfjalk;sjdflasdfas";
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
                    + (i % _col) * (gridWidth + SPACING);
            int y = (i / _col) * (gridHeight + SPACING);
        newGridList.insert(i, \
                           QRect(x, y, gridWidth, gridHeight));
    }
    _gridTable.append(newGridList);
    QList<DirItem*> newIconList;
    for (int i = 0; i < _col * _row; i++)
        newIconList.insert(i, NULL);
    _iconTable.append(newIconList);
    _nextIdx.append(0);
//    emit(pageIncreased());
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
            DirItem *icon = _iconTable[i][j];
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
//    emit pageDecreased();
}

void DirWidget::delIcon(const QString &text)
{

    DirItem *icon = _iconDict.take(text);
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
    qDebug() <<" appPath "<< appPath;
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

