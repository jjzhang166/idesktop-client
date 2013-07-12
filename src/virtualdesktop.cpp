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
#include <QDesktopServices>


#include "virtualdesktop.h"
#include "localapps.h"
#include "movingdialog.h"
#include "appmessagebox.h"
#include "config.h"
#include "dirwidget.h"
//#include "movewidget.h"
#include "paascommuinication.h"

//#define INDICATOR_ITEMSIZE QSize(14, 14)
#define ICONWIDTH 143                                   //72
#define ICONHEIGHT 143                                  //72

#define SPACING 30
#define FONTSIZE 10

#define ICONITEMWIDTH 143                               //(ICONWIDTH + SELECTWIDTH/2 + 8)
#define ICONITEMHEIGHT 143                              //(ICONHEIGHT + 15 + FONTSIZE * 2)

#define CLOSEWIDTH 30
#define CLOSEHEIGHT 30
#define SMALLSLIDERWIDTH 38

#define APPICON 0
#define ADDICON 1
#define HSPACING 4
#define VSPACING 0
#define BOTTOMSPACING 24

#define LARGESIZE QSize(143, 143)     //72*72
#define MEDIUMSIZE QSize(119, 119)     //48*48
#define SMALLSIZE QSize(103, 103)   //32*32

#define ICONHSPACING (88 - 40)
#define ICONVSPACING (88 - 36)

#define KEY "\\Windows\\CurrentVersion\\App Paths\\"
extern QString WIN_VAPP_IconPath;
extern QList<APP_LIST> g_myVappList;
extern QList<LOCAL_LIST> g_RemotelocalList;
extern QList<APP_LIST> g_RemoteappList;
extern QList<PAAS_LIST> g_RemotepaasList;
extern int ICON_TYPE;

/* ####################################################
 * definition of IconItem methods
 */
static QPoint gap;

//IconDesktopItem::IconDesktopItem(QWidget *parent)
//    : IconItem(parent)
//{

//}

//IconDesktopItem::~IconDesktopItem()
//{
//}

//void IconDesktopItem::addMinWidget()
//{
//    _dirMinShowWidget = new DirMinShowWidget(this);
//    _dirMinShowWidget->move(0,0);

//    connect(_dirMinShowWidget, SIGNAL(iconEnter()), this, SIGNAL(iconEnter()));
//    connect(_dirMinShowWidget, SIGNAL(iconMove()), this, SIGNAL(iconMove()));
//    connect(_dirMinShowWidget, SIGNAL(iconLeave()), this, SIGNAL(iconLeave()));
////        connect(_dirMinShowWidget, SIGNAL(iconDrop(int, const QString &, const QString &, const QString &)),
////                this, SLOT(iconDropEvent(int, const QString &, const QString &, const QString &)));
////        connect(_dirMinShowWidget, SIGNAL(openItem(int, int, int)), this, SLOT(openDirWidget(int, int, int)));
//    connect(_dirMinShowWidget, SIGNAL(iconDrop(const QString &, const QString &, const QString &)),
//            this, SLOT(iconDropEvent(const QString &, const QString &, const QString &)));
//    connect(_dirMinShowWidget, SIGNAL(openItem()), this, SLOT(openDirWidget()));
//    connect(_dirMinShowWidget, SIGNAL(dragEnterMinWidget()), SIGNAL(dragEnterMinWidget()));
//}

//void IconDesktopItem::openDirWidget()
//{
//    emit openDir(id(), page(), index());
//}

//void IconDesktopItem::iconDropEvent(const QString &text, const QString &iconPath, const QString &url)
//{
//    emit iconDrop(id(), text, iconPath, url);
//}

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
//    QString path = QFileDialog::getOpenFileName(this, tr("选择一个应用程序或快捷方式"),
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
//    win->setFixedSize(17, 19);
//    win->setPixmap(QPixmap(":images/window_icon_bottom.png"));
//    win->move(0, 0);
//    win->show();
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
    , _animationScreenDown(false)
    , _url("")
    , _iconDragEnter(false)
    , _dirId(-1)
//    , _iconNum(-1)
    , _upDistance(0)
    , _distance(0)
    , _openDir(false)
    , _iconSize(ICON_TYPE)
    , _dragEnterMinWidget(false)
    , _isIconMove(true)
    , _isDirWidgetObject(false)
    , _dragItem(NULL)
    , _localIconNum(0)
    , _dirMovingFinished(true)
//    , _vappCount(0)

{

//    int gridWidth;
//    int gridHeight;
    setFocusPolicy(Qt::ClickFocus);

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

//    gridWidth = ICONITEMWIDTH + HSPACING * 2;
//    gridHeight = ICONITEMWIDTH + BOTTOMSPACING;
    changeSpacing();

    _col = (_width - ICONHSPACING) / gridWidth;
    _row = (_height - ICONVSPACING) / gridHeight;

    _iconsPerPage = _col * _row;
    _current  = 0;

    _local = LocalAppList::getList();

    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == -1)
        {
            _localIconNum ++;
        }
    }

    for (int i = 0; i < _localIconNum; i++)   //_local->count()
    {
        _count = i / _iconsPerPage + 1;
    }

    for (int i = 0; i < _count; i++)
        _pages.insert(i, -(i * _width));

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            //vertical display
//            int x =  _pageSize.width() * i
//                    + (j / _row) * gridWidth;
//            int y = (j % _row) * gridHeight;

            //horizontal display
            int x =  _pageSize.width() * i \
                    + (j % _col) * gridWidth + ICONHSPACING;
            int y = (j / _col) * gridHeight + ICONVSPACING;

            newList.insert(j, \
                           QRect(x, y, gridWidth, gridHeight));
        }
        _gridTable.insert(i, newList);
    }
//    for (int k = 0 ; k < _gridTable.count(); k++)
//        qDebug() << _gridTable.at(k);


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
    //pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    pal.setColor(QPalette::Background, QColor(255,255,255,0));
    setPalette(pal);

    _desktopWidget = QApplication::desktop();
    _desktopRect = _desktopWidget->availableGeometry();
    /*for local desktop add addbutton*/
//    g_addIcon = new IconAddItem(tr(""),this);
//    g_addIcon->setVisible(false);

//    if (_local->count() == 0)
//        g_addIcon->setGeometry(_gridTable[0][0].translated(HSPACING, VSPACING));

    initIconItem();

    dragLeftTimer = new QTimer(this);
    dragLeftTimer->setInterval(500);
    dragLeftTimer->setSingleShot(true);

    dragRightTimer = new QTimer(this);
    dragRightTimer->setInterval(500);
    dragRightTimer->setSingleShot(true);

    _addAction = new QAction(QString(tr("添加")), this);
    _deleteAction = new QAction(QString(tr("开始抖动")), this);
    _cancelAction = new QAction(QString(tr("取消抖动")), this);

    _arrangeWidget = new ArrangeWidget(this);
    _arrangeWidget->move(200,200);
    _arrangeWidget->setVisible(false);

    _normalMenu = new MenuWidget(MenuWidget::normal, this);
    _normalMenu->setVisible(false);

    _showIconMenu = new MenuWidget(MenuWidget::showIcon, this);
    _showIconMenu->setVisible(false);

    _createMenu = new MenuWidget(MenuWidget::create, this);
    _createMenu->setVisible(false);

    _iconMenu = new MenuWidget(MenuWidget::iconMenu, this);
    _iconMenu->setVisible(false);

//    connect(_iconMenu, SIGNAL(rename()), this, SLOT(rename));

    connect(_normalMenu, SIGNAL(menuChanged(int)), this, SLOT(menuChanged(int)));
    connect(_normalMenu, SIGNAL(createDir()), this, SLOT(addDirItem()));
    connect(_normalMenu, SIGNAL(refresh()), this, SLOT(refreshMenu()));
    connect(_showIconMenu, SIGNAL(largeIcon()), this, SLOT(setLargeIcon()));
    connect(_showIconMenu, SIGNAL(mediumIcon()), this, SLOT(setMediumIcon()));
    connect(_showIconMenu, SIGNAL(smallIcon()), this, SLOT(setSmallIcon()));
    connect(_createMenu, SIGNAL(createDir()),this, SLOT(addDirItem()));
    connect(_iconMenu, SIGNAL(run()), this, SLOT(iconMenuRunClicked()));
    connect(_iconMenu, SIGNAL(del()), this, SLOT(iconMenuDelClicked()));


//    _dirWidget = new DirShowWidget(QSize(_desktopRect.width() ,240), this);
//    _dirWidget->setVisible(false);

//    _mW = new MoveWidget(this);
//    _mW->setVisible(false);

//    _animationScreen = new QPropertyAnimation(_mW, "geometry");

//    connect(_animationScreen,SIGNAL(finished()), this, SLOT(scrFinished()));
    connect(_arrangeWidget, SIGNAL(setPos(QPoint)), this, SLOT(arrangeWidgetMove(QPoint)));
    connect(_addAction, SIGNAL(triggered()), this, SLOT(appAdd()));
    connect(_deleteAction, SIGNAL(triggered()), this, SLOT(appDelete()));
    connect(_cancelAction, SIGNAL(triggered()), this, SLOT(appCancel()));

    connect(dragLeftTimer, SIGNAL(timeout()), this, SLOT(dragLeft()));
    connect(dragRightTimer, SIGNAL(timeout()), this, SLOT(dragRight()));

    connect(_local, SIGNAL(appAdded(const QString&, const QString&, const QString &, int))
            , this, SLOT(addIcon(const QString&, const QString&, const QString &, int)));
    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));

    connect(&_communi, SIGNAL(appRun()), this, SLOT(runServerApp()));//

//    connect(g_addIcon, SIGNAL(addApp()), this, SLOT(appAdd()));


//    _dirMinShowWidget = new DirMinShowWidget(this);
//    _dirMinShowWidget->move(300, 200);
//    _dirMinShowWidget->show();

//    connect(_dirMinShowWidget, SIGNAL(iconEnter()), this, SLOT(iconDragEnter()));
//    connect(_dirMinShowWidget, SIGNAL(iconMove()), this, SLOT(iconDragMove()));
//    connect(_dirMinShowWidget, SIGNAL(iconLeave()), this, SLOT(iconDragLeave()));
//    connect(_dirMinShowWidget, SIGNAL(iconDrop(const QString &)), this, SLOT(iconDragDrop(const QString &)));

}

VirtualDesktop::~VirtualDesktop()
{
//    delete /;
#if 0
    for (int j = 0; j < _iconDict.size(); j++)
    {
        delete &_iconDict[0][j];
    }
#endif

    for (int i = 0; i < _dirList.count(); i++) {
        _dirList.at(i)->setParent(NULL);
        delete _dirList.at(i);
    }
    _dirList.clear();

//    for (int i = 0; i < _dirMinList.count(); i++) {
//        _dirMinList.at(i).setParent(NULL);
//        delete _dirMinList.at(i);
//    }
    _dirMinList.clear();
}

void VirtualDesktop::iconDragEnter()
{
    _iconDragEnter = true;

    qDebug() << "DirWidget*************************_iconDragEnter";
//    _dragEnterMinWidget = true;
}

void VirtualDesktop::iconDragMove()
{
    _iconDragEnter = true;

        qDebug() << "DirWidget*************************iconDragMove";
//    _dragEnterMinWidget = true;
}

void VirtualDesktop::iconDragLeave()
{
    _iconDragEnter = false;
//    _dragEnterMinWidget = false;
        qDebug() << "DirWidget*************************iconDragLeave";
}

void VirtualDesktop::iconDragDrop(int id, const QString &text,
                                  const QString &iconPath, int page,
                                  int index, const QString &url, int type)
{
    _iconDragEnter = false;
//    _dragEnterMinWidget = false;
//    getIconByName(text)->setHidden(true);

//    QString icoText(iconPath);
//    icoText.replace(".ico", ".png");

////    emit setDirIcon(text, icoText, url);
//    _dirList.at(id)->addDirIcon(text, icoText, url);
   _dirList.at(id)->addDirIcon(text, iconPath, -1, index, url, type);

    if (!_inDrag)
        return;

    int p = _inDrag->page();
    int s = _inDrag->index();

    _iconDict.take(_inDrag->text());
    _inDrag = NULL;
//    _iconNum--;
//    int expandPageCount = _iconNum / _iconsPerPage + 1;
//    if (expandPageCount < _count)
//        delPage(_count - 1);
    moveBackIcons(p, s);
    _isDirWidgetObject = false;

//    for (int i = 0; i <_dirMinList.count(); i++)
//    {
//        _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(true);
//    }

}

void VirtualDesktop::arrangeWidgetMove(QPoint pos)
{
    _arrangeWidget->move(pos);
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
        box.setText("    确定删除图标？");
        if (!box.exec())
            return;
        _local->delApp(name);
    } else {
        QString uninstCommand = LocalAppList::getList()->getUninstExec(app->uninstName());

        if (!uninstCommand.isEmpty()) {
            AppMessageBox box(true, NULL);
            box.setText("    确定删除图标并调用卸载程序？");
            if (!box.exec())
                return;

            _local->delApp(name);
            QByteArray tmp = uninstCommand.toAscii();
            if (uninstCommand.startsWith("\""))
                tmp = uninstCommand.mid(1, uninstCommand.length() - 2).toAscii();

            /////////////////////////////////////
            STARTUPINFO si; //一些必备参数设置
            memset(&si, 0, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOW;
            PROCESS_INFORMATION pi; //必备参数设置结束

            CreateProcess(tmp.data(),NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
        }
        else {
            AppMessageBox box(true, NULL);
            box.setText("应用已经被卸载，是否删除图标？");
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
                .translated(HSPACING, VSPACING).topLeft();
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
                                      .translated(HSPACING, VSPACING));
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
            .translated(HSPACING, VSPACING).topLeft();
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

//    gridWidth = ICONWIDTH + SPACING * 2;
//    gridHeight = ICONHEIGHT + SPACING * 2;
    gridWidth = ICONITEMWIDTH + HSPACING * 2;
    gridHeight = ICONITEMWIDTH + VSPACING * 2;

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j % _col) * gridWidth + ICONHSPACING;
            int y = (j / _col) * gridHeight + ICONVSPACING;
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
            seqList[m]->move(_gridTable[currentPage][tmpCount].translated(HSPACING, VSPACING).topLeft());
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
        seqList[i]->move(_gridTable[currentPage][eachPage].translated(HSPACING, VSPACING).topLeft());
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

//    gridWidth = ICONWIDTH + SPACING * 2;
//    gridHeight = ICONHEIGHT + SPACING * 2;
    gridWidth = ICONITEMWIDTH + HSPACING * 2;
    gridHeight = ICONITEMWIDTH + VSPACING * 2;

    for (int i = 0; i < _count; i++) {
        QList<QRect> newList;
        for (int j = 0; j < _col * _row; j++) {
            int x =  _pageSize.width() * i \
                    + (j % _col) * gridWidth + ICONHSPACING;
            int y = (j / _col) * gridHeight + ICONVSPACING;
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
        seqList[m]->move(_gridTable[page][idx].translated(HSPACING, VSPACING).topLeft());
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
//    if (_openDir)
//    {
//        closeDir(_dirPage, _dirIndex);
//        return;
//    }

    if (event->modifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

    emit desktopClicked();
    _iconMenu->setVisible(false);
    hideDirWidget();
    hideMenuWidget();

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
            QPoint end = _gridTable[p][i-1].translated(HSPACING, VSPACING).topLeft();
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
                    last->move(_gridTable[target][0].translated(HSPACING, VSPACING).topLeft());
                    last->setPage(target);
                    last->setIndex(0);
                    _nextIdx[target]++;
                    if (moving != _inDrag) {
                        int idx = _nextIdx[source];
                        if (idx >= _iconsPerPage)
                            idx = _iconsPerPage - 1;
                        for (int i = idx; i > 0; i--) {
                            _iconTable[source][i] = _iconTable[source][i-1];
                            _iconTable[source][i]->move(_gridTable[source][i].translated(HSPACING, VSPACING).topLeft());
                            _iconTable[source][i]->setIndex(i);
                        }
                        _iconTable[source][0] = moving;
                        moving->move(_gridTable[source][0].translated(HSPACING, VSPACING).topLeft());
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
                        _iconTable[target][i]->move(_gridTable[target][i].translated(HSPACING, VSPACING).topLeft());
                        _iconTable[target][i]->setIndex(i);
                    }
                    _iconTable[target][0] = last;
                    last->move(_gridTable[target][0].translated(HSPACING, VSPACING).topLeft());
                    last->setPage(target);
                    last->setIndex(0);
                    _nextIdx[target]++;
                    if (moving != _inDrag) {
                        int idx = _nextIdx[source];
                        if (idx >= _iconsPerPage)
                            idx = _iconsPerPage - 1;
                        for (int i = idx; i > 0; i--) {
                            _iconTable[source][i] = _iconTable[source][i-1];
                            _iconTable[source][i]->move(_gridTable[source][i].translated(HSPACING, VSPACING).topLeft());
                            _iconTable[source][i]->setIndex(i);
                        }
                        _iconTable[source][0] = moving;
                        moving->move(_gridTable[source][0].translated(HSPACING, VSPACING).topLeft());
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
            QPoint end = _gridTable[p][i-1].translated(HSPACING, VSPACING).topLeft();
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
                _iconTable[target][i]->move(_gridTable[target][i].translated(HSPACING, VSPACING).topLeft());
                _iconTable[target][i]->setIndex(i);
            }
            _iconTable[target][0] = last;
            last->move(_gridTable[target][0].translated(HSPACING, VSPACING).topLeft());
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
//    gridWidth = ICONWIDTH + SPACING * 2;
//    gridHeight = ICONHEIGHT + SPACING * 2;
//    gridWidth = ICONITEMWIDTH + HSPACING * 2;
//    gridHeight = ICONITEMWIDTH + VSPACING * 2;
    changeSpacing();

    for (int i = 0; i < _col * _row; i++) {
        //vertical display
//        int x =  _pageSize.width() * (_count - 1)
//                + (i / _row) * gridWidth;
//        int y = (i % _row) * gridHeight;

            //horizontal display
            int x =  _pageSize.width() * (_count - 1)\
                    + (i % _col) * gridWidth + ICONHSPACING;
            int y = (i / _col) * gridHeight + ICONVSPACING;

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

void VirtualDesktop::dragEnterEvent(QDragEnterEvent *event)
{
    _dragEvent = true;

    _itemHeld = false;
	
    if (event->keyboardModifiers()==Qt::ControlModifier)
	{
	    event->ignore();
        return;

	}

    IconItem *icon = qobject_cast<IconItem*> (event->source());
    if (icon->parent() == this)
    {
        _inDrag = icon;
        icon->hide();

        if (_inDrag->type() == dirIcon)
        {
            for (int i = 0; i <_dirMinList.count(); i++)
            {
                if (_iconDict.value(_dirMinList.at(i)) == _inDrag)
                    continue;

                _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(false);
            }
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        qDebug() << "VirtualDesktop::dragEnterEvent(QDragEnterEvent *event)";

        event->acceptProposedAction();

        if (_isDirWidgetObject)
            return;

        if (!_isIconMove)
            closeDir(_dirPage, _dirIndex);

        _isDirWidgetObject = true;

        if (_dragItem != NULL)
            _dragItem = NULL;
        _dragItem = icon;

        addIcon(icon->text(), icon->pix()
                , _count - 1, _nextIdx[_count - 1]
                , icon->url(), icon->type());

        for (int i = 0; i <_dirMinList.count(); i++)
        {
            _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(false);
        }
    }
}

void VirtualDesktop::dragMoveEvent(QDragMoveEvent *event)
{
    if (!_isIconMove)
        return;
    if (_iconDragEnter)
        return;

    if (event->keyboardModifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}


    _dragEvent = true;

    event->setDropAction(Qt::MoveAction);
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

void VirtualDesktop::dragLeaveEvent(QDragLeaveEvent *event)
{
    _iconDragEnter = false;
    qDebug() << "*********************************dragLeaveEventdragLeaveEvent";
    _itemHeld = false;

    hideDirWidget();
//    _inDrag->show();
//    int p = _inDrag->page();
//    int s = _inDrag->index();
//    _inDrag->move(_gridTable[p][s].translated(HSPACING, VSPACING).topLeft());
    //drag->setPixmap(QPixmap(""));
    //icon->show();
    //event->accept();

    _dragEvent = false;


    Q_UNUSED(event);
}

void VirtualDesktop::dropEvent(QDropEvent *event)
{
    qDebug() << "111111111111111111111111111111111111111";

    if (event->keyboardModifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

	event->setDropAction(Qt::MoveAction);
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
    QPoint end = _gridTable[p][s].translated(HSPACING, VSPACING).topLeft();
    qDebug() << "End: " << end;
    _iconTable[p][s] = _inDrag;
    _inDrag->move(end);
    qDebug() << "Pos: " << _inDrag->pos();
//    _inDrag->show();
//    qDebug() << "444444444444444444444444444444444444444";
//    _inDrag=NULL;
//    qDebug() << "555555555555555555555555555555555555555";

    _inDrag->show();
    qDebug() << "444444444444444444444444444444444444444";
    _inDrag=NULL;
    qDebug() << "555555555555555555555555555555555555555";

    if (_dragItem != NULL)
    {

        _dirList.at(_dragItem->dirId())->removeIcon(_dragItem->text());

        _iconDict.value(_dirMinList.at(_dragItem->dirId()))->removeDirMinItem(_dragItem->text());

        _dragItem = NULL;
    }

    for (int i = 0; i <_dirMinList.count(); i++)
    {
        _iconDict.value(_dirMinList.at(i))->setMinWidgetDragEnable(true);
    }

    _dragEvent = false;
    _isDirWidgetObject = false;
}

void VirtualDesktop::mouseReleaseEvent(QMouseEvent *event)
{
    _itemHeld = false;

    if (event->modifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

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
    if (event->modifiers() == Qt::ControlModifier)
	{
	    event->ignore();
        return;
	}

    if (_animation->state() == QAbstractAnimation::Running)
        _animation->stop();

//    VirtualDesktop *mouseM = static_cast<VirtualDesktop*>(this);
//    if (mouseM == this) {
        event->accept();
        int right_bound = -_pageSize.width() * (_count - 1) - 100;

        if (x() < right_bound)
            return;
        int to = x() + event->x() - m2v;
        move(to, y());
        event->accept();
//    }
}

void VirtualDesktop::keyPressEvent(QKeyEvent *event)
{
	qDebug() << "\nkey event in board:" << event->key();
    if (event->modifiers()==Qt::ControlModifier)
	{
	  //做选中效果操作
	  //如果是ctrl+点击控件，实现IconItem::enterEvent相同的动作
        qDebug() <<  "VirtualDesktop::keyPressEvent to do\r\n";
        QWidget::enterEvent(event);
	}
    update();
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
                            const QString &url,\
                            int type)
{
    _localIconNum ++;
//    qDebug() << "_local->count() - 1 : " << _local->count() - 1;
//    qDebug() << "_iconsPerPage : " << _iconsPerPage;
    int expandPageCount = (_localIconNum - 1) / _iconsPerPage + 1;    //_local->count()
    if (expandPageCount > _count)
        expand();
//test

//    _iconNum++;

//    int expandPageCount = _iconNum / _iconsPerPage + 1;
//    if (expandPageCount > _count)
//        expand();


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
    icon->setType(type);

    if(type == dirIcon)
    {
//        qDebug() << "icon->addMinWidget()icon->addMinWidget()icon->addMinWidget()";
        icon->addMinWidget();
//        icon->setContextMenuBool(true);
    }
//    else
//    {
//            icon->setContextMenuBool(true);
//    }
    if (type != dirIcon)
        icon->setRunAction(true);

    icon->setDelAction(true);
    icon->setClosePixBool(false);
    icon->setSelectPixBool(false);
    icon->setDoubleClickBool(true);
    icon->setContextMenuBool(true);
    icon->setTrembleBool(false);
    icon->setDragEventBool(true);
    icon->setEnterEventBool(true);
    icon->setLeaveEventBool(true);
    icon->setEqualIcon(false);
//    if (dirIcon != type)
        icon->setSaveDataBool(true);
//    else
//        icon->setSaveDataBool(false);

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
            if ((page > 0) && (_nextIdx[page - 1] == _iconsPerPage) && (_nextIdx[page] < _iconsPerPage)) {
                index = _nextIdx[page];
            }
            else if ((page == 0) && (_nextIdx[page] < _iconsPerPage)) {
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
//            if (_nextIdx[page] < _iconsPerPage)
//            {
//                index = _nextIdx[page];
//            }
//            else {
//                for (int i = 0; i < _count; i++) {
//                    if (_nextIdx[i] < _row * _col) {
//                        page = i;
//                        index = _nextIdx[i];
//                        move(_pages[page], y());
//                        break;
//                    }
//                }
//            }
       }
    }

    icon->setPixmap(iconPath);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
//    qDebug() << "VirtualDesktop::addIcon() --> setPage(page)" << page;
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(url);
    _iconDict.insert(text, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    if (!_isDirWidgetObject)
    {
        icon->show();
    }
    else
    {
        _inDrag = icon;
        icon->hide();
    }
    icon->setDirId(-1);

    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));
    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(delIcon(const QString&)));
//    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
   //dirIcon
    connect(icon, SIGNAL(iconEnter()), this, SLOT(iconDragEnter()));
    connect(icon, SIGNAL(iconMove()), this, SLOT(iconDragMove()));
    connect(icon, SIGNAL(iconLeave()), this, SLOT(iconDragLeave()));
    connect(icon, SIGNAL(iconDrop(int, const QString &, const QString &, int, int, const QString &, int)),
            this, SLOT(iconDragDrop(int, const QString &, const QString &, int, int, const QString &, int)));
    connect(icon, SIGNAL(openDir(int, int, int)), this, SLOT(openDir(int, int, int)));
    connect(icon, SIGNAL(dragEnterMinWidget()), this, SLOT(iconDragEnter()));
    connect(icon, SIGNAL(showContextMenu(QPoint, QPoint,const QString &))
            , this, SLOT(showIconContextMenu(QPoint, QPoint,const QString &)));

    if (dirIcon == type)
    {
        icon->setId(_dirMinList.count());
        _dirMinList.append(text);

        _dirShowWidget = new DirShowWidget(QSize(_desktopRect.width(), 290), this);

        //    _dirShowWidget->raise();
        _dirShowWidget->setVisible(false);
        _dirShowWidget->setMaxRow(_row);
        _dirShowWidget->setId(_dirList.count());
        _dirList.append(_dirShowWidget);

        qDebug() << "_dirList.count()" << _dirList.count();

        connect(_dirShowWidget, SIGNAL(dirWidgetDragLeave()), this, SLOT(hideDirWidget()));
        connect(_dirShowWidget, SIGNAL(dirWidgetDelIcon(int, const QString &)), this, SLOT(dirWidgetDelIcon(int, const QString &)));
    }
    else
    {
        icon->setId(-1);
    }

    /*addicon add the last*/
//    showAddIcon(page, -1);

//    _iconLists.append(icon);

     return page;
    //LocalAppList::getList()->save();
}

int VirtualDesktop::addIcon(const QString &text, const QString &icon, const QString &url, int type)
{
    return addIcon(text, icon, _current, -1, url, type);
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
//   g_addIcon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
//   g_addIcon->show();
    
    return page;  
}

void VirtualDesktop::delIcon(const QString &text)
{
    if (_iconDict.value(text) == NULL)
        return;

    IconItem *icon = _iconDict.take(text);
    //icon->setHidden(true);
    qDebug() << icon->page();
    int p = icon->page();
    int s = icon->index();
    //icon->setPage(-1);
    //icon->setIndex(-1);
    delete _iconTable[p][s];

    moveBackIcons(p, s);

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

//    if (_nextIdx[_count - 1] == 0)
//    {
//        _iconTable[_count - 1 - 1][_nextIdx[_count - 1 - 1] - 1] = NULL;
//        _nextIdx[_count - 1 - 1]--;
//    }else
//    {
    //        _iconTable[_count - 1][_nextIdx[_count - 1] - 1] = NULL;
    //        _nextIdx[_count - 1]--;
    //    }

//    if ((_count > 1) && (_local->count() < _iconsPerPage * (_count - 1)))
//        delPage(_count - 1);

//    /* move addicon add the last*/
//    if (_nextIdx[_count - 1] == _iconsPerPage - 1)
//        showAddIcon(_count - 1, _nextIdx[_count - 1]);
//    else
//        showAddIcon(_count - 1, -1);

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

void VirtualDesktop::moveBackIcons(int page, int index)
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

    _localIconNum--;

}

void VirtualDesktop::runApp(const QString &text)
{
    QString tmp;
    QString pram;

//    if (!_iconDict.value(text))
//        return;

    if (!_iconDict.value(text)->url().isEmpty())
    {

        QDesktopServices::openUrl(QUrl(_iconDict.value(text)->url()));
        return;
    }

    bool isRmote = false;
    for (int i = 0; i < g_myVappList.count(); i++)
    {
        qDebug() << "g_myVappList.at(i).name" << g_myVappList.at(i).name;
        qDebug() << "text" << text;
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
    if (la->type().toInt() == localIcon) {
        QFileInfo link(la->execname());

        qDebug() << "la->execname():" << la->execname();
        if (!link.exists()) {
            AppMessageBox box(true, NULL);
            box.setText("指向的应用或快捷方式已不存在\n点击\"确定\"删除图标");
            if (box.exec())
                LocalAppList::getList()->delApp(la->name());
            return;
        }
        sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());
    } else {
        QFileInfo link(la->execname());
        //   QString exec = AppDataReadExe::Instance()->getExec(la->execname());
        if (!link.exists()) {
//            QString msg = QString("%1没有被正确安装, 请在\n软件商店中重新安装, 是否删除图标?").arg(text);
//            AppMessageBox box(true, NULL);
//            box.setText(msg);
//            if (box.exec())
//                LocalAppList::getList()->delApp(la->name());

            QMessageBox::information(NULL,"NO","Linke to Function is not OK!!!!");
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
    _mousePos = event->pos();
    _normalMenu->move(_mousePos);

    hideMenuWidget();
    _normalMenu->raise();
    _normalMenu->setVisible(true);


    //    QCursor cur = this->cursor();
//    _menu = new QMenu(this);

////    _menu->addAction(_addAction);
//    if (_local->count() == 0) {

//    } else {
//        if (!_trembling)
//        {
//            for (int i = 0; i < _local->count(); i++)
//            {
//                if (!_local->at(i)->isRemote())
//                {
//                    _menu->addAction(_deleteAction);
//                    break;
//                }
//            }
//        }
//        else {
//            for (int i = 0; i < _local->count(); i++)
//            {
//                if (!_local->at(i)->isRemote())
//                {
//                    _menu->addAction(_cancelAction);
//                    break;
//                }
//            }
//        }
//    }
//    _menu->exec(cur.pos());

}

void VirtualDesktop::appAdd()
{

//    QSettings reg(QSettings::NativeFormat,
//        QSettings::SystemScope, "Microsoft", KEY);
//    for (int i = 0; i < reg.childGroups().count(); i++) {
//        QSettings tmp(QSettings::NativeFormat,
//            QSettings::SystemScope, "Microsoft",
//            KEY + reg.childGroups().at(i));
////        if (tmp.value("DisplayName").toString() == display) {
////            return tmp.value("UninstallString").toString();
////        }
//        qDebug() << "************" << tmp.value("Path").toString();
//    }
    _addAppState = true;
    QString path = QFileDialog::getOpenFileName(this, tr("选择一个应用程序或快捷方式"),
                                                Config::get("CommonProgramDir"),
                                                tr("app (*.lnk *.exe)"));

    if (path.isEmpty())
    {
        _addAppState = false;
        return;
    }

    path.replace('/','\\');
    //AppDataReadExe::Instance()->addLocalApp(path);
    LocalAppList::getList()->addLocalApp(path);
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

    int expandPageCount =  _localIconNum / _iconsPerPage + 1;  //_local->count()
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
                        + (j % _col) * gridWidth + ICONHSPACING;
                int y = (j / _col) * gridHeight + ICONVSPACING;

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

    qDebug()<<"VirtualDesktop reload all.";
}

//void VirtualDesktop::deleteAllIconItem()
//{
//    _local->save();

//    for (int i=0;i<_count;i++)
//    {
//        for(int j=0;j<_iconsPerPage;j++) // also you can use _nextIdx[i]
//        {
//            if(_iconTable[i][j]!=NULL)
//            {
//                delete _iconTable[i][j];
//                _iconTable[i][j]=NULL;
//            }
//        }
//        _nextIdx[i]=0;
//    }
//}

void VirtualDesktop::addDirItem()
{
    hideMenuWidget();

    addDesktopApp(QString("_%1").arg(_dirMinList.count()), "", "", 3);

}

void VirtualDesktop::openDir(int id, int page, int index)
{
    hideMenuWidget();

    if (!_dirMovingFinished)
        return;

    _dirMovingFinished = false;

    qDebug() << "void VirtualDesktop::openDir(int id, int page, int index)void VirtualDesktop::openDir(int id, int page, int index)";

//    setContextMenuPolicy(Qt::NoContextMenu);

    _openDir = true;
    _isIconMove = false;

    _dirPage = page;
    _dirIndex = index;

    qDebug() << "VirtualDesktop::openDir() ------>_dirPage" << _dirPage;
    qDebug() << "VirtualDesktop::openDir() ------>_dirIndex" << _dirIndex;

//    setIconEnabled(false);

    if (_animationScreenDown)
    {
        closeDir(page, index);
        return;
    }

    _dirId = id;
    _distance = _dirList.at(_dirId)->getHeight();

    int x = page * _desktopRect.width();
    int y = _gridTable[page][index].y() + gridHeight;
    int w = _desktopRect.width();
    int upH = _gridTable[page][index].y() + gridHeight;
//    int downH = _desktopRect.height() - upH;
    int downH = _desktopRect.height() - _gridTable[page][index].y() - gridHeight - 30;
//    int h = _dirShowWidget->getHeight();

//    _dirList.at(_dirId)->move(x, y);
//    _dirList.at(_dirId)->setVisible(true);
//    _dirList.at(_dirId)->raise();

    int mx = _gridTable[page][index].x() + (gridWidth - SMALLSLIDERWIDTH) / 2 + x;  // 39
    int my = y - 15;                            //_gridTable[page][index].y() + gridHeight + 21;
    int mw = 38;                                //gridWidth - 40;
    int mh = 15;                                //40;


//    int _upDistance;
    int row = (_gridTable[page][index].y() + gridHeight) / gridHeight;

    if (row + _dirList.at(_dirId)->getRow() > _row)
    {
        _upDistance = (row + _dirList.at(_dirId)->getRow() - _row) * gridHeight;
    }
    else
    {
        _upDistance = 0;
    }

    _dirList.at(_dirId)->move(x, y - _upDistance);

    emit upMove(0, 0, x + w, upH, _upDistance);
    emit desktopOpenMove(x, y, w, downH, _distance);
    emit openMinWidget(mx, my, mw, mh, _distance);
    emit upMinMove(mx, my, mw, mh, _upDistance);
//    if (_upDistance > 0)
//        emit desktopBgMove(gridHeight);
    qDebug() << "before" ;
    _dirList.at(_dirId)->raise();
    _dirList.at(_dirId)->setVisible(true);

    qDebug() << "after";

    _animationScreenDown = true;
}

void VirtualDesktop::closeDir(int page, int index)
{
    if (!_dirMovingFinished)
        return;

    _dirMovingFinished = false;

        qDebug() << "void VirtualDesktop::closeDir(int page, int index)void VirtualDesktop::closeDir(int page, int index)";
    _distance = _dirList.at(_dirId)->getHeight();

    int x = page * _desktopRect.width();
    int y = _gridTable[page][index].y() + gridHeight + _distance;
    int w = _desktopRect.width();
    int upH = _gridTable[page][index].y() + gridHeight;
//    int downH = _desktopRect.height() - upH;
    int downH = _desktopRect.height() - _gridTable[page][index].y() - gridHeight - 30;

    int mx = _gridTable[page][index].x() + (gridWidth - SMALLSLIDERWIDTH) / 2 + x; // 39
    int my = y - 15;              //_gridTable[page][index].y() + gridHeight + 249
    int mw = 38;                       //gridWidth - 40
    int mh = 15;                       //40

    emit closeMinWidget(mx, my, mw, mh, _distance);
    emit desktopCloseMove(x, y, w, downH, _distance);
    emit upBackMove(0, -_upDistance, x + w, upH, _upDistance);
    emit upMinBackMove(mx, my - _distance - _upDistance, mw, mh, _upDistance);
//    if (_upDistance > 0)
//        emit desktopBgBack(gridHeight);
    _openDir = false;
    _animationScreenDown = false;

//    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void VirtualDesktop::setDirHide()
{
    _dirList.at(_dirId)->setVisible(false);
}

void VirtualDesktop::dirMovingFinished()
{
    _dirMovingFinished = true;
}

void VirtualDesktop::addDesktopApp(const QString &text, const QString &pix, const QString &url, int type)
{
    _url = url;
    qDebug() << _url;

    if (localIcon == type)
    {
            LocalApp *app = new LocalApp();
            app->setName(text);
            app->setIcon(pix);
            app->setType(QString("%1").arg(type));
            app->setUrl(url);
            app->setPage(_count - 1);
            app->setIndex(_nextIdx[_count - 1]);
            app->setDirId(-1);
            app->setId("111");

            for (int i = 0; i < g_RemotelocalList.count(); i++)
            {
                if(g_RemotelocalList.at(i).name == text)
                {
                    qDebug() << "text" << text;
                    app->setExecname(g_RemotelocalList.at(i).execname);
                    qDebug() << "***************execname*******************" << g_RemotelocalList.at(i).execname;
                    break;
                }
            }

            if (LocalAppList::getList()->getAppByName(app->name())) {
                QApplication::processEvents();
                AppMessageBox box(false, NULL);
                box.setText("    已添加该图标");
                box.exec();
            } else {
                LocalAppList::getList()->addApp(app);
            }

    }
    else if (vacIcon == type)
    {
            LocalApp *RemoteApp = new LocalApp();

            for (int i = 0; i < g_myVappList.count(); i++)
            {
                if(g_myVappList.at(i).name == text)
                {
                    RemoteApp->setName(g_myVappList[i].name);
                    RemoteApp->setId(g_myVappList[i].id);
                    RemoteApp->setIcon(pix);
                    RemoteApp->setPage(_count - 1);
                    RemoteApp->setIndex(_nextIdx[_count - 1]);
                    RemoteApp->setType(QString("%1").arg(type));
                    RemoteApp->setIsRemote(true);
                    RemoteApp->setUrl(url);
                    RemoteApp->setDirId(-1);
                    RemoteApp->setId("111");
                    break;
                }
            }

            LocalAppList::getList()->addRemoteApp(RemoteApp);
            addIcon(RemoteApp->name(), RemoteApp->icon()
                    , RemoteApp->page(), RemoteApp->index()
                    , RemoteApp->url(), RemoteApp->type().toInt());

    }
    else if (paasIcon == type)
    {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(_count - 1);
        app->setIndex(_nextIdx[_count - 1]);
        app->setExecname(url);
        app->setDirId(-1);
        app->setId("111");


        if (LocalAppList::getList()->getAppByName(app->name())) {
            QApplication::processEvents();
            AppMessageBox box(false, NULL);
            box.setText("    已添加该图标");
            box.exec();
        } else {
            LocalAppList::getList()->addApp(app);
        }
    }
    else if (dirIcon == type)
    {
        LocalApp *app = new LocalApp();
        app->setName(text);
        app->setIcon(pix);
        app->setType(QString("%1").arg(type));
        app->setUrl(url);
        app->setPage(_count - 1);
        app->setIndex(_nextIdx[_count - 1]);
        app->setExecname("");
        app->setDirId(-1);
        app->setId(QString("%1").arg(_dirMinList.count()));

        LocalAppList::getList()->addApp(app);

    }


//    addIcon(text, pix, - 1, -1, localIcon);

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

//    AppDataReadExe::Instance()->addLocalApp(path);

}

void VirtualDesktop::hideDirWidget()
{
    if (_openDir)
    {
        closeDir(_dirPage, _dirIndex);
        return;
    }
}

void VirtualDesktop::setIconEnabled(bool enabled)
{
//    for (int i = 0; i < _count; i++)
//    {
//        for(int j = 0; j < _nextIdx[i]; j++)
//        {
//            if ((i == _dirPage) && (j == _dirIndex))
//                continue;

//            _iconTable[i][j]->setEnabled(enabled);
//            if(enabled)
//                _iconTable[i][j]->setNormalPixmap();
//            else
//                _iconTable[i][j]->setGrayPixmap();
//        }
//    }
}

void VirtualDesktop::menuChanged(int value)
{
    _menuValue = value;

    switch(_menuValue)
    {
    case 0 :
        if (_createMenu->isVisible())
            _createMenu->setVisible(false);

        _showIconMenu->move(_mousePos.x() + 162 - 25, _mousePos.y());
        _showIconMenu->raise();
        _showIconMenu->setVisible(true);

        break;

    default:
        if (_createMenu->isVisible())
            _createMenu->setVisible(false);

        if (_showIconMenu->isVisible())
            _showIconMenu->setVisible(false);
        break;
    }

    //    case 2 :
    //        if (_showIconMenu->isVisible())
    //            _showIconMenu->setVisible(false);

    //        _createMenu->move(_mousePos.x() + 162 - 25, _mousePos.y() + 20 + 19 + 19 - 10);
    //        _createMenu->raise();
    //        _createMenu->setVisible(true);

    //        break;
}

void VirtualDesktop::hideMenuWidget()
{
    if (_normalMenu->isVisible())
        _normalMenu->setVisible(false);

    if (_createMenu->isVisible())
        _createMenu->setVisible(false);

    if (_showIconMenu->isVisible())
        _showIconMenu->setVisible(false);

    if (_iconMenu->isVisible())
        _iconMenu->setVisible(false);
}

void VirtualDesktop::setLargeIcon()
{
    hideMenuWidget();

    LocalAppList::getList()->save();

    _iconSize = IconItem::large_size;
    ICON_TYPE = _iconSize;

    for (int i = 0; i < _dirList.count(); i++)
    {
        _dirList.at(i)->largeIcon();
    }


    refresh(LARGESIZE);

    emit largeIcon();

//    for (int i = 0; i < _local->count(); i++)
//    {
//        addIcon(_local->at(i)->name(), _local->at(i)->icon()
//                , _local->at(i)->page(),_local->at(i)->index()
//                , _local->at(i)->url(), _local->at(i)->type().toInt());
//    }
    initIconItem();
}

void VirtualDesktop::setMediumIcon()
{
    hideMenuWidget();

    LocalAppList::getList()->save();

    _iconSize = IconItem::medium_size;
    ICON_TYPE = _iconSize;
    for (int i = 0; i < _dirList.count(); i++)
    {
        _dirList.at(i)->mediumIcon();
    }

    refresh(MEDIUMSIZE);

    emit mediumIcon();

//    for (int i = 0; i < _local->count(); i++)
//    {
//        addIcon(_local->at(i)->name(), _local->at(i)->icon()
//                , _local->at(i)->page(),_local->at(i)->index()
//                , _local->at(i)->url(), _local->at(i)->type().toInt());
//    }

    initIconItem();
}

void VirtualDesktop::setSmallIcon()
{
    hideMenuWidget();

    LocalAppList::getList()->save();

    _iconSize = IconItem::small_size;
    ICON_TYPE = _iconSize;

    for (int i = 0; i < _dirList.count(); i++)
    {
        _dirList.at(i)->smallIcon();
    }

    refresh(SMALLSIZE);

    emit smallIcon();

//    for (int i = 0; i < _local->count(); i++)
//    {
//        addIcon(_local->at(i)->name(), _local->at(i)->icon()
//                , _local->at(i)->page(),_local->at(i)->index()
//                , _local->at(i)->url(), _local->at(i)->type().toInt());
//    }

    initIconItem();
}

void VirtualDesktop::refresh(QSize size)
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
    _dirList.clear();
    _dirMinList.clear();

    reloadApplist(size);

//    getLocalIcon();
//    getVacIcon();
//    getPaasIcon();
}

void VirtualDesktop::movetoFirst()
{
    _current = 0;
    move(_pages[_current], y());
//    emit pageChanged(_current);

}

void VirtualDesktop::reloadApplist(QSize size)
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

    _localIconNum= 0;

//    for(int i = 0; i < _iconLists.count(); i++)
//    {
//        _tempLists[i] = _iconLists[i];
////        _iconLists[i] = NULL;
////        delete _iconLists[i];
//    }

//    _iconLists.clear();

//    QString name;

//    for(int i = 0; i < _tempLists.count(); i++)
//    {
//        qDebug() << _tempLists.count();
//        qDebug() << _tempLists[i]->text();
//        qDebug() << _tempLists[i]->url();
        //_url = _iconLists.at(i)->url();

//        if (_iconLists[i]->type() == dirIcon)
//        {
//            name = _iconLists[i]->getDirText();
//        }
//        else
//            name = _iconLists[i]->text();

//        addIcon(name, _iconLists[i]->pix()
//                , _iconLists[i]->page(), _iconLists[i]->index()
//                , _iconLists[i]->type());

//    }

    qDebug()<<"reload all.";
}

void VirtualDesktop::deleteAllIconItem()
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

void VirtualDesktop::changeSpacing()
{
    switch(_iconSize)
    {
        case IconItem::large_size :

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = LARGESIZE.width() + HSPACING * 2;
            gridHeight = LARGESIZE.height() + VSPACING + BOTTOMSPACING;

            break;

        case IconItem::medium_size :

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = MEDIUMSIZE.width() + HSPACING * 2;
            gridHeight = MEDIUMSIZE.height() + BOTTOMSPACING;
            break;

        default:

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = SMALLSIZE.width() + HSPACING * 2;
            gridHeight = SMALLSIZE.height() + BOTTOMSPACING;
            break;
    }
}

void VirtualDesktop::showIconContextMenu(QPoint pos, QPoint mPos, const QString &text)
{
    Q_UNUSED(pos);
    _currentIconItem = text;

    if ( _iconDict.value(_currentIconItem)->type() == dirIcon)
    {
        if (_iconDict.value(_currentIconItem)->getMinIconNum() != 0)
        {
            _iconMenu->_delBtn->setEnabled(false);
        }
        else
            _iconMenu->_delBtn->setEnabled(true);
    }
    else
    {
        _iconMenu->_delBtn->setEnabled(true);
    }

    _iconMenu->move(mPos);
    _iconMenu->raise();
    _iconMenu->setVisible(true);
}

void VirtualDesktop::iconMenuRunClicked()
{
    _iconMenu->setVisible(false);

    if (_iconDict.value(_currentIconItem)->type() == dirIcon)
    {
//        IconItem *iconItem = _iconDict.value(_currentIconItem);
        openDir(_iconDict.value(_currentIconItem)->id(), _iconDict.value(_currentIconItem)->page(), _iconDict.value(_currentIconItem)->index());
//        iconItem = NULL;
    }
    else
    {
        runApp(_currentIconItem);
    }
}

void VirtualDesktop::iconMenuDelClicked()
{
    _iconMenu->setVisible(false);
    qDebug() << _currentIconItem;
//    delIcon(_currentIconItem);
    if ( _iconDict.value(_currentIconItem)->type() == dirIcon)
    {
        int index = _iconDict.value(_currentIconItem)->id();
        QList<DirShowWidget*>::iterator iter = _dirList.begin() + index;
        _dirList.at(index)->setParent(NULL);
        _dirList.at(index)->deleteLater();
        _dirList.erase(iter);

        _dirMinList.removeAt(index);

        for (int i = index + 1; i <= _dirList.last()->id(); i++)
        {
            for (int j = 0; j < _local->count(); j++) {
                if (_local->at(j)->hidden())
                    continue;
                if (_local->at(j)->dirId() == i)
                {
                    _local->at(j)->setDirId(i - 1);
                }
            }
        }
    }

    LocalAppList::getList()->delApp(_currentIconItem);

    emit desktopDelIcon(_currentIconItem);
}

void VirtualDesktop::refreshMenu()
{
    switch(ICON_TYPE)
    {
        case IconItem::large_size :
            setLargeIcon();
            break;

        case IconItem::medium_size :
            setMediumIcon();
            break;

        default:
            setSmallIcon();
            break;
    }
}

void VirtualDesktop::dirWidgetDelIcon(int id, const QString &text)
{
    qDebug() <<"VirtualDesktop::dirWidgetDelIcon"<<id;
    _iconDict.value(_dirMinList.at(id))->removeDirMinItem(text);

    emit desktopDelIcon(text);
}

void VirtualDesktop::moveWidgetDrop(IconItem *iconItem)
{
    if (!iconItem)
        return;

    addIcon(iconItem->text(), iconItem->pix()
            , _count - 1, _nextIdx[_count - 1]
            , iconItem->url(), iconItem->type());


    _dirList.at(iconItem->dirId())->removeIcon(iconItem->text());

    _iconDict.value(_dirMinList.at(iconItem->dirId()))->removeDirMinItem(iconItem->text());

}

void VirtualDesktop::initIconItem()
{
    for (int i = 0; i < _local->count(); i++) { //_localIconNum
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == -1)
        {

            if(_local->at(i)->type().toInt() == localIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 0);
            }
            else if (_local->at(i)->type().toInt() == vacIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 1);
            }
            else if (_local->at(i)->type().toInt() == paasIcon)
            {
                addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                        _local->at(i)->page(), _local->at(i)->index(),
                        _local->at(i)->url(), 2);
            }
            else if (_local->at(i)->type().toInt() == dirIcon)
            {
                addIcon(_local->at(i)->name(), "",
                        _local->at(i)->page(), _local->at(i)->index(),
                        "", 3);
            }
        }
    }

    for (int i = 0; i < _dirList.count(); i++)
    {
        for (int j = 0; j < _local->count(); j++) {
            if (_local->at(j)->hidden())
                continue;
            if (_local->at(j)->dirId() == i)
            {
                _iconDict.value(_dirMinList.at(_local->at(j)->dirId()))->addDirMinItem(_local->at(j)->name(), _local->at(j)->icon(),
                                                                      _local->at(j)->page(), _local->at(j)->index(),
                                                                      _local->at(j)->url());

                _dirList.at(i)->addDirIcon(_local->at(j)->name(), _local->at(j)->icon(),
                                           _local->at(j)->page(), _local->at(j)->index(),
                                           _local->at(j)->url(), _local->at(j)->type().toInt());
            }
        }
    }
}
