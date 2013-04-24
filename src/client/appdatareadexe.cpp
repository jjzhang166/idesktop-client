#include "appdatareadexe.h"
#include "../appmessagebox.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>

#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QStringList>
#include <QtScript/QScriptValueIterator>
#include <QTextCodec>
#include <QtNetwork/QNetworkCookieJar>

#include <QPainter>
#include <QFileInfo>
#include <QApplication>
#include <QProcess>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QFileInfoList>
#include <QLibrary>

#include "../localapps.h"
#include "../appmessagebox.h"

#include "appdata.h"
#include "appdatabase.h"
#include "widgetsrefreshexe.h"
#include "../client/downloader.h"
#include "appdetaildialogexe.h"
#include "appdatabase.h"
//#include "../appmessagebox.h"

#include "../config.h"
#include "../lnklist.h"

#include <stdio.h>
#include <windows.h>


#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"

AppDataReadExe* AppDataReadExe::_instance = NULL;

AppDataReadExe::AppDataReadExe()
    : _finished(false)
//    , _inProgress(false)
    , _loadError(false)
    , _loadIcon(true)
    , _cookieJar(NULL)
{
    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);


    _p = new QProcess(this);

    _manager = new QNetworkAccessManager(this);
    _categoryManager = new QNetworkAccessManager(this);
    _searchManager = new QNetworkAccessManager(this);
    _appManager = new QNetworkAccessManager(this);
    _postManager = new QNetworkAccessManager(this);
    _loginManager = new QNetworkAccessManager(this);

    _url = Config::get("Server");
    _url += "/api/package";

    QSysInfo::WinVersion ver = QSysInfo::windowsVersion();
    if (ver == QSysInfo::WV_XP)
        _winVersion = "winxp";
    else if (ver == QSysInfo::WV_2003)
        _winVersion = "win03";
    else if (ver == QSysInfo::WV_VISTA)
        _winVersion = "winvista";
    else if (ver == QSysInfo::WV_WINDOWS7)
        _winVersion = "win7";

    _loader = new Downloader;
    _iconLoader = new Downloader;

    connect(_manager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(replyFinished(QNetworkReply*)));

    connect(_categoryManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(categoryFinished(QNetworkReply*)));

    connect(_searchManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(searchFinished(QNetworkReply*)));

    connect(_appManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(appFinished(QNetworkReply*)));

    connect(_postManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(postFinished(QNetworkReply*)));

    connect(_loginManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(loginFinished(QNetworkReply*)));

    connect(_p, SIGNAL(started()),
            this, SLOT(onProcessStarted()));

    connect(_p, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));

    connect(_loader, SIGNAL(done()), this, SLOT(onDone()));
    connect(_loader, SIGNAL(loadError(QString)), this, SLOT(onLoadError(QString)));
    connect(_loader, SIGNAL(progress(qint64, qint64)), this, SLOT(onProgress(qint64, qint64)));

    connect(_iconLoader, SIGNAL(done()), this, SLOT(onIconDone()));
    connect(_iconLoader, SIGNAL(loadError(QString)), this, SLOT(onIconDone()));
}


AppDataReadExe* AppDataReadExe::Instance()
{
    if (_instance == NULL)
        _instance = new AppDataReadExe;
    return _instance;
}

void AppDataReadExe::destroy()
{
    if (_instance)
        delete _instance;
    _instance = NULL;
}

void AppDataReadExe::readSortData(const int &tabIndex,
                                  const int &start/* = 0*/, const int &count/*  = 9*/)
{


    QByteArray tmp = _url.toAscii();
    WidgetsRefreshExe::Instance()->startReadSort();
    QString url;
    //推荐
    if (tabIndex == 0) {
        url.sprintf("%s?os=%s&recommended=1&start=%d&count=%d",
                    tmp.data(), _winVersion.data(), start, count);
        _curCategory = "精品推荐";
    }
    else//普通分类
    {
        if (tabIndex > 0 && tabIndex <= _categoryList.size())
            url.sprintf("%s?os=%s&category=%d&start=%d&count=%d",tmp.data(),
                        _winVersion.data() ,_categoryList.at(tabIndex - 1)._index
                        , start, count);
        _curCategory = _categoryList.at(tabIndex - 1)._category;
    }
    _reply = _manager->get(QNetworkRequest(QUrl(url)));
}

void AppDataReadExe::replyFinished(QNetworkReply* reply)
{
    QByteArray _buffer;
    if (reply->error() == QNetworkReply::NoError) {

        AppDatabase::Instance()->removeAll();
        _buffer = reply->readAll();

        QScriptValue sc;
        QScriptEngine engine;
        sc = engine.evaluate("value = " + QString(_buffer));

        QScriptValueIterator it(sc.property("PackageList"));
        while (it.hasNext()) {
            it.next();
            if (!it.hasNext())
                break;

            AppData* data = getAppData(&it);// = new AppData;
            AppDatabase::Instance()->addData(data);
            delete data;
        }
        AppDatabase::Instance()->totalCount(sc.property("Count").toInteger());
        WidgetsRefreshExe::Instance()->refreshUI();
    }
    else {
        WidgetsRefreshExe::Instance()->uiError(convertError(reply->error()));
    }
}

AppData* AppDataReadExe::getAppData(QScriptValueIterator* it)
{
    QString category = convertCategory(it->value().property("category").toInteger());
    int recommended = it->value().property("recommended").toInteger();//1推荐

    if ((_curCategory != "精品推荐" && category != _curCategory) ||
        (_curCategory == "精品推荐" && recommended != 1))
        return NULL;

    _loadIcon = true;
    AppData* data = new AppData;

    data->_category = category;
    data->_recommended = recommended;

    data->_name = it->value().property("name").toString();
    data->_brief = it->value().property("brief").toString();

    QFileInfo info(it->value().property("icon").toString());
    QString path(Config::get("IconDir"));

    data->_strIconPath = path + "\\" + info.fileName();
    _iconLoader->download(it->value().property("icon").toString(), data->_strIconPath);

    while (_loadIcon)
        QApplication::processEvents();

    data->_intro =  it->value().property("intro").toString();//详细介绍
    data->_execname =  it->value().property("execname").toString();//快捷方式名称
    data->_lastupdate =  it->value().property("lastupdate").toInteger();//最后更新日期
    data->_updateinfo = it->value().property("updateinfo").toString();//更新提示
    data->_uninstall = it->value().property("uninstall").toString();//控制面板里显示的名称
    data->_developer = it->value().property("developer").toString();
    data->_source = it->value().property("source").toString();
    data->_version = it->value().property("version").toString();
    data->_size = it->value().property("size").toString();
    data->_rate = it->value().property("rate").toInteger();

    bool added = false;//是否已添加
    bool isCanUpdated = false;//是否有更新
    readLocalApp(data, added, isCanUpdated, false);
    data->_added = added;
    data->_isCanUpdated = isCanUpdated;

    return data;
}

void AppDataReadExe::categoryFinished(QNetworkReply* reply)
{
    QByteArray _buffer;
    if (reply->error() == QNetworkReply::NoError) {

        _categoryList.clear();
        _buffer = reply->readAll();

        QScriptValue sc;
        QScriptEngine engine;
        QString strValue = "value = " + QString(_buffer);

        sc = engine.evaluate(strValue);//QString(_buffer));

        QScriptValueIterator it(sc.property("CategoryList"));
        while (it.hasNext()) {
            it.next();
            category cate;
            cate._category =  it.value().toString();
            cate._index = it.scriptName().toString().toInt();
            _categoryList.push_back(cate);
        }
    }

    WidgetsRefreshExe::Instance()->refreshSort();
}

void AppDataReadExe::searchFinished(QNetworkReply* reply)
{
    QStringList result;
    QByteArray _buffer;
    if (reply->error() == QNetworkReply::NoError) {
        _buffer = reply->readAll();

        QScriptValue sc;
        QScriptEngine engine;
        sc = engine.evaluate("value = " + QString(_buffer));

        QScriptValueIterator it(sc.property("result"));
        while (it.hasNext()) {
            it.next();
            if (!it.hasNext())
                break;
            result.push_back(it.value().toString());
        }
    }

    WidgetsRefreshExe::Instance()->searchResult(result);
}

void AppDataReadExe::readCategory()
{
    QString url = Config::get("Server").toAscii();
    url += "api/category";
    _categoryManager->get(QNetworkRequest(url));
}

AppDataReadExe::category AppDataReadExe::getCategory(const int &index)
{
    if (index >= 0 && index < _categoryList.size())
        return _categoryList.at(index);
    AppDataReadExe::category cate;
    return cate;
}

int AppDataReadExe::size()
{
    return _categoryList.size();
}

void AppDataReadExe::searchApp(const QString &keyword)
{
    QString server = Config::get("Server");
    QString url = QString("%1api/search?wd=%2&os=%3").arg(server).arg(keyword).arg(_winVersion.data());
    _searchManager->get(QNetworkRequest(QUrl(url)));
}

void AppDataReadExe::readAppData(const QString &appName)
{
    AppDetailDialogExe::Instance()->showWidget();
    QString url = QString("%1/%2").arg(_url).arg(appName);

    _appManager->get(QNetworkRequest(QUrl(url)));
}

void AppDataReadExe::appFinished(QNetworkReply* reply)
{
    QByteArray _buffer;
    if (reply->error() == QNetworkReply::NoError) {

        AppData* appData = new AppData;
        _buffer = reply->readAll();

        QScriptValue sc;
        QScriptEngine engine;
        sc = engine.evaluate("value = " + QString(_buffer));

        QString scriptName;
        QScriptValueIterator it(sc.property("PackageInfo"));
        while (it.hasNext()) {
            it.next();

            scriptName = it.scriptName().toString();
            if (scriptName == "intro")
                appData->_intro = it.value().toString();
            else if (scriptName == "execname")
                appData->_execname = it.value().toString();
            else if (scriptName == "developer")
                appData->_developer = it.value().toString();
            else if (scriptName == "category")
                appData->_category = convertCategory(it.value().toInteger());
            else if (scriptName == "lastupdate")
                appData->_lastupdate = it.value().toInteger();
               // appData->_lastupdate = 0;
            else if (scriptName == "name")
                appData->_name = it.value().toString();
            else if (scriptName == "updateinfo")
                appData->_updateinfo = it.value().toString();
            else if (scriptName == "brief")
                appData->_brief = it.value().toString();
            else if (scriptName == "source")
                appData->_source = it.value().toString();
            else if (scriptName == "recommended")
                appData->_recommended = it.value().toInteger();
            else if (scriptName == "version")
                appData->_version = it.value().toString();
            else if (scriptName == "uninstall")
                appData->_uninstall = it.value().toString();
            else if (scriptName == "size")
                appData->_size = it.value().toString();
            else if (scriptName == "rate")
                appData->_rate = it.value().toInteger();
            else if (scriptName == "icon") {
                QFileInfo info(it.value().toString());
                QString path(Config::get("IconDir"));
                appData->_strIconPath = path + "\\" + info.fileName();
                _loader->download(it.value().toString(), appData->_strIconPath);
            }
        }
        bool bAdded = false;
        bool bCanUpdated = false;
        readLocalApp(appData, bAdded, bCanUpdated, true);
        appData->_added = bAdded;
        appData->_isCanUpdated = bCanUpdated;
        AppDetailDialogExe::Instance()->showApp(appData);
    }
    else {
        AppDetailDialogExe::Instance()->AppError(convertError(reply->error()));
    }
}

QString AppDataReadExe::convertCategory(const int &index)
{
    for (int i = 0;i < _categoryList.size();i++)
        if (_categoryList.at(i)._index == index)
            return _categoryList.at(i)._category;
    return "";
}

void AppDataReadExe::readLocalApp(AppData* data, bool &bAdded, bool &bCanUpdated, const bool &checkOS)
{
    if (data == NULL) {
       bAdded = false;
       bCanUpdated = false;
      return;
    }

    LocalApp *app = LocalAppList::getList()->getAppByName(data->_name);
    if (app == NULL){
        bAdded = false;
        bCanUpdated = false;
    }
    else {
        QString exeN, uninstallN;
        bool bInstalled = true;

        if (checkOS)
            bInstalled = isInstalled(data->_execname, data->_uninstall, exeN, uninstallN);

        if (bInstalled) {
            bAdded = true;
            if (app->date() != data->_lastupdate)
                bCanUpdated = true;
            else
                bCanUpdated = false;
        }
        else {
            bAdded = false;
            bCanUpdated = false;
        }
    }
}

QString AppDataReadExe::convertError(QNetworkReply::NetworkError error)
{
    if (error == QNetworkReply::ConnectionRefusedError)
        return "服务器拒绝连接";
    else if (error == QNetworkReply::RemoteHostClosedError)
        return "远端服务器已断开连接";
    else if (error == QNetworkReply::HostNotFoundError)
        return "未找到主机";
    else if (error == QNetworkReply::TimeoutError)
            return "网络超时，请检查网络连接";
    else if (error == QNetworkReply::UnknownNetworkError)
        return "未知的网络";
    else if (error == QNetworkReply::UnknownProxyError)
        return "未知的代理服务器";
    else if (error == QNetworkReply::UnknownContentError)
        return "读取数据失败";
    else if (error == QNetworkReply::OperationCanceledError)
        return "操作已取消";

 /*   if (error == QNetworkReply::ConnectionRefusedError)
        return "服务器拒绝连接";
    else if (error == QNetworkReply::RemoteHostClosedError)
        return "远端服务器已断开连接";
    else if (error == QNetworkReply::HostNotFoundError)
        return "未找到主机";
    else if (error == QNetworkReply::TimeoutError)
        return "网络超时，请检查网络连接";
    else if (error == QNetworkReply::OperationCanceledError)
        return "操作已取消";
    else if (error == QNetworkReply::SslHandshakeFailedError)
        return "SSL / TLS连接失败";
    else if (error == QNetworkReply::TemporaryNetworkFailureError)
        return "";
    else if (error == QNetworkReply::ProxyConnectionRefusedError)
        return "";
    else if (error == QNetworkReply::ProxyConnectionClosedError)
        return "";
    else if (error == QNetworkReply::ProxyNotFoundError)
        return "";
    else if (error == QNetworkReply::ProxyTimeoutError)
        return "";
    else if (error == QNetworkReply::ProxyAuthenticationRequiredError)
        return "";
    else if (error == QNetworkReply::ContentAccessDenied)
        return "";
    else if (error == QNetworkReply::ContentOperationNotPermittedError)
        return "";
    else if (error == QNetworkReply::ContentNotFoundError)
        return "";
    else if (error == QNetworkReply::AuthenticationRequiredError)
        return "";
    else if (error == QNetworkReply::ContentReSendError)
        return "";
    else if (error == QNetworkReply::ProtocolUnknownError)
        return "";
    else if (error == QNetworkReply::ProtocolInvalidOperationError)
        return "";
    else if (error == QNetworkReply::UnknownNetworkError)
        return "";
    else if (error == QNetworkReply::UnknownProxyError)
        return "";
    else if (error == QNetworkReply::UnknownContentError)
        return "";
    else if (error == QNetworkReply::ProtocolFailure)
        return "";*/
    return "读取数据失败， 请检查网络连接";
}

void AppDataReadExe::installApp(AppData* appData)
{
    if (appData == NULL)
        return;

    AppData* data = new AppData(*appData);

    //先检查是否已安装
    QString execn;
    QString uninstExec;
    bool bInstalled = isInstalled(data->_execname, data->_uninstall, execn, uninstExec);

    if (!bInstalled) {
         /////////////////////////////////////
         STARTUPINFO si; //一些必备参数设置
         memset(&si, 0, sizeof(STARTUPINFO));
         si.cb = sizeof(STARTUPINFO);
         si.dwFlags = STARTF_USESHOWWINDOW;
         si.wShowWindow = SW_SHOW;
         PROCESS_INFORMATION pi; //必备参数设置结束

         _installCanceled = false;
         _finished = false;
         _loadError = false;
         QFileInfo info(data->_source);
         QString path(Config::get("InstDir"));

         _loader->download(data->_source, path + "\\" + info.fileName());
         while (!_finished)
               QApplication::processEvents();

         if (_loadError) {
                AppDetailDialogExe::Instance()->loadFailed();
                delete data;
                return;
          }
          _finished = false;

         if (_installCanceled) {
                delete data;
                return;
         }
          AppDetailDialogExe::Instance()->installDownloaded();

          QString strPath = path + "\\" + info.fileName();
          QByteArray tmp = strPath.toAscii();

         // if (app == NULL)

          if (!CreateProcess(tmp.data(),NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) {
         //     app->setExecname("");
              execn = "";
            //  return;
          }
          else {
              DWORD dwExitCode = STILL_ACTIVE;
              while(dwExitCode == STILL_ACTIVE) {
                  GetExitCodeProcess(pi.hProcess, &dwExitCode);
                  QApplication::processEvents();
              }

              bInstalled = isInstalled(data->_execname, data->_uninstall, execn, uninstExec);
              if (!bInstalled)
                  execn = "";
                //  app->setExecname(execn);
           //   else
           //       app->setExecname("");
         }
         LocalApp* app =  addLocalApp(data, execn);
    }
    else {
        LocalApp* app = addLocalApp(data, execn);
      //  app->setExecname(execn);
    }
    AppDetailDialogExe::Instance()->installFinished();
 //   data->_added = true;
    delete data;
}

LocalApp* AppDataReadExe::addLocalApp(AppData* data, const QString &execName/* = ""*/)
{
    if (data == NULL)
        return NULL;

    LocalApp* app = LocalAppList::getList()->getAppByName(data->_name);
    if (app)
        return app;
    app = new LocalApp();
    app->setName(data->_name);
    app->setIcon(data->_strIconPath);
    app->setVersion(data->_version);
    if (execName.isEmpty())
        app->setExecname(data->_execname);
    else
        app->setExecname(execName);

    app->setUninstName(data->_uninstall);
    app->setDate(data->_lastupdate);
    LocalAppList::getList()->addApp(app);

    return app;
}

QString AppDataReadExe::getUninstExec(QString display)
{
    QSettings reg(QSettings::NativeFormat, \
        QSettings::SystemScope, "Microsoft", KEY);
    for (int i = 0; i < reg.childGroups().count(); i++) {
        QSettings tmp(QSettings::NativeFormat, \
            QSettings::SystemScope, "Microsoft", \
            KEY + reg.childGroups().at(i));
        if (tmp.value("DisplayName").toString() == display) {
            return tmp.value("UninstallString").toString();
        }
    }
    return QString("");
}

QString AppDataReadExe::getExec(QString execName)
{
    QString exec;
    execName += ".lnk";

    bool bStartMenu = false;
    bStartMenu = findFile(Config::get("UserProgramDir"), execName,exec);

    if (!bStartMenu)
        bStartMenu = findFile(Config::get("DesktopDir"), execName,exec);
    if (!bStartMenu)
        bStartMenu = findFile(Config::get("CommonProgramDir"), execName,exec);
    if (!bStartMenu)
        bStartMenu = findFile(Config::get("CommonDesktopDir"), execName,exec);
    return exec;
}


void AppDataReadExe::onDone()
{
    _finished = true;
}

void AppDataReadExe::onProgress(qint64 recieve, qint64 total)
{
    AppDetailDialogExe::Instance()->Progress(recieve, total);
}

void AppDataReadExe::onProcessFinished(int, QProcess::ExitStatus)
{
 //   _inProgress = false;
}

void AppDataReadExe::onProcessStarted()
{
//    backButton->setEnabled(false);
}

void AppDataReadExe::InstallCanceled()
{
    _finished = true;
    _installCanceled = true;
}

bool AppDataReadExe::findFile(QString path, QString fileName, QString &lnkPath)
{
    QDir dir(path);
    if (!dir.exists())
        return false;

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files |QDir::NoDotAndDotDot,
                                           QDir::DirsLast);

    for (int i = 0;i < list.size();i++) {
        QFileInfo fileinfo = list.at(i);

        bool bisDir = fileinfo.isDir();
        if (bisDir && !fileinfo.filePath().endsWith(".lnk")) {
            if (findFile(fileinfo.filePath(), fileName, lnkPath))
                return true;
        }
        else {
            QString name = fileinfo.fileName();
            if (name == fileName) {
             //   lnkPath = "\"";
                lnkPath = fileinfo.filePath();
             //   lnkPath += "\"";
                return true;
            }
        }
    }
    return false;
}

bool AppDataReadExe::isInstalled(QString execName, QString uninstallName,
                                 QString &exec, QString &uninstall)
{
    uninstall = getUninstExec(uninstallName);
    exec = getExec(execName);

    if (!exec.isEmpty() && !uninstall.isEmpty())
        return true;

    return false;
}

void AppDataReadExe::onLoadError(QString error)
{
    _finished = true;
    _loadError = true;
}

void AppDataReadExe::onIconDone()
{
    _loadIcon = false;
}

void AppDataReadExe::postFinished(QNetworkReply* reply)
{
    QByteArray _buffer;
    if (reply->error() == QNetworkReply::NoError) {

        _buffer = reply->readAll();

        int n = _buffer.indexOf('\n');
        if (n != -1) {

            QByteArray name = _buffer.mid(0,n);
            QByteArray sc = _buffer.mid(n + 1, _buffer.length() - n - 1);

            WidgetsRefreshExe::Instance()->updateScore(QString::fromUtf8(name.data()),sc.toInt());

        }
    }
    else {
        QString str = convertError(reply->error());
    }
}

void AppDataReadExe::postData(QString appName, QByteArray data)
{
    QString url = QString("%1/%2").arg(_url).arg(appName);

    _postManager->setCookieJar(_cookieJar);

    _postManager->post(QNetworkRequest(QUrl(url)),data);
}

void AppDataReadExe::loginFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
       _cookieJar = _loginManager->cookieJar();
}

void AppDataReadExe::login()
{
    //http://192.168.33.13/api/login?name=xxxx&password=xxxx
    QString str = QString("%1api/login?name=%2&password=%3").arg(Config::get("Server")).arg(Config::get("User")).arg(Config::get("password"));

    _loginManager->get(QNetworkRequest(QUrl(str)));
}

QString AppDataReadExe::getAppImage(QString appPath)
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

void AppDataReadExe::addLocalApp(QString appPath)
{
    QString newApp = getAppImage(appPath);

    if (newApp.isEmpty())
        return;

    QImage image = QImage(newApp).scaled(48, 48);
    QImage normal = QImage(":images/app_bg.png");
    setImgOpacity(normal, 0);

    QPainter pt1(&normal);
    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
    pt1.drawImage(17, 8, image);
    pt1.end();
    QPixmap pix = QPixmap::fromImage(normal);
    pix.save(newApp, "PNG", -1);

    QFileInfo info = QFileInfo(appPath);
    LocalApp *app = new LocalApp();
    app->setName("/" + info.baseName());
    app->setIcon(newApp);
    app->setExecname(appPath);
    if (LocalAppList::getList()->getAppByName(app->name())) {
        QApplication::processEvents();
        AppMessageBox box(false, NULL);
        box.setText("    已添加该图标");
        box.exec();
    } else {
        LocalAppList::getList()->addApp(app);
    }
}

void AppDataReadExe::setImgOpacity(QImage &img, const int &alpha)
{
    for (int i = 0; i < img.width(); i++) {
        for (int j = 0; j < img.height(); j++) {
            QRgb pixel = img.pixel(i,j);
            int a = qAlpha(pixel);
            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                    qBlue(pixel), a * alpha / 255);
            img.setPixel(i, j, lightPixel);
        }
    }
}
