#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QDesktopWidget>
#include <QWebSettings>
#include <QFileInfo>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTextCodec>
#include <QDesktopServices>
#include <QtDebug>

#include "managewidget.h"
#include "config.h"

extern QString serverip;

ManageWidget::ManageWidget(QWidget *parent) :
    QMainWindow(parent)
{

    resize(QApplication::desktop()->width(),QApplication::desktop()->height());
    setWindowFlags(Qt::FramelessWindowHint \
                   | Qt::WindowMinimizeButtonHint \
                   | Qt::WindowSystemMenuHint);


    _webview = new QWebView(this);
    _webpage = new WebPage();
    _webview->setPage(_webpage);

    //_webview->setMouseTracking(true);

    //_toolsMenu = addToolBar(tr("&Tools"));
    //_toolsMenu->addAction(_webview->pageAction(QWebPage::Back));
    //_toolsMenu->addAction(_webview->pageAction(QWebPage::Forward));
    //_toolsMenu->addAction(_webview->pageAction(QWebPage::Reload));
    //_toolsMenu->addAction(_webview->pageAction(QWebPage::Stop));

    QWebSettings *websetting= QWebSettings::globalSettings();
    websetting->setAttribute(QWebSettings::JavascriptEnabled,true);
    // Plug-ins must be set to be enabled to use plug-ins.
    websetting->setAttribute(QWebSettings::PluginsEnabled,true);
    websetting->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
    websetting->setObjectCacheCapacities(0, 0, 0);

    //_webview->load(QUrl("http://192.168.49.252/office.html"));
/*
    QFileInfo fi(Config::get("SysManage"));
    if (!fi.exists())
        _webview->load(QUrl(Config::get("SystemManage")));
    else
        _webview->load(QUrl(Config::get("SysManage")));
*/

    _webview->load(QUrl(Config::get("SystemManage")));

    setCentralWidget(_webview);

    _webview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    connect(_webview, SIGNAL(linkClicked(const QUrl&)), this, SLOT(link(const QUrl&)));
}

ManageWidget::~ManageWidget()
{

}

void ManageWidget::link(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}


void ManageWidget::upLoad()
{
    QString vUrl="#";
    QString sUrl="#";
    QString htmlText;

    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT virtualurl,serviceurl FROM urls where id=\'%1\';").arg(1));
    while (query.next()) {
        vUrl = QString(query.value(0).toString());
        sUrl = QString(query.value(1).toString());
    }

    qDebug() << vUrl;
    qDebug() << sUrl;

    QFile fr(Config::get("SystemManage"));
    fr.open(QIODevice::ReadOnly);
    QTextStream tsr(&fr);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    tsr.setCodec(codec);

    while(!tsr.atEnd()) {
        QString str(tsr.readLine());

        if(str.contains("name=\"vurl\"", Qt::CaseInsensitive)){
            str.replace(QRegExp("href=\"\.\*\""),QString("href=\"%1\"").arg(vUrl));
        }
        else if(str.contains("name=\"surl\"", Qt::CaseInsensitive)) {
            str.replace(QRegExp("href=\"\.\*\""),QString("href=\"%1\"").arg(sUrl));
        }
        else if(str.contains("id=\"content\"", Qt::CaseInsensitive)) {
            str.replace(QRegExp("src=\"\.\*\""),QString("src=\"%1\"").arg(vUrl));
        }

        htmlText += str;
        htmlText += "\n";
    }
    fr.close();

    QFile fw(Config::get("SystemManage"));
    fw.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream tsw(&fw);
    tsw.setCodec(codec);

    tsw << htmlText << endl;

    fw.close();

    _webview->load(QUrl(Config::get("SystemManage")));
}
