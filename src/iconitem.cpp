#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QMenu>
#include <QImage>
#include <QtDebug>
#include <QFontMetrics>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QLabel>
#include "iconitem.h"
#include "dustbin.h"
#define APPICON 0
#define FONTSIZE 10

#define SELECTWIDTH 37
#define SELECTHEIGHT 37
#include "qitemmanager.h"
#define LARGESIZE QSize(143, 143)
#define MEDIUMSIZE QSize(119, 119)
#define SMALLSIZE QSize(103, 103)

#define SMALL_L_SIZE QSize(42, 42)    //72*72
#define SMALL_M_SIZE QSize(36, 36)    //48*48
#define SMALL_S_SIZE QSize(30, 30)    //32*32
#define HOVERALPHA  0
#define SELECTALPHA 25
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
    , _currentStatus(normal)
    , _uniqueName("")
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
    _lineEdit = new DirLineEdit(QString(""),this);
//    _lineEdit->setStyleSheet("QLineEdit{selection-color: white}");
//    _lineEdit->setDisabled(true);
    _editLeftNormal =  QPixmap("");
    _editCenterNormal =  QPixmap("");
    _editRightNormal =  QPixmap("");
    setLineEditBg(_editLeftNormal, _editCenterNormal, _editRightNormal);
    connect(_lineEdit, SIGNAL(focusIn()), this, SLOT(LineEditFocusIn()));
    connect(_lineEdit, SIGNAL(focusOut()), this, SLOT(LineEditFocusOut()));
//    connect(_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(changeItemName(QString)));

}

IconItem::~IconItem()
{

}

void IconItem::setId(int id)
{
    _id = id;

    if (_saveDataBool)
        _app->setId(QString("%1").arg(id));
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

    if (_dirId == 1000)
    {
        //_lineEdit->setReadOnly(true);
        _lineEdit->setEnabled(false);
    }
}
void IconItem::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;

    _app = LocalAppList::getList()->getAppByUniqueName(_uniqueName);

    if (_saveDataBool)
        _app->setUniqueName(_uniqueName);

}
QString IconItem::uniqueName()
{
    return _uniqueName;
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

    switch(_iconSize)
    {
    case 0:
    {
        QFont font1 = QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal);
        QFontMetrics fm1(font1);
        chopText(fm1,108);
//        if(textsize.width() > _textWidth_firstrow)
//            _lineEdit->setAlignment(Qt::AlignRight);
//        else
//            _lineEdit->setAlignment(Qt::AlignCenter);
        break;
    }
    case 1:
    {
        QFont font2 = QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE -1 , QFont::Normal);
        QFontMetrics fm2(font2);
        chopText(fm2,84);
//        if(textsize.width() > _textWidth_firstrow)
//            _lineEdit->setAlignment(Qt::AlignRight);
//        else
//            _lineEdit->setAlignment(Qt::AlignCenter);
        break;
    }
    case 2:
    {
        QFont font3 = QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE - 2 , QFont::Normal);
        QFontMetrics fm3(font3);
        chopText(fm3,65);
//        if(textsize.width() > _textWidth_firstrow)
//            _lineEdit->setAlignment(Qt::AlignRight);
//        else
//            _lineEdit->setAlignment(Qt::AlignCenter);

        break;
    }
    }
    _lineEdit->setText(_texticon);
    _lineEdit->saveFullName(_text);
}
void IconItem::chopText(QFontMetrics fm, int width)
{
    _textWidth = fm.width(_text);

    if (_text.startsWith("/")) {
        _texticon = _text.right(_text.length() - 1);
        _textWidth = fm.width(_text.right(_text.length() - 1));
    }
    else
        _texticon = _text;
    int x = width;
    if (_textWidth > width)
    {
        for (int i = 0; i < _textWidth; i++)
        {
            _textWidth_firstrow = fm.width(_texticon);
            if (_textWidth_firstrow > width)
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

    _runAction = new QAction(tr("ÔËÐÐ"), this);
    connect(_runAction, SIGNAL(triggered()), this, SLOT(runClicked()));
}

void IconItem::setDelAction(bool delItem)
{
    _delBool = delItem;
    if (!_delBool)
        return;

    _delAction = new QAction(tr("É¾³ý"), this);
    connect(_delAction, SIGNAL(triggered()), this, SLOT(delClicked()));
}


void IconItem::setLargeSize()
{
    _width = LARGESIZE.width();
    _height = LARGESIZE.height();

    _iconX = 37;
    _iconY = 36;

    _iconWidth = 72;
    _iconHeight = 72;
    init();
    setFixedSize(_width, _height);

    repaint();
}

void IconItem::setMediumSize()
{
    _width = MEDIUMSIZE.width();
    _height = MEDIUMSIZE.height();

    _iconX = 32;
    _iconY = 30;

    _iconWidth = 60;
    _iconHeight = 60;
    init();
    setFixedSize(_width, _height);

    repaint();
}

void IconItem::setSmallSize()
{
    _width = SMALLSIZE.width();
    _height = SMALLSIZE.height();

    _iconX = 28;
    _iconY = 24;

    _iconWidth = 52;
    _iconHeight = 52;
    init();
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
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont(QString::fromLocal8Bit(" Microsoft YaHei"), FONTSIZE, QFont::Black));
//    if(_currentStatus == normal)
//        painter.drawPixmap(0, 0, width(), height(), _normalBackground.scaled(width(),height()));
//    else if(_currentStatus == hover)
//        painter.drawPixmap(0, 0, width(), height(), _hoverBackgroud.scaled(width(),height()));
    if(_currentStatus == selected)
        painter.drawPixmap(0, 0, width(), height(), _selectedBackgroud.scaled(width(),height()));

    switch(_iconSize)
    {
        case 0 :

            painter.drawPixmap((_width - 140) / 2, _height - 26, \
                              _left.width(), _left.height(),
                              _left);
            painter.drawPixmap((_width + 140) / 2 - _right.width(), _height - 26, \
                              _right.width(), _right.height(),
                              _right);
            painter.drawPixmap((_width - 140) / 2 + _left.width(), _height - 26, \
                              140 - _left.width() - _right.width(), _center.height(),
                              _center);
            _lineEdit->setGeometry( 4 + (width() - 110) / 2, _height - 26, 110, 23);

            break;

        case 1 :

            painter.drawPixmap((_width - 118) / 2, _height - 24, \
                              _left.width(), _left.height() ,
                              _left);
            painter.drawPixmap((_width + 118) / 2 - _right.width(), _height - 24, \
                              _right.width(), _right.height(),
                              _right);
            painter.drawPixmap((_width - 118) / 2 + _left.width(), _height - 24, \
                              118 - _left.width() - _right.width(), _center.height(),
                              _center);
           _lineEdit->setGeometry(4 + (width() - 84) / 2, _height - 24, 84, 18 );
            break;

        default:

            painter.drawPixmap((_width - 102) / 2, _height - 22, \
                              _left.width(), _left.height(),
                              _left);
            painter.drawPixmap((_width + 102) / 2 - _right.width(), _height - 22,\
                                      _right.width(), _right.height(),
                                      _right);
            painter.drawPixmap((_width - 102) / 2 + _left.width(), _height - 22, \
                              102 - _left.width() - _right.width(), _center.height(),
                              _center);
           _lineEdit->setGeometry( 4 + (width() - 65) / 2, _height - 22, 65, 15 );
            break;
    }
    if(_type == 3)
    {
        _lineEdit->setVisible(false);
        return;
    }

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

    switch(_iconSize)
    {
        case 0 :

            if (_equalBool)
                painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT, _selectPixmap);
            else
                painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT, QPixmap(""));

            painter.drawPixmap(_width - 36 - 26, 36, _iconClassPixmap);
            break;

        case 1 :

            if (_equalBool)
                painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT + 6, _selectPixmap);
            else
                painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT + 6, QPixmap(""));

            painter.drawPixmap(_width - 29 - 27, 29 + 1, _iconClassPixmap);
            break;

        default:

            if (_equalBool)
                painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT + 11, _selectPixmap);
            else
                painter.drawPixmap(_width / 4 * 3 - SELECTWIDTH / 2, _height - 25 - SELECTHEIGHT + 11, QPixmap(""));

            painter.drawPixmap(_width - 25 - 27, 25 + 1, _iconClassPixmap);
            break;
    }


    QWidget::paintEvent(event);

}

void IconItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && QItemManager::getManager()->keyAt(0) == 0x01000021)
    {

        bool isSelected = false;
        for(int i=0; i < QItemManager::getManager()->getItemListSize(); i++)
        {
            if(QItemManager::getManager()->itemAt(i) == this)
            {
                isSelected = true;
                break;
            }
            else
                isSelected = false;
        }
        if(isSelected)
        {
            _currentStatus = hover;
            QItemManager::getManager()->deleteRecordIconItem(this);
        }
        else
        {
            _currentStatus = selected;
            this->clearFocus();
            QItemManager::getManager()->saveSelectedIconItem(this);
        }
        update();
//        emit showContextMenu( false,event->pos(), mapToGlobal(event->pos()), _text);
        return;
    }
    if(event->button() == Qt::LeftButton)
    {
        if(_currentStatus == normal)
             return;
        else if(_currentStatus == hover)
        {
            _currentStatus = selected;
            this->setFocus();
//            _lineEdit->clearFocus();
//            _lineEdit->setEnabled(true);
        }
        else if(_currentStatus == selected)
        {
            if(_lineEdit->hasFocus())
                _currentStatus = hover;
        }
        update();
    }
    if(event->button() == Qt::RightButton)
    {
        return;
    }
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
            if (QRect(_iconX, _iconY, _iconWidth, _iconHeight).contains(event->pos())) {
                if (!_equalBool)
                {
                    setEqualIcon(true);

                    emit addApp(_text, _pixText, _url , _type, _uniqueName);
                }
                else
                {
                    //emit delItem(_uniqueName);
                    LocalAppList::getList()->delApp(_uniqueName);
                    setEqualIcon(false);

                }
            }
        }
    }

    event->ignore();
}

void IconItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (QRect(_iconX, _iconY, _iconWidth, _iconHeight).contains(event->pos()))
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
}

void IconItem::mouseMoveEvent(QMouseEvent *event)
{
    if (!_dragEventBool)
        return;

    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (QRect(_iconX, _iconY, _iconWidth, _iconHeight).contains(event->pos()))
    {
        if ((event->pos() - _dragStartPosition).manhattanLength() < QApplication::startDragDistance())
            return;

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << _text << _pixText << _page << _index << _url << _type << _id << _uniqueName;

        _drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("image/x-iconitem", itemData);
        if(_type == 3)
            setDirBackground();

        _drag->setMimeData(mimeData);
        _drag->setPixmap(grayPixmap());
        _drag->setHotSpot(event->pos());
        gap = _drag->hotSpot();
        if (!(_drag->exec(Qt::MoveAction) == Qt::MoveAction)) {
            emit dragOut();
        }
    //    drag->exec(Qt::MoveAction);
        delete _drag;
    }

}

void IconItem::contextMenuEvent(QContextMenuEvent *event)
{
    if (!_contextMenuBool)
        return;

    if (QRect(_iconX, _iconY, _iconWidth, _iconHeight).contains(event->pos()))
    {
        emit showContextMenu( true, event->pos(), mapToGlobal(event->pos()), this);
    }
}

void IconItem::enterEvent(QEvent *event)
{
    if(_currentStatus == normal)
        _currentStatus=hover;
    else if(_currentStatus == selected)
        _currentStatus = selected;
    update();
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
    if(_currentStatus == hover)
    {
        _currentStatus = normal;
//        _lineEdit->setDisabled(true);
        clearFocus();
    }
    else if(_currentStatus == selected && QItemManager::getManager()->containsItem(this))
    {
        _currentStatus = selected;
    }
    update();
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

void IconItem::setPixmap(const QString &icon, const QString &text)
{
    if (icon.isEmpty())
    {
        if(_type == 3)
            setDirBackground();
        return;
    }
    _pixText = icon;

    _originPixmap = QPixmap(icon);

    QImage image = QImage(icon).scaled(_width, _height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);

    normal.scaled(_width, _height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QFont font(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal);
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
//     pp.addText(px, py, font, _texticon);
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
    pt1.drawRect(1, 1, _width - 1, _height - 1);
    pt1.setCompositionMode(QPainter::CompositionMode_Source);
    pt1.fillRect(normal.rect(), Qt::transparent);
    pt1.drawImage(QRect(0, 0, _width, _height), image);

    font.setStyleHint(QFont::Cursive, QFont::PreferBitmap);
    font.setStyleStrategy(QFont::PreferAntialias);
    pt1.setFont(font);
    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
//       pt1.drawText( QRect((_width - _textWidth_firstrow) / 2,  _height - FONTSIZE * 2,\
//                        _textWidth_firstrow, _textHeight), Qt::TextSingleLine, _texticon);
    pt1.fillPath(path, glow_color);
    pt1.setPen(glow_color.lighter(66));
    pt1.drawPath(path);
    pt1.setPen(QPen(QColor(Qt::white)));

    pt1.end();

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(_lineEdit);
    effect->setBlurRadius(8);
    effect->setColor(QColor(0, 0, 0));
    effect->setOffset(-1,1);
    _lineEdit->setGraphicsEffect(effect);
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

    QPainter pt2(&light);
    pt2.setFont(font);
    pt2.setPen(QPen(QColor(Qt::white)));
    pt2.setOpacity(0.5);
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
    emit runItem(_uniqueName);
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
                             const QString &url, int type, const QString &uniqueName)
{
    emit iconDrop(_id, text, iconPath, page, index, url, type, uniqueName);
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
//dir_min_show_widget
void IconItem::addMinWidget(int type)
{
    _dirMinShowWidget = new DirMinShowWidget(type, this);
    _dirMinShowWidget->move(0,0);
    _dirMinShowWidget->setUniqueName(_uniqueName);
    _dirMinShowWidget->setText(_text);

    connect(_dirMinShowWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dirMinShowWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
    connect(_dirMinShowWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
//        connect(_dirMinShowWidget, SIGNAL(iconDrop(int, const QString &, const QString &, const QString &)),
//                this, SLOT(iconDropEvent(int, const QString &, const QString &, const QString &)));
//        connect(_dirMinShowWidget, SIGNAL(openItem(int, int, int)), this, SLOT(openDirWidget(int, int, int)));
    connect(_dirMinShowWidget, SIGNAL(iconDrop(const QString &, const QString &, int, int, const QString &, int, const QString &)),
            this, SLOT(iconDropEvent(const QString &, const QString &, int, int, const QString &, int, const QString &)));
    connect(_dirMinShowWidget, SIGNAL(openItem()), this, SLOT(openDirWidget()));
    connect(_dirMinShowWidget, SIGNAL(dragEnterMinWidget()), SIGNAL(dragEnterMinWidget()));
    connect(_dirMinShowWidget, SIGNAL(dirMinLineEditFocusOut(int, const QString &))
            , this, SLOT(dirMinLineEditChanged(int, const QString &)));
}

void IconItem::setMinWidgetDragEnable(bool enable)
{
    if (_dirMinShowWidget == NULL)
        return;

    if (enable)
    {
        _dirMinShowWidget->setAcceptDrops(true);
    }
    else
    {
        _dirMinShowWidget->setAcceptDrops(false);
    }
}

void IconItem::removeDirMinItem(const QString &uniqueName)
{
    if (_dirMinShowWidget == NULL)
        return;

        _dirMinShowWidget->removeDirMinItem(uniqueName);
}

void IconItem::addDirMinItem(const QString &text, const QString &icon,
                             int page, int index, const QString &url, const QString &uniqueName)
{
    if (_dirMinShowWidget == NULL)
        return;

        _dirMinShowWidget->addDirMinItem(text, icon, page, index, url, uniqueName);
}
int IconItem::getMinIconNum()
{
    return _dirMinShowWidget->getMinIconNum();
}

void IconItem::setDirMinItemId(int id)
{
    _id = id;

    if (_saveDataBool)
        _app->setId(QString("%1").arg(id));

    _dirMinShowWidget->setId(id);
}

//void IconItem::dirMinLineEditFocusOut(const QString &text)
//{
//    emit dirMinFocusOut(text);
//}

void IconItem::dirMinLineEditChanged(int i, const QString &text)
{
    _text = text;

    emit dirMinLineEditFocusOut(i, _text);
}

void IconItem::refreshDirMinWidgetIcon()
{
    _dirMinShowWidget->refresh();
}

//dustbin
void IconItem::addDustbin()
{
    _dustbin = new Dustbin(this);
    _id = 1000;
    _dustbin->move(0,0);

    connect(_dustbin, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
    connect(_dustbin, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
    connect(_dustbin, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
    connect(_dustbin, SIGNAL(iconDrop(const QString &, const QString &, int, int, const QString &, int, const QString&)),
            this, SLOT(iconDropEvent(const QString &, const QString &, int, int, const QString &, int, const QString&)));
    connect(_dustbin, SIGNAL(openItem()), this, SLOT(openDirWidget()));
}


//void IconItem::dustbinOpenDirWidget()
//{
//    emit openDir(_id, _page, _index);
//}

//void IconItem::dustbinIconDropEvent(const QString &text, const QString &iconPath, int page, int index,
//                             const QString &url, int type)
//{
//    emit iconDrop(_id, text, iconPath, page, index, url, type);
//}
void IconItem::changeItemName(QString name)
{
    setText(name);
    if (_saveDataBool)
        _app->setName(_texticon);
}
void IconItem::setDirBackground()
{
    if(_type == 3)
    {
        QPixmap source = QPixmap::grabWidget(this).scaled(width(),height());
        QImage image = source.toImage().scaled(width(),height());
        QImage light = QImage(_width, _height, QImage::Format_ARGB32);
        QPainter painter(&light);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(image.rect(), Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setOpacity(0.5);
        painter.drawImage(0,0,image);
        painter.end();
        _grayPixmap = QPixmap::fromImage(light).scaled(width() * 1.0, height() * 1.0);
    }
}
void IconItem::setLineEditBg(QPixmap strLeft, QPixmap strCenter, QPixmap strRight)
{
    _left = strLeft;
    _center = strCenter;
    _right = strRight;

    repaint();
}

void IconItem::LineEditFocusIn()
{
    _lineEdit->setText(_text);
    setLineEditBg(_editLeft, _editCenter, _editRight);
    _currentStatus = hover;
    update();
}
void IconItem::LineEditFocusOut()
{
//    if(_currentStatus == normal)
//        _lineEdit->setDisabled(true);
//    changeItemName(_text);

    if (_dirId == 1000)
        return;

    if (!_lineEdit->text().isEmpty())
    {
        setText(_lineEdit->text());
        _app->setName(_text);
        emit iconItemNameChanged(_uniqueName, _text);
    }
    else
    {
        setText(_text);
    }



//    QSqlQuery query(QSqlDatabase::database("local"));
//    QString qstr = QString("update localapps set name=\'%1\' where uniquename=\'%2\'").arg(_text).arg(_uniqueName);
//    if(!query.exec(qstr)) {
//        qDebug() <<"query failed";
//        return;
//    }
    setLineEditBg(_editLeftNormal, _editCenterNormal, _editRightNormal);
}
QPixmap IconItem::setTransparentPixmap(const QString &pix)
{
    QImage normal = QImage(pix);

    for (int i = 0; i < normal.width(); i++) {
        for (int j = 0; j < normal.height(); j++) {
            QRgb pixel = normal.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel) * 1, qGreen(pixel) * 1, \
                                    qBlue(pixel) * 1, a * 50 / 255);
            normal.setPixel(i, j, lightPixel);
        }
    }
    return QPixmap::fromImage(normal);
}
void IconItem::focusOutEvent(QFocusEvent *event)
{
    if(_currentStatus == selected && QItemManager::getManager()->containsItem(this))
    {
        _currentStatus = selected;
        update();
        return;
    }
//    emit showContextMenu( false,QPoint(0,0), mapToGlobal(QPoint(0,0)), _text);
    _currentStatus = normal;
    update();
}
void IconItem::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    switch(_iconSize)
    {
    case 0:
    {
        _lineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE, QFont::Normal));
        _editLeft = setTransparentPixmap(":/images/dir_edit_left.png").scaled(11,23);
        _editCenter = setTransparentPixmap(":/images/dir_edit_center.png").scaled(1,23);
        _editRight = setTransparentPixmap(":/images/dir_edit_right.png").scaled(11,23);
        break;
    }
    case 1:
    {
        _lineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE - 1 , QFont::Normal));
        _editLeft = setTransparentPixmap(":/images/dir_edit_left.png").scaled(9,19);
        _editCenter = setTransparentPixmap(":/images/dir_edit_center.png").scaled(1,19);
        _editRight = setTransparentPixmap(":/images/dir_edit_right.png").scaled(9,19);
        break;
    }
    default:
    {
        _lineEdit->setFont(QFont(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"), FONTSIZE - 1 , QFont::Normal));
        _editLeft = setTransparentPixmap(":/images/dir_edit_left.png").scaled(8,16);
        _editCenter = setTransparentPixmap(":/images/dir_edit_center.png").scaled(1,16);
        _editRight = setTransparentPixmap(":/images/dir_edit_right.png").scaled(8,16);
        break;
    }
    }
}
void IconItem::init()
{
     QImage normal(_width, _height, QImage::Format_ARGB32_Premultiplied);
     QPainter painter(&normal);
     painter.fillRect(QRectF(0,0,_width,_height),Qt::transparent);
     _normalBackground = QPixmap::fromImage(normal);

     for(int i = 0; i < _width; i ++)
     {
         for(int j = 0; j < _height; j++)
         {
             normal.setPixel(QPoint(i , j),qRgba(0,0,0,HOVERALPHA));
         }
     }
     _hoverBackgroud = QPixmap::fromImage(normal);
     for(int i = 0; i < _width; i ++)
     {
         for(int j = 0; j < _height; j++)
         {
             normal.setPixel(QPoint(i , j),qRgba(0,0,0,SELECTALPHA));
         }
     }
     _selectedBackgroud = QPixmap::fromImage(normal);

}

void IconItem::setIconClass(int iconClass)
{
    if (iconClass == vapp)
    {
        _iconClassPixmap.load(":/images/app_normal.png");
    }
    else if (iconClass == paas)
    {
        _iconClassPixmap.load(":/images/paas_normal.png");
    }
    else
    {
        _iconClassPixmap.load("");
    }

    update();
}

void IconItem::setIconSize(int iconSize)
{
    _iconSize = iconSize;

//    repaint();
}

void IconItem::setLineEditReadOnly(bool readOnly)
{
//    _lineEdit->setReadOnly(true);
    _lineEdit->setEnabled(false);
}
