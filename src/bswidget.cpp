#include "bswidget.h"
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QDesktopWidget>
#include <QWebSettings>
#include <QDesktopServices>
#include <ActiveQt/QAxWidget>
#include <QtDebug>
#include <QPushButton>
#include <QWebSettings>
#include <QSettings>
#include <QDir>
#include <QTimer>
#include <QDesktopServices>
#include "config.h"

extern QString serverip;

MyWebView::MyWebView(QWidget *parent)
    : QWebView(parent)
{

}

void MyWebView::setNewUrl(QString url)
{
    QUrl u(url);
    emit linkClicked(u);
}

void MyWebView::assembling()
{
    emit startAssembly();
}

void MyWebView::assembled()
{
    emit stopAssembly();
}

BsWidget::BsWidget(int width, int height, QWidget *parent)
    : QWidget(parent)
    , _width(width)
    , _height(height)
{
    _leftWebKit = new LeftWebKit(this);
    //_leftWebKit->setGeometry(0, 20, _width * 0.14 + 10, _height);
    _leftWebKit->setGeometry(0, 20, _width, _height - 40);

    _rightWidget = new RightWidget(this);
    _rightWidget->setGeometry(_width * 0.14 + 28, 20, _width - _width * 0.14 - 28, _height - 40);
    _rightWidget->setVisible(true);


    connect(_leftWebKit, SIGNAL(startAssembly()), this, SLOT(startAssembly()));
    connect(_leftWebKit, SIGNAL(stopAssembly()), this, SLOT(stopAssembly()));
    connect(_leftWebKit, SIGNAL(link(const QUrl&)), _rightWidget, SLOT(createTab(const QUrl&)));
    connect(_leftWebKit, SIGNAL(hideMenu()), this, SIGNAL(hideMenu()));
}

BsWidget::~BsWidget()
{
}

void BsWidget::startAssembly()
{
    _leftWebKit->setGeometry(0, 20, _width, _height - 80);
    _rightWidget->setVisible(false);
}

void BsWidget::stopAssembly()
{
    //_leftWebKit->setGeometry(0, 20, _width * 0.14 + 10, _height);
    _leftWebKit->setGeometry(0, 20, _width, _height - 80);
    _rightWidget->setVisible(true);
}

LeftWebKit::LeftWebKit(QWidget *parent) :
    QMainWindow(parent)
{
    //resize(QApplication::desktop()->width(),QApplication::desktop()->height());
    setWindowFlags(Qt::FramelessWindowHint);

    //_webview = new QWebView(this);
    _webview = new MyWebView(this);
    _webview->setGeometry(0, 0, width(), height());
    _webpage = new WebPage();
    _webview->setPage(_webpage);

    QWebSettings *websetting= QWebSettings::globalSettings();
    websetting->setAttribute(QWebSettings::JavascriptEnabled,true);    
    // Plug-ins must be set to be enabled to use plug-ins.
    websetting->setAttribute(QWebSettings::PluginsEnabled,true);
    websetting->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
    websetting->setObjectCacheCapacities(0, 0, 0);

    _webview->load(QUrl("http://"+serverip+"/point.html"));

    //_webview->load(QUrl("./system_manage/revamp/demo.html"));
    //_webview->load(QUrl("http://192.168.30.68/point.html"));
    _webview->page()->mainFrame()->addToJavaScriptWindowObject("mywebkit",_webview);

    setCentralWidget(_webview);

    _webview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(_webview, SIGNAL(startAssembly()), this, SIGNAL(startAssembly()));
    connect(_webview, SIGNAL(stopAssembly()), this, SIGNAL(stopAssembly()));
    connect(_webview, SIGNAL(linkClicked(const QUrl&)), this, SIGNAL(link(const QUrl&)));
}

LeftWebKit::~LeftWebKit()
{

}

void LeftWebKit::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    //_webview->setGeometry(0, 0, width(), height());
    //QMainWindow::resizeEvent(event);
}
RightWidget::RightWidget(QWidget *parent)
    : QWidget(parent)
    , _width(0)
    , _height(0)
{
    _tabWidget = new TabWidget(this);
    _tabWidget->setVisible(false);

    _rightTopPix.load(":/images/bs_rightbg_top.png");
    _rightCenterPix.load(":/images/bs_rightbg_center.png");
    _rightBottomPix.load(":/images/bs_rightbg_bottom.png");

}
RightWidget::~RightWidget()
{
    delete _tabWidget;
}

void RightWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _width, 9,\
                       _rightTopPix.scaled(_width, 9));
    painter.drawPixmap(0, 9, _width, _height - 18,\
                       _rightCenterPix.scaled(_width, _height - 18));
    painter.drawPixmap(0, _height - 9, _width, 9,\
                       _rightBottomPix.scaled(_width, 9));

    QWidget::paintEvent(event);
}

void RightWidget::resizeEvent(QResizeEvent *event)
{
    _width = width();
    _height = height();
    _tabWidget->setGeometry(5, 5, _width - 10, _height - 10);
    _tabWidget->setVisible(true);
    QWidget::resizeEvent(event);
}

void RightWidget::createTab(const QUrl &url)
{
    _tabWidget->createTab(url);
}

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setDocumentMode(true);

    setStyleSheet("QTabWidget{border: 0px solid gray;background:rgba(255,255,255,0);}\
                  QTabWidget::tab-bar{left: 110px;}\
                  QTabBar::tab{border:1px solid #bdbcbd; \
                               border-top-left-radius:5px; border-top-right-radius:5px; \
                               min-width:185px; min-height:35px;max-width:185px; color:#FFFFFF}\
                  QTabBar::tab:selected,QTabBar::tab:hover{border-bottom-color: #f5fbfe; \
                               border-image:url(:/images/bs_tab_selected.png);}\
                  QTabBar::tab:!selected{border-bottom-color: #bdbcbd;\
                               border-image:url(:/images/bs_tab_normal.png);} \
                  QTabBar::close-button{border-image:url(:/images/bs_tab_close_normal.png);\
                               subcontrol-position:right;}\
                  QTabBar::close-button:hover{border-image:url(:/images/bs_tab_close_hover.png);}");

    _goBack = new QPushButton(this);
    _goBack->setGeometry(5, 7, 28, 26);
    _goBack->setStyleSheet("QPushButton{background-image:url(:images/bs_back_normal.png);border-style:flat;}\
                QPushButton:hover:pressed{background-image:url(:/images/bs_back_normal.png);border-style:flat;} \
                QPushButton:hover{background-image:url(:/images/bs_back_hover.png);border-style:flat;}\
                QPushButton:!enabled{background-image:url(:/images/bs_back_disabled.png);border-style:flat;}");
    _goBack->setEnabled(false);
    _goBack->setVisible(false);

    _goForward = new QPushButton(this);
    _goForward->setGeometry(37, 7, 28, 26);
    _goForward->setStyleSheet("QPushButton{background-image:url(:/images/bs_forward_normal.png);border-style:flat;}\
                QPushButton:hover:pressed{background-image:url(:/images/bs_forward_normal.png);border-style:flat;} \
                QPushButton:hover{background-image:url(:/images/bs_forward_hover.png);border-style:flat;}\
                QPushButton:!enabled{background-image:url(:/images/bs_forward_disabled.png);border-style:flat;}");
    _goForward->setEnabled(false);
    _goForward->setVisible(false);

    _refresh = new QPushButton(this);
    _refresh->setGeometry(69, 7, 28, 26);
    _refresh->setStyleSheet("QPushButton{background-image:url(:/images/bs_refresh_normal.png);border-style:flat;}\
                QPushButton:hover:pressed{background-image:url(:/images/bs_refresh_normal.png);border-style:flat;} \
                QPushButton:hover{background-image:url(:images/bs_refresh_hover.png);border-style:flat;}\
                QPushButton:!enabled{background-image:url(:/images/bs_refresh_disabled.png);border-style:flat;}");
    _refresh->setEnabled(false);
    _refresh->setVisible(false);

    this->setElideMode(Qt::ElideRight);
    this->setTabsClosable(false);

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTabWidget(int)));

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(curChanged(int)));
    connect(_goBack, SIGNAL(clicked()), this, SLOT(goBack()));
    connect(_goForward, SIGNAL(clicked()), this, SLOT(goForward()));
    connect(_refresh, SIGNAL(clicked()), this, SLOT(refresh()));

}
/*
void TabWidget::resizeEvent(QResizeEvent *event)
{
    _axWidget->resize(width(), height());
}
*/
/*
void TabWidget::setUrl(const QUrl &url)
{
    qDebug() << url;
    _axWs.last()->setUrl(url);
}
*/
void TabWidget::createTab(const QUrl &url)
{
    _url = url;
    _pal.setBrush(QPalette::Window, QBrush(QPixmap("")));
    setPalette(_pal);

    _axWidget = new AxWidget(_axWs.count(), this);
    addTab(_axWidget, "");
    _axWidget->resize(width(), height());
    _axWidget->setUrl(_url);

    _axWs.append(_axWidget);

    setCurrentIndex(_axWs.count() - 1);

    if(_axWs.count() > 1)
        this->setTabsClosable(true);

    connect(_axWs.last(), SIGNAL(createTab(const QUrl&)),
            this, SLOT(createTab(const QUrl&)));
    connect(_axWs.last(), SIGNAL(titleChange(int, QString)),
            this, SLOT(titleChange(int, QString)));
    connect(_axWs.last(), SIGNAL(iconChanged(int, QIcon)),
            this, SLOT(iconChanged(int, QIcon)));
    connect(_axWs.last(), SIGNAL(comStateChange(int,bool)),
            this, SLOT(comStateChange(int, bool)));
    connect(_axWs.last(), SIGNAL(commandState(bool,bool,bool)),
            this, SLOT(setCommandState(bool,bool,bool)));

}

void TabWidget::titleChange(int index, QString title)
{
    //qDebug() << index;
    setTabText(index, title);
}

void TabWidget::iconChanged(int index, QIcon icon)
{
    setTabIcon(index, icon.pixmap(16, 16));
}

void TabWidget::removeTabWidget(int i)
{

    int index = i;
    //qDebug() << index;
    if (index < 0 || index > _axWs.count())
        return;

    QList<AxWidget*>::iterator iter = _axWs.begin() + index;
    _axWs.at(index)->setParent(NULL);
    _axWs.at(index)->deleteLater();
    _axWs.erase(iter);

//    for (int i = 0;i < _axWs.size();i++) {
//        _axWs.at(i)->setParent(NULL);
//        delete _axWs.at(i);
//    }
//    _axWs.clear();

    for(int i = 0; i < _axWs.count(); i++)
    {
       _axWs.at(i)->setId(i);
    }

    if (_axWs.count() == 1)
        this->setTabsClosable(false);

    //this->removeTab(index);
}

void TabWidget::curChanged(int index)
{
    if (!_goBack->isVisible() || !_goForward->isVisible()
            || !_refresh->isVisible())
    {
        _goBack->setVisible(true);
        _goForward->setVisible(true);
        _refresh->setVisible(true);
    }

    if (_axWs.count() == 0)
        return;

    qDebug() << index;
    _axWs.at(index)->getCommandState();
}

void TabWidget::goBack()
{
    _axWs.at(currentIndex())->goBack();
}

void TabWidget::goForward()
{
    _axWs.at(currentIndex())->goForward();
}

void TabWidget::refresh()
{
    _axWs.at(currentIndex())->refresh();
}

void TabWidget::comStateChange (int command, bool enable)
{
    if (command == -1)
    {
        _refresh->setEnabled(true);
    }
    else if (command == 1)
    {
        if (enable)
        {
            _goForward->setEnabled(true);
        }
        else
        {
            _goForward->setEnabled(false);
        }
    }
    else if (command == 2)
    {
        if (enable)
        {
            _goBack->setEnabled(true);
        }
        else
        {
            _goBack->setEnabled(false);
        }
    }
}

void TabWidget::setCommandState(bool backState, bool forwardState, bool refresh)
{
    _goBack->setEnabled(backState);
    _goForward->setEnabled(forwardState);
    _refresh->setEnabled(refresh);
}

AxWidget::AxWidget(int id, QWidget *parent)
    : QWidget(parent)
    , _id(id)
    , _backState(false)
    , _forwardState(false)
    , _refreshState(false)
{
    _ax = new QAxWidget(this);

    //_ax = new WebAxWidget(this);
    _ax->setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}"));
    _ax->setObjectName(QString::fromUtf8("WebBrowser"));
    _ax->setStyleSheet("QAxWidget{margin : 0px; border:0px solid red; padding:0px;}");

    _ax->dynamicCall("SetFullScreen(bool)", false);

#ifndef AUTOTESTS
    QTimer::singleShot(0, this, SLOT(postLaunch()));
#endif

    //�µ���ҳ
//    connect(_ax, SIGNAL(NewWindow(QString, int, QString, QVariant&, QString, bool&)), \
//            this, SLOT(newWindow(QString, int, QString, QVariant&, QString, bool&)));
    connect(_ax, SIGNAL(TitleChange(QString)), this, SLOT(titleChange(QString)));
    //connect(_ax, SIGNAL(WindowActivate()), receiver, SLOT(windowActivate()));

    connect(_ax, SIGNAL(NewWindow2(IDispatch**, bool&)), \
            this, SLOT(newWindow2(IDispatch**, bool&)));
    connect(_ax, SIGNAL(NewWindow3(IDispatch**, bool&, uint, QString,QString)),\
            this, SLOT(newWindow3(IDispatch**,bool&,uint,QString,QString)));

    connect(_ax, SIGNAL(ReadyStateChanged(tagREADYSTATE)), \
            this, SLOT(readyStateChanged(tagREADYSTATE)));
    connect(_ax, SIGNAL(DownloadComplete()), \
            this, SLOT(downloadComplete()));
    connect(_ax, SIGNAL(CommandStateChange(int, bool)),
            this,SLOT(commandStateChange(int, bool)));
    //connect(this, SIGNAL(setFullScreen(bool)),
            //_ax, SLOT(SetFullScreen(bool)));
    //connect(this, SIGNAL(setWidth(int)), _ax, SLOT(SetWidth(int)));
}

void AxWidget::commandStateChange (int command, bool enable)
{
    if (command == -1)
    {
        _refreshState = true;
    }
    else if (command == 1)
    {
        if (enable)
        {
            _forwardState = true;
        }
        else
        {
            _forwardState = false;
        }
    }
    else if (command == 2)
    {
        if (enable)
        {
            _backState = true;
        }
        else
        {
            _backState = false;
        }
    }

    emit comStateChange(command, enable);
}

void AxWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    _ax->setGeometry(-3, -3, width() + 21, height() + 21);
    //_ax->setGeometry(0, 0, width(), height());
    //emit setWidth(width());
}

void AxWidget::setUrl(const QUrl &url)
{
    _ax->dynamicCall("SetFullScreen(bool)", false);
    _ax->dynamicCall("Navigate(const QString&)", url);
    _ax->dynamicCall("SetFullScreen(bool)", false);
}

void AxWidget::newWindow3 (IDispatch** ppDisp, bool& Cancel, uint dwFlags,
                           QString bstrUrlContext, QString bstrUrl)
{
    Q_UNUSED(ppDisp);
    Q_UNUSED(Cancel);
    Q_UNUSED(dwFlags);
    Q_UNUSED(bstrUrlContext);

    qDebug() << bstrUrl;
    QUrl u(bstrUrl);
    emit createTab(u);
}

void AxWidget::newWindow2 (IDispatch** ppDisp, bool& Cancel)
{   
    Cancel = true;
    Q_UNUSED(ppDisp);
}
/*
void AxWidget::newWindow(QString URL, int Flags, QString TargetFrameName, \
                                        QVariant& PostData, QString Headers, bool& Processed)
{
    QUrl u(URL);

    emit createTab(u);
}
*/
void AxWidget::titleChange(QString title)
{
    emit titleChange(_id, title);
}

void AxWidget::downloadComplete()
{
//    qDebug() << "Finished";
//    qDebug()<<_ax->property("LocationURL").toString();
    //if (_loadFinished)
    emit iconChanged(_id, icon(_ax->property("LocationURL").toString()));

}

void AxWidget::readyStateChanged (tagREADYSTATE ReadyState)
{
//    if (_ax->dynamicCall("ReadyState()").toInt() == READYSTATE_COMPLETE)
//    {
    Q_UNUSED(ReadyState);

    emit iconChanged(_id, icon(_ax->property("LocationURL").toString()));
//    }
}

void AxWidget::setId(int id)
{
    _id = id;
}

int AxWidget::getId()
{
    return _id;
}

QIcon AxWidget::icon(const QUrl &url)
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

void AxWidget::postLaunch()
{
    QDesktopServices::StandardLocation location;
    location = QDesktopServices::CacheLocation;
    QString directory = QDesktopServices::storageLocation(location);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    QWebSettings::setIconDatabasePath(directory);
}

void AxWidget::getCommandState()
{
    emit commandState(_backState, _forwardState, _refreshState);
}

void AxWidget::goBack()
{
    _ax->dynamicCall("GoBack()");
}

void AxWidget::goForward()
{
    _ax->dynamicCall("GoForward()");
}

void AxWidget::refresh()
{
    _ax->dynamicCall("Refresh()");
}
