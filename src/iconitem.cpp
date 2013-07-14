#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QMenu>
#include <QImage>
#include <QtDebug>

#include "iconitem.h"

#define APPICON 0
#define FONTSIZE 10
#define ICONX 35
#define ICONY 36
#define SELECTWIDTH 37
#define SELECTHEIGHT 37

#define LARGESIZE QSize(143, 143)
#define MEDIUMSIZE QSize(119, 119)
#define SMALLSIZE QSize(103, 103)

#define SMALL_L_SIZE QSize(42, 42)    //72*72
#define SMALL_M_SIZE QSize(36, 36)    //48*48
#define SMALL_S_SIZE QSize(30, 30)    //32*32

static QPoint gap;

IconItem::IconItem(QWidget *parent)
    : QWidget(parent)
    , _trembling(0)
    , _id(-1)
    , _selectePixBool(false)
    , _closePixBool(false)
    , _trembleBool(false)
    , _dragEventBool(false)
    , _equalBool(false)
    , _runBool(false)
    , _delBool(false)
    , _propertyAnimationBool(false)
    , _timeLineBool(false)
    , _doubleClickeBool(false)
    , _contextMenuBool(false)
    , _enterEventBool(false)
    , _leaveEventBool(false)
    , _saveDataBool(false)
    , _pixText("")
    , _type(0)
    , _url("")
    , _text("")
    , _page(-1)
    , _index(-1)
    , _dirId(-1)
    , _dirMinShowWidget(NULL)
{
    _shadowPixmap.load(":/images/icon_shadow.png");
    _selectPixmap.load("");
    _pixmap.load("");

//       if (parent) {
//            connect(static_cast<VirtualDesktop*>(parent), \
//                    SIGNAL(trembleStarted()), \
//                    this, SLOT(startTremble()));
//            connect(static_cast<VirtualDesktop*>(parent), \
//                    SIGNAL(trembleStoped()), \
//                    this, SLOT(stopTremble()));
//        }

}

IconItem::~IconItem()
{

}

void IconItem::setId(int id)
{
    _id = id;
}

void IconItem::setPage(int page)
{
    _page = page;

    if (_saveDataBool)
        _app->setPage(page);
}

void IconItem::setIndex(int index)
{
    _index = index;
    if (_saveDataBool)
        _app->setIndex(index);
}

void IconItem::setDirId(int dirId)
{
    _dirId = dirId;

    if (_saveDataBool)
        _app->setDirId(_dirId);
}

void IconItem::setHidden(bool hide)
{
//    _app->setHidden(hide);
}

void IconItem::setType(int type)
{
    _type = type;
}

void IconItem::setUrl(const QString &url)
{
    _url = url;
}

void IconItem::setIsRmote(bool isRemote)
{
    _isRemote = isRemote;
}

void IconItem::setText(const QString &text)
{
    if (text.isEmpty())
        return;
    _text = text;

    _app = LocalAppList::getList()->getAppByName(_text);

    QFontMetrics fm(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));

    _textWidth = fm.width(_text);

    if (_text.startsWith("/")) {
        _texticon = _text.right(_text.length() - 1);
        _textWidth = fm.width(_text.right(_text.length() - 1));
    }
    else
        _texticon = _text;

    if (_textWidth > _width)
    {
        for (int i = 0; i < _textWidth; i++)
        {
            _textWidth_firstrow = fm.width(_texticon);
            if (_textWidth_firstrow > _width)
            {
                _texticon.chop(1);
            }else{
                _texticon.chop(3);
                _texticon.append("...");
                break;
            }
        }
    }

    _textHeight = fm.height();
}

void IconItem::setTimeLine(bool timeLine)
{
    _timeLineBool = timeLine;
    if (!_timeLineBool)
        return;

    _timeline = new QTimeLine(5000, this);
    _timeline->setLoopCount(0);
    _timeline->setUpdateInterval(65);
    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(doTremble(qreal)));
}
void IconItem::setPropertyAnimation(bool propertyAnimation)
{
    _propertyAnimationBool = propertyAnimation;
    if (!_propertyAnimationBool)
        return;

    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setDuration(200);
    _animation->setEasingCurve(QEasingCurve::OutBack);
}

void IconItem::setRunAction(bool runItem)
{
    _runBool = runItem;
    if (!_runBool)
        return;

    _runAction = new QAction(tr("����"), this);
    connect(_runAction, SIGNAL(triggered()), this, SLOT(runClicked()));
}

void IconItem::setDelAction(bool delItem)
{
    _delBool = delItem;
    if (!_delBool)
        return;

    _delAction = new QAction(tr("ɾ��"), this);
    connect(_delAction, SIGNAL(triggered()), this, SLOT(delClicked()));
}


void IconItem::setLargeSize()
{
    _width = LARGESIZE.width();
    _height = LARGESIZE.height();

    _iconWidth = 72;
    _iconHeight = 72;

    setFixedSize(_width, _height);

    repaint();
}

void IconItem::setMediumSize()
{
    _width = MEDIUMSIZE.width();
    _height = MEDIUMSIZE.height();

    _iconWidth = 60;
    _iconHeight = 60;

    setFixedSize(_width, _height);

    repaint();
}

void IconItem::setSmallSize()
{
    _width = SMALLSIZE.width();
    _height = SMALLSIZE.height();

    _iconWidth = 52;
    _iconHeight = 52;

    setFixedSize(_width, _height);

    repaint();
}

void IconItem::animationMove(const QRect &start, const QRect &end)
{
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _animation->start();
}

void IconItem::doTremble(qreal value)
{
    Q_UNUSED(value);

#define LEFT -1
#define RIGHT 1
    static int direction = LEFT;
    if (_trembling == -1) {
        _trembling = 0;
        direction = RIGHT;
    }
    else if (_trembling == 0) {
        if (direction == LEFT) {
            _trembling = -1;
        } else {
            _trembling = 1;
        }
    }
    else {
        _trembling = 0;
        direction = LEFT;
    }
    repaint();
#undef LEFT
#undef RIGHT
}

void IconItem::startTremble()
{
    if (!_trembleBool)
        return;

    _timeline->start();
    _pixmap = _closePixmap;
}

void IconItem::stopTremble()
{
    if (!_trembleBool)
        return;

    _timeline->stop();
    _trembling = 0;
    _pixmap = _normalPixmap;
    repaint();
}

void IconItem::paintEvent(QPaintEvent *event)
{
    if(_type == 3)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (_trembleBool)
    {
        if (_trembling)
            painter.translate(width()/2, height()/2);
        painter.rotate(2*_trembling);
        if (_trembling)
            painter.translate(-1*width()/2, -1*height()/2);
    }

    painter.drawPixmap(3, 0, _pixmap);
//    painter.drawPixmap(-1 * (_shadowPixmap.width() - _width) / 2
//    , -1 * (_shadowPixmap.width() - _width) / 2, QPixmap(":/images/icon_shadow.png"));
    if (_equalBool)
        painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT, _selectPixmap);
    else
        painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT, QPixmap(""));
    QWidget::paintEvent(event);

}

void IconItem::mousePressEvent(QMouseEvent *event)
{
    if(_closePixBool)
    {
        if (_timeline->state() == QTimeLine::Running)
        {
            if (QRect(0, 5, 30, 35).contains(event->pos()))
            {
                delClicked();
                return;
            }
        }
    }

    if (_dragEventBool)
    {
        if (event->button() == Qt::LeftButton && geometry().contains(event->pos()))
        {
            _dragStartPosition = event->pos();
        }
    }

    if(_selectePixBool)
    {
        if (event->button() == Qt::LeftButton)
        {
            if (QRect(ICONX, ICONY, _iconWidth, _iconHeight).contains(event->pos())) {
                if (!_equalBool)
                {
                    setEqualIcon(true);

                    emit addApp(_text, _pixText, _url , _type);
                }
            }
        }
    }

    event->ignore();
}

void IconItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!_doubleClickeBool)
    {
        QWidget::mouseDoubleClickEvent(event);
        return;
    }

    if (!_url.isEmpty())
    {
        qDebug() << _url;
        QDesktopServices::openUrl(QUrl(_url));
        return;
    }
    else
    {
        //if ((event->button() == Qt::LeftButton) && (_timeline->state() != QTimeLine::Running))
        if (event->button() == Qt::LeftButton)
        {
            runClicked();
        }
    }
}

void IconItem::mouseMoveEvent(QMouseEvent *event)
{
    if (!_dragEventBool)
        return;

    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - _dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << _text << _pixText << _page << _index << _url << _type;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("image/x-iconitem", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(grayPixmap());
    drag->setHotSpot(event->pos());
    gap = drag->hotSpot();
    drag->exec(Qt::MoveAction);

}

void IconItem::contextMenuEvent(QContextMenuEvent *event)
{
    if (!_contextMenuBool)
        return;


//    Q_UNUSED(event);

//    QCursor cur=this->cursor();
//    QMenu *iconItemcontextMenu = new QMenu(this);
//    /*
//    QPalette pal = iconItemcontextMenu->palette();
//    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0x00));
//    iconItemcontextMenu->setPalette(pal);

//    iconItemcontextMenu->setStyleSheet("QMenu{color:#FFFFFF;}\
//                                        QMenu::item{color:#FFFFFF;}");
//*/
//    if (_runBool)
//        iconItemcontextMenu->addAction(_runAction);

//    if (_delBool)
//        iconItemcontextMenu->addAction(_delAction);

//    iconItemcontextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);

//    iconItemcontextMenu->exec(cur.pos());

//    _mousePos = event->pos();
//    _iconMenu->move(event->pos());

//    hideMenuWidget();
//    _iconMenu->raise();
//    _iconMenu->setVisible(true);

//    return;

    emit showContextMenu( event->pos(), mapToGlobal(event->pos()), _text);
}

void IconItem::enterEvent(QEvent *event)
{
    if (!_enterEventBool)
    {
        QWidget::enterEvent(event);
        return;
    }

   _pixmap = darkPixmap();
//    _pixmap = _normalPixmap;
    repaint();

}

void IconItem::leaveEvent(QEvent *event)
{
    if (!_leaveEventBool)
    {
        QWidget::leaveEvent(event);
        return;
    }

    if (_timeLineBool)
    {
        if (_timeline->state() == QTimeLine::Running)
            _pixmap = _closePixmap;
        else
            _pixmap = _normalPixmap;

        repaint();
        return;
    }

    _pixmap = _normalPixmap;
    repaint();
}

void IconItem::setPixmap(const QString &icon)
{
    if (icon.isEmpty())
        return;

    if(_type == 3)
        return;

    _pixText = icon;

    QString text = _text;
    if (isUserType())
        text = _text.right(_text.length() - 1);

    _originPixmap = QPixmap(icon);

    QImage image = QImage(icon).scaled(_width, _height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);

    normal.scaled(_width, _height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QFont font(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal);
    QFontMetrics fm(font);
    int textflags = Qt::AlignLeft | Qt::TextExpandTabs;
    QSize textsize = fm.size(textflags, _texticon);

    int margin = 8;
    _textWidth_firstrow = fm.width(_texticon);
    int x = (_width - _textWidth_firstrow) / 2;
    int y = _height - FONTSIZE * 2;
    textsize.setWidth(textsize.width() + 2 * margin);
    textsize.setHeight(textsize.height() + 2 * margin);

    QPainterPath pp(QPointF(x, y));
    qreal px = x, py = y + fm.ascent();
//    //    foreach(const QString& line, text_lines) {
     pp.addText(px, py, font, _texticon);
//    //        py += fm2.lineSpacing();
//    //    }

    QPainterPathStroker pps;
    pps.setCapStyle(Qt::RoundCap);
    pps.setJoinStyle(Qt::RoundJoin);
    pps.setWidth(2);
    QPainterPath path = pps.createStroke(pp).united(pp).simplified();

    QColor glow_color = QColor(50,50,50).lighter(180);

    QPixmap textpixmap(textsize);
    textpixmap.fill(QColor(0, 0, 0, 0));


    QPainter pt1(&normal);
    pt1.setPen(Qt::white);
//    pt1.drawRect(1, 1, _width - 1, _height - 1);
    pt1.setCompositionMode(QPainter::CompositionMode_Source);
    pt1.fillRect(normal.rect(), Qt::transparent);
    pt1.drawImage(QRect(0, 0, _width, _height), image);

//    QFont font("", FONTSIZE, QFont::Normal);
//    QFont font(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal);
//    QFontMetrics fm(font);
//    font.setStyleHint(QFont::Cursive, QFont::PreferBitmap);
//    font.setStyleStrategy(QFont::PreferAntialias);

//    pt1.fillPath(path, glow_color);

//    pt1.setPen(glow_color.lighter(66));
//    pt1.drawPath(path);

//    pt1.setPen(QPen(QColor(Qt::white)));
//    pt1.drawText(x, y
//                 , textpixmap.width(), textpixmap.height()
//                 , textflags, _texticon);

   //    pt1.drawRect(1, 1, _width - 1, _height - 1);

   //    QFont font("", FONTSIZE, QFont::Normal);
//       QFont font(QString::fromLocal8Bit("?�騨��??o��"), FONTSIZE, QFont::Normal);
       font.setStyleHint(QFont::Cursive, QFont::PreferBitmap);
       font.setStyleStrategy(QFont::PreferAntialias);
       pt1.setFont(font);
       pt1.setRenderHint(QPainter::HighQualityAntialiasing);
//    pt1.drawText( QRect((_width - _textWidth_firstrow) / 2,  _height - FONTSIZE * 2,\
//                        _textWidth_firstrow, _textHeight), Qt::TextSingleLine, _texticon);
    pt1.fillPath(path, glow_color);
    pt1.setPen(glow_color.lighter(66));
    pt1.drawPath(path);
    pt1.setPen(QPen(QColor(Qt::white)));
    pt1.drawText(x, y
                 , textpixmap.width(), textpixmap.height()
                 , textflags, _texticon);
       pt1.end();

    QImage light = QImage(_width, _height, QImage::Format_ARGB32);
    QImage dark =  QImage(_width, _height, QImage::Format_ARGB32);
    QImage transparent = QImage(_width, _height, QImage::Format_ARGB32);

    for (int i = 0; i < width(); i++) {
        for (int j = 0; j < height(); j++) {
            QRgb pixel = normal.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                    qBlue(pixel), a/2);
            light.setPixel(i, j, lightPixel);

            QRgb darkPixel;
            if (j < height() - FONTSIZE * 2)
            {
                darkPixel = qRgba(qRed(pixel) * 0.8, qGreen(pixel)* 0.8, \
                                       qBlue(pixel)* 0.8, a);
            }
            else
            {
                //darkPixel = qRgba(0, 0, \
                //                    0, 0);
				 darkPixel = pixel;                
            }
            dark.setPixel(i, j, darkPixel);

            QRgb transparentPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                   qBlue(pixel), 50);
            transparent.setPixel(i, j, transparentPixel);
        }
    }

#if 0
    //��ӰЧ��
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
        effect->setBlurRadius(7);
        effect->setColor(QColor(19,29,31));
        effect->setOffset(1,1);
        ui->label->setGraphicsEffect(effect)
#endif

#if 0
    QPainter pt2(&dark);

    QFont font2(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal);
    font2.setStyleStrategy(QFont::PreferOutline);
    pt2.setFont(font);

        pt2.setPen(Qt::white);
 //       pt2.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Light));
        pt2.setRenderHint(QPainter::HighQualityAntialiasing);
        pt2.drawText(QRect((_width - _textWidth_firstrow) / 2,  _height - FONTSIZE * 2,\
                            _textWidth_firstrow, _textHeight), Qt::TextSingleLine, _texticon);
        pt2.end();
#endif
//        pt2.setPen(Qt::white);
//        pt2.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Light));
//        pt2.setRenderHint(QPainter::HighQualityAntialiasing);
//        pt2.drawText(QRect((_width - _textWidth_firstrow) / 2,  _height - FONTSIZE * 2,\
//                            _textWidth_firstrow, _textHeight), Qt::TextSingleLine, _texticon);
//        pt2.end();
//    QStringList text_lines = _texticon;
//    QString text = _texticon.join("n");
//   QFont font2(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal);
//    QFontMetrics fm2(font2);
//    int textflags = Qt::AlignLeft | Qt::TextExpandTabs;
//    QSize textsize = fm2.size(textflags, _texticon);

//    int margin = 8;
//    int x = (_width - _textWidth_firstrow) / 2;
//    int y = _height - FONTSIZE * 2;
//    textsize.setWidth(textsize.width() + 2*margin);
//    textsize.setHeight(textsize.height() + 2*margin);

//    QPainterPath pp(QPointF(x, y));
//    qreal px = x, py = y + fm2.ascent();
////    foreach(const QString& line, text_lines) {
//        pp.addText(px, py, font2, _texticon);
//        py += fm2.lineSpacing();
//    }

//    QPainterPathStroker pps;
//    pps.setCapStyle(Qt::RoundCap);
//    //pps.setJoinStyle(Qt::MiterJoin);
//    pps.setWidth(2);
//    QPainterPath path = pps.createStroke(pp).united(pp).simplified();

//    QColor glow_color = QColor(50,50,50).lighter(180);

//    QPixmap textpixmap(textsize);
//    textpixmap.fill(QColor(0, 0, 0, 0));

    QPainter pt2(&dark);
    pt2.setFont(font);

    pt2.fillPath(path, glow_color);

    pt2.setPen(glow_color.lighter(66));
    pt2.drawPath(path);

    pt2.setPen(QPen(QColor(Qt::white)));
    pt2.drawText(x, y, textpixmap.width(), textpixmap.height()
                       , textflags, _texticon);
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
    if (_closePixBool)
    {
        QImage closeImage(":images/close_icon.png");
        painter.drawImage(0, 5, closeImage);
    }
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.end();
    _closePixmap = QPixmap::fromImage(resultImage);
}

bool IconItem::isUserType()
{
    if (_text.startsWith("/"))
        return true;
    return false;
}

const QPixmap& IconItem::originPixmap()
{
    return _originPixmap;
}

const QString & IconItem::text()
{
    return _text;
}

const QPixmap & IconItem::pixmap()
{
    return _pixmap;
}

const QPixmap & IconItem::grayPixmap()
{
    return _grayPixmap;
}

const QPixmap & IconItem::darkPixmap()
{
    if (_timeLineBool)
    {
        if (_timeline->state() == QTimeLine::Running)
            return _closePixmap;
    }

    return _darkPixmap;
}

void IconItem::runClicked()
{
    emit runItem(_text);
}

void IconItem::delClicked()
{
    emit delItem(_text);
}

void IconItem::openDirWidget()
{
    emit openDir(_id, _page, _index);
}

void IconItem::iconDropEvent(const QString &text, const QString &iconPath, int page, int index,
                             const QString &url, int type)
{
    qDebug() << "IconItem::iconDropEvent()" << _id;
    emit iconDrop(_id, text, iconPath, page, index, url, type);
}

void IconItem::setEqualIcon(bool equal)
{
    if (equal)
    {
        _equalBool = true;
        _selectPixmap.load(":/images/select_icon.png");
    }
    else
    {
        _equalBool = false;
        _selectPixmap.load("");
    }

    repaint();
}

void IconItem::addMinWidget()
{
    _dirMinShowWidget = new DirMinShowWidget(this);
    _dirMinShowWidget->move(0,0);

    _dirMinShowWidget->setId(_id);

    connect(_dirMinShowWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dirMinShowWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
    connect(_dirMinShowWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
//        connect(_dirMinShowWidget, SIGNAL(iconDrop(int, const QString &, const QString &, const QString &)),
//                this, SLOT(iconDropEvent(int, const QString &, const QString &, const QString &)));
//        connect(_dirMinShowWidget, SIGNAL(openItem(int, int, int)), this, SLOT(openDirWidget(int, int, int)));
    connect(_dirMinShowWidget, SIGNAL(iconDrop(const QString &, const QString &, int, int, const QString &, int)),
            this, SLOT(iconDropEvent(const QString &, const QString &, int, int, const QString &, int)));
    connect(_dirMinShowWidget, SIGNAL(openItem()), this, SLOT(openDirWidget()));
    connect(_dirMinShowWidget, SIGNAL(dragEnterMinWidget()), SIGNAL(dragEnterMinWidget()));
}

void IconItem::setMinWidgetDragEnable(bool enable)
{
    if (_dirMinShowWidget == NULL)
        return;

    if (enable)
    {
        _dirMinShowWidget->setAcceptDrops(true);
        _dirMinShowWidget->setMinDragEnable(true);
    }
    else
    {
        _dirMinShowWidget->setAcceptDrops(false);
        _dirMinShowWidget->setMinDragEnable(false);
    }
}

void IconItem::removeDirMinItem(const QString &text)
{
    if (_dirMinShowWidget == NULL)
        return;

        _dirMinShowWidget->removeDirMinItem(text);
}

void IconItem::addDirMinItem(const QString &text, const QString &icon, \
                                      int page, int index, const QString &url)
{
    if (_dirMinShowWidget == NULL)
        return;

        _dirMinShowWidget->addDirMinItem(text, icon, page, index, url);
}
int IconItem::getMinIconNum()
{
    return _dirMinShowWidget->getMinIconNum();
}
