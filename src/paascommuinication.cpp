#include <QtDebug>
#include <QFile>
#include <QDomDocument>
#include <QTime>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include <QTextCodec>

#include <QVariant>
#include <QJSon/qjson.h>
#include <QJSon/serializer.h>
#include <QMessageBox>

#include "paascommuinication.h"
#include "PaasCommuinication.h"

#include "appmessagebox.h"

PaasCommuinication::PaasCommuinication(QObject *parent)
    : QObject(parent)
    , _reply(NULL)
    , _isPost(true)
    , errID("")
    , errInfo("")
    , _isNetError(false)
    , _isNetTimeout(false)
{
    _nam = new QNetworkAccessManager(this);
    connect(_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    timeOut = new QTimer(this);
    connect(timeOut, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
    qEvent=new QEventLoop();
    _reply=NULL;
    isBalance = 0;
}

PaasCommuinication::~PaasCommuinication()
{
    _buffer.clear();
    delete _nam;
    delete timeOut;
    delete qEvent;
}

//core post fuction;
void PaasCommuinication::myPost(const QUrl url, const QByteArray postData)
{
    _buffer.clear();
    if (!url.isValid())
    {
        return;
    }

    _isPost = true;
    QNetworkRequest* request=new QNetworkRequest(url);
    if (postData.isEmpty())
        _reply = _nam->get(*request);
    else
        _reply = _nam->post(*request,postData);


    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

    timeOut->start(5 * 1000);   //5000
    _isNetTimeout = false;
    qEvent->exec();
    delete request;
    return;
}

//core get fuction;
void PaasCommuinication::myGet(const QUrl url, const QString path)
{
    if(!url.isValid())
    {
        return;
    }

    _isPost = false;
    _dest = path;
    QNetworkRequest* request=new QNetworkRequest(url);
    _reply = _nam->get(*request);

    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));

    timeOut->start(5 * 1000);   //5000
    _isNetTimeout = false;
    qEvent->exec();
    delete request;
    return;
}

void PaasCommuinication::replyFinished(QNetworkReply*) /* download finished */
{
    errInfo.clear();
    _paasList.clear();

    qDebug()<<"replyFinished:"<<_reply->error()<<endl;
    if (_reply->error() != QNetworkReply::NoError)
    {
        errInfo.append(tr("Network error, please check!"));

//        AppMessageBox box(false, NULL);
//        box.setText(tr("Network error, please check!"));
//        box.exec();

        _isNetError = true;
        emit done();
        return;
    }

    if(_buffer.isEmpty())
    {
        errInfo.append(tr("Network error, please check!"));

//        AppMessageBox box(false, NULL);
//        box.setText(tr("Network error, please check!"));
//        box.exec();

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
        _isNetTimeout = false;

        return;
    }


    PAAS_LIST tempPaasList;

//    QString jsonResult = _buffer;

//    QTextCodec *codec = QTextCodec::codecForName("utf-8"); //utf-8

//    QString jsonResult = codec->toUnicode(_buffer);

//    qDebug() << jsonResult;

    QJson::QJson parser;
    bool ok;
    QVariantMap result = parser.parse(_buffer, &ok).toMap();
    if (!ok) {
      qFatal("An error occurred during parsing by buffer");
      emit done();
      return;
    }

    qDebug() << "Paas json" << result["userInfoJsonStr"].toString();

    if (result["userInfoJsonStr"].toString() == "")
    {
        emit done();
        return;
    }

    bool ok2;
    QVariantMap userInfoMap = parser.parse(result["userInfoJsonStr"].toString().toUtf8(), &ok2).toMap();
    if (!ok2) {
      qFatal("An error occurred during parsing by userInfoJsonStr");
      emit done();
      return;
    }

    foreach(QVariant pluginPaasApp, userInfoMap["paasList"].toList())
    {
        QVariantMap paasMap = pluginPaasApp.toMap();

        tempPaasList.name = paasMap["name"].toString();
        tempPaasList.cnName = paasMap["cnName"].toString();
        tempPaasList.logoURL = paasMap["logoURL"].toString();
        tempPaasList.previewURL = paasMap["previewURL"].toString();

        tempPaasList.urls = paasMap["urls"].toString();
        if (tempPaasList.urls.isEmpty())
        {
            foreach (QVariant pluginUrls, paasMap["urls"].toList())
                tempPaasList.urls = pluginUrls.toString();
        }

        tempPaasList.iconPath = QString("");

//        qDebug() << "------>" <<"tempPaasList.name" << tempPaasList.name;
//        qDebug() << "------>" <<"tempPaasList.cnName" << tempPaasList.cnName;
//        qDebug() << "------>" <<"tempPaasList.urls" << tempPaasList.urls;

        _paasList.append(tempPaasList);
    }

    timeOut->stop();
    qEvent->exit();
    _isNetError = false;
    _isNetTimeout = false;
    emit done();

}

void PaasCommuinication::slotError(QNetworkReply::NetworkError)
{
    qDebug()<<"slotError:"<<_reply->error()<<endl;
    timeOut->stop();
    _isNetTimeout = false;
    qEvent->exit();
}


void PaasCommuinication::handleTimeOut()
{

    QTime Time = QTime::currentTime();
    qDebug()<<" paas TimeOut"<<Time.minute()<<Time.second()<<":"<<Time.msec()<<endl;

    timeOut->stop();
    qEvent->exit();

    _isNetTimeout = true;

    errInfo.clear();
    errInfo.append(tr("Network error, please check!"));

//    AppMessageBox box(false, NULL);
//    box.setText(tr("获取平台服务应用超时，请稍后!"));
//    box.exec();

    _isNetError = true;
    emit done();
}

void PaasCommuinication::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal == -1)
        return;
    if (bytesReceived < bytesTotal)
        return;
    _buffer = _reply->readAll();

}

void PaasCommuinication::postfinished()
{
    //if(isBalance == 1)
    //    emit appRun();
    //isBalance = 0;
}

void PaasCommuinication::close()
{
    _buffer.clear();
    delete _nam;
    delete timeOut;
    delete qEvent;
}

void PaasCommuinication::login(const QString &url, const QString &userId)
{
    QByteArray inputStr("");
    QString inputUrl("");

    qDebug() << url;

    if (url.isEmpty())
        return;

        inputUrl = url;
        qDebug() << "inputUrl" << inputUrl;


    myPost(QUrl(inputUrl), inputStr);

}


void PaasCommuinication::getAppList(const QString &url, const QString &userId)
{
    _paasList.clear();

    login(url, userId);

}
