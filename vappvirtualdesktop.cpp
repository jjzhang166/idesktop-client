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
#include <cstdlib>
#include <cstdio>
#include <cassert>
#ifdef Q_WS_WIN
    #include <iostream>
    #include <windows.h>
    #include <shellapi.h>
    //#include "hook.h"
    #include "strings.h"
    using namespace std;
#endif
#ifdef Q_WS_X11
    #include <QFile>
    #include <QImageReader>
    #include <QFontMetrics>
    #include <QByteArray>
    #include <QtNetwork> 
    #include <QDesktopServices>
    #include <QDir>
#endif
#include <QDesktopWidget>
#include <QToolTip>
#include <QHelpEvent>
#include "vappvirtualDesktop.h"

//#include "dlglogin.h"

//#include "localapps.h"
#include <QAbstractEventDispatcher>
#include <QPalette>
//#include "movingdialog.h"
//#include "client/appdatareadexe.h"
//#include "appmessagebox.h"

#define INDICATOR_ITEMSIZE QSize(14, 14)
//??//
//#define ICONWIDTH 96
//#define ICONHEIGHT 96
#define ICONWIDTH 48
#define ICONHEIGHT 48
//#define CLOSEWIDTH 30
//#define CLOSEHEIGHT 30
#define CLOSEWIDTH 15
#define CLOSEHEIGHT 15
#ifdef Q_WS_WIN
	#define ICON_PATH ("./icon/")
#endif
#ifdef Q_WS_X11
   // #include "dlglogin.h"
	#define ICON_PATH (xmlPath + "/icon/")
#endif
//??//
#define FONTSIZE 10
//#define FONTSIZE 11

#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"

QString _icontext;
bool _quit;
int _tray;
#include <QDebug>
#include <QMessageBox>
/* ####################################################
 * definition of VappIconItem methods
 */
static QPoint gap;

#include "common.h"
extern bool g_isAutoLogin;
QList<APP_LIST> g_myList;
QString WIN_IconPath;
VappIconItem::VappIconItem(const QString &text, QWidget *parent)
    : QWidget(parent), _text(text), _trembling(0)
{
    int width, height;
    QFontMetrics fm(QFont("", FONTSIZE, QFont::Black));

    width = ICONWIDTH + CLOSEWIDTH/2 + 8;
    height = ICONHEIGHT + CLOSEHEIGHT/2 + fm.height() * 2;
    setFixedSize(width, height);
    _textWidth = fm.width(_text);

    _texticon = _text;
    _texticon_firstrow = _texticon;
    _texticon_secondrow = _texticon;

    int i;
    for (i = 0; i < _text.length(); i++)
    {
        _textWidth_firstrow = fm.width(_texticon_firstrow);
        //qDebug() << "_textWidth : " << _textWidth;
        //qDebug() << "width : " << width;
        if (_textWidth_firstrow > width)
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
    _texticon_secondrow = _texticon.right(_text.length() - _texticon_firstrow.length());
    qDebug() << "secondrow : " << _texticon_secondrow;

    _textWidth_secondrow = fm.width(_texticon_secondrow);
    if (_textWidth_secondrow > width)
    {
        for (i = 0; i < _text.length(); i++)
        {
            _textWidth_secondrow = fm.width(_texticon_secondrow);
            if (_textWidth_secondrow > width)
            {
                _texticon_secondrow.chop(1);
            }else{
                _texticon_secondrow.chop(3);
                _texticon_secondrow.append("...");
                break;
            }
        }
    }

    //if (_text.startsWith("/"))
     //   _textWidth = fm.width(_text.right(_text.length() - 1));
    _textHeight = fm.height();


    //_app = VirutalAppList::getList()->getAppByName(_text);
    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setDuration(200);
    _animation->setEasingCurve(QEasingCurve::OutBack);

    _timeline = new QTimeLine(5000, this);
    _timeline->setLoopCount(0);
    _timeline->setUpdateInterval(65);
    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(doTremble(qreal)));
    if (parent) {
            connect(static_cast<VappVirtualDesktop*>(parent), \
                SIGNAL(trembleStarted()), \
                this, SLOT(startTremble()));
            connect(static_cast<VappVirtualDesktop*>(parent), \
                SIGNAL(trembleStoped()), \
                this, SLOT(stopTremble()));
    }
}

VappIconItem::~VappIconItem()
{
    delete _animation;
}

void VappIconItem::setPage(int page)
{
    _page = page;
    //_app->setPage(page);
}
void VappIconItem::setIndex(int index)
{
    _index= index;
    //_app->setIndex(index);
}
void VappIconItem::setHidden(bool hide)
{
    //_app->setHidden(hide);
    Q_UNUSED(hide);
}

void VappIconItem::animationMove(const QRect &start, const QRect &end)
{
    _animation->setStartValue(start);
    _animation->setEndValue(end);
    _animation->start();
}
	
void VappIconItem::setToolTip(const QString &toolTip)
{
	mytooltip = toolTip;
}

void VappIconItem::doTremble(qreal value)
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

void VappIconItem::startTremble()
{
    _timeline->start();
    _pixmap = _closePixmap;
}

void VappIconItem::stopTremble()
{
    _timeline->stop();
    _trembling = 0;
    _pixmap = _normalPixmap;
    repaint();
}

void VappIconItem::paintEvent(QPaintEvent *event)
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

void VappIconItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && \
        geometry().contains(event->pos())) {
        dragStartPosition = event->pos();
    }
    event->ignore();
}

void VappIconItem::mouseMoveEvent(QMouseEvent *event)
{
    qDebug()<<"left button up."<<endl;
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength() \
            < QApplication::startDragDistance()) {
        return;
    }
    _drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    qDebug()<<"icontext:"<<_text;
    _icontext=_text;
    #ifdef Q_WS_WIN
      mimeData->setText(_text);
    #endif
    _drag->setMimeData(mimeData);
    _drag->setPixmap(grayPixmap());
    _drag->setHotSpot(event->pos());
    gap = _drag->hotSpot();
    _drag->exec(Qt::MoveAction);
}

void VappIconItem::enterEvent(QEvent *event)
{
    setCursor(Qt::PointingHandCursor);
    qDebug()<<"enter";
    _pixmap = darkPixmap();

    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    qDebug()<<"Tooltip:"<<helpEvent->pos();
    QToolTip::showText(QCursor::pos(), _text);
    repaint();
}

void VappIconItem::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

    if (_timeline->state() == QTimeLine::Running)
        _pixmap = _closePixmap;
    else
        _pixmap = _normalPixmap;

    repaint();
}
/*
const QPixmap& VappIconItem::originPixmap()
{
    return _originPixmap;
}
*/
void VappIconItem::setPixmap(const QString &icon)
{
    int begin;

    QString icon1;
    if(icon.isEmpty())
       icon1 ="logo1.ico";
    else
         icon1 = icon;
    QImage image = QImage(icon).scaled(ICONWIDTH, ICONHEIGHT);
    QImage normal(width(), height(), QImage::Format_ARGB32_Premultiplied);
    QPainter pt1(&normal);
    QFont font("", FONTSIZE, QFont::Normal);

   // QPalette pal =this->palette();
   // pal->setColor(QPalette::Text,QColor(255,0,0);
   // this->setPalette(pal);
    pt1.setPen(Qt::white);
    //pt1.setFont(QFont("", FONTSIZE, QFont::Black));
    pt1.setFont(font);
    pt1.setRenderHint(QPainter::HighQualityAntialiasing);
    pt1.setCompositionMode(QPainter::CompositionMode_Source);
    pt1.fillRect(normal.rect(), Qt::transparent);
    pt1.drawImage(CLOSEWIDTH/2 - 1, CLOSEHEIGHT/2, image);
    //QFontMetrics fm(QFont("", FONTSIZE, QFont::Black));
    QFontMetrics fm(font);
//    begin = (ICONWIDTH - _textWidth) / 2 + CLOSEWIDTH/2 - 3;
//    if (begin < CLOSEWIDTH/2)
//        begin = CLOSEWIDTH/2;
//    pt1.drawText( QRect(begin, ICONHEIGHT + CLOSEHEIGHT/2 ,
//                        width() - begin, _textHeight), Qt::TextSingleLine, text);
    _textWidth = fm.width(_texticon_firstrow);
    QString text = _texticon_firstrow;
    pt1.drawText( QRect(0, ICONHEIGHT + CLOSEHEIGHT/2 ,width(), _textHeight), Qt::TextSingleLine, text);
    _textWidth = fm.width(_texticon_secondrow);
    text = _texticon_secondrow;
    pt1.drawText( QRect((ICONWIDTH - _textWidth) / 2, ICONHEIGHT + CLOSEHEIGHT/2 + _textHeight ,_textWidth, _textHeight), Qt::TextSingleLine, text);
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
    pt2.setPen(Qt::black);
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
    QImage closeImage(":images/close_icon.png");
    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(resultImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, normal);
    painter.drawImage(0, 5, closeImage);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.end();
    _closePixmap = QPixmap::fromImage(resultImage);
}

void VappIconItem::setText(const QString &text)
{
    _text = text;
}
/*
bool VappIconItem::isUserType()
{
    if (_text.startsWith("/"))
        return true;
    return false;
}
*/
const QString & VappIconItem::text()
{
    return _text;
}

const QPixmap & VappIconItem::pixmap()
{
    return _pixmap;
}

const QPixmap & VappIconItem::grayPixmap()
{
    return _grayPixmap;
}

const QPixmap & VappIconItem::darkPixmap()
{
    if (_timeline->state() == QTimeLine::Running)
        return _closePixmap;
    return _darkPixmap;
}

/* #####################################################
 * definition of Indicator methods
 */
VappIndicator::VappIndicator(VappVirtualDesktop *vdesk, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint), _vdesk(vdesk)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    reset();
    connect(_vdesk, SIGNAL(reInitiate()), this, SLOT(reset()));
    connect(_vdesk, SIGNAL(pageChanged(int)), this, SLOT(setCurrent(int)));
    connect(_vdesk, SIGNAL(pageIncreased()), this, SLOT(increase()));
    connect(_vdesk, SIGNAL(pageDecreased()), this, SLOT(decrease()));
}

void VappIndicator::reset()
{
    setFixedSize(25 + _vdesk->count() * 17, 19);
    for (int i = 0; i < _labels.count(); i++)
        delete _labels[i];
    _labels.clear();
    QLabel *win = new QLabel(this);
    win->setFixedSize(17, 19);
    win->setPixmap(QPixmap(""));
    win->move(0, 0);
    win->show();
    _labels.push_back(win);
    for (int i = 0; i < _vdesk->count(); i++) {
        QLabel *l = new QLabel(this);
#ifdef Q_WS_WIN
        l->setPixmap(QPixmap(":images/next_page_click.png"));
#endif
#ifdef Q_WS_X11
        l->setPixmap(QPixmap(":images/next_page_click_linux.png"));
#endif
        l->setFixedSize(17, 11);
        _labels.push_back(l);
        l->move(25 + i * 17, 4);
        l->show();
    }
    setCurrent(_vdesk->currentPage());
    emit iNeedMove();
}

void VappIndicator::increase()
{
    setFixedSize(25 + _vdesk->count() * 17, 19);
    QLabel *l = new QLabel(this);
#ifdef Q_WS_WIN
    l->setPixmap(QPixmap(":images/next_page_click.png"));
#endif
#ifdef Q_WS_X11
    l->setPixmap(QPixmap(":images/next_page_click_linux.png"));
#endif
    l->setFixedSize(17, 11);
    l->show();
    _labels.push_back(l);
    l->move(25 + ( _vdesk->count() - 1) * 17, 4);
    emit iNeedMove();
}
void VappIndicator::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"Indicator:Pos:"<<event->pos();
    int pressindex = 0;
    qDebug()<<"count:"<<_vdesk->count();
    for(int i=0; i <_vdesk->count(); i++)
    {
        if (QRect(25 + i*17, 4, 17, 11)\
                .contains(event->pos()))
        {
            pressindex = i;
            qDebug()<<"pressindex:"<<pressindex;
        }
    }
    _currentindex = _vdesk->currentPage();
    if(pressindex > _currentindex)
    {
        for(int i=0; i < (pressindex - _currentindex); i++)
            _vdesk->goNext();
    }else{
        for(int i=0; i < (_currentindex - pressindex); i++)
            _vdesk->goPrevious();
    }

}
void VappIndicator::enterEvent(QEvent *event)
{
    setCursor(Qt::PointingHandCursor);

    Q_UNUSED(event);

}
void VappIndicator::decrease()
{
    delete _labels.takeLast();
    setFixedSize(25 + _vdesk->count() * 17, 19);
    emit iNeedMove();
}

void VappIndicator::setCurrent(int index)
{

    for (int i = 1; i <= _vdesk->count(); i++)
#ifdef Q_WS_WIN
        _labels.at(i)->setPixmap(QPixmap(":images/next_page_click.png"));
#endif
#ifdef Q_WS_X11
        _labels.at(i)->setPixmap(QPixmap(":images/next_page_click_linux.png"));
#endif
#ifdef Q_WS_WIN
    _labels.at(index+1)->setPixmap(QPixmap(":images/next_page_normal.png"));
#endif
#ifdef Q_WS_X11
    _labels.at(index+1)->setPixmap(QPixmap(":images/next_page_normal_linux.png"));
#endif
}

/* ########################################################
 * definition of VappVirtualDesktop methods
 */
VappVirtualDesktop::VappVirtualDesktop(QSize pageSize,  QWidget *parent)
    : QWidget(parent), _vertical(true), \
      _itemClicked(false), _itemHeld(false), _trembling(false)
{

    _count=1;
#ifdef Q_WS_WIN
    //_tray = trayWnd();
#endif
#ifdef Q_WS_X11
    _tray = 0;
#endif
    tcpserverstart();
    //setCursor(QCursor(Qt::OpenHandCursor));
#ifdef Q_WS_WIN
    _quit = false;
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
#ifdef Q_WS_X11
    _run=false;
	linknum = 0; 
#endif

    QDesktopWidget *dl = QApplication::desktop();
    cr = dl->availableGeometry();
    if(_tray == 1)
        _traycou = qApp->desktop()->rect().width();
    else
        _traycou = cr.width();
    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = _pageSize.height();

    gridWidth = ICONWIDTH + SPACING * 2;
    gridHeight = ICONHEIGHT + SPACING * 2;
    _col = _width / gridWidth;
    _row = (_height - 150) / gridHeight;
    _iconsPerPage = _col * _row;
    _current  = 0;
    qDebug()<<"Iconpage:"<<_iconsPerPage;
    _local = VirutalAppList::getList();

    int maxPage = 0;
    for (int i = 0; i < g_myList.count(); i++) {
        _count = i / _iconsPerPage + 1;
    }


    //_count = ++maxPage;
    qDebug()<<"count:"<<_count;
    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
		     + (j / _row) * gridWidth;
            int y = 60 + (j % _row) * gridHeight;
            newList.insert(j, \
			    QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);
    }

    for (int i = 0; i < _count; i++) {
        QList<VappIconItem*> newList;
        for (int j = 0; j < _col * _row; j++)
            newList.insert(j, NULL);
        _iconTable.insert(i, newList);
    }

    for (int i = 0; i < _count; i++)
        _nextIdx.insert(i, 0);
    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setEasingCurve(QEasingCurve::OutBack);

    setAcceptDrops(true);
    //setAutoFillBackground(true);
    setFixedSize(_width * _count, _height);

    //QPalette pal = palette();
    //pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
   // setPalette(pal);

    for (int i = 0; i < g_myList.count(); i++)
    {
        if (g_myList.at(i).hidden)
            continue;
//        addIcon(g_myList.at(i).name, g_myList.at(i).icon,
//                g_myList.at(i).page, g_myList.at(i).index);
        //addIcon(g_myList.at(i).name, ICON_PATH + g_myList.at(i).id + ".ico",
       //           g_myList.at(i).page, g_myList.at(i).index);
        #ifdef Q_WS_WIN
        addIcon(g_myList.at(i).name, WIN_IconPath + g_myList.at(i).id + ".ico", \
                (i/_iconsPerPage), (i%_iconsPerPage));
        #else
        addIcon(g_myList.at(i).name, ICON_PATH + g_myList.at(i).id + ".ico", \
                (i/_iconsPerPage), (i%_iconsPerPage));
        #endif
        qDebug()<<"iconPage:"<<(i/_iconsPerPage);
        qDebug()<<"iconIndex:"<<(i%_iconsPerPage);
    }

    dragLeftTimer = new QTimer(this);
    dragLeftTimer->setInterval(500);
    dragLeftTimer->setSingleShot(true);

    dragRightTimer = new QTimer(this);
    dragRightTimer->setInterval(500);
    dragRightTimer->setSingleShot(true);

    connect(dragLeftTimer, SIGNAL(timeout()), this, SLOT(dragLeft()));
    connect(dragRightTimer, SIGNAL(timeout()), this, SLOT(dragRight()));
    connect(&tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    connect(&_communi, SIGNAL(appRun()), this, SLOT(runServerApp()));
    qDebug()<<"hello";

    //connect(_local, SIGNAL(appAdded(const QString&, const QString&)), this, SLOT(addIcon(const QString&, const QString&)));
    //connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));
}
void VappVirtualDesktop::tcpserverstart()
{
    if(!tcpServer.listen(QHostAddress::LocalHost,4889))
    {
    qDebug()<<tcpServer.errorString();
    close();
    return;
    }

    qDebug()<<"successfully";

}
void VappVirtualDesktop::tcprun(QString in)
{
    qDebug()<<"tcprun:"<<in;


}



void VappVirtualDesktop::acceptConnection()
{
   tcpServerConnection=tcpServer.nextPendingConnection();


   connect(tcpServerConnection,SIGNAL(readyRead()),this,
            SLOT(tcpread()));
   qDebug()<<"acceptConnection!!";

}
void VappVirtualDesktop::tcpread()
{
    QDataStream in(tcpServerConnection);

    //qDebug()<<"readData()"<<"IP"<<tcpServerConnection->peerAddress ().toString ()<<tcpServerConnection->peerPort ();
    qDebug()<<tcpServerConnection->bytesAvailable()<<endl;
    //qDebug()<<"data:"<<tcpServerConnection->readAll()<<endl;

    qDebug()<<"tcp";
    char iddata[50];
    char *p = (char*)malloc(50);
    memset(p, 0, 50);
    in >> p;
    qDebug("appid:%s",p);
    /*
    if(blockSize <= sizeof(quint16) )//
    {
        if(tcpServerConnection->bytesAvailable()>=(int)sizeof(quint16))
            in>>blockSize;

    }
    qDebug()<<"block"<<blockSize;


        //if(tcpServerConnection->bytesAvailable()>=(int)sizeof(quint16))
            //in>>blockSize;



    qDebug()<<"server"<<tcpServerConnection->bytesAvailable();
    if(tcpServerConnection->bytesAvailable()<blockSize)
        return;
    //QString b=QString::fromLocal8Bit(tcpServerConnection->readAll());
    //qDebug()<<"data::"<<b;
    int magic;
    //QString iddata;

    in>>magic;
    qDebug()<<"magic"<<magic;
    if(magic!=0x903 )
        qDebug()<<"tcpError!!!";

    char iddata[blockSize-3];
    char *p = iddata;
    in >> p;
    qDebug("appid:%s",iddata);
    //qDebug()<<"appid"<<iddata;
    */
    QString b;
    b=QString(p);
    for (int i = 0; i < g_myList.count(); i++)
    {
        if(g_myList.at(i).id == b)
        {
            runApp(g_myList.at(i).name);

        }
     }

}
void VappVirtualDesktop::updateClicked()
{
    _itemClicked = false;
}

void VappVirtualDesktop::itemHeld()
{
    if (!_itemHeld)
        return;
    _trembling = true;
    emit trembleStarted();
}

VappIconItem * VappVirtualDesktop::getIconByName(const QString &name)
{
    return _iconDict.value(name, NULL);
}
#ifdef Q_WS_X11
void VappVirtualDesktop::deletelink()
{
    QString qsFolder = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);

    qDebug()<<"desktop:"<<qsFolder;
    for(int i=0;i<link.count();i++)
    {
        QString path=qsFolder + "/" + link[i] + ".desktop";
        qDebug()<<"path::"<<path;
        QDir b;
        b.remove(path);
    }

}
#endif

void VappVirtualDesktop::atExit()
{
    qDebug()<<"atExit!!!";
    //QMessageBox msgBox;
    //msgBox.addButton(tr("OK"),QMessageBox::YesRole);
#ifdef Q_WS_WIN
    if(0 == QMessageBox::warning(NULL, tr("Warning"),
                                             tr("Are you sure to quit?\n"
                                                "Please make sure that you have saved data before exit"),
                                 tr("OK"), tr("Cancel")))
    {
        m_dllCloseClass();
        tcpServer.close();
        //deletelink();
        _quit = true;
    }
    else
        _quit = false;
        return;
#endif
#ifdef Q_WS_X11
    if(_run)
       {
            if(pro->state())
            {
                if(0 == QMessageBox::warning(NULL, tr("Caution"),
                                                         tr("Are you sure to quit?\n"
                                                            "Please make sure that you have saved data before exit"),
                                             tr("OK"), tr("Cancel")))
            {
                    qDebug()<<"vatExit:";
                    _quit = true;
                    tcpServer.close();
                    deletelink();

            }else{
                    qDebug()<<"NO:\n";
                    _quit = false;
                    return;
            }
       }
       }
            deletelink();
            _quit = true;
            tcpServer.close();
#endif
    //_local->save();
}

void VappVirtualDesktop::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    _inProgress = false;

    Q_UNUSED(exitCode);
    Q_UNUSED(status);
}

void VappVirtualDesktop::uninstall(const QString &name)
{
/*
    LocalApp *app = _local->getAppByName(name);
    if (app == NULL)
        return;
    if (name.startsWith("/")) {
        AppMessageBox box(true, NULL);
        box.setText("  delete icon");
        if (!box.exec())
            return;
        _local->delApp(name);
    } else {
        QString uninstCommand = AppDataReadExe::Instance()->getUninstExec(app->uninstName());

        if (!uninstCommand.isEmpty()) {
            AppMessageBox box(true, NULL);
            if (!box.exec())
                return;

            _local->delApp(name);
            QByteArray tmp = uninstCommand.toAscii();
            if (uninstCommand.startsWith("\""))
                tmp = uninstCommand.mid(1, uninstCommand.length() - 2).toAscii();

            /////////////////////////////////////
            memset(&si, 0, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOW;

            CreateProcess(tmp.data(),NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
        }
        else {
            AppMessageBox box(true, NULL);
            if (!box.exec())
                return;
            _local->delApp(name);
        }
    }
	*/
}

void VappVirtualDesktop::moveItem(VappIconItem *item, int target)
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
                VappIconItem *icon = _iconTable[i][j];
                if (icon != NULL) {
                    icon->setPage(i);
                    icon ->setIndex(j);
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

void VappVirtualDesktop::reload()
{
    emit reInitiate();
}

void VappVirtualDesktop::arrangeEqually(int pageCount)
{
    QList<VappIconItem*> seqList;
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

    int gridWidth = ICONWIDTH + SPACING * 2;
    int gridHeight = ICONHEIGHT + SPACING * 2;
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
        QList<VappIconItem*> newList;
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

void VappVirtualDesktop::arrangeMinimum()
{
    //printDesktop();
    QList<VappIconItem*> seqList;
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

    int gridWidth = ICONWIDTH + SPACING * 2;
    int gridHeight = ICONHEIGHT + SPACING * 2;
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
        QList<VappIconItem*> newList;
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

void VappVirtualDesktop::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }


    if (_trembling && event->button() == Qt::RightButton) {
        emit trembleStoped();
        _trembling = false;
    }
    VappIconItem *tmp = static_cast<VappIconItem*>(childAt(event->pos()));
    if (tmp) {

        if (event->button() == Qt::LeftButton) {

            runApp(tmp->text());
            _itemClicked = false;

        }
    } else {
    }
    m2v = event->x();
    gm2v = mapToGlobal(event->pos()).x();
    event->accept();
}

void VappVirtualDesktop::mousePressEvent(QMouseEvent *event)
{

    qDebug()<<"Virtual desk..Press!";

    if (_animation->state() == QAbstractAnimation::Running) {
        _animation->stop();
    }

    VappIconItem *item = static_cast<VappIconItem*>(childAt(event->pos()));
    if (event->button() == Qt::RightButton) {
        if (item) {
            return;
            if (count() == 1) {
                return;
            }

            return;
        }
        else if (_trembling) {
            emit trembleStoped();
            _trembling = false;
            return;
        }
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
                //uninstall(item->text());
                return;
            }
        }
        if (_itemClicked) {
            if (!_trembling) {
                //runApp(item->text());
                _itemClicked = false;
            }
        } else {
            //QTimer::singleShot(400, this, SLOT(updateClicked()));
            //QTimer::singleShot(1000, this, SLOT(itemHeld()));
            _itemHeld = true;
            _itemClicked = true;
        }
    } else {
    }
    m2v = event->x();
    gm2v = mapToGlobal(event->pos()).x();
    event->accept();
}

void VappVirtualDesktop::dragEnterEvent(QDragEnterEvent *event)
{
    _itemHeld = false;
    VappIconItem *icon = qobject_cast<VappIconItem*> (event->source());
    _inDrag = icon;
    icon->hide();
    event->accept();
}

void VappVirtualDesktop::dragRight()
{
    if (!_inDrag)
        return;
    QDesktopWidget *dl = QApplication::desktop();
    QRect r = dl->availableGeometry();
    if (QRect(_traycou - 20, r.height()/2-_height/2, 20, _pageSize.height())\
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
            VappIconItem *moving = _inDrag;
            moving->setPage(source);
            moving->setIndex(_iconsPerPage - 1);
            VappIconItem *last = _iconTable[source][_iconsPerPage - 1];
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
        cursor().setPos(_traycou-150, cr.height()/2-100);
        qDebug() << "END OF DRAGRIGHT";
    }
}

void VappVirtualDesktop::dragLeft()
{
    qDebug()<<"drag left.";

    if (!_inDrag)
    {
        qDebug()<<"drag left..return";
        return;
    }
    if (QRect(_traycou-_width, y(), 20, _pageSize.height())\
            .contains(cursor().pos()))
    {
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
            VappIconItem *last = _iconTable[_current][_iconsPerPage-1];
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
        cursor().setPos(_traycou-150, cr.height()/2);
    }
    else
        return;
}

void VappVirtualDesktop::expand()
{
    _count++;
    qDebug()<<"expand now,the new count is:"<<_count;

    setFixedSize(_count * _pageSize.width(), _pageSize.height());
    _pages.append(-(_count-1) * _pageSize.width());
    QList<QRect> newGridList;
    int gridWidth = ICONWIDTH + SPACING * 2;
    int gridHeight = ICONHEIGHT + SPACING * 2;
    for (int i = 0; i < _col * _row; i++) {
        int x =  _pageSize.width() * (_count - 1)\
                 + (i / _row) * gridWidth;
        int y = 60 + (i % _row) * gridHeight;

        qDebug()<<"new Grid:"<<x<<","<<y<<","<<gridWidth<<","<<gridHeight;

        newGridList.insert(i, \
            QRect(x, y, gridWidth, gridHeight));
    }
    _gridTable.append(newGridList);
    QList<VappIconItem*> newIconList;
    for (int i = 0; i < _col * _row; i++)
        newIconList.insert(i, NULL);
    _iconTable.append(newIconList);
    _nextIdx.append(0);
    emit(pageIncreased());
}

void VappVirtualDesktop::delPage(int page)
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
            VappIconItem *icon = _iconTable[i][j];
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

bool VappVirtualDesktop::largerThan(const QSize &a, const QSize &b)
{
    int areaA = abs(a.width() * a.height());
    int areaB = abs(b.width() * b.height());
    return areaA > areaB ? true : false;
}

int VappVirtualDesktop::getNearestIndex(const QRect &rect)
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

void VappVirtualDesktop::dragMoveEvent(QDragMoveEvent *event)
{
    QDesktopWidget *dl = QApplication::desktop();
    QRect r = dl->availableGeometry();
    event->accept();
    _itemHeld = false;
    int p = _inDrag->page();
    int s = _inDrag->index();
    /* The virtual desktop can only expand to right */ 
    /* If the page is the last page and there is only one icon
     * on this page, this icon can't be moved to add a page
     */
    if (QRect(_traycou - _width, r.height()/2-_height/2, _width, _pageSize.height())\
            .contains(mapToGlobal(event->pos()))) {   //???????
    /* move to the right page */
    if (QRect(_traycou - 20, r.height()/2 - _height/2, 20, _pageSize.height())\
            .contains(mapToGlobal(event->pos()))) {
        if (p == (_count-1) && _iconTable[p][1] == NULL)
            return;
        dragRightTimer->start();
        //QTimer::singleShot(500, this, SLOT(dragRight()));
    }
    /* move to the left page */
    else if (QRect(_traycou-_width, r.height()/2-_height/2, 40, _pageSize.height())\
            .contains(mapToGlobal(event->pos()))) {
        if (_current == 0)
            return;
        dragLeftTimer->start();
    }
    /* move in this page */
    else {
        QRect moving(event->pos() - gap, _inDrag->size() * 1.0);
        int index = getNearestIndex(moving);
        if (index == -1 || s == index) 
            return;
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
    else{


        qDebug()<<"screen!!\n";
    }
}
void VappVirtualDesktop::dragLeaveEvent(QDragLeaveEvent *event)
{
    _itemHeld = false;
    _inDrag->show();
    int p = _inDrag->page();
    int s = _inDrag->index();
    _inDrag->move(_gridTable[p][s].translated(SPACING, SPACING).topLeft());
    //drag->setPixmap(QPixmap(""));
    //icon->show();
    if (QRect(_traycou-_width, cr.height()/2- _height/2, _width, _pageSize.height())\
                .contains(QCursor::pos())) {
        qDebug()<<"drag in leave!!!";
    }else
    {
        //setCursor(QCursor(Qt::SizeAllCursor));
        qDebug()<<"out!!!";
        qDebug()<<QCursor::pos();

        qDebug()<<"outhook!!";
#ifdef Q_WS_WIN
        //setHook();
#endif
#ifdef Q_WS_X11
                QString _appid;
                for (int i = 0; i < g_myList.count(); i++)
                {
                    if(g_myList.at(i).name == _icontext)
                    {
                        _appid = g_myList.at(i).id;
                    }
                 }
                QString qsFolder = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
                QString appname = "V-" + _icontext;
                //QTextCodec *codec = QTextCodec::codecForName("gb18030");q
                //QString string = codec->toUnicode(appname.toUtf8());
                QString _nameD = "Name=" + appname + "\n";
                QString _desktoppath = qsFolder + "/" + appname + ".desktop";
                qDebug()<<"desktoppath:"<<_desktoppath;
                QString _exec = "Exec=/usr/local/bin/connect " + _appid + "\n";
                qDebug()<<"exec:"<<_exec;
                QString _iconpath = "Icon=" + ICON_PATH + _appid + ".ico\n";
                qDebug()<<"iconpath:"<<_iconpath;
                QFile data(_desktoppath);
                if (data.open(QFile::WriteOnly)) {
                QTextStream out(&data);
                out << "[Desktop Entry]\n";
                out << "Encoding=UTF-8\n";
                out << "Comment=Client for connection to a APS server\n";
                out << _exec;
                out << _iconpath;
                out << _nameD;
                out << "Type=Application\n";
                link.insert(linknum, appname);
                linknum++;
		data.close();	
		data.setPermissions(QFile::WriteUser | QFile::ReadUser | QFile::ExeOwner);
                }
#endif
    }
    //event->accept();
}

void VappVirtualDesktop::dropEvent(QDropEvent *event)
{

    event->accept();
    _itemHeld = false;

    if (!_inDrag)
        return;
    dragLeftTimer->stop();
    dragRightTimer->stop();
    int p = _inDrag->page();
    int s = _inDrag->index();
    qDebug() << "P: " << p << " S: " << s;
    qDebug()<<"After drag the rect:"<<_gridTable[p][s].x()<<","<<
              _gridTable[p][s].y();
    QPoint end = _gridTable[p][s].translated(SPACING, SPACING).topLeft();

    qDebug()<<"new item point:"<<end.x()<<","<<end.y();
    _iconTable[p][s] = _inDrag;
    _inDrag->move(end);
    qDebug() << "Pos: " << _inDrag->pos();
    _inDrag->show();

    _inDrag=NULL;

}

void VappVirtualDesktop::mouseReleaseEvent(QMouseEvent *event)
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

void VappVirtualDesktop::mouseMoveEvent(QMouseEvent *event)
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

void VappVirtualDesktop::goNext()
{
    _current++;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    emit pageChanged(_current);
}

void VappVirtualDesktop::goPrevious()
{
    _current--;
    _animation->setDuration(900);
    _animation->setStartValue(QRect(x(), y(), width(), height()));
    _animation->setEndValue(QRect(_pages[_current], y(), \
                                  width(), height()));
    _animation->start();
    emit pageChanged(_current);
}

int VappVirtualDesktop::addIcon(const QString &text, \
                             const QString &iconPath, \
                             int page, \
                             int index)
{

    VappIconItem *icon = new VappIconItem(text, this);
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

    return page;
    //VirutalAppList::getList()->save();
}

int VappVirtualDesktop::addIcon(const QString &text, const QString &icon)
{
    return addIcon(text, icon, _current, -1);
}

void VappVirtualDesktop::delIcon(const QString &text)
{

    VappIconItem *icon = _iconDict.take(text);
    //icon->set Hidden(true);
    int p = icon->page();
    int s = icon->index();

    qDebug()<<"delete name:"<<text<<endl<<"delete icon -->page:"<<p<<"index:"<<s;

    if(icon)
    {
        qDebug()<<"item is right";
        delete _iconTable[p][s];
    }
    else
        qDebug()<<"item is delete all.";

    return;
    int i;
    for (i = s + 1; _iconTable[p][i] != NULL; i++) {
        QRect start = _gridTable[p][i].translated(SPACING, SPACING);
        QRect end = _gridTable[p][i-1].translated(SPACING, SPACING);
        _iconTable[p][i]->animationMove(start, end);
        _iconTable[p][i]->setIndex(i-1);
        _iconTable[p][i -1] = _iconTable[p][i];
    }
    _iconTable[p][i-1] = NULL;
    _nextIdx[p]--;
    if (_nextIdx[p] == 0) {
        delPage(p);
        _current = 0;
        move(_pages[_current], y());
        emit pageChanged(_current);
    }
    //VirutalAppList::getList()->save();
}



void VappVirtualDesktop::runApp(const QString &text)
{
    QString tmp;
    QString pram;
    for (int i = 0; i < g_myList.count(); i++)
    {
        if(g_myList.at(i).name == text)
        {
            _appid = g_myList.at(i).id;
            Commui._type = g_myList.at(i).type;
        }
     }
    appText = text;
    _communi.loadBalance(Commui._name,_appid);
    if(_communi._isNetError)
    {
        QMessageBox::warning(this, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
    }
}



#ifdef Q_WS_WIN
void VappVirtualDesktop::runServerApp()
{

    QString tmp;
    QString pram;
    QString _appid;

    qDebug()<<appText;
    for (int i = 0; i < g_myList.count(); i++)
    {
        if(g_myList.at(i).name == appText)
        {
            _appid = g_myList.at(i).id;
            Commui._type = g_myList.at(i).type;
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
#endif
#ifdef Q_WS_X11
void VappVirtualDesktop::runServerApp()
{
    QString pram;
    QString tmp;
    _run=true;
    pro=new QProcess(this);
    qDebug()<<"text:"<<appText<<endl;
    for (int i = 0; i < g_myList.count(); i++)
    {
        if(_communi.errID == "10000")
        {
            if(g_myList.at(i).name == appText)
            {
                pram=Commui._param;
                tmp=Commui._path;
                qDebug()<<"#######param:"<<pram<<"#######";
                 qDebug()<<"#######path:"<<tmp<<"#######";
                QString rdpplugin;
                if(Commui._audio_clientAudioOutput == "1")
                        rdpplugin = " --plugin rdpsnd --data alsa -- -k 0x00000409 ";
                else
                        rdpplugin = "";
                //1 disable 0 enable
                if(Commui._driver_mapClientStorageOnLogin_check_harddisk == "1" && (Commui._interface_COM == "0") && (Commui._printer_supportVirtualPrint == "0"))
                        rdpplugin = rdpplugin + "";
                else{
                        rdpplugin = rdpplugin + " --plugin rdpdr --data";                  
                        if(Commui._driver_mapClientStorageOnLogin_check_harddisk == "0")
                        {
                                char *path = getenv("HOME");
                                QString home_path = QString(path);
                                qDebug()<<"home_path~~~"<<home_path;
                                rdpplugin = rdpplugin + " disk:"+Commui._name+":"+home_path;
                        }
                        else
                                rdpplugin = rdpplugin;

                        if(Commui._driver_mapClientStorageOnLogin_check_CD== "0")
                                rdpplugin = rdpplugin + " disk:CDROM"+":/media";
                        else
                                rdpplugin = rdpplugin;


                        if(Commui._interface_COM == "1")
                                rdpplugin = rdpplugin + " serial:COM1:/dev/ttyS0 serial:COM2:/dev/ttyS1 serial:COM3:/dev/ttyS2 serial:COM4:/dev/ttyUSB0 serial:COM5:/dev/ttyUSB1 serial:COM6:/dev/ttyUSB2";
                        else
                                rdpplugin = rdpplugin;
                        if(Commui._printer_supportVirtualPrint == "1")
                                rdpplugin = rdpplugin + " printer:\"HP-LaserJet-4050\"";
                        else
                                rdpplugin = rdpplugin;
                        rdpplugin = rdpplugin + " --";
                }


                if(Commui._viewLimited_resolution_height=="0")
                {
                    QString width,height;
                    width=QString::number(  qApp->desktop()->rect().width(), 10 );
                    height=QString::number(  qApp->desktop()->rect().height(), 10 );
                    rdpplugin = rdpplugin    +" -g " + width + "*" + height + " ";
                    qDebug()<<"~~~~~~"<<rdpplugin;
                }
                else
                    rdpplugin = rdpplugin    +" -g " + Commui._viewLimited_resolution_width + "*" + Commui._viewLimited_resolution_height + " ";
                qDebug()<<"rdpplugin"<<rdpplugin<<"\n";
                if(Commui._userType == "1" || Commui._userType == "2")
                            Commui._applogin = Commui._name + "@" + Commui._domain;
                else
                            Commui._applogin = Commui._name;
               // qDebug()<<g_myList.at(iath<<"\n";

                if(tmp.startsWith("c:\\"))
                {
                        QString x="\%CRoot%\\";
                        tmp.replace(0, 3, x);

                        if(pram!="")
                        {
                            qDebug()<<"parameter cannot be NULL~!";
                            tmp=tmp+"::"+pram;
                        }
                        else
                        {
                            qDebug()<<"parameter is NULL~!";
                        }

                        qDebug()<<tmp;
                        //system("export LD_LIBRARY_PATH=/usr/local/lib/:LD_LIBRARY_PATH");
                        QString buf="/opt/i-soft/freerdp/xfreerdp --fonts -u "+Commui._applogin+" -p "+Commui._pswd+" --app --plugin rail --data \""+tmp+"\" -- " + rdpplugin +Commui._rdpIpPort;
                        qDebug()<<"buf"<<buf<<endl;

                        pro->start(buf);
                }
                if(tmp.startsWith("C:\\"))
                {
                        QString x="\%CRoot%\\";
                        tmp.replace(0, 3, x);
                        qDebug()<<"tmp"<<tmp;
                        //system("export LD_LIBRARY_PATH=/usr/local/lib/:LD_LIBRARY_PATH");
                        QString buf="/opt/i-soft/freerdp/xfreerdp --fonts -u "+Commui._applogin+" -p "+Commui._pswd+" --app --plugin rail --data \""+tmp+"\" -- " + rdpplugin + Commui._rdpIpPort;
                        qDebug()<<"buf"<<buf<<endl;

                        pro->start(buf);
                }
                if(tmp.startsWith("d:\\"))
                {
                        QString x="\%DRoot%\\";
                        tmp.replace(0, 3, x);
                        qDebug()<<tmp;
                        //system("export LD_LIBRARY_PATH=/usr/local/lib/:LD_LIBRARY_PATH");
                        QString buf="/opt/i-soft/freerdp/xfreerdp --fonts -u "+Commui._applogin+" -p "+Commui._pswd+" --app --plugin rail --data \""+tmp+"\" -- " + rdpplugin + Commui._rdpIpPort;
                        qDebug()<<"buf"<<buf<<endl;

                        pro->start(buf);
                }
                if(tmp.startsWith("D:\\"))
                {
                        QString x="\%DRoot%\\";
                        tmp.replace(0, 3, x);
                        qDebug()<<tmp;
                        //system("export LD_LIBRARY_PATH=/usr/local/lib/:LD_LIBRARY_PATH");
                        QString buf="/opt/i-soft/freerdp/xfreerdp --fonts -u "+Commui._applogin+" -p "+Commui._pswd+" --app --plugin rail --data \""+tmp+"\" -- " + rdpplugin + Commui._rdpIpPort;
                        qDebug()<<"buf"<<buf<<endl;

                        pro->start(buf);
                }
                if(tmp.startsWith("e:\\"))
                {
                        QString x="\%ERoot%\\";
                        tmp.replace(0, 3, x);
                        qDebug()<<tmp;
                        //system("export LD_LIBRARY_PATH=/usr/local/lib/:LD_LIBRARY_PATH");
                        QString buf="/opt/i-soft/freerdp/xfreerdp --fonts -u "+Commui._applogin+" -p "+Commui._pswd+" --app --plugin rail --data \""+tmp+"\" -- " + rdpplugin + Commui._rdpIpPort;
                        qDebug()<<"buf"<<buf<<endl;

                        pro->start(buf);
                }
                if(tmp.startsWith("E:\\"))
                {
                        QString x="\%ERoot%\\";
                        tmp.replace(0, 3, x);
                        qDebug()<<tmp;
                        //system("export LD_LIBRARY_PATH=/usr/local/lib/:LD_LIBRARY_PATH");
                        QString buf="/opt/i-soft/freerdp/xfreerdp --fonts -u "+Commui._applogin+" -p "+Commui._pswd+" --app --plugin rail --data \""+tmp+"\" -- " + rdpplugin + Commui._rdpIpPort;
                        qDebug()<<"buf"<<buf<<endl;

                        pro->start(buf);
                }
        }
        }

        else
        {
            QMessageBox::warning(this, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
            return;
        }



    }

   qDebug()<<pro->state();
//???????
    /*
    LocalApp *la = _local->getAppByName(text);
    if (!la)
        return;
    if (la->execname() == "/") {
        if (!_center)
            _center = new AppCenter();
        else
            _center->refresh();
        _center->show();
        return;
    }
    char command[2048];
    sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());
    ShellExecute(NULL, "open", command, "", "", SW_SHOW);
    */
}
#endif
void VappVirtualDesktop::printDesktop()
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
             VappIconItem *item = _iconTable[i][j];
             qDebug() << item->text()<< "\t\t" \
                      << item->page() << ":" \
                      << item->index()<< "\t" \
                      << item->geometry();
        }
    }
    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
}

void VappVirtualDesktop::movetoFist()
{
    _current = 0;
    move(_pages[_current], y());
    emit pageChanged(_current);

}

/*
    this function is used by reload the Applist
    As we click the refresh menu,we need reload Applist.
*/
void VappVirtualDesktop::reloadApplist()
{

    //we need check the page. because we only have one page now;
    int expandPageCount=(g_myList.count()-1)/_iconsPerPage;
    for(int i=0;i<expandPageCount;i++)
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
                int y = 60 + (j % _row) * gridHeight;

                qDebug()<<"new Grid:"<<x<<","<<y<<","<<gridWidth<<","<<gridHeight;
                newList.insert(j, \
                                QRect(x, y, gridWidth, gridHeight));
            }
            _gridTable.insert(i, newList);
        }

    for (int i = 0; i < _count; i++) {
            QList<VappIconItem*> newList;
            for (int j = 0; j < _col * _row; j++)
                newList.insert(j, NULL);
            _iconTable.insert(i, newList);
        }

    for (int i = 0; i < _count; i++)
           _nextIdx.insert(i, 0);


    for (int i = 0; i < g_myList.count(); i++)
        {
            if (g_myList.at(i).hidden)
                continue;

            #ifdef Q_WS_WIN
            addIcon(g_myList.at(i).name, WIN_IconPath + g_myList.at(i).id + ".ico", \
                    (i/_iconsPerPage), (i%_iconsPerPage));
            #else
            addIcon(g_myList.at(i).name, ICON_PATH + g_myList.at(i).id + ".ico", \
                    (i/_iconsPerPage), (i%_iconsPerPage));
            #endif
        }

    qDebug()<<"reload all.";
}

void VappVirtualDesktop::deleteAllVappIconItem()
{
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
