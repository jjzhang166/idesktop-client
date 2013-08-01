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
#include <QMessageBox>

#include <QtDebug>
#include <QDrag>
#include <QDesktopServices>

#include "DirWidget.h"
#include "config.h"
#include "dirwidget.h"
#include "appmessagebox.h"
#include "paascommuinication.h"
#include "strings.h"
#include <string.h>

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
extern QString WIN_VAPP_IconPath;
extern QList<APP_LIST> g_myVappList;
extern QList<LOCAL_LIST> g_RemotelocalList;
extern QList<APP_LIST> g_RemoteappList;
extern QList<PAAS_LIST> g_RemotepaasList;

static QPoint gap;

//******/////
//
DirShowWidget::DirShowWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _id(0)
{
//    setContextMenuPolicy(Qt::NoContextMenu);
//    _rightTopPix.load(":/images/bs_rightbg_top.png");
//    _rightCenterPix.load(":/images/bs_rightbg_center.png");
//    _rightBottomPix.load(":/images/bs_rightbg_bottom.png");

    QTextCodec *codec = QTextCodec::codecForName("System"); //System
    QTextCodec::setCodecForCStrings(codec);

    _width = pageSize.width();

    _dirWidget = new DirWidget(pageSize, this);
    _dirWidget->setGeometry(0, 30, _dirWidget->width() - 5, _dirWidget->height());
    _dirWidget->setVisible(true);

    _height = _dirWidget->height() + 30;

    resize(_width, _height);

    _titleLabel = new QLabel(this);
    _titleLabel->move(40,10);
    _titleLabel->setVisible(true);

    _clearBtn = new Switcher(this);
    _clearBtn->setPixmap(QString(":images/dushbin_clear_normal.png"));
    _clearBtn->setGeometry(_width - _clearBtn->width() - 15, 15
                           , _clearBtn->width(), _clearBtn->height());
    _clearBtn->setVisible(false);

    connect(_clearBtn, SIGNAL(switcherActivated()), this, SLOT(clearAllIcon()));



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
    connect(_dirWidget, SIGNAL(dirWidgetDragLeave(const QString &)), this, SLOT(dirWidgetLeave(const QString &)));
    connect(_dirWidget, SIGNAL(dirWidgetDelIcon(int, const QString &)), this, SIGNAL(dirWidgetDelIcon(int, const QString &)));
}

DirShowWidget::~DirShowWidget()
{

}

void DirShowWidget::dirWidgetLeave(const QString &uniqueName)
{
    emit dirWidgetDragLeave(uniqueName, _id);
}

void DirShowWidget::showTitle(QString text)
{
    QFont font(QString::fromLocal8Bit("Microsoft YaHei"), 14, QFont::Normal);
    QFontMetrics fm(font);
    int textflags = Qt::AlignLeft | Qt::TextExpandTabs;
    QSize textsize = fm.size(textflags, text);
    _titleLabel->setStyleSheet("color:white");
    _titleLabel->setFont(font);
    _titleLabel->resize(textsize);
    _titleLabel->setText(QString(tr("%1")).arg(text));
     QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(_titleLabel);
     effect->setBlurRadius(8);
     effect->setColor(QColor(0, 0, 0));
     effect->setOffset(-1,1);
     _titleLabel->setGraphicsEffect(effect);

}

void DirShowWidget::showClearBtn()
{
    _clearBtn->setVisible(true);
}

void DirShowWidget::removeIcon(const QString &uniqueName)
{
    _dirWidget->removeIcon(uniqueName);
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

void DirShowWidget::mousePressEvent(QMouseEvent *event)
{
    if (Qt::RightButton == event->button())
        return;
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
    qDebug() << "DirShowWidget::setSize() : ";
//    _dirWidget->setMaxPage(_maxRow);

    _dirWidget->setGeometry(0, 30, _dirWidget->width() - 5, _dirWidget->getHeight());
    _height = _dirWidget->width() + 30;

    setFixedSize(_width, _height);
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

void DirShowWidget::addDirIcon(const QString &text, const QString &iconPath,
                               int page, int index, const QString &url,
                               int type, const QString &uniqueName)
{
//    _dirWidget->setUrl(url);
    qDebug() << "void DirShowWidget::addDirIcon(" << text;
    _dirWidget->addIcon(text, iconPath, page, index, url, type, uniqueName);
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

void DirShowWidget::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;
    _dirWidget->setUniqueName(uniqueName);
}

void DirShowWidget::clearAllIcon()
{
    _dirWidget->clearAllIcon();
}

//
DirWidget::DirWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _localCount(0)
    , _iconNum(0)
    , _url("")
    , _iconSize(ICON_TYPE)
    , _dragEvent(false)
{
//    setContextMenuPolicy(Qt::NoContextMenu);
    _count = 1;
    _pageSize = pageSize;
    _width = _pageSize.width();

    changeSpacing();
//    gridWidth = ICONITEMWIDTH + HSPACING * 2;
//    gridHeight = ICONITEMWIDTH + VSPACING + BOTTOMSPACING;

    _col = (_width - ICONHSPACING) / gridWidth;
    _row = (_height - ICONVSPACING) / gridHeight;

    _iconsPerPage = _col * _row;
    _current  = 0;

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

    _local = LocalAppList::getList();
    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));
    connect(&_communi, SIGNAL(appRun()), this, SLOT(runServerApp()));//

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

//    for (int i = 0; i < ICONNUM; i++) {
//        addIcon("", QString(":images/custom/z_%1.png").arg(i + 15),
//                -1, i);
//    }
//    initIconItem();
    _iconMenu = new MenuWidget(MenuWidget::iconMenu, this);
    _iconMenu->setVisible(false);

    connect(_iconMenu, SIGNAL(run()), this, SLOT(iconMenuRunClicked()));
    connect(_iconMenu, SIGNAL(del()), this, SLOT(iconMenuDelClicked()));
}

DirWidget::~DirWidget()
{

}

int DirWidget::addIcon(const QString &text,
                       const QString &iconPath,
                       int page,
                       int index,
                       const QString &url,
                       int type,
                       const QString &uniqueName)
{
        qDebug() <<  "DirWidget::addIcon() -->_iconNum" << _iconNum;
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

    icon->setSaveDataBool(true);    //  true
    icon->setUniqueName(uniqueName);
    icon->setText(text);
    icon->setIconClass(type);
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
            else
            {
                for (int i = 0; i < _count; i++) {
                    if (_nextIdx[i] < _row * _col)
                    {
                        page = i;
                        index = _nextIdx[i];
                        move(_pages[page], y());
                        break;
                    }
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
    icon->setPixmap(iconPath,text);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(url);
    icon->setDirId(_id);
    icon->setId(111);
    icon->setType(type);
    _iconDict.insert(uniqueName, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

    _iconNum ++;
//    connect(icon, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
//   connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(uninstall(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SLOT(delIcon(const QString&)));
    connect(icon, SIGNAL(showContextMenu(bool, QPoint, QPoint,const QString &))
            , this, SLOT(showIconContextMenu(bool, QPoint, QPoint,const QString &)));

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
    setFixedSize(_pageSize.width(),  _count * _pageSize.height());
    move(_pages[_current], y());
    emit pageDecreased();
}

void DirWidget::delIcon(const QString &_uniqueName)
{
    if (!_iconDict.value(_uniqueName))
        return;

    qDebug() << _uniqueName;
    IconItem *icon = _iconDict.take(_uniqueName);
    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();
    //icon->setPage(-1);
    //icon->setIndex(-1);
    delete _iconTable[p][s];

    moveBackIcons(p, s);
}


void DirWidget::removeIcon(const QString &_uniqueName)
{
    int p = _iconDict.value(_uniqueName)->page();
    int s = _iconDict.value(_uniqueName)->index();
    _iconTable[p][s] = NULL;
    moveBackIcons(p, s);

    _iconDict.take(_uniqueName);
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

    qDebug() << "_nextIdx[_count - 1] : " << _nextIdx[_count - 1];
    if (_nextIdx[_count - 1] == 0)
    {
        if (_count - 1 != 0)
        {
            delPage(_count - 1);
        }
    }

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

    _iconNum = 0;

//    initIconItem();


}

//void DirWidget::initIconItem()
//{
//    for (int j = 0; j < _local->count(); j++) {
//        if (_local->at(j)->hidden())
//            continue;
//        if (_local->at(j)->dirId() == _id)
//        {
//            addIcon(_local->at(j)->name(), _local->at(j)->icon(),
//                    _local->at(j)->page(), _local->at(j)->index(),
//                    _local->at(j)->url(), _local->at(j)->type().toInt());
//        }
//    }
//}

void DirWidget::mediumIcon()
{
    _iconSize = IconItem::medium_size;
    refresh(MEDIUMSIZE);
    _iconNum = 0;
}

void DirWidget::smallIcon()
{
    _iconSize = IconItem::small_size;
    refresh(SMALLSIZE);
    _iconNum = 0;
}

void DirWidget::refresh(QSize size)
{
    movetoFirst();
    deleteAllIconItem();

    for(int i = _count; i > 1; i--)
    {
        delPage(i-1);
    }

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

    qDebug()<<" DirWidget reload all.";
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
void DirWidget::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    return;
}
void DirWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    _dragEvent = false;
    qDebug() << "DirWidget::dragLeaveEvent(QDragLeaveEvent *event)";


    if (!_inDrag)
        return;

    int p = _inDrag->page();
    int s = _inDrag->index();

    emit dirWidgetDragLeave(_inDrag->uniqueName());

    _iconDict.take(_inDrag->uniqueName());
    _inDrag = NULL;

    _iconTable[p][s] = NULL;

    moveBackIcons(p, s);



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

void DirWidget::mousePressEvent(QMouseEvent *event)
{
    _iconMenu->setVisible(false);
    event->accept();

    if (Qt::RightButton == event->button())
        return;
}

void DirWidget::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << event->pos();
}

void DirWidget::mouseReleaseEvent(QMouseEvent *)
{

}

void DirWidget::showIconContextMenu(bool visiable,QPoint pos, QPoint mPos, const QString &uniqueName)
{
    qDebug() << "void DirWidget::showIconContextMenu(QPoint pos, const QString &text)" << pos << uniqueName;
    Q_UNUSED(mPos);
    _currentIconItem = uniqueName;

    _iconMenu->move(pos.x() + _iconDict.value(uniqueName)->pos().x(), pos.y() + _iconDict.value(uniqueName)->pos().y());
    _iconMenu->raise();
    _iconMenu->setVisible(visiable);
}

void DirWidget::iconMenuRunClicked()
{
    _iconMenu->setVisible(false);

    runApp(_currentIconItem);

}

void DirWidget::iconMenuDelClicked()
{
    _iconMenu->setVisible(false);
    qDebug() << _currentIconItem;
//    delIcon(_currentIconItem);
//    emit delApp(_currentIconItem);
    LocalAppList::getList()->delApp(_currentIconItem);
    emit dirWidgetDelIcon(_id, _currentIconItem);

}

void DirWidget::runApp(const QString &uniqueName)
{
    QString tmp;
    QString pram;

    if (uniqueName.startsWith("0_"))
    {
        LocalApp *la = _local->getAppByUniqueName(uniqueName);
        if (!la)
            return;

        char command[2048];

        QFileInfo link(la->execname());

        qDebug() << "la->execname():" << la->execname();
        if (!link.exists())
        {
            AppMessageBox box(true, NULL);
            box.setText("指向的应用或快捷方式已不存在\n点击\"确定\"删除图标");
            if (box.exec())
                LocalAppList::getList()->delApp(la->name());
            return;
        }
        sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());

        ShellExecute(NULL, "open", command, "", "", SW_SHOW);

        return;
        }
    else if(uniqueName.startsWith("1_"))
    {
        bool isRmote = false;
        for (int i = 0; i < g_myVappList.count(); i++)
        {
//                qDebug() << "g_myVappList.at(i).name" << g_myVappList.at(i).name;
//                qDebug() << "text" << text;
            if(g_myVappList.at(i).id == uniqueName.right(uniqueName.length() - 2))
            {
                _appid = g_myVappList.at(i).id;
                Commui._type = g_myVappList.at(i).type;
                isRmote = true;
                appText = g_myVappList.at(i).name;
            }
        }
        if(isRmote)
        {
            _communi.loadBalance(Commui._name,_appid);
            if(_communi._isNetError)
            {
                QMessageBox::warning(this, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
            }
            return;
        }

        return;
    }
    else if (uniqueName.startsWith("2_"))
    {
        QDesktopServices::openUrl(QUrl(_iconDict.value(uniqueName)->url()));
        return;
    }

}

void DirWidget::runServerApp()
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

void DirWidget::clearAllIcon()
{
    qDebug() << "clicked";

    if (_clearNames.count() != 0)
        _clearNames.clear();

    for (int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == 1000)
        {
            _clearNames.append(_local->at(i)->uniqueName());
        }
    }

    for (int i = 0; i < _clearNames.count(); i++)
    {
        LocalAppList::getList()->delApp(_clearNames.at(i));
        emit dirWidgetDelIcon(_id , _clearNames.at(i));
    }

    _clearNames.clear();
}

void DirWidget::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;
}
