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
#include "config.h"

extern QString serverip;

MyWebView::MyWebView(QWidget *parent)
    : QWebView(parent)
{

}

void MyWebView::setNewUrl(const QUrl &url)
{
    emit hideMenu();
    emit linkClicked(url);
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
    _leftWebKit->setGeometry(0, 0, _width * 0.14 + 11, _height);

    _rightAxWidget = new RightAxWidget(this);
    _rightAxWidget->setGeometry(_width * 0.14 + 11, 20, _width - _width * 0.14 - 11, _height);\
    _rightAxWidget->setVisible(true);

    connect(_leftWebKit, SIGNAL(startAssembly()), this, SLOT(startAssembly()));
    connect(_leftWebKit, SIGNAL(stopAssembly()), this, SLOT(stopAssembly()));
    connect(_leftWebKit, SIGNAL(link(const QUrl&)), _rightAxWidget, SLOT(createTab(const QUrl&)));
    connect(_leftWebKit, SIGNAL(hideMenu()), this, SIGNAL(hideMenu()));
}

BsWidget::~BsWidget()
{
}

void BsWidget::startAssembly()
{
    _leftWebKit->setGeometry(0, 0, _width, _height);
    _rightAxWidget->setVisible(false);
}

void BsWidget::stopAssembly()
{
    _leftWebKit->setGeometry(0, 0, _width * 0.14 + 10, _height);
    _rightAxWidget->setVisible(true);
}

LeftWebKit::LeftWebKit(QWidget *parent) :
    QMainWindow(parent)
{
    //resize(QApplication::desktop()->width(),QApplication::desktop()->height());
    setWindowFlags(Qt::FramelessWindowHint \
                   | Qt::WindowMinimizeButtonHint \
                   | Qt::WindowSystemMenuHint);

    //_webview = new QWebView(this);
    _webview = new MyWebView(this);
    //_webview->setGeometry(0, 0, width(), height());
    _webpage = new WebPage();
    _webview->setPage(_webpage);

    _toolsMenu = addToolBar(tr("&Tools"));
    _toolsMenu->addAction(_webview->pageAction(QWebPage::Back));
    _toolsMenu->addAction(_webview->pageAction(QWebPage::Forward));
    _toolsMenu->addAction(_webview->pageAction(QWebPage::Reload));
    _toolsMenu->addAction(_webview->pageAction(QWebPage::Stop));

    QWebSettings *websetting= QWebSettings::globalSettings();
    websetting->setAttribute(QWebSettings::JavascriptEnabled,true);    
    // Plug-ins must be set to be enabled to use plug-ins.
    websetting->setAttribute(QWebSettings::PluginsEnabled,true);
    websetting->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
    websetting->setObjectCacheCapacities(0, 0, 0);

    //_webview->load(QUrl("http://"+serverip+"/point.html"));

    _webview->load(QUrl("http://www.hao123.com"));
    //_webview->load(QUrl(Config::get("SystemManage")));
    _webview->page()->mainFrame()->addToJavaScriptWindowObject(QString("mywebkit"),_webview);

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
    //_webview->setGeometry(0, 0, width(), height());
    //QMainWindow::resizeEvent(event);
}

RightAxWidget::RightAxWidget(QWidget *parent)
    : QTabWidget(parent)
{
//    setWindowFlags(Qt::FramelessWindowHint \
//                   | Qt::WindowMinimizeButtonHint \
//                   | Qt::WindowSystemMenuHint);

    this->setTabsClosable(false);

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTabWidget(int)));
}
/*
void RightAxWidget::resizeEvent(QResizeEvent *event)
{
    _axWidget->resize(width(), height());
}
*/
/*
void RightAxWidget::setUrl(const QUrl &url)
{
    qDebug() << url;
    _axWs.last()->setUrl(url);
}
*/
void RightAxWidget::createTab(const QUrl &url)
{
    _axWidget = new AxWidget(_axWs.count(), this);
    addTab(_axWidget, "");
    _axWidget->resize(width(), height());
    _axWidget->setUrl(url);

    _axWs.append(_axWidget);

    setCurrentIndex(_axWs.count() - 1);

    if(_axWs.count() > 1)
        this->setTabsClosable(true);

    connect(_axWs.last(), SIGNAL(createTab(const QUrl&)),\
            this, SLOT(createTab(const QUrl&)));
    connect(_axWs.last(), SIGNAL(titleChange(int, QString)),\
            this, SLOT(titleChange(int, QString)));

}

void RightAxWidget::titleChange(int index, QString title)
{
    qDebug() << index;
    setTabText(index, title);
}

void RightAxWidget::removeTabWidget(int i)
{

    int index = i;
    qDebug() << index;
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

AxWidget::AxWidget(int id, QWidget *parent)
    : QWidget(parent)
    , _id(id)
{
    _ax = new QAxWidget(this);

    //_ax = new WebAxWidget(this);
    //_ax->setStyleSheet("QAxWidget{border: 0px solid gray;background:rgba(255,255,255,0);}");
    _ax->setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}"));
    _ax->setObjectName(QString::fromUtf8("_ax"));

    //ÐÂµ¯³öÒ³
    connect(_ax, SIGNAL(NewWindow(QString, int, QString, QVariant&, QString, bool&)), \
            this, SLOT(newWindow(QString, int, QString, QVariant&, QString, bool&)));
    connect(_ax, SIGNAL(TitleChange(QString)), this, SLOT(titleChange(QString)));
    //connect(_ax, SIGNAL(WindowActivate()), receiver, SLOT(windowActivate()));

}

void AxWidget::resizeEvent(QResizeEvent *event)
{
    _ax->resize(width(), height());
}

void AxWidget::setUrl(const QUrl &url)
{
    _ax->dynamicCall("Navigate(const QString&)", url);
   //_ax->dynamicCall( "get_Document(QString &)",url );
}

void AxWidget::newWindow(QString URL, int Flags, QString TargetFrameName, \
                                        QVariant& PostData, QString Headers, bool& Processed)
{
    qDebug() << "New";
    qDebug() << URL;
    qDebug() << Flags;
    qDebug() << TargetFrameName;
    qDebug() << PostData;
    qDebug() << Headers;
    qDebug() << Processed;
    QUrl u(URL);

    emit createTab(u);
}

void AxWidget::titleChange(QString title)
{
    emit titleChange(_id, title);
}

void AxWidget::setId(int id)
{
    _id = id;
}

int AxWidget::getId()
{
    return _id;
}
