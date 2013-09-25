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
#include <QtWebKit/QWebSettings>
#include <cassert>

//#include <QtScript/QScriptValue>
//#include <QtScript/QScriptEngine>
//#include <QtScript/QScriptValueIterator>

#include "idesktopsettings.h"
#include "vacwidget.h"
#include "config.h"
#include "commuinication.h"

extern QString WIN_LOCAL_IconPath;
extern QString WIN_VAPP_IconPath;
extern QString WIN_PAAS_IconPath;


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

    connect(_closeBtn, SIGNAL(clicked()), this, SLOT(closeBtnClicked()));

    connect(_vacScrollBarWidget, SIGNAL(addApp(const QString&,const QString&, const QString&, int, const QString&)),
            this, SIGNAL(addApp(const QString&,const QString&, const QString&, int, const QString&)));
    connect(_vacScrollBarWidget, SIGNAL(delItem(const QString &)), this, SIGNAL(delItem(const QString &)));
}

VacShowWidget::~VacShowWidget()
{

}

void VacShowWidget::showApp(bool localApp)
{
    _vacScrollBarWidget->showApp(localApp);
}

int VacShowWidget::addIcon(QString text, const QString &icon, int page, int index, const QString &url, int type, const QString &uniqueName)
{
    return _vacScrollBarWidget->addIcon(text, icon, page, index, url, type, uniqueName);
}

void VacShowWidget::delIcon(const QString &uniqueName)
{
    _vacScrollBarWidget->delIcon(uniqueName);
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

void VacShowWidget::desktopDelIcon(const QString &uniqueName)
{
    _vacScrollBarWidget->desktopDelIcon(uniqueName);
}

void VacShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, _width, _height, _bgPix.scaled(_width, _height));
    painter.drawPixmap(18, 25, QPixmap(":images/software_normal.png"));

    QWidget::paintEvent(event);
}

void VacShowWidget::iconItemNameChanged(const QString &uniqueName, const QString &name)
{
    _vacScrollBarWidget->iconItemNameChanged(uniqueName, name);
}
void VacShowWidget::initIconItem()
{
    _vacScrollBarWidget->initIconItem();
}

void VacShowWidget::closeBtnClicked()
{
    emit vacCloseBtnClicked();
    movetoFirst();
}

void VacShowWidget::movetoFirst()
{
    _vacScrollBarWidget->movetoFirst();
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
    connect(_vacWidget, SIGNAL(addApp(const QString&,const QString&, const QString&, int, const QString&)),
            this, SIGNAL(addApp(const QString&,const QString&, const QString&, int, const QString&)));
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

int VacScrollBarWidget::addIcon(QString text, const QString &icon, int page, int index, const QString &url, int type, const QString &uniqueName)
{
    return _vacWidget->addIcon(text, icon, page, index, url, type, uniqueName);
}

void VacScrollBarWidget::delIcon(const QString &uniqueName)
{
    _vacWidget->delIcon(uniqueName);
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

void VacScrollBarWidget::desktopDelIcon(const QString &uniqueName)
{
    _vacWidget->unChecked(uniqueName);
}

void VacScrollBarWidget::iconItemNameChanged(const QString &uniqueName, const QString &name)
{
    _vacWidget->iconItemNameChanged(uniqueName, name);
}

void VacScrollBarWidget::initIconItem()
{
    _vacWidget->initIconItem();
}

void VacScrollBarWidget::movetoFirst()
{
    _vacWidget->move(_vacWidget->pos().x(), 0);
    _scrollBar->setValue(0);
}

//
VacWidget::VacWidget(QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _localCount(0)
{
    _settings = IDesktopSettings::instance();
    _count = 1;
    _pageSize = pageSize;
    _width = _pageSize.width();
    _height = _pageSize.height();

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

    _animation = new QPropertyAnimation(this, "geometry");
    _animation->setEasingCurve(QEasingCurve::OutBack);

    setAcceptDrops(true);
    setAutoFillBackground(true);

    setFixedSize(_width, _height * _count);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);

//    getLocalIcon();
//    getVacIcon();
//    getPaasIcon();
}

VacWidget::~VacWidget()
{

}

int VacWidget::addIcon(QString text,
                       const QString &iconPath,
                       int page,
                       int index,
                       const QString &url,
                       int type,
                       const QString &uniqueName)
{
//    int expandPageCount;
//    int iconNum = _settings->remoteLocalList().count() + _settings->paasList().count() + _settings->vappList().count();
//    if (iconNum % _iconsPerPage == 0)
//        expandPageCount = iconNum / _iconsPerPage;
//    else
//        expandPageCount = iconNum / _iconsPerPage + 1;

//    if (expandPageCount > _count)
//        expand();

    _localCount ++;

    int expandPageCount = (_localCount - 1) / iconsPerPage() + 1;
    if (expandPageCount > _count)
        expand();

    _url = url;

    IconItem *icon = new IconItem(this);
    switch(_settings->iconSize())
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

    icon->setSaveDataBool(false);
    icon->setIconSize(_settings->iconSize());
    icon->setUniqueName(uniqueName);
    LocalApp * _app = LocalAppList::getList()->getAppByUniqueName(uniqueName);

    if (_app)
    {
        icon->setEqualIcon(true);
        icon->setText(_app->name());
    }
    else
    {
        icon->setEqualIcon(false);
        icon->setText(text);
    }
    icon->setIconClass(type);
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
                    move(x(), _pages[page]);
                    break;
                }
            }
        }
    }

    icon->setLineEditReadOnly(true);
    icon->setPixmap(iconPath,text);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(_url);
    icon->setType(type);
    icon->setDirId(0);
    _iconDict.insert(uniqueName, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();

    connect(icon, SIGNAL(addApp(const QString&, const QString&, const QString&, int, const QString&)),
            this, SIGNAL(addApp(const QString&,const QString&, const QString&, int, const QString&)));

//    if (text == "XX的虚拟机" || text == "IE " || text == "WPP" || text == " Word")
//    {
//        icon->setEqualIcon(true);
//        emit addVacApp(text, iconPath, _url);
//    }

//    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
    connect(icon, SIGNAL(delItem(const QString&)), this, SIGNAL(delItem(const QString&)));


     return page;
}

void VacWidget::delIcon(const QString &uniqueName)
{

    if (!_iconDict.value(uniqueName))
        return;

    IconItem *icon = _iconDict.take(uniqueName);
    //icon->setHidden(true);
    int p = icon->page();
    int s = icon->index();

    delete _iconTable[p][s];
    _iconTable[p][s] = NULL;

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

    _localCount--;

    if (_nextIdx[_count - 1] == 0)
        delPage(_count - 1);

}

//void VacWidget::moveBackIcons(int page, int index)
//{
//}

void VacWidget::showApp(bool localApp)
{
        Q_UNUSED(localApp);
}

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
//            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel),
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
//        int x =  _pageSize.width() * (_count - 1)
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
    move(x(), _pages[_current]);
    emit pageDecreased();
}

void VacWidget::getPaasIcon()
{
    QList<PAAS_LIST>& remotePaasList = _settings->remotePaasList();
    for(int i = 0; i < remotePaasList.count(); i++)
    {
        addIcon(remotePaasList.at(i).cnName, remotePaasList.at(i).iconPath,
                -1, i, remotePaasList.at(i).urls, paasIcon, "2_" + remotePaasList.at(i).name);
    }

    qDebug() << remotePaasList.size() << " paasIcon init over!";
}

void VacWidget::getVacIcon()
{
    QList<APP_LIST>& remoteAppList = _settings->remoteAppList();
    for(int i = 0; i < remoteAppList.count(); i++)
    {
        addIcon(remoteAppList[i].name, WIN_VAPP_IconPath + remoteAppList[i].id + ".png", \
                - 1, -1, QString(""), vacIcon, "1_" + remoteAppList[i].id);
    }
    qDebug() << remoteAppList.size() << " VacIcon init over!";
}

void VacWidget::getLocalIcon()
{
    QList<LOCAL_LIST>& remoteLocalList = _settings->remoteLocalList();
    for(int i = 0; i < remoteLocalList.count(); i++)
    {
        addIcon(remoteLocalList[i].name, remoteLocalList[i].iconPath, \
                - 1, -1, QString(""), localIcon, "0_" + remoteLocalList[i].uniqueName);
    }

    qDebug() << remoteLocalList.size() << " LocalIcon init over!";
}

void VacWidget::largeIcon()
{
    _settings->setIconSize(IconWidget::large_size);
    refresh(LARGESIZE);
}

void VacWidget::mediumIcon()
{
    _settings->setIconSize(IconWidget::medium_size);
    refresh(MEDIUMSIZE);
}

void VacWidget::smallIcon()
{
    _settings->setIconSize(IconWidget::small_size);
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

    qDebug()<<" VacWidget reload all.";
    movetoFirst();
    update();
}

void VacWidget::movetoFirst()
{
    _current = 0;
    move(x(), _pages[_current]);
//    qDebug() << "------------------>movetoFirst";
//    emit pageChanged(_current);

}

void VacWidget::reloadApplist(QSize size)
{
    Q_UNUSED(size);
    _count = 1;
    _localCount = 0;

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
    switch(_settings->iconSize())
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

void VacWidget::unChecked(const QString &uniqueName)
{
    if (!_iconDict.value(uniqueName))
        return;

    _iconDict.value(uniqueName)->setEqualIcon(false);
}

void VacWidget::iconItemNameChanged(const QString &uniqueName, const QString &name)
{
    if (!_iconDict.value(uniqueName))
        return;

    _iconDict.value(uniqueName)->setText(name);
}

void VacWidget::initIconItem()
{
    getLocalIcon();
    getVacIcon();
    getPaasIcon();
}
