#include <QAction>
#include <QPainter>
#include <QApplication>

#include <QtDebug>

#include <QPalette>
#include <QSettings>
#include <QApplication>
#include <QMenu>
#include <QFileInfoList>
#include <QLibrary>
#include <windows.h>
#include <stdio.h>
#include <QMessageBox>

#include <QtDebug>
#include <QDrag>
#include <QDesktopServices>

#include "idesktopsettings.h"
#include "DirWidget.h"
#include "config.h"
#include "dirwidget.h"
#include "appmessagebox.h"
#include "paascommuinication.h"
#include "runapp.h"
#include "strings.h"
#include <string.h>

#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"

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

#define LARGESIZE QSize(143, 143)
#define MEDIUMSIZE QSize(119, 119)
#define SMALLSIZE QSize(103, 103)

#define ICONHSPACING (88 - 40)
#define ICONVSPACING (36 - 36)

extern QString WIN_VAPP_IconPath;
extern QList<APP_LIST> g_RemoteappList;
extern QList<PAAS_LIST> g_RemotepaasList;

QList<TEMP_LIST> dirWidget_FirstLists;

static QPoint gap;

//******/////
DirShowWidget::DirShowWidget(int id, QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _id(id)
{
    QTextCodec *codec = QTextCodec::codecForName("System"); //System
    QTextCodec::setCodecForCStrings(codec);

    _width = pageSize.width();

    _dirWidget = new DirWidget(_id, pageSize, this);
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

    if (_dirWidget->getIconNum() == 0)
        _clearBtn->setSwitcherEnabled(false);

    _clearBtn->setVisible(false);

    connect(_clearBtn, SIGNAL(switcherActivated()), this, SLOT(clearAllIcon()));

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
                    QScrollBar::up-arrow:vertical{background-color: rgba(255,255,255,0);height:13px;}\
                    QScrollBar::down-arrow:vertical{background-color: rgba(255,255,255,0);height:13px;}");

    _scrollBar->setVisible(false);

    _scrollBar->setSingleStep(_dirWidget->getHeight());
    _scrollBar->setPageStep(_dirWidget->getHeight());

    _animation = new QPropertyAnimation(_dirWidget, "geometry");

    connect(_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
 //   connect(_dirWidget, SIGNAL(sendUrl(const QString&)), this, SIGNAL(sendUrl(const QString&)));
    if (_id != 1000)
    {
        connect(_dirWidget, SIGNAL(pageIncreased()), this, SLOT(setSize()));
        connect(_dirWidget, SIGNAL(pageDecreased()), this, SLOT(setSize()));
        connect(_dirWidget, SIGNAL(heightChanged()), this, SLOT(setSize()));
    }

    connect(_dirWidget, SIGNAL(dirWidgetDragLeave(const QString &))
            , this, SLOT(dirWidgetLeave(const QString &)));
    connect(_dirWidget, SIGNAL(dirWidgetDelIcon(int, const QString &))
            , this, SIGNAL(dirWidgetDelIcon(int, const QString &)));
    connect(_dirWidget, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , this, SIGNAL(iconItemNameChanged(const QString &, const QString &)));
    connect(_dirWidget, SIGNAL(refreshDirMinWidget(int))
            , this, SIGNAL(refreshDirMinWidget(int)));
    connect(_dirWidget, SIGNAL(getParentGlobalPos())
            , this, SLOT(getParentGlobalPos()));
    connect(_dirWidget, SIGNAL(setClearBtnEnabled(bool))
            , this, SLOT(setClearBtnEnabled(bool)));
    connect(_dirWidget, SIGNAL(dustbinRestore(IconItem*))
            , this, SIGNAL(dustbinRestore(IconItem*)));
    connect(_dirWidget, SIGNAL(iconTovDesktop(const QString &))
            , this, SIGNAL(iconTovDesktop(const QString &)));
    connect(_dirWidget, SIGNAL(dushbinDirWidgetRefresh(int))
            , this, SIGNAL(dushbinDirWidgetRefresh(int)));
    connect(_dirWidget, SIGNAL(showIconMenu(QPoint, const QString &))
            , this, SIGNAL(showIconMenu(QPoint, const QString &)));
    connect(_dirWidget, SIGNAL(showDustbinMenu(QPoint, IconItem *))
            , this, SIGNAL(showDustbinMenu(QPoint, IconItem *)));
    connect(_dirWidget, SIGNAL(hideMenu())
            , this, SIGNAL(hideMenu()));


}

DirShowWidget::~DirShowWidget()
{

}

void DirShowWidget::setClearBtnEnabled(bool enabled)
{
    _clearBtn->setSwitcherEnabled(enabled);
}

void DirShowWidget::getParentGlobalPos()
{
    QPoint GlobalPoint(this->mapToGlobal(QPoint(0, 0)));
    int x = GlobalPoint.x();
    int y = GlobalPoint.y();

    _dirWidget->setGlobalPos(QRect(x, y + 30
                             ,_width, _height - 30));
}

void DirShowWidget::dirWidgetLeave(const QString &uniqueName)
{
    emit dirWidgetDragLeave(uniqueName, _id);
}

void DirShowWidget::showTitle(QString text)
{
    _text = text;

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

void DirShowWidget::delIcon(const QString &uniqueName)
{
    _dirWidget->delIcon(uniqueName);
}

void DirShowWidget::scrollBarValueChanged(int val)
{
    _oldPagePos = _newPagePos;
    _newPagePos = val / _dirWidget->getHeight();
    if( _oldPagePos == _newPagePos )
        return;

    if (_animation->state() == QAbstractAnimation::Running)
    {
        return;
    }

    _animation->setDuration(10);
    _animation->setStartValue(QRect(0, _dirWidget->pos().y(), _dirWidget->width(), _dirWidget->height()));
    _animation->setEndValue(QRect(0, -1 * (_newPagePos * _dirWidget->getHeight()) + 20, _dirWidget->width(), _dirWidget->height()));
    _animation->start();
}

void DirShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, _width, _height, QPixmap(":images/dir_bg.png").scaled(_width, _height, \
                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QWidget::paintEvent(event);
}

void DirShowWidget::mousePressEvent(QMouseEvent *)
{

}

void DirShowWidget::mouseMoveEvent(QMouseEvent *)
{

}


void DirShowWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    _width = width();

    int w = 15;
    int x = width() - w;
    x = x < 0 ? 0: x;
    int h = height() - 30;
    _scrollBar->setGeometry(x, 8, w, h - 16);

//    _scrollBar->setRange(0, _dirWidget->count() * _dirWidget->height() - h);

    update();
}

void DirShowWidget::setSize()
{

    if (_maxPage == 0)
        return;

    if ( count() > _maxPage )
    {
        _height = _dirWidget->getHeight() * _maxPage + 30;

        setFixedSize(_width, _height);

        _dirWidget->setGeometry(0, 30, _dirWidget->width() - 5, _dirWidget->height());

        _scrollBar->setRange(0, (_dirWidget->count() - _maxPage)* _dirWidget->getHeight());
        _scrollBar->setVisible(true);
    }
    else
    {
        _height = _dirWidget->getHeight() * _dirWidget->count() + 30;
        setFixedSize(_width, _height);

        _dirWidget->setGeometry(0, 30, _dirWidget->width() - 5, _dirWidget->height());
        _scrollBar->setVisible(false);
    }

    repaint();
}

void DirShowWidget::wheelEvent(QWheelEvent *event)
{
    _scrollBar->event(event);
}

void DirShowWidget::addDirIcon(const QString &text, const QString &iconPath,
                               int page, int index, const QString &url,
                               int type, const QString &uniqueName)
{
    _dirWidget->addIcon(text, iconPath, page, index, url, type, uniqueName);
}

void DirShowWidget::setMaxRow(int row)
{
    _maxPage = row;

    setSize();

    _dirWidget->setMaxPage(row);

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

void DirShowWidget::clearDirIcon()
{
    _dirWidget->clearDirIcon();
}

void DirShowWidget::getFirstIconItem()
{
    _dirWidget->getFirstIconItem();
}

void DirShowWidget::initIconItem()
{
    _dirWidget->initIconItem();
}

int DirShowWidget::getDustBinNewHeight()
{
    setSize();
    return _height;
}

void DirShowWidget::dushbinRefreshDirMinWidget(const QString &uniqueName)
{
    _dirWidget->dushbinRefreshDirMinWidget(uniqueName);
}

void DirShowWidget::restoreIcon(IconItem *iconItem)
{
    if (!iconItem)
        return;

    _dirWidget->dustbinMenuRestoreClicked(iconItem);
}

//
DirWidget::DirWidget(int id, QSize pageSize, QWidget *parent)
    : QWidget(parent)
    , _localCount(0)
    , _iconNum(0)
    , _url("")
    , _dragEvent(false)
    , _dragDown(false)
    , _dragUp(false)
    , _id(id)
//    , _iconMenu(NULL)
//    , _dustbinMenu(NULL)
    , _currentIconItem(NULL)
{
    _iconSize = IDesktopSettings::instance()->iconSize();
    _count = 1;
    _pageSize = pageSize;
    _width = _pageSize.width();

    changeSpacing();

    _col = (_width - ICONHSPACING) / gridWidth;
    _row = (_height - ICONVSPACING) / gridHeight;

    _iconsPerPage = _col * _row;
    _current  = 0;

    _local = LocalAppList::getList();

    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == _id)
        {
            _iconNum ++;
        }
    }

//    for (int i = 0; i < _iconNum; i++)   //_local->count()
//    {
//        _count = i / _iconsPerPage + 1;
//    }

    _count = (_iconNum - 1) / _iconsPerPage + 1;

    _iconNum = 0;

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
 //       qDebug() << _gridTable.count() << "-0------------0!<< DirWidget::DirWidget ";
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

    setAcceptDrops(true);
    setAutoFillBackground(true);

    setFixedSize(_width, _height * _count);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);

    if (_id != 1000)
        initIconItem();

//    if (_id == 1000)
//    {
//        _dustbinMenu = new MenuWidget(MenuWidget::dustbinMenu, this);
//        _dustbinMenu->setVisible(false);

//        connect(_dustbinMenu, SIGNAL(restore()), this, SLOT(dustbinMenuRestoreClicked()));
//        connect(_dustbinMenu, SIGNAL(del()), this, SLOT(menuDelClicked()));
//    }
//    else
//    {
//        _iconMenu = new MenuWidget(MenuWidget::iconMenu, this);
//        _iconMenu->setVisible(false);

//        connect(_iconMenu, SIGNAL(run()), this, SLOT(iconMenuRunClicked()));
//        connect(_iconMenu, SIGNAL(del()), this, SLOT(menuDelClicked()));
//    }

    connect(_local, SIGNAL(appRemoved(const QString&)), this, SLOT(delIcon(const QString&)));
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

    icon->setSaveDataBool(true);
    icon->setIconSize(_iconSize);
    icon->setUniqueName(uniqueName);
    icon->setText(text);
    icon->setTimeLine(false);
    icon->setPropertyAnimation(true);
    icon->setType(type);

    if (_id == 1000)
    {
        if(type == 3)   //dirIcon
        {
            icon->addMinWidget(_iconSize);
        }

        icon->setDragEventBool(false);
        icon->setDoubleClickBool(false);
    }
    else
    {
        icon->setDragEventBool(true);
        icon->setDoubleClickBool(true);
    }

    icon->setIconClass(type);
    icon->setEnterEventBool(false);
    icon->setLeaveEventBool(false);
    icon->setRunAction(false);
    icon->setDelAction(false);
    icon->setClosePixBool(false);
    icon->setSelectPixBool(false);
    icon->setEqualIcon(false);
    icon->setContextMenuBool(true);
    icon->setTrembleBool(false);

    if (page == -1) {
        for (int i = 0; i < _count; i++)
        {
            if (_nextIdx[i] < _row * _col) {
                page = i;
                index = _nextIdx[i];
 //               move(_pages[page], y());
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
//                        move(_pages[page], y());
                        break;
                    }
                }
            }
        }
        else
        {
            if (index >= _iconsPerPage)
            {
                for (int i = 0; i < _count; i++) {
                    if (_nextIdx[i] < _row * _col)
                    {
                        page = i;
                        index = _nextIdx[i];
 //                       move(_pages[page], y());
                        break;
                    }
                }
            }
        }
    }

    icon->setPixmap(iconPath, text);
    icon->setGeometry(_gridTable[page][index].translated(HSPACING, VSPACING));
    icon->setPage(page);
    icon->setIndex(index);
    icon->setUrl(url);
    icon->setDirId(_id);
//    icon->setId(111);
    if (type == 3)
    {
        for (int i = 0; i < _local->count(); i++)
        {
            if (_local->at(i)->hidden())
                continue;

            if (_local->at(i)->uniqueName() == uniqueName)
            {
                icon->setDirMinItemId(_local->at(i)->id().toInt());
                emit dushbinDirWidgetRefresh(_local->at(i)->id().toInt());
                break;
            }
        }
    }
    else
    {
        icon->setId(111);
    }

    _iconDict.insert(uniqueName, icon);
    _iconTable[page][index] = icon;
    _nextIdx[page]++;
    icon->show();
    if (type == 3)
    {
        dushbinRefreshDirMinWidget(uniqueName);
    }

    _iconNum ++;
    connect(icon, SIGNAL(runItem(const QString&)), this, SLOT(runApp(const QString&)));
    connect(icon, SIGNAL(showContextMenu(bool, QPoint, QPoint, IconItem *))
            , this, SLOT(showIconContextMenu(bool, QPoint, QPoint,IconItem *)));
    connect(icon, SIGNAL(iconItemNameChanged(const QString &, const QString &))
            , this, SIGNAL(iconItemNameChanged(const QString &, const QString &)));
    _url = QString("");

    getFirstIconItem();

    if(_iconNum > 0)
        emit setClearBtnEnabled(true);

     return page;
}

void DirWidget::expand()
{
    _count++;

    setFixedSize(_pageSize.width(), _count * _height);

    _pages.append(-(_count-1) * _height);
    QList<QRect> newGridList;

    changeSpacing();

    for (int i = 0; i < _col * _row; i++)
    {
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
//    if (_current > page) {
//        _current--;
//        if (_inDrag)
//            _inDrag->setPage(_inDrag->page() - 1);
//        emit pageChanged(_current);
//    }
    setFixedSize(_pageSize.width(),  _count * _pageSize.height());
    move(_pages[_count - 1], y());
    emit pageDecreased();
}

void DirWidget::delIcon(const QString &_uniqueName)
{
    qDebug() << _uniqueName;
    if (!_iconDict.value(_uniqueName))
        return;

    IconItem *icon = _iconDict.take(_uniqueName);
    int p = icon->page();
    int s = icon->index();
    delete _iconTable[p][s];

    moveBackIcons(p, s);
}


void DirWidget::removeIcon(const QString &_uniqueName)
{
    if (!_iconDict.value(_uniqueName))
        return;

    int p = _iconDict.value(_uniqueName)->page();
    int s = _iconDict.value(_uniqueName)->index();

    _iconDict.take(_uniqueName);

    _iconTable[p][s]->deleteLater();
    _iconTable[p][s] = NULL;

    moveBackIcons(p, s);

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
    {
        if (_count - 1 != 0)
        {
            delPage(_count - 1);
        }
    }

    _iconNum--;

    if (_id != 1000)
        getFirstIconItem();

    if(_iconNum == 0)
        emit setClearBtnEnabled(false);
}
void DirWidget::largeIcon()
{
    _iconSize = IconItem::large_size;
    refresh(LARGESIZE);

//    _iconNum = 0;

//    initIconItem();

    emit heightChanged();


}

void DirWidget::mediumIcon()
{
    _iconSize = IconItem::medium_size;
    refresh(MEDIUMSIZE);

    emit heightChanged();
}

void DirWidget::smallIcon()
{
    _iconSize = IconItem::small_size;
    refresh(SMALLSIZE);

    emit heightChanged();
}

void DirWidget::refresh(QSize size)
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

    _iconNum = 0;
    _count = 1;

    for (int i = 0; i < _dirTempLists.count(); i++)
    {

        addIcon(_dirTempLists.at(i).name, _dirTempLists.at(i).iconPath,
                -1, -1,
                _dirTempLists.at(i).url, _dirTempLists.at(i).type,
                _dirTempLists.at(i).uniqueName);
    }

    _dirTempLists.clear();


}

void DirWidget::movetoFirst()
{
    _current = 0;
    move(_pages[_current], y());
//    emit pageChanged(_current);
//    move(0, 0);

}

void DirWidget::reloadApplist(QSize size)
{
    Q_UNUSED(size);

    _count = 1;

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

    setFixedSize(_width, _height * _count);

//    qDebug()<<" DirWidget reload all.";
}

void DirWidget::deleteAllIconItem()
{
    _dirTempLists.clear();

    TEMP_LIST dirTempList;

    for (int i = 0; i < _count; i++)
    {
        for(int j = 0;j < _iconsPerPage; j++)
        {
            if(_iconTable[i][j] != NULL)
            {
                dirTempList.name = _iconTable[i][j]->text();
                dirTempList.iconPath = _iconTable[i][j]->pix();
                dirTempList.page = -1;
                dirTempList.index = -1;
                dirTempList.url = _iconTable[i][j]->url();
                dirTempList.type = _iconTable[i][j]->type();
                dirTempList.id = _iconTable[i][j]->id();
                dirTempList.uniqueName = _iconTable[i][j]->uniqueName();

                _dirTempLists.append(dirTempList);

                delete _iconTable[i][j];
                _iconTable[i][j] = NULL;
            }
        }
        _nextIdx[i]=0;
    }
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
            gridHeight = MEDIUMSIZE.height() + VSPACING + BOTTOMSPACING;
            break;

        default:

            _height = SMALLSIZE.height() + VSPACING + BOTTOMSPACING;

            _iconHSpacing = ICONHSPACING;
            _iconVSpacing = ICONVSPACING;

            gridWidth = SMALLSIZE.width() + HSPACING * 2;
            gridHeight = SMALLSIZE.height() + VSPACING + BOTTOMSPACING;
            break;
    }

    update();
}

void DirWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit getParentGlobalPos();
    _dragEvent = true;

     IconItem *icon = qobject_cast<IconItem*> (event->source());
    _inDrag = icon;
    icon->hide();

    event->setDropAction(Qt::MoveAction);
    event->accept();
}


void DirWidget::dragMoveEvent(QDragMoveEvent *event)
{
    _dragEvent = true;

    event->setDropAction(Qt::MoveAction);
    event->accept();

    int p = _inDrag->page();
    int s = _inDrag->index();

    _currentPage = event->pos().y() / _height;
    /* The virtual desktop can only expand to right */
    /* If the page is the last page and there is only one icon
     * on this page, this icon can't be moved to add a page
     */

    /* move to the next page */
    if ((event->pos().y() > (p + 1) * _height) && (mapFromGlobal(event->pos()).y() < _parentRect.y() + _parentRect.height())){


        if (p + 1 == _count)
            return;

        dragDown(event->pos());
    }
    else if ((event->pos().y() > 0) && (event->pos().y() < p * _height))      /* move to the up page */
    {
        dragUp(event->pos());

    }
    else {          /* move in this page */

        QRect moving(event->pos() - gap, _inDrag->size() * 1.0);

        int index = getNearestIndex(moving);
        if (index == -1 || s == index)
        {
           return;
        }
#if 0
        if ((ADDICON == _iconTable[_current][index]->_icontype)||(ADDICON == _iconTable[_current][s]->_icontype))
        {
            return;
        }
#endif
        index = index > _nextIdx[_currentPage] - 1 ? _nextIdx[_currentPage] - 1: index;
        if (s > index) {
            for (int i = s; i > index; i--) {
                QPoint end = _gridTable[_currentPage][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_currentPage][i-1]->move(end);
                _iconTable[_currentPage][i] = _iconTable[_currentPage][i-1];
                _iconTable[_currentPage][i]->setIndex(i);
            }
        }
        else {
            for (int i = s; i < index; i++) {
                QPoint end = _gridTable[_currentPage][i].translated(HSPACING, VSPACING).topLeft();
                _iconTable[_currentPage][i+1]->move(end);
                _iconTable[_currentPage][i] = _iconTable[_currentPage][i+1];
                _iconTable[_currentPage][i]->setIndex(i);
            }
        }
        _iconTable[_currentPage][index] = _inDrag;
        _inDrag->setPage(_currentPage);
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
    Q_UNUSED(event);

    if (!_inDrag)
        return;

    if (_dragDown || _dragUp)
        return;

    if (_parentRect.width() == 0 || _parentRect.height() == 0)
        return;

    if (QRect(_parentRect).contains(cursor().pos())) {
        return;
    }

    _dragEvent = false;

    int p = _inDrag->page();
    int s = _inDrag->index();
//    _inDrag->hide();

    if (!_iconDict.value(_inDrag->uniqueName()))
        return;

    emit dirWidgetDragLeave(_inDrag->uniqueName());

    _iconDict.take(_inDrag->uniqueName());

    _iconTable[p][s] = NULL;

    moveBackIcons(p, s);

    _inDrag = NULL;

}

void DirWidget::dropEvent(QDropEvent *event)
{

    event->setDropAction(Qt::MoveAction);
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
    //int page = rect.x()/_pageSize.width();
    int page = _currentPage;
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
//    if (_iconMenu)
//        _iconMenu->setVisible(false);
//    else
//        _dustbinMenu->setVisible(false);

    emit hideMenu();

    event->accept();

    if (Qt::RightButton == event->button())
        return;
}

void DirWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void DirWidget::mouseReleaseEvent(QMouseEvent *)
{

}

void DirWidget::showIconContextMenu(bool visiable,QPoint pos, QPoint mPos, IconItem *iconItem)
{
    Q_UNUSED(pos);
    Q_UNUSED(visiable);

    if(!iconItem)
        return;

    if (_currentIconItem)
        _currentIconItem = NULL;

    _currentIconItem = iconItem;
    _currentUniqueName = _currentIconItem->uniqueName();

    if (_id != 1000)
    {
//        _iconMenu->move(pos.x() + _iconDict.value(_currentUniqueName)->pos().x(), pos.y() + _iconDict.value(_currentUniqueName)->pos().y());
//        _iconMenu->raise();
//        _iconMenu->setVisible(visiable);
        emit showIconMenu(mPos, iconItem->uniqueName());
    }
    else
    {
//        _dustbinMenu->move(pos.x() + _iconDict.value(_currentUniqueName)->pos().x(), pos.y() + _iconDict.value(_currentUniqueName)->pos().y());
//        _dustbinMenu->raise();
//        _dustbinMenu->setVisible(visiable);
        emit showDustbinMenu(mPos, _currentIconItem);
    }
}

void DirWidget::iconMenuRunClicked()
{
//    if (_iconMenu)
//        _iconMenu->setVisible(false);
//    else
//        _dustbinMenu->setVisible(false);

    emit hideMenu();

    runApp(_currentUniqueName);

}

void DirWidget::menuDelClicked()
{
//    if (_iconMenu)
//        _iconMenu->setVisible(false);
//    else
//        _dustbinMenu->setVisible(false);
    emit hideMenu();

//    delIcon(_currentUniqueName);
//    emit delApp(_currentUniqueName);
    LocalAppList::getList()->delApp(_currentUniqueName);
    emit dirWidgetDelIcon(_id, _currentUniqueName);

}

void DirWidget::dustbinMenuRestoreClicked(IconItem *iconItem)
{
//    if (_iconMenu)
//        _iconMenu->setVisible(false);
//    else
//        _dustbinMenu->setVisible(false);

//    emit hideMenu();

    if (!iconItem)
        return;
    _currentIconItem = iconItem;

    emit dirWidgetDragLeave("");

    _iconDict.take(_currentIconItem->uniqueName());
    _iconTable[_currentIconItem->page()][_currentIconItem->index()]->hide();
    _iconTable[_currentIconItem->page()][_currentIconItem->index()] = NULL;

    moveBackIcons(_currentIconItem->page(), _currentIconItem->index());


    emit dustbinRestore(_currentIconItem);

//    _currentIconItem = NULL;
}

void DirWidget::runApp(const QString &uniqueName)
{
    if (_id == 1000)
        return;

    RunApp::getRunApp()->runApp(uniqueName);
}

void DirWidget::clearAllIcon()
{
    if (_clearNames.count() != 0)
        _clearNames.clear();

//    if(_iconInDirs.count() != 0)
//        _iconInDirs.clear();

    for (int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == 1000)
        {
//            if (_local->at(i)->type().toInt() == 3)
//            {
//                for (int j = 0; j < _local->count(); j++)
//                {
//                    if (_local->at(j)->hidden())
//                        continue;
//                    if (_local->at(j)->type().toInt() == 3)
//                        continue;

//                    if (_local->at(i)->id().toInt() == _local->at(j)->dirId())
//                    {
//                        _iconInDirs.append(_local->at(j)->uniqueName());
//                    }
//                }
//            }
            _clearNames.append(_local->at(i)->uniqueName());
        }
    }

//    for (int i = _iconInDirs.count(); i > 0; i--)
//    {
//        LocalAppList::getList()->delApp(_iconInDirs.at(i - 1));
////        emit dirWidgetDelIcon(_id , _iconInDirs.at(i - 1));
//    }


    for (int i = _clearNames.count(); i > 0; i--)
    {
        LocalAppList::getList()->delApp(_clearNames.at(i - 1));
        emit dirWidgetDelIcon(_id , _clearNames.at(i - 1));
    }
//    _iconInDirs.clear();
    _clearNames.clear();
}

void DirWidget::clearDirIcon()
{
    if (_clearNames.count() != 0)
        _clearNames.clear();

    for (int i = 0; i < _local->count(); i++)
    {
        if (_local->at(i)->hidden())
            continue;

        if (_local->at(i)->dirId() == _id)
        {
            _clearNames.append(_local->at(i)->uniqueName());
        }
    }

    for (int i = _clearNames.count(); i > 0; i--)
    {
//        LocalAppList::getList()->delApp(_clearNames.at(i - 1));
//        emit dirWidgetDelIcon(_id , _clearNames.at(i - 1));
        delIcon(_clearNames.at(i - 1));
    }

    for (int i = 0; i < _clearNames.count(); i++)
    {
//        LocalAppList::getList()->delApp(_clearNames.at(i - 1));
//        emit dirWidgetDelIcon(_id , _clearNames.at(i - 1));
        emit iconTovDesktop(_clearNames.at(i));
    }

    _clearNames.clear();
}

void DirWidget::setUniqueName(const QString &uniqueName)
{
    _uniqueName = uniqueName;
}

void DirWidget::setMaxPage(int page)
{
    _maxPage = page;
}

void DirWidget::dragDown(QPoint pos)
{

    if (!_inDrag)
        return;

    if (_dragDown || _dragUp)
        return;

    _dragDown = true;

    int p = _inDrag->page();
    int s = _inDrag->index();
    _currentPage = pos.y() / _height;
    //        IconItem *first;

    if ((pos.y() > (p + 1) * _height)
        && (mapFromGlobal(pos).y() < _parentRect.y() + _parentRect.height()))
    {
        if (p + 1 != _count) {
            for (int i = p; i <= _currentPage; i++)
            {
                if (i == p)
                {
                    for (int j = s + 1; j < _nextIdx[p]; j++) {
                        QPoint end = _gridTable[p][j-1].translated(HSPACING, VSPACING).topLeft();
                        _iconTable[p][j]->move(end);
                        _iconTable[p][j]->setIndex(j-1);
                        _iconTable[p][j - 1] = _iconTable[p][j];
                        _iconTable[p][j-1]->setIndex(j-1);
                    }
                }
                else
                {
                    //first = _iconTable[i][0];

                    _iconTable[i - 1][_iconsPerPage-1] = _iconTable[i][0];
                    _iconTable[i - 1][_iconsPerPage-1]->move(_gridTable[i - 1][_iconsPerPage-1].translated(HSPACING, VSPACING).topLeft());
                    _iconTable[i - 1][_iconsPerPage-1]->setPage(i - 1);
                    _iconTable[i - 1][_iconsPerPage-1]->setIndex(_iconsPerPage-1);

                    if (i == _currentPage)
                    {
                        _iconTable[i][0] = _inDrag;
                        _inDrag->setPage(i);
                        _inDrag->setIndex(0);
                    }
                    else
                    {
                        for (int k = 0; k < _iconsPerPage - 1; k++) {
                            _iconTable[i][k] = _iconTable[i][k+1];
                            _iconTable[i][k]->move(_gridTable[i][k].translated(HSPACING, VSPACING).topLeft());
                            _iconTable[i][k]->setIndex(k);
                        }
                    }
                }
            }
        }
    }

    _dragDown = false;
}

void DirWidget::dragUp(QPoint pos)
{
    if (!_inDrag)
        return;

    if (_dragDown || _dragUp)
        return;

    _dragUp = true;

    int p = _inDrag->page();
    int s = _inDrag->index();
    _currentPage = pos.y() / _height;
//    IconItem *last;
    if((pos.y() > 0) && (pos.y() < p * _height))
    {
        if (p != 0) {
            for (int i = p; i >= _currentPage; i--)
            {
                if (i == p)
                {
                    for (int j = s; j > 0; j--) {
                        QPoint end = _gridTable[p][j].translated(HSPACING, VSPACING).topLeft();
                        _iconTable[p][j - 1]->move(end);
                        _iconTable[p][j - 1]->setIndex(i);
                        _iconTable[p][j] = _iconTable[p][j - 1];
                        _iconTable[p][j]->setIndex(j);
                    }
                }
                else
                {
                    //last = _iconTable[i][_iconsPerPage-1];

                    _iconTable[i + 1][0] = _iconTable[i][_iconsPerPage-1];
                    _iconTable[i + 1][0]->move(_gridTable[i + 1][0].translated(HSPACING, VSPACING).topLeft());
                    _iconTable[i + 1][0]->setPage(i + 1);
                    _iconTable[i + 1][0]->setIndex(0);
                    if (i == _currentPage)
                    {
                        _iconTable[i][_iconsPerPage-1] = _inDrag;
                        _inDrag->setPage(i);
                        _inDrag->setIndex(_iconsPerPage-1);
                    }
                    else
                    {
                        for (int k = _nextIdx[i] - 1; k > 0; k--)
                        {
                            _iconTable[i][k - 1]->move(_gridTable[i][k].translated(HSPACING, VSPACING).topLeft());
                            _iconTable[i][k] = _iconTable[i][k-1];
                            _iconTable[i][k]->setIndex(k);
                        }

                    }
                }
            }
        }
    }

    _dragUp = false;

}

void DirWidget::getFirstIconItem()
{
    dirWidget_FirstLists.clear();

    TEMP_LIST dirTempList;

    for (int i = 0; i < 1; i++) //_count
    {
        for(int j = 0;j < 4; j++) // _iconsPerPage
        {
            if(_iconTable[i][j] != NULL)
            {
                if (dirWidget_FirstLists.count() == 4)
                    return;

                dirTempList.name = _iconTable[i][j]->text();
                dirTempList.iconPath = _iconTable[i][j]->pix();
                dirTempList.page = -1;
                dirTempList.index = -1;
                dirTempList.url = _iconTable[i][j]->url();
                dirTempList.type = _iconTable[i][j]->type();
                dirTempList.id = _iconTable[i][j]->id();
                dirTempList.uniqueName = _iconTable[i][j]->uniqueName();

                dirWidget_FirstLists.append(dirTempList);

            }
        }
    }

    emit refreshDirMinWidget(_id);
}

void DirWidget::setGlobalPos(QRect r)
{
    _parentRect = r;
}

void DirWidget::initIconItem()
{
    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;
        if (_local->at(i)->dirId() == _id)
        {
            addIcon(_local->at(i)->name(), _local->at(i)->icon(),
                    _local->at(i)->page(), _local->at(i)->index(),
                    _local->at(i)->url(), _local->at(i)->type().toInt()
                    , _local->at(i)->uniqueName());
        }
    }
}

void DirWidget::setId(int id)
{
    int oldId = _id;
    _id = id;

    for (int i = 0; i < _local->count(); i++) {
        if (_local->at(i)->hidden())
            continue;
        if (_local->at(i)->dirId() == oldId)
        {
            if (_iconDict.value(_local->at(i)->uniqueName()))
            {
                _iconDict.value(_local->at(i)->uniqueName())->setDirId(_id);
            }
        }
    }
}

void DirWidget::dushbinRefreshDirMinWidget(const QString &uniqueName)
{
    if (_iconDict.value(uniqueName))
    {
        _iconDict.value(uniqueName)->refreshDirMinWidgetIcon();
    }
}
