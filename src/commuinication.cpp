#include "commuinication.h"
#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <QTime>

_Commui Commui;
commuinication::commuinication(QObject *parent):QObject(parent),
    _reply(NULL),_isPost(true),_type(INIT),errID(""),errInfo(""),_isNetError(false)
{
    _nam = new QNetworkAccessManager(this);
    connect(_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    timeOut = new QTimer(this);
    connect(timeOut, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
    qEvent=new QEventLoop();
    _reply=NULL;
    isBalance = 0;
}

commuinication::~commuinication()
{
    _buffer.clear();
    delete _nam;
    delete timeOut;
    delete qEvent;
}

//core post fuction;
void commuinication::myPost(const QUrl url, const QByteArray postData)
{
    _buffer.clear();
    if (!url.isValid())
    {
        return;
    }

    _isPost = true;
    QNetworkRequest* request=new QNetworkRequest(url);
    _reply = _nam->post(*request,postData);

    qDebug() << "post slotError";
    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

    timeOut->start(5000);
    qEvent->exec();
    delete request;
    return;
}

//core get fuction;
void commuinication::myGet(const QUrl url, const QString path)
{
    if(!url.isValid())
    {
        return;
    }

    _isPost = false;
    _dest = path;
    QNetworkRequest* request=new QNetworkRequest(url);
    _reply = _nam->get(*request);
    qDebug() << "get : slotError";
    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));

    timeOut->start(5000);
    qEvent->exec();
    delete request;
    return;
}

void commuinication::replyFinished(QNetworkReply*) /* download finished */
{
    errInfo.clear();
    _appList.clear();

    qDebug()<<"replyFinished:"<<_reply->error()<<endl;
    if (_reply->error() != QNetworkReply::NoError)
    {
        errInfo.append(tr("Network error, please check!"));
        _isNetError = true;
        emit done();
        return;
    }

    if(_buffer.isEmpty())
    {
        errInfo.append(tr("Network error, please check!"));
        _isNetError = true;
        emit done();
        return;
    }

    if(!_isPost)
    {
        QFile file(_dest);
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "open file failed when writing";
        }
        //if get function , do the readAll in here;
        _buffer = _reply->readAll();
        //
        file.write(_buffer);
        file.close();
	qEvent->exit();
	timeOut->stop();
        emit done();
        _isNetError = false;
        return;
    }


    QDomDocument doc;
    doc.setContent(_buffer);

    //Get error code;

    QDomNodeList nodelist = doc.elementsByTagName("message");
    errID = nodelist.item(0).toElement().attribute("code");
    qDebug()<<"commuinication::onFinished 3"<<errID<<"\n";

    //Get error Info;
    nodelist = doc.elementsByTagName("message");
    errInfo.append(nodelist.item(0).toElement().attribute("info"));
    qDebug()<<"commuinication::onFinished 3"<<errInfo<<"\n";



    int index = 0;
    APP_LIST tempAppList;

    switch(_type)
    {
    case LOGIN:
        nodelist = doc.elementsByTagName("user");
        Commui._userType = nodelist.item(index).toElement().attribute("userType");
        if(Commui._userType == "1")
        {
            Commui._domain = nodelist.item(index).toElement().attribute("domain");
        }
        qDebug()<<"userType:"<<Commui._userType;
        qDebug()<<_ip<<"ip";

        break;

    case GETAPP:
        nodelist = doc.elementsByTagName("application");
        qDebug()<<"nodelist.count"<<nodelist.count()<<"\n";
        while (index < nodelist.count())
        {
            tempAppList.name = nodelist.item(index).toElement().attribute("name");
            tempAppList.id = nodelist.item(index).toElement().attribute("appId");
            Commui._id=nodelist.item(index).toElement().attribute("appId");

            tempAppList.icon = nodelist.item(index).toElement().attribute("iconPath");
            tempAppList.type = nodelist.item(index).toElement().attribute("type");

            tempAppList.page = -1;
            tempAppList.index = -1;
            tempAppList.hidden = false;

            _appList.append(tempAppList);
//            qDebug() <<"end append list\n"<<endl;
//            qDebug()<<_appList[index].name<<"\n";

            index++;
        }

        break;
    case LDBALANCE:

        nodelist = doc.elementsByTagName("server");
        Commui._rdpIpPort= nodelist.item(0).toElement().attribute("svrIp");
        //qDebug()<<svrIp<<"\n";
        qDebug()<<"ip:"<<Commui._rdpIpPort;
        //QString rdpPort= nodelist.item(0).toElement().attribute("rdpPort");
        //qDebug()<< rdpPort<<"\n";
        Commui._viewLimited_carryWallpaper=nodelist.item(0).toElement().attribute("viewLimited_carryWallpaper");
        Commui._viewLimited_dragWithWindowContent=nodelist.item(0).toElement().attribute("viewLimited_dragWithWindowContent");
        Commui._viewLimited_resolution_width=nodelist.item(0).toElement().attribute("viewLimited_resolution_width");
        Commui._viewLimited_resolution_height=nodelist.item(0).toElement().attribute("viewLimited_resolution_height");
        Commui._viewLimited_colorDepth=nodelist.item(0).toElement().attribute("viewLimited_colorDepth");
        Commui._viewLimited_advanced_windowWidth=nodelist.item(0).toElement().attribute("viewLimited_advanced_windowWidth");
        Commui._viewLimited_advanced_windowHeight=nodelist.item(0).toElement().attribute("viewLimited_advanced_windowHeight");
        Commui._viewLimited_advanced_dragEffect=nodelist.item(0).toElement().attribute("viewLimited_advanced_dragEffect");
        Commui._viewLimited_advanced_activeEffect=nodelist.item(0).toElement().attribute("viewLimited_advanced_activeEffect");
        Commui._viewLimited_advanced_fontSmooth=nodelist.item(0).toElement().attribute("viewLimited_advanced_fontSmooth");
        Commui._bandwidthLimited_audio=nodelist.item(0).toElement().attribute("bandwidthLimited_audio");
        Commui._bandwidthLimited_clipboard=nodelist.item(0).toElement().attribute("bandwidthLimited_clipboard");
        Commui._bandwidthLimited_COM=nodelist.item(0).toElement().attribute("bandwidthLimited_COM");
        Commui._bandwidthLimited_driver=nodelist.item(0).toElement().attribute("bandwidthLimited_driver");
        Commui._bandwidthLimited_LPT=nodelist.item(0).toElement().attribute("bandwidthLimited_LPT");
        Commui._bandwidthLimited_session=nodelist.item(0).toElement().attribute("bandwidthLimited_session");
        Commui._bandwidthLimited_printer=nodelist.item(0).toElement().attribute("bandwidthLimited_printer");
        Commui._bandwidthLimited_TWAIN=nodelist.item(0).toElement().attribute("bandwidthLimited_TWAIN");
        Commui._audio_micInput=nodelist.item(0).toElement().attribute("audio_micInput");
        Commui._audio_clientAudioOutput=nodelist.item(0).toElement().attribute("audio_clientAudioOutput");
        Commui._audio_quality=nodelist.item(0).toElement().attribute("audio_quality");
        Commui._driver_loadClientDriverOnLogin=nodelist.item(0).toElement().attribute("driver_loadClientDriverOnLogin");
        Commui._driver_mapClientStorageOnLogin_check_floppy=nodelist.item(0).toElement().attribute("driver_mapClientStorageOnLogin_check_floppy");
        Commui._driver_mapClientStorageOnLogin_check_harddisk=nodelist.item(0).toElement().attribute("driver_mapClientStorageOnLogin_check_harddisk");
        Commui._driver_mapClientStorageOnLogin_check_CD=nodelist.item(0).toElement().attribute("driver_mapClientStorageOnLogin_check_CD");
        Commui._driver_mapClientStorageOnLogin_check_shareStorage=nodelist.item(0).toElement().attribute("driver_mapClientStorageOnLogin_check_shareStorage");
        Commui._interface_COM=nodelist.item(0).toElement().attribute("interface_COM");
        Commui._interface_LPT=nodelist.item(0).toElement().attribute("interface_LPT");
        Commui._other_TWAINRedirect_compress_value=nodelist.item(0).toElement().attribute("other_TWAINRedirect_compress_value");
        Commui._other_closeClipboard=nodelist.item(0).toElement().attribute("other_closeClipboard");
        Commui._printer_supportServerPrint=nodelist.item(0).toElement().attribute("printer_supportServerPrint");
        Commui._printer_supportVirtualPrint=nodelist.item(0).toElement().attribute("printer_supportVirtualPrint");
        Commui._redirect_contentRedirect=nodelist.item(0).toElement().attribute("redirect_contentRedirect");
        Commui._redirect_applicationRedirect=nodelist.item(0).toElement().attribute("redirect_applicationRedirect");
        Commui._redirect_inputMethodRedirect=nodelist.item(0).toElement().attribute("inputMethod_localInputMethod");
        Commui._onceLogin_loginInfoStore=nodelist.item(0).toElement().attribute("onceLogin_loginInfoStore");
        Commui._onceLogin_rememberPassword=nodelist.item(0).toElement().attribute("onceLogin_rememberPassword");
        Commui._encryptLevel_encryptLevel=nodelist.item(0).toElement().attribute("encryptLevel_encryptLevel");
        Commui._userSpatialIsolation=nodelist.item(0).toElement().attribute("userSpatialIsolation");
        Commui._userPriorityLevel=nodelist.item(0).toElement().attribute("userPriorityLevel");
        Commui._appIcoColorDeep=nodelist.item(0).toElement().attribute("appIcoColorDeep");
        Commui._picCompress=nodelist.item(0).toElement().attribute("picCompress");
        Commui._appRedirect=nodelist.item(0).toElement().attribute("appRedirect");
        Commui._contentRedirect=nodelist.item(0).toElement().attribute("contentRedirect");
        Commui._DesktopHeight=nodelist.item(0).toElement().attribute("viewLimited_resolution_height");
        Commui._DesktopWidth=nodelist.item(0).toElement().attribute("viewLimited_resolution_width");
        Commui._FontSmooth=nodelist.item(0).toElement().attribute("viewLimited_advanced_fontSmooth");
        Commui._RdpPort   =nodelist.item(0).toElement().attribute("rdpPort");
        Commui._path      =nodelist.item(0).toElement().attribute("path");
        Commui._param = nodelist.item(0).toElement().attribute("param")+" "+nodelist.item(0).toElement().attribute("extraParam");

        Commui._applicationForbid_cmd = nodelist.item(0).toElement().attribute("applicationForbid_applicationForbid_cmd");
        Commui._applicationForbid_explorer = nodelist.item(0).toElement().attribute("applicationForbid_applicationForbid_explorer");
        Commui._applicationForbid_SysManage = nodelist.item(0).toElement().attribute("applicationForbid_applicationForbid_SysManage");
        Commui._applicationForbid_help = nodelist.item(0).toElement().attribute("applicationForbid_applicationForbid_help");

        if(isBalance == 1)
            emit appRun();
        isBalance = 0;
        break;

    case LOGOFF:
    case CHPWD:
    case HEARTBEAT:
    default:
        break;
    }
    timeOut->stop();
    qEvent->exit();
    _isNetError = false;
    emit done();
}

void commuinication::slotError(QNetworkReply::NetworkError)
{
    qDebug()<<"post slotError:"<<_reply->error()<<endl;
    timeOut->stop();
    qEvent->exit();
}


void commuinication::handleTimeOut()
{

    QTime Time = QTime::currentTime();
    qDebug()<<"TimeOut"<<Time.minute()<<Time.second()<<":"<<Time.msec()<<endl;

    timeOut->stop();
    qEvent->exit();

    errInfo.clear();
    errInfo.append(tr("Network error, please check!"));

    _isNetError = true;
    emit done();
}

void commuinication::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal == -1)
        return;
    if (bytesReceived < bytesTotal)
        return;
    _buffer = _reply->readAll();

    //qDebug()<<"downloadProgress::_buffer"<<_buffer<<endl;
}

void commuinication::postfinished()
{
    //if(isBalance == 1)
    //    emit appRun();
    //isBalance = 0;
}

void commuinication::close()
{
    _buffer.clear();
    delete _nam;
    delete timeOut;
    delete qEvent;
}

//===========================================================//
//=========public fuction====================================//
//===========================================================//

void commuinication::login(const QString ip, const QString name, const QString pwd, const QString sysInfo)
{
    if(ip.isEmpty() || name.isEmpty() || pwd.isEmpty())
    {
        return;
    }
    Commui._name=name;

    //----post session----
    QString postStr = QString("<?xml version='1.0' encoding='utf-8'?><document>"\
                              "<user loginName='%1' password='%2' sysInfo='%3' /></document>")
            .arg(QUrl::toPercentEncoding(name).constData()).arg(QUrl::toPercentEncoding(pwd).constData()).arg(sysInfo);

    QByteArray inputStr = postStr.toAscii();
    qDebug()<<"postStr:"<<inputStr<<"\n";

    QUrl url("http://" + ip + "/webservices/api.php?m=2&a=9");


    Commui._ipPort = ip;
    Commui._pswd = pwd;

    qDebug()<<"inputStr:"<<url.toString()<<"\n";
    _url = url;
    _type = LOGIN;
    qDebug()<<"login";
    myPost(url, inputStr);

    return;
}

void commuinication::logoff()
{
    //----post session----
    QString name = Commui._name;
    QString postStr = QString("<?xml version='1.0' encoding='utf-8'?><document><user loginName='%1' /></document>")
            .arg(QUrl::toPercentEncoding(name).constData());

    QByteArray inputStr = postStr.toAscii();
    qDebug()<<"postStr:"<<inputStr<<"\n";

    QUrl url("http://" + Commui._ipPort + "/webservices/api.php?m=2&a=10");
    qDebug()<<"url:"<<url.toString()<<"\n";

    _type = LOGOFF;
    qDebug()<<"logoff";
    myPost(url,inputStr);

    return;
}

void commuinication::getAppList()
{
    _appList.clear();
    //----post session----
    QString name = Commui._name;
    QString postStr = QString("<?xml version='1.0' encoding='utf-8'?><document><user loginName='%1' /></document>")
            .arg(QUrl::toPercentEncoding(name).constData());

    QByteArray inputStr = postStr.toAscii();
    qDebug()<<"inputStr:"<<inputStr<<"\n";

    QUrl url("http://" + Commui._ipPort + "/webservices/api.php?m=3&a=7");
    qDebug()<<"url:"<<url.toString()<<"\n";

    _type = GETAPP;
    qDebug()<<"get applist";
    myPost(url,inputStr);

    return;
}

void commuinication::changePwd(const QString strCurPwd,
                               const QString strNewPwd,
                               const QString strConfPwd)
{
    //----post session----
    QString name = Commui._name;
    QString postStr = QString("<?xml version='1.0' encoding='utf-8'?><document>"\
                              "<user loginName='%1' password='%2' repeat='%3' oldPassword='%4'/></document>")
                              .arg(QUrl::toPercentEncoding(name).constData())
                              .arg(QUrl::toPercentEncoding(strNewPwd).constData())
                              .arg(QUrl::toPercentEncoding(strConfPwd).constData())
                              .arg(QUrl::toPercentEncoding(strCurPwd).constData());

    QByteArray inputStr = postStr.toAscii();
    qDebug()<<"postStr:"<<inputStr<<"\n";

    QUrl url("http://" + Commui._ipPort + "/webservices/api.php?m=2&a=11");
    qDebug()<<"url:"<<url.toString()<<"\n";

    _type = CHPWD;
    qDebug()<<"changpwd";
    myPost(url,inputStr);

    return;
}

void commuinication::loadBalance(QString name,QString appId)
{
    QString postStr = QString("<?xml version='1.0' encoding='utf-8'?><document><user loginName='%1' appId='%2'/></document>")
            .arg(QUrl::toPercentEncoding(name).constData()).arg(appId);
    isBalance = 1;
    QByteArray inputStr = postStr.toAscii();
    qDebug()<<"postStr:"<<inputStr<<"\n";

    QUrl url("http://" + Commui._ipPort + "/webservices/api.php?m=1&a=10");
    qDebug()<<"url:"<<url.toString()<<"\n";

    _type = LDBALANCE;
    qDebug()<<"loadbanlance";
    myPost(url,inputStr);

    return;
}


void commuinication::heartBeat()
{

    //----post session----
    QString name = Commui._name;
    QString postStr = QString("<?xml version='1.0' encoding='utf-8'?><document><user loginName='%1' /></document>")
            .arg(QUrl::toPercentEncoding(name).constData());

    QByteArray inputStr = postStr.toAscii();
   // qDebug()<<"postStr:"<<inputStr<<"\n";

    QUrl url("http://" + Commui._ipPort  + "/webservices/api.php?m=4&a=5");
   // qDebug()<<"url:"<<url.toString()<<"\n";

    _type = HEARTBEAT;
    qDebug()<<"heartbeat";
    myPost(url,inputStr);
    return;
}
