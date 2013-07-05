#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <QTime>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include "paascommuinication.h"
#include "PaasCommuinication.h"

//#define PAASURL "http://192.168.30.128/dongfang.php"
//#define PAASURL "http://192.168.49.243:8080/CloudManage/rest/service/getUserAllService"

PaasCommuinication::PaasCommuinication(QObject *parent)
    : QObject(parent)
    , _reply(NULL)
    , _isPost(true)
    , errID("")
    , errInfo("")
    , _isNetError(false)
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

    timeOut->start(5000);
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

    timeOut->start(5000);
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
        errInfo.append(tr("Paas Network error, please check!"));
        _isNetError = true;
        emit done();
        return;
    }

    if(_buffer.isEmpty())
    {
        errInfo.append(tr("Paas Network error, please check!"));
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

    QScriptValue sc;
    QScriptEngine engine;
    QString result;
    QString url;
    QString logoURL;
    QString previewURL;
    QString cnName("");
    PAAS_LIST tempPaasList;

   QString jsonResult = _buffer;
    jsonResult.replace(QString("[{"), QString("{"));
    jsonResult.replace(QString("}]"), QString("}"));
    QStringList jsonSections = jsonResult.split(QString("},{"), QString::SkipEmptyParts);

    for (int i = 0; i < jsonSections.size(); i++)
    {
        if (i == 0)
            result = jsonSections.at(i) + "}";
        else if (i == jsonSections.size() - 1)
            result = "{" + jsonSections.at(i);
        else
            result = "{" + jsonSections.at(i) + "}";

        sc = engine.evaluate("value = " + result);

        QScriptValueIterator it(sc);


        while (it.hasNext()) {
            it.next();
            if (!it.name().isEmpty())
            {
                if (it.name() == "cnName")
                {
                    cnName = it.value().toString();
                }
                else if (it.name() == "logoURL")
                {
                    logoURL = it.value().toString();
                    qDebug() << it.name() << it.value().toString();
                }
                else if (it.name() == "previewURL")
                {
                    previewURL = it.value().toString();
                }
                else if (it.name() == "urls")
                {
                    url = it.value().toString();
                    qDebug() << it.name() << it.value().toString();
                }
           }
        }
        if (!cnName.isEmpty())
        {
            tempPaasList.cnName = cnName;
            tempPaasList.logoURL = logoURL;
            tempPaasList.previewURL = previewURL;
            tempPaasList.urls = url;
            tempPaasList.iconPath = QString("");

            _paasList.append(tempPaasList);
        }
    }

    timeOut->stop();
    qEvent->exit();
    _isNetError = false;
    emit done();
}

void PaasCommuinication::slotError(QNetworkReply::NetworkError)
{
    qDebug()<<"slotError:"<<_reply->error()<<endl;
    timeOut->stop();
    qEvent->exit();
}


void PaasCommuinication::handleTimeOut()
{

    QTime Time = QTime::currentTime();
    qDebug()<<"TimeOut"<<Time.minute()<<Time.second()<<":"<<Time.msec()<<endl;

    timeOut->stop();
    qEvent->exit();

    errInfo.clear();
    errInfo.append(tr("Paas Network error, please check!"));

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

    //qDebug()<<"downloadProgress::_buffer"<<_buffer<<endl;
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

void PaasCommuinication::login(const QString &url)
{
    QByteArray inputStr("");
//    QUrl url;

//    if (u.isEmpty())
//        url = QUrl(PAASURL);
//    else
//        url = QUrl(u);

    _url = url;

    myPost(_url, inputStr);

    return;
}

void PaasCommuinication::getAppList(const QString &url)
{
    _paasList.clear();

//    QByteArray inputStr("");

////    QUrl url(PAASURL);

//    _url = url;

//    myPost(_url, inputStr);

    login(url);

    return;
}
