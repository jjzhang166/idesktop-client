#include <QLabel>
#include <QTimeLine>
#include <QColor>
#include <QTimer>
#include <QApplication>
#include <QPainter>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QProcess>
#include <QSettings>
#include <QTextEdit>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include "strings.h"
using namespace std;
#include <QMessageBox>
#include <QCursor>


#include "virtualdesktop.h"
#include "localapps.h"
#include "movingdialog.h"
#include "client/appdatareadexe.h"
#include "appmessagebox.h"
#include "config.h"

#define INDICATOR_ITEMSIZE QSize(14, 14)
#define ICONWIDTH 96
#define ICONHEIGHT 96
#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30
#define FONTSIZE 10
#define APPICON   0
#define ADDICON 1

//#define FONTSIZE 11

#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"
extern QString WIN_VAPP_IconPath;
extern QList<APP_LIST> g_myVappList;
/* ####################################################
 * definition of IconItem methods
 */
static QPoint gap;

IconItem::IconItem(const QString &text, const VirtualDesktop::iconStyle &iSt, QWidget *parent)
    : QWidget(parent), _text(text), _trembling(0), _style(iSt)
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
        //qDebug() << "_textWidth : " << _textWidth;
        //qDebug() << "width : " << width;
        if (_textWidth_firstrow > width - 20)
        {
            _texticon_firstrow.chop(1);
            //qDebug() << "_texticon = " << _texticon.length();
        }
        else{
            //_texticon.chop(1);
            //_texticon.append("...");
            break;
        }
    }
    _texticon_secondrow = _texticon.right(_texticon.length() - _texticon_firstrow.length());
//    qDebug() << "secondrow : " << _texticon_secondrow;

    _textWidth_secondrow = fm.width(_texticon_secondrow);
    for (i = 0; i < _textWidth_secondrow; i++)
    {
        _textWidth_secondrow = fm.width(_texticon_secondrow);
        //qDebug() << "_textWidth : " << _textWidth;
        //qDebug() << "width : " << width;
        if (_textWidth_secondrow > width - 20)
        {
            _texticon_secondrow.chop(1);
            //qDebug() << "_texticon = " << _texticon.length();
        }
        else{
            //_texticon.chop(1);
            //_texticon.append("...");
            break;
        }
    }
    _texticon_thirdrow = _texticon.right(_texticon.length() - _texticon_firstrow.length() - _texticon_secondrow.length());
//    qDebug() << "secondrow : " << _texticon_thirdrow;

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


    _app = LocalAppList::getList()->getAppByName(_text);
    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setDuration(200);
    _animation->setEasingCurve(QEasingCurve::OutBack);

    _timeline = new QTimeLine(5000, this);
    _timeline->setLoopCount(0);
    _timeline->setUpdateInterval(65);

    _icontype = APPICON;

    _hoverIconItem = new HoverIconItem(this->width(), this->height(), this);
    _hoverIconItem->setVisible(false);

    _openAction = new QAction(tr("����"), this);
    connect(_openAction, SIGNAL(triggered()), this, SLOT(openClicked()));

    if (_style == VirtualDesktop::localIcon)
    {
        _delAction = new QAction(tr("ɾ��"), this);
        connect(_delAction, SIGNAL(triggered()), this, SLOT(delClicked()));
    }

    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(doTremble(qreal)));
    if (parent) {
        if (_style == VirtualDesktop::localIcon)
        {
            connect(static_cast<VirtualDesktop*>(parent), \
                    SIGNAL(trembleStarted()), \
                    this, SLOT(startTremble()));
            connect(static_cast<VirtualDesktop*>(parent), \
                    SIGNAL(trembleStoped()), \
                    this, SLOT(stopTremble()));
        }
    }
}

IconItem::~IconItem()
{
    //if (_style == VirtualDesktop::localIcon)
        //delete _delAction;

    delete _openAction;
    delete _hoverIconItem;

    delete _timeline;
    delete _animation;
}

void IconItem::setPage(int page)
{
    _page = page;
    _app->setPage(page);
}
void IconItem::setIndex(int index)
{
    _index= index;
   _app->setIndex(index);
}
void IconItem::setHidden(bool hide)
{
    _app->setHidden(hide);
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
    _timeline->start();
    _pixmap = _closePixmap;
}

void IconItem::stopTremble()
{
    _timeline->stop();
    _trembling = 0;
    _pixmap = _normalPixmap;
    repaint();
}

void IconItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    if (_trembling)
        painter.translate(width()/2, height()/2);
    painter.rotate(2*_trembling);
    if (_trembling)
        painter.translate(-1*width()/2, -1*height()/2);

    painter.drawPixmap(3, 0, _pixmap);
    QWidget::paintEvent(event);
}

void IconItem::mousePressEvent(QMouseEvent *event)
{
    if (_timeline->state() == QTimeLine::Running) {
        if (QRect(0, 5, 30, 35).contains(event->pos())) {
            delClicked();
            return;
        }
    }

    if (event->button() == Qt::LeftButton && \
            geometry().contains(event->pos())) {
        dragStartPosition = event->pos();
    }
    event->ignore();

}

void IconItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && (_timeline->state() != QTimeLine::Running))
    {
        openClicked();
    }
}

void IconItem::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength() \
            < QApplication::startDragDistance()) {
        return;
    }
    _drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(_text);
    _drag->setMimeData(mimeData);
    _drag->setPixmap(grayPixmap());
    _drag->setHotSpot(event->pos());
    gap = _drag->hotSpot();
    _drag->exec(Qt::MoveAction);
}

void IconItem::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);

    qDebug() << "contextMenuEvent";
    /*
    QAction * openAction = new QAction("��", this);
    QAction * delAction = new QAction("ɾ��", this);

    QMenu *iconItemcontextMenu = new QMenu(this);
    iconItemcontextMenu->addAction(openAction);
    iconItemcontextMenu->addAction(delAction);
    iconItemcontextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(sort_ascend()));
    connect(delAction, SIGNAL(triggered(bool)), this, SLOT(sort_descend()));

    iconItemcontextMenu->exec(QCursor::pos());//�ڵ�ǰ���λ����ʾ
*/
    QCursor cur=this->cursor();
    QMenu *iconItemcontextMenu = new QMenu(this);
    /*
    QPalette pal = iconItemcontextMenu->palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0x00));
    iconItemcontextMenu->setPalette(pal);

    iconItemcontextMenu->setStyleSheet("QMenu{color:#FFFFFF;}\
                                        QMenu::item{color:#FFFFFF;}");
*/
    //QAction * openAction = new QAction(tr("open"), this);
    //QAction * delAction = new QAction(tr("delete"), this);
    iconItemcontextMenu->addAction(_openAction);

    if (_style == VirtualDesktop::localIcon)
    {
        iconItemcontextMenu->addAction(_delAction);
    }

    iconItemcontextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);

    iconItemcontextMenu->exec(cur.pos());

    return;
}


void IconItem::enterEvent(QEvent *event)
{
    //_pixmap = darkPixmap();
    _hoverIconItem->setVisible(true);
    repaint();

    Q_UNUSED(event);
}

void IconItem::leaveEvent(QEvent *event)
{
    /*
    if (_timeline->state() == QTimeLine::Running)
        _pixmap = _closePixmap;
    else
        _pixmap = _normalPixmap;
*/
    _hoverIconItem->setVisible(false);
    repaint();

    Q_UNUSED(event);
}

const QPixmap& IconItem::originPixmap()
{
    return _originPixmap;
}

void IconItem::setPixmap(const QString &icon)
{
    int begin;
    QString text = _text;
    if (isUserType())
        text = _text.right(_text.length() - 1);
    _originPixmap = QPixmap(icon);
    QImage image = QImage(icon).scaled(ICONWIDTH, ICONHEIGHT);
    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);
    QPainter pt1(&normal);
    pt1.setPen(Qt::white);
    //pt1.setFont(QFont("", FONTSIZE, QFont::Black));
    pt1.setFont(QFont("", FONTSIZE, QFont::Normal));
    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
    pt1.setCompositionMode(QPainter::CompositionMode_Source);
    pt1.fillRect(normal.rect(), Qt::transparent);
    pt1.drawImage(CLOSEWIDTH/2 - 3, CLOSEHEIGHT/2, image);
//    begin = (ICONWIDTH - _textWidth) / 2 + CLOSEWIDTH/2 - 3;
//    if (begin < CLOSEWIDTH/2)
//        begin = CLOSEWIDTH/2;
    // pt1.drawText( QRect(begin, ICONHEIGHT + CLOSEHEIGHT/2 - 10 ,
    //width(), _textHeight), Qt::TextSingleLine, text);

//    QTextEdit *tEdit = new QTextEdit(this);
//    tEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0);\
//                         margin-left:0px; color:#FFFFFF");\
//                         tEdit->setText(text);\
//    tEdit->setAlignment(Qt::AlignCenter);
//    tEdit->setFixedWidth(width());
//    tEdit->setLineWrapColumnOrWidth(width());
//    tEdit->setEnabled(false);
//    tEdit->move(0, height() - 48);

//    begin = (ICONWIDTH - _textWidth) / 2 + CLOSEWIDTH/2 - 3;
//    if (begin < CLOSEWIDTH/2)
//        begin = CLOSEWIDTH/2;
//    pt1.drawText( QRect(begin, ICONHEIGHT + CLOSEHEIGHT/2 , \
//                        width() - begin, _textHeight), Qt::TextSingleLine, text);
    QFont font("", FONTSIZE, QFont::Normal);
    QFontMetrics fm(font);
    _textWidth_firstrow = fm.width(_texticon_firstrow);
    QString tx = _texticon_firstrow;
    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_firstrow) / 2, height() - _textHeight * 3, _textWidth_firstrow, _textHeight), Qt::TextSingleLine, tx);
    _textWidth_secondrow = fm.width(_texticon_secondrow);
    tx = _texticon_secondrow;
    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_secondrow) / 2, height() - _textHeight * 2, _textWidth_secondrow, _textHeight), Qt::TextSingleLine, tx);
    _textWidth_thirdrow = fm.width(_texticon_thirdrow);
    tx = _texticon_thirdrow;
    pt1.drawText( QRect(((ICONWIDTH + CLOSEWIDTH/2) + 8 - _textWidth_thirdrow) / 2, height() - _textHeight , _textWidth_thirdrow, _textHeight), Qt::TextSingleLine, tx);
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
    /*
    pt2.drawText( QRect(begin, ICONHEIGHT + CLOSEHEIGHT/2 , \
                        width(), _textHeight), Qt::TextSingleLine, \
                  text);
    */
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
    if (_style == VirtualDesktop::localIcon)
    {
        QImage closeImage(":images/close_icon.png");
        painter.drawImage(0, 5, closeImage);
    }
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.end();
    _closePixmap = QPixmap::fromImage(resultImage);

}

void IconItem::setText(const QString &text)
{
    _text = text;
}

bool IconItem::isUserType()
{
    if (_text.startsWith("/"))
        return true;
    return false;
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
    if (_timeline->state() == QTimeLine::Running)
        return _closePixmap;
    return _darkPixmap;
}

void IconItem::addApp()
{

}

void IconItem::openClicked()
{
    emit runItem(_text);
}

void IconItem::delClicked()
{
    emit delItem(_text);
}

/* ####################################################
 * definition of IconAddItem methods
 */
IconAddItem::IconAddItem(const QString &text, QWidget *parent)
    : QWidget(parent), _text(text)
{

    int width, height;
    QFontMetrics fm(QFont("", FONTSIZE, QFont::Black));
    width = ICONWIDTH + CLOSEWIDTH/2;
    height = ICONHEIGHT + CLOSEHEIGHT/2 + fm.height();
    setFixedSize(width, height);
    _textWidth = fm.width(_text);
    if (_text.startsWith("/"))
        _textWidth = fm.width(_text.right(_text.length() - 1));
    _textHeight = fm.height();

    _icontype = ADDICON;

    //_hoverAddItem = new HoverIconItem(this->width(), this->height(), this);
    //_hoverAddItem->setVisible(false);

    //connect(this, SIGNAL(clicked()), this, SLOT(addApp()));
    _pixmap.load(":/images/icon_add_normal.png");
}

IconAddItem::~IconAddItem()
{
}

//void IconAddItem::addApp()
//{
//    qDebug() << "IconAddItem::addApp()";
//    QString path = QFileDialog::getOpenFileName(this, tr("ѡ��һ��Ӧ�ó�����ݷ�ʽ"),
//                                                Config::get("CommonProgramDir"),
//                                                tr("app (*.lnk *.exe)"));

//    if (path.isEmpty())
//        return;

//    path.replace('/','\\');
//    AppDataReadExe::Instance()->addLocalApp(path);
//}

void IconAddItem::setPage(int page)
{
    _page = page;
}
void IconAddItem::setIndex(int index)
{
    _index= index;
}

void IconAddItem::setHidden(bool hide)
{
    qDebug() << "IconAddItem::setHidden()";
    setVisible(hide);
}

void IconAddItem::animationMove(const QRect &start, const QRect &end)
{
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _animation->start();
}

void IconAddItem::doTremble(qreal value)
{
    Q_UNUSED(value);
}

void IconAddItem::startTremble()
{

}

void IconAddItem::stopTremble()
{
}

void IconAddItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.drawPixmap(3, 0, _pixmap);
    QWidget::paintEvent(event);
}

void IconAddItem::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "IconAddItem::mousePressEvent";

    if( Qt::LeftButton == event->button())
    {
        emit addApp();
    }

    event->accept();

}
/*
void IconAddItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "IconAddItem::mouseDoubleClickEvent";
    if( Qt::LeftButton == event->button())
    {
      emit clicked();
    }

    event->accept();

}
*/

void IconAddItem::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "IconAddItem::mouseMoveEvent";
    event->accept();

}

void IconAddItem::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << "contextMenuEvent";
    //event->ignore();

    Q_UNUSED(event);

    return;
}


void IconAddItem::enterEvent(QEvent *event)
{
    //_pixmap = darkPixmap();

    _pixmap.load(":/images/icon_add_hover.png");

    update();

    Q_UNUSED(event);
}

void IconAddItem::leaveEvent(QEvent *event)
{   
    Q_UNUSED(event);

    _pixmap.load(":/images/icon_add_normal.png");

    update();
#if 0
    _pixmap = _normalPixmap;
    repaint();
#endif 
}

const QPixmap& IconAddItem::originPixmap()
{
    return _originPixmap;
}

void IconAddItem::setPixmap(const QString &icon)
{
    int begin;
    QString text = _text;
    if (isUserType())
        text = _text.right(_text.length() - 1);
    _originPixmap = QPixmap(icon);
    QImage image = QImage(icon).scaled(ICONWIDTH, ICONHEIGHT);
    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);
    QPainter pt1(&normal);
    pt1.setPen(Qt::white);
    pt1.setFont(QFont("", FONTSIZE, QFont::Black));
    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
    pt1.setCompositionMode(QPainter::CompositionMode_Source);
    pt1.fillRect(normal.rect(), Qt::transparent);
    pt1.drawImage(CLOSEWIDTH/10 + 2, CLOSEHEIGHT/10 + 5, image);
    begin = (ICONWIDTH - _textWidth) / 2 + CLOSEWIDTH/2 - 3;
    if (begin < CLOSEWIDTH/2)
        begin = CLOSEWIDTH/2;
    //pt1.drawText( QRect(begin, ICONHEIGHT + CLOSEHEIGHT/2 ,
    //width(), _textHeight), Qt::TextSingleLine, text);
    pt1.drawText(QRect((width() - _textWidth) / 2, ICONHEIGHT - 10, \
                       width(), _textHeight), Qt::TextSingleLine, text);
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
    /*
    pt2.drawText( QRect(begin, ICONHEIGHT + CLOSEHEIGHT/2 , \
                        width(), _textHeight), Qt::TextSingleLine, \
                  text);
    */
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
    //painter.drawImage(0, 5, closeImage);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.end();
    _closePixmap = QPixmap::fromImage(resultImage);
}

void IconAddItem::setText(const QString &text)
{
    _text = text;
}

bool IconAddItem::isUserType()
{
    if (_text.startsWith("/"))
        return true;
    return false;
}

const QString & IconAddItem::text()
{
    return _text;
}

const QPixmap & IconAddItem::pixmap()
{
    return _pixmap;
}

const QPixmap & IconAddItem::grayPixmap()
{
    return _grayPixmap;
}

const QPixmap & IconAddItem::darkPixmap()
{
    return _darkPixmap;
}


/* #####################################################
 * definition of Indicator methods
 */
Indicator::Indicator(VirtualDesktop *vdesk, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool), _vdesk(vdesk)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    reset();
    connect(_vdesk, SIGNAL(reInitiate()), this, SLOT(reset()));
    connect(_vdesk, SIGNAL(pageChanged(int)), this, SLOT(setCurrent(int)));
    connect(_vdesk, SIGNAL(pageIncreased()), this, SLOT(increase()));
    connect(_vdesk, SIGNAL(pageDecreased()), this, SLOT(decrease()));
}

void Indicator::reset()
{
    setFixedSize(25 + _vdesk->count() * 17, 19);
    for (int i = 0; i < _labels.count(); i++)
        delete _labels[i];
    _labels.clear();
    QLabel *win = new QLabel(this);
    win->setFixedSize(17, 19);
    win->setPixmap(QPixmap(":images/window_icon_bottom.png"));
    win->move(0, 0);
    win->show();
    _labels.push_back(win);
    for (int i = 0; i < _vdesk->count(); i++) {
        QLabel *l = new QLabel(this);
        l->setPixmap(QPixmap(":images/bottomPage_icon.png"));
        l->setFixedSize(17, 11);
        _labels.push_back(l);
        l->move(25 + i * 17, 4);
        l->show();
    }
    setCurrent(_vdesk->currentPage());
    emit iNeedMove();
}

void Indicator::increase()
{
    setFixedSize(25 + _vdesk->count() * 17, 19);
    QLabel *l = new QLabel(this);
    l->setPixmap(QPixmap(":images/bottomPage_icon.png"));
    l->setFixedSize(17, 11);
    l->show();
    _labels.push_back(l);
    l->move(25 + ( _vdesk->count() - 1) * 17, 4);
    emit iNeedMove();
}

void Indicator::decrease()
{
    delete _labels.takeLast();
    setFixedSize(25 + _vdesk->count() * 17, 19);
    emit iNeedMove();
}

void Indicator::setCurrent(int index)
{
    for (int i = 1; i <= _vdesk->count(); i++)
        _labels.at(i)->setPixmap(QPixmap(":images/bottomPage_icon.png"));
    _labels.at(index+1)->setPixmap(QPixmap(":images/bottomPage_current_icon.png"));
}

/* ########################################################
 * definition of VirtualDesktop methods
 */
VirtualDesktop::VirtualDesktop(QSize pageSize,  QWidget *parent)
    : QWidget(parent)
    , _movingDialog(NULL)
    , _vertical(true)
    , _itemClicked(false)
    , _itemHeld(false)
    , _trembling(false)
    , _dragEvent(false)
    , _addAppState(false)
//    , _vappCount(0)

{

//    int gridWidth;
//    int gridHeight;

    _count = 1;
    //_count = PAGENUM;

    /*
    int maxPage = 0;
    for (int i = 0; i < _local->count(); i++) {
        int iconPage = _local->at(i)->page();
        maxPage = maxPage < iconPage ? iconPage : maxPage;
    }
    */

    //_count = Config::get("PageCount").toInt();
    /*****app******/
#ifdef Q_WS_WIN
    mylib= new QLibrary("DllClientEngineMain.dll");   //
    if (mylib->load())              //check whether load successfully
    {
        //QMessageBox::information(NULL,"OK","DLL load is OK!");

        m_dllInitClass=(Dll_InitClass)mylib->resolve("DllCEM_InitClass");    //get function add()
        m_dllCloseClass=(Dll_CloseClass)mylib->resolve("DllCEM_CloseClass");
        m_dllCloseAppAll=(Dll_CloseAppAll)mylib->resolve("DllCEM_closeAppAll");
        m_dllStartAppEx=(Dll_StartAppEx)mylib->resolve("DllCEM_startAppEx");
    }
    else
        QMessageBox::information(NULL,"NO","DllClientEngineMain is not loaded!");
#endif

    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = _pageSize.height();

    gridWidth = ICONWIDTH + SPACING * 2;
    gridHeight = ICONHEIGHT + SPACING * 2;
    _col = _width / gridWidth;
    _row = _height / gridHeight;
    _iconsPerPage = _col * _row;
    _current  = 0;

    _local = LocalAppList::getList();

    for (int i = 0; i < _local->count(); i++)
    {
        _count = i / _iconsPerPage + 1;
    }

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j / _row) * gridWidth;
            int y = (j % _row) * gridHeight;
//                    qDebug()<<"new Grid:" << x << "," << y << "," << gridWidth << "," << gridHeight;
            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }

        _gridTable.insert(i, newList);
        //for (int i=0; i < _gridTable.count(); i++)
//            qDebug() << _gridTable.at(i);
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
    setFixedSize(_width * _count, _height);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);


    /*for local desktop add addbutton*/
    g_addIcon = new IconAddItem(tr(""),this);

    if (_local->count() == 0)
        g_addIcon->setGeometry(_gridTable[0][0].translated(SPACING, SPACING));

    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;

        if(_local->at(i)->isRemote())
        {
            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                    _local->at(i)->page(), _local->at(i)->index(), vappIcon);
        }else{
            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                    _local->at(i)->page(), _local->at(i)->index(), localIcon);
        }
    }



    dragLeftTimer = new QTimer(this);
    dragLeftTimer->setInterval(500);
    dragLeftTimer->setSingleShot(true);

    dragRightTimer = new QTimer(this);
    dragRightTimer->setInterval(500);
    dragRightTimer->setSingleShot(true);

//    _bsWidget = new BsWidget(_width - 40, _height - 80, this);
//    _bsWidget->setGeometry(1 *_width, 0, _width - 40, _height - 80);

//    _manageWidget = new ManageWidget(this);
//    _manageWidget->setGeometry(2 *_width, 40, _width, _height);

    _addAction = new QAction(QString(tr("���")), this);
    _deleteAction = new QAction(QString(tr("��ʼ����")), this);
    _cancelAction = new QAction(QString(tr("ȡ������")), this);

    connect(_addAction, SIGNAL(triggered()), this, SLOT(appAdd()));
    connect(_deleteAction, SIGNAL(triggered()), this, SLOT(appDelete()));
    connect(_cancelAction, SIGNAL(triggered()), this, SLOT(appCancel()));

    connect(dragLeftTimer, SIGNAL(timeout()), this, SLOT(dragLeft()));
    connect(dragRightTimer, SIGNAL(timeout()), this, SLOT(dragRight()));

    connect(_local, SIGNAL(appAdded(const QString&, const QString&)), this, SLOT(addIcon(const QString&, const QString&)));
    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));

    connect(&_communi, SIGNAL(appRun()), this, SLOT(runServerApp()));

    connect(g_addIcon, SIGNAL(addApp()), this, SLOT(appAdd()));
}

VirtualDesktop::~VirtualDesktop()
{
    delete g_addIcon;
#if 0
    for (int j = 0; j < _iconDict.size(); j++)
    {
        delete &_iconDict[0][j];
    }
#endif
}

void VirtualDesktop::updateClicked()
{
    _itemClicked = false;
}

void VirtualDesktop::itemHeld()
{
    if (!_itemHeld)
        return;
    _trembling = true;
    emit trembleStarted();
}

IconItem * VirtualDesktop::getIconByName(const QString &name)
{
    return _iconDict.value(name, NULL);
}

void VirtualDesktop::atExit()
{
    _local->save();
}

void VirtualDesktop::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(status);

    _inProgress = false;
}

void VirtualDesktop::uninstall(const QString &name)
{
    LocalApp *app = _local->getAppByName(name);
    if (app == NULL)
        return;
    if (name.startsWith("/")) {
        AppMessageBox box(true, NULL);
        box.setText("    ȷ��ɾ��ͼ�ꣿ");
        if (!box.exec())
            return;
        _local->delApp(name);
    } else {
        QString uninstCommand = AppDataReadExe::Instance()->getUninstExec(app->uninstName());

        if (!uninstCommand.isEmpty()) {
            AppMessageBox box(true, NULL);
            box.setText("    ȷ��ɾ��ͼ�겢����ж�س���");
            if (!box.exec())
                return;

            _local->delApp(name);
            QByteArray tmp = uninstCommand.toAscii();
            if (uninstCommand.startsWith("\""))
                tmp = uninstCommand.mid(1, uninstCommand.length() - 2).toAscii();

            /////////////////////////////////////
            STARTUPINFO si; //һЩ�ر���������
            memset(&si, 0, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOW;
            PROCESS_INFORMATION pi; //�ر��������ý���

            CreateProcess(tmp.data(),NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
        }
        else {
            AppMessageBox box(true, NULL);
            box.setText("Ӧ���Ѿ���ж�أ��Ƿ�ɾ��ͼ�ꣿ");
            if (!box.exec())
                return;
            _local->delApp(name);
        }
    }
}

void VirtualDesktop::moveItem(IconItem *item, int target)
{
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    /* move the items in the source page */
    //printDesktop();
    int i;
    int source = item->page();
    int srcI = item->index();
    for (i = srcI + 1; _iconTable[source][i] != NULL; i++) {
        QPoint end = _gridTable[source][i-1]\
                .translated(SPACING, SPACING).topLeft();
        _iconTable[source][i]->move(end);
        _iconTable[source][i]->setIndex(i-1);
        _iconTable[source][i - 1] = _iconTable[source][i];
        _iconTable[source][i-1]->setIndex(i-1);
    }
    _iconTable[source][i-1] = NULL;
    _nextIdx[source]--;
    /* The page is empty now, need to be deleted */
    if (_nextIdx[source] == 0) {
        for (int i = source; i < (_count - 1); i++) {
            for (int j = 0; j < _col * _row; j++) {
                _iconTable[i][j] = _iconTable[i+1][j];
                IconItem *icon = _iconTable[i][j];
                if (icon != NULL) {
                    icon->setPage(i);
                    icon->setIndex(j);
                    icon->setGeometry(_gridTable[i][j]\
                                      .translated(SPACING, SPACING));
                }
            }
        }
        _iconTable.removeLast();
        _gridTable.removeLast();
        _pages.removeLast();
        _nextIdx.removeAt(source);
        _count--;
        if (target > source)
            target--;
        setFixedSize( _count * _pageSize.width(), _pageSize.height());
        _current = target;
        move(_pages[_current], y());
        emit pageChanged(_current);
        emit pageDecreased();
    }
    item->setPage(target);
    item->setIndex(_nextIdx[target]);
    _iconTable[item->page()][item->index()] = item;
    QPoint dest = _gridTable[item->page()][item->index()]\
            .translated(SPACING, SPACING).topLeft();
    _nextIdx[target]++;
    item->move(dest);
    //move(_pages[_current], y());
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), \
                                    width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    //printDesktop();
    //_current = target;
    //move(_pages[target], y());
    //emit pageChanged(_current);
}

void VirtualDesktop::reload()
{
    emit reInitiate();
}

void VirtualDesktop::arrangeEqually(int pageCount)
{
    QList<IconItem*> seqList;
    for (int i = 0; i < _count; i++)
        for (int j = 0; j < _row * _col; j++)
            if (_iconTable[i][j]) {
                seqList.push_back(_iconTable[i][j]);
                _iconTable[i][j] = NULL;
            }
    int iconCount = seqList.count();
    if (iconCount < pageCount)
        pageCount = iconCount;

    _iconTable.clear();
    _gridTable.clear();
    _nextIdx.clear();
    _count = pageCount;
    _current = 0;
    move(_pages[0], y());
    _pages.clear();
    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    gridWidth = ICONWIDTH + SPACING * 2;
    gridHeight = ICONHEIGHT + SPACING * 2;
    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j / _row) * gridWidth;
            int y = (j % _row) * gridHeight;
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
    setFixedSize(_width * _count, _height);

    int eachPage = iconCount / pageCount;
    int currentPage = 0;
    int tmpCount = 0;
    int m = 0;
    while (currentPage < pageCount) {
        while (tmpCount < eachPage) {
            _iconTable[currentPage][tmpCount] = seqList[m];
            seqList[m]->move(_gridTable[currentPage][tmpCount].translated(SPACING, SPACING).topLeft());
            seqList[m]->setPage(currentPage);
            seqList[m]->setIndex(tmpCount);
            m++;
            tmpCount++;
        }
        _nextIdx[currentPage] = eachPage;
        currentPage++;
        tmpCount = 0;
    }
    currentPage = 0;
    for (int i = m; i < iconCount; i++) {
        _iconTable[currentPage][eachPage] = seqList[i];
        seqList[i]->move(_gridTable[currentPage][eachPage].translated(SPACING, SPACING).topLeft());
        seqList[m]->setPage(currentPage);
        seqList[m]->setIndex(eachPage);
        _nextIdx[currentPage]++;
        currentPage++;
    }
    emit reInitiate();
}

void VirtualDesktop::arrangeMinimum()
{
    //printDesktop();
    QList<IconItem*> seqList;
    for (int i = 0; i < _count; i++)
        for (int j = 0; j < _row * _col; j++)
            if (_iconTable[i][j]) {
                seqList.push_back(_iconTable[i][j]);
                _iconTable[i][j] = NULL;
            }
    int iconCount = seqList.count();
    int pageCount = iconCount / (_row * _col);
    if (iconCount % (_row * _col) != 0)
        pageCount++;

    _iconTable.clear();
    _gridTable.clear();
    _nextIdx.clear();
    _count = pageCount;
    _current = 0;
    move(_pages[0], y());
    _pages.clear();
    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    gridWidth = ICONWIDTH + SPACING * 2;
    gridHeight = ICONHEIGHT + SPACING * 2;
    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j / _row) * gridWidth;
            int y = (j % _row) * gridHeight;
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
    setFixedSize(_width * _count, _height);

    int page = 0;
    int idx = 0;
    int m = 0;
    while (m < iconCount ) {
        if (idx == _row * _col) {
            page++;
            idx = 0;
            continue;
        }
        _iconTable[page][idx] = seqList[m];
        seqList[m]->move(_gridTable[page][idx].translated(SPACING, SPACING).topLeft());
        seqList[m]->setPage(page);
        seqList[m]->setIndex(idx);
        _nextIdx[page]++;
        idx++;
        m++;
    }

    //printDesktop();
    emit reInitiate();
}

void VirtualDesktop::mousePressEvent(QMouseEvent *event)
{
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }
    if (_movingDialog) {
        _movingDialog->reject();
        delete _movingDialog;
        _movingDialog = NULL;
    }
    IconItem *item = static_cast<IconItem*>(childAt(event->pos()));

    if (event->button() == Qt::RightButton) {
        if (item) {
            return;
            if (count() == 1) {
                return;
            }
            _movingDialog = new MovingDialog(this, item, this);
            _movingDialog->move(mapToGlobal(event->pos()));
            _movingDialog->show();
            return;
        }
        /*       else if (_trembling) {
            emit trembleStoped();
            _trembling = false;
            return;
        }
*/
    }

    /*
    if (_trembling && event->button() == Qt::RightButton) {
        emit trembleStoped();
        _trembling = false;
    }
    */

    if (item) {
        if (_trembling) {
            if (QRect(0, 0, 30, 30).contains(event->pos() - item->pos())) {
                //delIcon(item->text());
                uninstall(item->text());
                return;
            }
        }
        if (_itemClicked) {
            if (!_trembling) {
                //runApp(item->text());
                _itemClicked = false;
            }
        } else {
            QTimer::singleShot(400, this, SLOT(updateClicked()));
            QTimer::singleShot(1000, this, SLOT(itemHeld()));
            _itemHeld = true;
            _itemClicked = true;
        }
    }
    else
    {
        emit pageChanged(_current);
    }
    m2v = event->x();
    gm2v = mapToGlobal(event->pos()).x();
    event->accept();
}

void VirtualDesktop::dragEnterEvent(QDragEnterEvent *event)
{
    _dragEvent = true;

    _itemHeld = false;
    IconItem *icon = qobject_cast<IconItem*> (event->source());
    _inDrag = icon;
    icon->hide();
    event->accept();
}

void VirtualDesktop::dragRight()
{
    if (!_inDrag)
        return;
    if (QRect(_pageSize.width() - 20, y(), 20, _pageSize.height())\
            .contains(cursor().pos())) {
        /* last page, expand */
        if (_current == _count - 1) {
            expand();
            qDebug() << "EXPAND";
        }
        move(_pages[++_current], y());
        emit pageChanged(_current);
        int i;
        int p = _inDrag->page();
        int s = _inDrag->index();
        for (i = s + 1; i < _nextIdx[p]; i++) {
            QPoint end = _gridTable[p][i-1].translated(SPACING, SPACING).topLeft();
            _iconTable[p][i]->move(end);
            _iconTable[p][i]->setIndex(i-1);
            _iconTable[p][i - 1] = _iconTable[p][i];
            _iconTable[p][i-1]->setIndex(i-1);
        }
        _iconTable[p][i-1] = NULL;
        _nextIdx[p]--;
        if (_nextIdx[_current] == _iconsPerPage) {
            int source = _current;
            int target = _current + 1;
            IconItem *moving = _inDrag;
            moving->setPage(source);
            moving->setIndex(_iconsPerPage - 1);
            IconItem *last = _iconTable[source][_iconsPerPage - 1];
            while (true) {
                if (target == _count) {
                    expand();
                    _iconTable[target][0] = last;
                    last->move(_gridTable[target][0].translated(SPACING, SPACING).topLeft());
                    last->setPage(target);
                    last->setIndex(0);
                    _nextIdx[target]++;
                    if (moving != _inDrag) {
                        int idx = _nextIdx[source];
                        if (idx >= _iconsPerPage)
                            idx = _iconsPerPage - 1;
                        for (int i = idx; i > 0; i--) {
                            _iconTable[source][i] = _iconTable[source][i-1];
                            _iconTable[source][i]->move(_gridTable[source][i].translated(SPACING, SPACING).topLeft());
                            _iconTable[source][i]->setIndex(i);
                        }
                        _iconTable[source][0] = moving;
                        moving->move(_gridTable[source][0].translated(SPACING, SPACING).topLeft());
                        moving->setPage(source);
                        moving->setIndex(0);
                    } else {
                        _iconTable[source][_iconsPerPage-1] = moving;
                    }
                    break;
                }
                else if (_nextIdx[target] < _iconsPerPage) {
                    for (int i = _nextIdx[target]; i > 0; i--) {
                        _iconTable[target][i] = _iconTable[target][i-1];
                        _iconTable[target][i]->move(_gridTable[target][i].translated(SPACING, SPACING).topLeft());
                        _iconTable[target][i]->setIndex(i);
                    }
                    _iconTable[target][0] = last;
                    last->move(_gridTable[target][0].translated(SPACING, SPACING).topLeft());
                    last->setPage(target);
                    last->setIndex(0);
                    _nextIdx[target]++;
                    if (moving != _inDrag) {
                        int idx = _nextIdx[source];
                        if (idx >= _iconsPerPage)
                            idx = _iconsPerPage - 1;
                        for (int i = idx; i > 0; i--) {
                            _iconTable[source][i] = _iconTable[source][i-1];
                            _iconTable[source][i]->move(_gridTable[source][i].translated(SPACING, SPACING).topLeft());
                            _iconTable[source][i]->setIndex(i);
                        }
                        _iconTable[source][0] = moving;
                        moving->move(_gridTable[source][0].translated(SPACING, SPACING).topLeft());
                        moving->setPage(source);
                        moving->setIndex(0);
                    } else {
                        _iconTable[source][_iconsPerPage-1] = moving;
                    }
                    break;
                }
                else {
                    source++;
                    target++;
                    moving = last;
                    last = _iconTable[source][_iconsPerPage - 1];
                    continue;
                }
            }
        } else {
            _inDrag->setPage(_current);
            _inDrag->setIndex(_nextIdx[_current]);
            _nextIdx[_current]++;
        }
        if (_nextIdx[p] == 0) {
            delPage(p);
        }
        cursor().setPos(_pageSize.width()/3, _pageSize.height()/3);
        qDebug() << "END OF DRAGRIGHT";
    }
}

void VirtualDesktop::dragLeft()
{
    if (!_inDrag)
        return;
    if (!QRect(0, y(), 20, _pageSize.height())\
            .contains(cursor().pos()))
        return;
    if (_current != 0) {
        move(_pages[--_current], y());
        int i;
        int p = _inDrag->page();
        int s = _inDrag->index();
        for (i = s + 1; i < _nextIdx[p]; i++) {
            QPoint end = _gridTable[p][i-1].translated(SPACING, SPACING).topLeft();
            _iconTable[p][i]->move(end);
            _iconTable[p][i]->setIndex(i-1);
            _iconTable[p][i-1] = _iconTable[p][i];
            _iconTable[p][i-1]->setIndex(i-1);
        }
        _iconTable[p][i-1] = NULL;
        _nextIdx[p]--;
        if (_nextIdx[_current] == _iconsPerPage) {
            int source = _current;
            int target = _current + 1;
            IconItem *last = _iconTable[_current][_iconsPerPage-1];
            for (int i = _nextIdx[target]; i > 0; i--) {
                _iconTable[target][i] = _iconTable[target][i-1];
                _iconTable[target][i]->move(_gridTable[target][i].translated(SPACING, SPACING).topLeft());
                _iconTable[target][i]->setIndex(i);
            }
            _iconTable[target][0] = last;
            last->move(_gridTable[target][0].translated(SPACING, SPACING).topLeft());
            last->setPage(target);
            last->setIndex(0);
            _nextIdx[target]++;
            _iconTable[source][_iconsPerPage-1] = _inDrag;
            _inDrag->setPage(source);
            _inDrag->setIndex(_iconsPerPage-1);
        } else {
            _inDrag->setPage(_current);
            _inDrag->setIndex(_nextIdx[_current]);
            _nextIdx[_current]++;
        }
        if (_nextIdx[p] == 0) {
            delPage(p);
        }
        emit pageChanged(_current);
    }
    cursor().setPos(_pageSize.width()/3, _pageSize.height()/3);
}

void VirtualDesktop::expand()
{
    _count++;
    setFixedSize(_count * _pageSize.width(), _pageSize.height());
    _pages.append(-(_count-1) * _pageSize.width());
    QList<QRect> newGridList;
    gridWidth = ICONWIDTH + SPACING * 2;
    gridHeight = ICONHEIGHT + SPACING * 2;
    for (int i = 0; i < _col * _row; i++) {
        int x =  _pageSize.width() * (_count - 1)\
                + (i / _row) * gridWidth;
        int y = (i % _row) * gridHeight;
        newGridList.insert(i, \
                           QRect(x, y, gridWidth, gridHeight));
    }
    _gridTable.append(newGridList);
    QList<IconItem*> newIconList;
    for (int i = 0; i < _col * _row; i++)
        newIconList.insert(i, NULL);
    _iconTable.append(newIconList);
    _nextIdx.append(0);
    emit(pageIncreased());
}

void VirtualDesktop::delPage(int page)
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
    emit(pageDecreased());
}

bool VirtualDesktop::largerThan(const QSize &a, const QSize &b)
{
    int areaA = abs(a.width() * a.height());
    int areaB = abs(b.width() * b.height());
    return areaA > areaB ? true : false;
}

int VirtualDesktop::getNearestIndex(const QRect &rect)
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

void VirtualDesktop::dragMoveEvent(QDragMoveEvent *event)
{
    _dragEvent = true;

    event->accept();
    _itemHeld = false;
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
                QPoint end = _gridTable[_current][i].translated(SPACING, SPACING).topLeft();
                _iconTable[_current][i-1]->move(end);
                _iconTable[_current][i] = _iconTable[_current][i-1];
                _iconTable[_current][i]->setIndex(i);
            }
        }
        else {
            for (int i = s; i < index; i++) {
                QPoint end = _gridTable[_current][i].translated(SPACING, SPACING).topLeft();
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

void VirtualDesktop::dragLeaveEvent(QDragLeaveEvent *event)
{
    _itemHeld = false;
    _inDrag->show();
    int p = _inDrag->page();
    int s = _inDrag->index();
    _inDrag->move(_gridTable[p][s].translated(SPACING, SPACING).topLeft());
    //drag->setPixmap(QPixmap(""));
    //icon->show();
    //event->accept();

    _dragEvent = false;

    Q_UNUSED(event);
}

void VirtualDesktop::dropEvent(QDropEvent *event)
{
    qDebug() << "111111111111111111111111111111111111111";
    event->accept();
    qDebug() << "222222222222222222222222222222222222222";
    _itemHeld = false;
    /*
    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (!icon)
        return;
    icon->show();
    int p = icon->page();
    int s = icon->index();
    qDebug() << "p: " << p << "\ts: " << s;
    qDebug() << "inDrag: " << _inDrag->page() << "  " << _inDrag->index();
    */
    if (!_inDrag)
        return;
    qDebug() << "333333333333333333333333333333333333333";
    dragLeftTimer->stop();
    dragRightTimer->stop();
    int p = _inDrag->page();
    int s = _inDrag->index();
    qDebug() << "P: " << p << " S: " << s;
    QPoint end = _gridTable[p][s].translated(SPACING, SPACING).topLeft();
    qDebug() << "End: " << end;
    _iconTable[p][s] = _inDrag;
    _inDrag->move(end);
    qDebug() << "Pos: " << _inDrag->pos();
    _inDrag->show();
    qDebug() << "444444444444444444444444444444444444444";
    _inDrag=NULL;
    qDebug() << "555555555555555555555555555555555555555";

    _dragEvent = false;
}

void VirtualDesktop::mouseReleaseEvent(QMouseEvent *event)
{
    _itemHeld = false;
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }

    event->accept();
    int offset = x() - _pages[_current];
    _animation->setDuration(900);
    //_animation->setEasingCurve(QEasingCurve::OutBack);
    if (abs(offset) > _pageSize.width()/6) {
        if (offset < 0) {
            if (_current == _count - 1) {
                _animation->setStartValue(QRect(x(), y(), \
                                                width(), height()));
                _animation->setEndValue(QRect(_pages[_current], y(), \
                                              width(), height()));
            } else {
                goNext();
            }
        } else {
            if (_current == 0) {
                emit toOrigin();
                _animation->setStartValue(QRect(x(), y(), \
                                                width(), height()));
                _animation->setEndValue(QRect(_pages[_current], y(), \
                                              width(), height()));
            } else {
                goPrevious();
            }
        }
    } else {
        _animation->setStartValue(QRect(x(), y(), \
                                        width(), height()));
        _animation->setEndValue(QRect(_pages[_current], y(), \
                                      width(), height()));
    }
    _animation->start();
}

void VirtualDesktop::mouseMoveEvent(QMouseEvent *event)
{
    if (_animation->state() == QAbstractAnimation::Running)
        _animation->stop();

    event->accept();
    int right_bound = -_pageSize.width() * (_count - 1) - 100;

    if (x() < right_bound)
        return;
    int to = x() + event->x() - m2v;
    move(to, y());
    event->accept();
}

void VirtualDesktop::goPage(int page)
{
    _current = page;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();

    emit pageChanged(_current);

    update();
}

void VirtualDesktop::goNext()
{
    _current++;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    emit pageChanged(_current);
}

void VirtualDesktop::goPrevious()
{
    _current--;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    emit pageChanged(_current);
}

int VirtualDesktop::addIcon(const QString &text, \
                            const QString &iconPath, \
                            int page, \
                            int index,\
                            const iconStyle &iSt)
{
//    for (int i = 0; i < _count; i++)
        qDebug() << "************************************************************" << _nextIdx[page];

    int expandPageCount = _local->count() / _iconsPerPage + 1;
    if (expandPageCount > _count)
        expand();

    IconItem *icon = new IconItem(text, iSt, this);
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
            if (_nextIdx[page] < _iconsPerPage) {
                index = _nextIdx[page];
            } else {
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
    _iconDict.insert(text, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));

    /*addicon add the last*/
    showAddIcon(page, -1);

     return page;
    //LocalAppList::getList()->save();
}

int VirtualDesktop::addIcon(const QString &text, const QString &icon, const iconStyle &iSt)
{
    return addIcon(text, icon, _current, -1, iSt);
}

int VirtualDesktop::showAddIcon(int page, int index)
{

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
            if (_nextIdx[page] < _iconsPerPage) {
                index = _nextIdx[page];
            } else {
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
   g_addIcon->setGeometry(_gridTable[page][index].translated(SPACING, SPACING));
   g_addIcon->show();
    
    return page;  
}

void VirtualDesktop::delIcon(const QString &text)
{

    IconItem *icon = _iconDict.take(text);
    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();
    //icon->setPage(-1);
    //icon->setIndex(-1);
    delete _iconTable[p][s];

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

    if ((_count > 1) && (_local->count() < _iconsPerPage * (_count - 1)))
        delPage(_count - 1);

    /*move addicon add the last*/
    if (_nextIdx[_count - 1] == _iconsPerPage)
        showAddIcon(_count - 1, _nextIdx[_count - 1] - 1);
    else
        showAddIcon(_count - 1, -1);

    if (_local->count() == 0) {
        appCancel();
    }

    bool trembling = true;

    for (int i = 0; i < _local->count(); i++)
    {
        if (!_local->at(i)->isRemote())
        {
            trembling = false;
            break;
        }
    }

    if (trembling) {
        appCancel();
    }
}

void VirtualDesktop::runApp(const QString &text)
{
    qDebug() << text;
    QString tmp;
    QString pram;
    bool isRmote = false;
    for (int i = 0; i < g_myVappList.count(); i++)
    {
        if(g_myVappList.at(i).name == text)
        {
            _appid = g_myVappList.at(i).id;
            Commui._type = g_myVappList.at(i).type;
            isRmote = true;
        }
    }
    if(isRmote)
    {
        appText = text;
        _communi.loadBalance(Commui._name,_appid);
        if(_communi._isNetError)
        {
            QMessageBox::warning(this, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
        }
        return;
    }

    LocalApp *la = _local->getAppByName(text);
    if (!la)
        return;

    char command[2048];
    if (text.startsWith("/")) {
        QFileInfo link(la->execname());
        if (!link.exists()) {
            AppMessageBox box(true, NULL);
            box.setText("ָ���Ӧ�û��ݷ�ʽ�Ѳ�����\n���\"ȷ��\"ɾ��ͼ��");
            if (box.exec())
                LocalAppList::getList()->delApp(la->name());
            return;
        }
        sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());
    } else {
        QFileInfo link(la->execname());
        //   QString exec = AppDataReadExe::Instance()->getExec(la->execname());
        if (!link.exists()) {
            QString msg = QString("%1û�б���ȷ��װ, ����\n����̵������°�װ, �Ƿ�ɾ��ͼ��?").arg(text);
            AppMessageBox box(true, NULL);
            box.setText(msg);
            if (box.exec())
                LocalAppList::getList()->delApp(la->name());
            return;
        }
        sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());
    }
    ShellExecute(NULL, "open", command, "", "", SW_SHOW);
}

void VirtualDesktop::runServerApp()
{

    QString tmp;
    QString pram;
    QString _appid;

    qDebug()<<appText;
    for (int i = 0; i < g_myVappList.count(); i++)
    {
        if(g_myVappList.at(i).name == appText)
        {
            qDebug() << g_myVappList.at(i).name;

            _appid = g_myVappList.at(i).id;
            Commui._type = g_myVappList.at(i).type;
        }
    }


    if(_communi.errID == "10000")
    {
        if (m_dllInitClass && m_dllCloseClass && m_dllCloseAppAll && m_dllStartAppEx )                  //??????? add() ??
        {
            char buf[100];
            DWORD len = GetLogicalDriveStringsA(sizeof(buf)/sizeof(char),buf);
            //
            //
            string strDiskVolume;
            string strFixed;
            string strCDROM;
            string strRemovable;

            for (char* s=buf; *s; s+=strlen(s)+1)
            {
                string strDrivePath = s;
                // GetDriveType
                //
                UINT uDriveType;
                uDriveType = GetDriveTypeA(strDrivePath.c_str());
                strDiskVolume.append(strDrivePath);

                if (uDriveType == DRIVE_CDROM)
                {
                    strCDROM.append(strDrivePath);
                    //qDebug() << "cdrom : " << strDrivePath.c_str();
                }
                else if (uDriveType == DRIVE_REMOVABLE)//mass storage
                {
                    strRemovable.append(strDrivePath);
                    //qDebug() << "removable : " << strDrivePath.c_str();
                }
                else if (uDriveType == DRIVE_FIXED)
                {
                    strFixed.append(strDrivePath);
                    //qDebug() << "fixed : " << strDrivePath.c_str();
                }
            }

            qDebug()<<"string:"<<QString::fromLocal8Bit(strDiskVolume.c_str())<<endl;
            QString diskpath;
            //            if(Commui._driver_loadClientDriverOnLogin == "1")
            //            {
            //                diskpath = QString::fromLocal8Bit(strDiskVolume.c_str());
            //            }
            //            else
            //            {
            //                if(Commui._driver_mapClientStorageOnLogin_check_harddisk == 0)
            //                    diskpath = QString::fromLocal8Bit(strFixed.c_str());
            //                if(Commui._driver_mapClientStorageOnLogin_check_CD == 0)
            //                    diskpath = diskpath + QString::fromLocal8Bit(strCDROM.c_str());
            //            }
            if(Commui._driver_mapClientStorageOnLogin_check_harddisk == "1")
            {
                diskpath = QString::fromLocal8Bit(strFixed.c_str());
                //qDebug() << "fixed : " << strFixed.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_CD == "1")
            {
                diskpath = diskpath + QString::fromLocal8Bit(strCDROM.c_str());
                //qDebug() << "CDROM : " << strCDROM.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_floppy == "1")
            {
                diskpath = diskpath + QString::fromLocal8Bit(strRemovable.c_str());
                //qDebug() << "Removable : " << strRemovable.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_shareStorage == "1")
            {
                //diskpath = diskpath + QString::fromLocal8Bit(strRemovable.c_str());
            }

            qDebug()<<"diskpath:"<<diskpath;
            diskpath.replace("\\" , ",");
            qDebug()<<"count:"<<diskpath.count();
            diskpath.remove((diskpath.count() - 1),1);
            qDebug()<<"diskPath:"<<diskpath;
            qDebug()<<"cdrom:"<<QString::fromLocal8Bit(strCDROM.c_str());

            tmp = Commui._path;
            pram= Commui._param;
            const wchar_t* _param=reinterpret_cast<const wchar_t*>(pram.utf16());
            const wchar_t* ty = reinterpret_cast<const wchar_t*>(tmp.utf16());
            qDebug() << "passwd::" << Commui._pswd;
            //Commui._pswd = decryptRsa();
            qDebug() << "passwd::" << Commui._pswd;
            const wchar_t* pswd = reinterpret_cast<const wchar_t*>(Commui._pswd.utf16());
            const wchar_t* localime = reinterpret_cast<const wchar_t*>(Commui._redirect_inputMethodRedirect.utf16());

            if(Commui._userType == "1" || Commui._userType == "2")
            {
                Commui._applogin = Commui._domain +  "\\" + Commui._name;
            }
            else
            {
                Commui._applogin = Commui._name;
            }
            const wchar_t* name = reinterpret_cast<const wchar_t*>(Commui._applogin.utf16());
            const wchar_t* appName = reinterpret_cast<const wchar_t*>(appText.utf16());
            const wchar_t* ipPort = reinterpret_cast<const wchar_t*>(Commui._rdpIpPort.utf16());
            const wchar_t* audio = reinterpret_cast<const wchar_t*>(Commui._audio_clientAudioOutput.utf16());
            const wchar_t* _diskpath = reinterpret_cast<const wchar_t*>(diskpath.utf16());
            qDebug()<<"name:"<<Commui._name;
            qDebug()<<"ipPort:"<<Commui._rdpIpPort<<endl;
            bool tray;
            if("1" == Commui._type)
            {
                tray = true;
            }
            else
            {
                tray = false;
            }

            bool ok;
            m_dllInitClass(L"", L"");
            qDebug()<<"audio::"<<Commui._audio_clientAudioOutput.toULong(&ok,10);
            int _audio;
            if(Commui._audio_clientAudioOutput.toULong(&ok,10)==1)
            {
                _audio = 3;
            }
            else
            {
                _audio = 1;
            }


            DWORD colorDepth=24;
            switch (Commui._viewLimited_colorDepth.toUInt(&ok,10))
            {
            case 8:
                colorDepth = 8;
                break;
            case 16:
                colorDepth = 16;
                break;
            case 24:
                colorDepth = 24;
                break;
            case 32:
                colorDepth = 32;
                break;
            default:
                colorDepth = 24;
                break;
            }

            //    qDebug()<<"_param"<<pram;
            BOOL FontSmooth;
            if(Commui._FontSmooth=="1")
                FontSmooth=true;
            else
                FontSmooth=false;


            int _DesktopWidth,_DesktopHeight;
            /*if(Commui._viewLimited_resolution_width=="0")
             {
                 _DesktopWidth=qApp->desktop()->rect().width();
                 _DesktopHeight=qApp->desktop()->rect().height();
             }
            else
            {*/
            _DesktopWidth=Commui._DesktopWidth.toULong(&ok,10);
            _DesktopHeight=Commui._DesktopHeight.toULong(&ok,10);
            //}

            DWORD dwForbidApps = 0x00;
            if(Commui._applicationForbid_cmd == "1")
            {
                dwForbidApps |= 0x0001;
            }
            if(Commui._applicationForbid_explorer == "1")
            {
                dwForbidApps |= 0x0002;
            }
            if(Commui._applicationForbid_SysManage == "1")
            {
                dwForbidApps |= 0x0004;
            }
            if(Commui._applicationForbid_help == "1")
            {
                dwForbidApps |= 0x0008;
            }

            if(tray)
                qDebug()<<"tray program";
            else
                qDebug()<<"no tray program.";

            qDebug()<<"-----dwForbidApps-" << dwForbidApps;
            m_dllStartAppEx(0, 0, Commui._redirect_inputMethodRedirect.toULong(&ok,10), _audio, \
                            colorDepth, 0, Commui._type.toULong(&ok, 10), 0, L"{123654789}",pswd, \
                            name, ipPort, appName, ty,\
                            L"", _diskpath, _param , Commui._interface_COM.toULong(&ok,10),\
                            Commui._printer_supportVirtualPrint.toULong(&ok,10) , tray,\
                            _DesktopHeight ,_DesktopWidth,Commui._other_closeClipboard.toULong(&ok,10),\
                            FontSmooth,Commui._RdpPort.toULong(&ok,10),dwForbidApps);
        }
        else
        {
            QMessageBox::information(NULL,"NO","Linke to Function is not OK!!!!");
            return;  //???????
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
        return;
    }
}


void VirtualDesktop::printDesktop()
{
    qDebug() << "-----------------------------------------------";
    qDebug() << "Count : " << _count  << "; " \
             << "Current: " << _current << "; " \
             << "Geo: " << geometry() << "; ";
    for (int i = 0; i < _count; i++) {
        qDebug() << "@@@@@@@@@@@@@  " \
                 << "Page: " << i \
                 << "  @@@@@@@@@@@@@";
        for (int j = 0; j < _nextIdx[i]; j++) {
            IconItem *item = _iconTable[i][j];
            qDebug() << item->text()<< "\t\t" \
                     << item->page() << ":" \
                     << item->index()<< "\t" \
                     << item->geometry();
        }
    }
    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
}

void VirtualDesktop::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);

    QCursor cur = this->cursor();
    _menu = new QMenu(this);

    _menu->addAction(_addAction);
    if (_local->count() == 0) {

    } else {
        if (!_trembling)
        {
            for (int i = 0; i < _local->count(); i++)
            {
                if (!_local->at(i)->isRemote())
                {
                    _menu->addAction(_deleteAction);
                    break;
                }
            }
        }
        else {
            for (int i = 0; i < _local->count(); i++)
            {
                if (!_local->at(i)->isRemote())
                {
                    _menu->addAction(_cancelAction);
                    break;
                }
            }
        }
    }
    _menu->exec(cur.pos());

}

void VirtualDesktop::appAdd()
{
    _addAppState = true;
    QString path = QFileDialog::getOpenFileName(this, tr("ѡ��һ��Ӧ�ó�����ݷ�ʽ"),
                                                Config::get("CommonProgramDir"),
                                                tr("app (*.lnk *.exe)"));

    if (path.isEmpty())
    {
        _addAppState = false;
        return;
    }

    path.replace('/','\\');
    AppDataReadExe::Instance()->addLocalApp(path);
    _addAppState = false;
}

void VirtualDesktop::appDelete()
{
    _trembling = true;
    emit trembleStarted();
}

void VirtualDesktop::appCancel()
{
    _trembling = false;
    emit trembleStoped();
}

void VirtualDesktop::upLoad()
{
//    _manageWidget->upLoad();
}

void VirtualDesktop::movetoFist()
{
    _current = 0;
    move(_pages[_current], y());
    emit pageChanged(_current);

}

void VirtualDesktop::reloadApplist()
{
    //we need check the page. because we only have one page now;

    int expandPageCount = _local->count() / _iconsPerPage + 1;
    for(int i = _count; i < expandPageCount; i++)
    {
        expand();
    }
    qDebug()<<"reloadApplist  the _count is:"<<_count;

    // we need init some QList
    for (int i = 0; i < _count; i++)
           _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
            QList<QRect> newList;
            for (int j = 0; j < _col * _row; j++) {
                int x =  _pageSize.width() * i \
                         + (j / _row) * gridWidth;
                int y = (j % _row) * gridHeight;

//                qDebug()<<"new Grid:"<<x<<","<<y<<","<<gridWidth<<","<<gridHeight;
                newList.insert(j, QRect(x, y, gridWidth, gridHeight));
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

    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;

        if(_local->at(i)->isRemote())
        {
            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                    _local->at(i)->page(), _local->at(i)->index(), vappIcon);
        }else{
            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                    _local->at(i)->page(), _local->at(i)->index(), localIcon);
        }
    }

    qDebug()<<"reload all.";
}

void VirtualDesktop::deleteAllIconItem()
{
    _local->save();

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

HoverIconItem::HoverIconItem(int width, int height, QWidget *parent)
    : QWidget(parent)
    , _width(width)
    , _height(height)
{
    resize(_width, _height);
}

void HoverIconItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(QBrush(QColor(255, 255, 255, 25)));
    painter.drawRoundRect(QRect(0, 0, _width - 1, _height - 1), 5, 5);

    Q_UNUSED(event);
}
