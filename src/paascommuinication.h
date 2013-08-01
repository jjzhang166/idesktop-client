#ifndef PAASCOMMUINICATION_H
#define PAASCOMMUINICATION_H

#include <QWidget>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

#include <QTimer>
#include <QByteArray>
#include <QThread>
#include <QEventLoop>
#include <QtNetwork/QNetworkReply>

//#include "commuinication.h"
//extern _Commui Commui;

typedef struct _LOCAL_LIST
{
    int id;
    QString name;
    QString iconPath;
    QString execname;
    QString uniqueName;
} LOCAL_LIST;

typedef struct _PAAS_LIST
{
    int serviceId;
    QString name;
    QString cnName;
    QString version;
    QString fullName;
    QString type;
    QString deseription;
    QString logoURL;
    QString previewURL;
    QString urls;
    QString parent;
    QString subFunction;
    QString iconPath;
} PAAS_LIST;

class PaasCommuinication : public QObject
{
    Q_OBJECT
public:

    PaasCommuinication(QObject *parent = 0);
    ~PaasCommuinication();
    void myPost(const QUrl url, const QByteArray postData);
    void myGet(const QUrl url, const QString path);

    //--------------
public:
    void login(const QString &u);
//    void logoff();
    void getAppList(const QString &url);
//    void changePwd(const QString strCurPwd, const QString strNewPwd, const QString strConfPwd);
    void heartBeat();
    void loadBalance(QString name,QString appId);
public slots:
    void replyFinished(QNetworkReply*); /* download finished */
    void slotError(QNetworkReply::NetworkError); /* handle error */
    void handleTimeOut(); /* handle time out */
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void postfinished();//判断post数据是否发送完成
signals:
    void done();
//    void appRun();

private:
    QNetworkAccessManager *_nam;
    QEventLoop*   qEvent;
    QTimer        *timeOut;
    QByteArray _buffer;
    QNetworkReply* _reply;

    bool _isPost;

    QUrl _url;
    QString _dest;
//    POST_TYPE _type;
    QList<PAAS_LIST> _paasList;

public:
    QString errID;
    QString errInfo;

//    QString _ip;
//    QString _sessionID;
    int isBalance;
    bool _isNetError;
public:
//    QString& getSessionID()
//    {
//        return _sessionID;
//    }

    QByteArray& buffer()
    {
        return _buffer;
    }

    int getCount()
    {
        return _paasList.count();
    }

//    void setInfo(QString smIP, QString sID)
//    {
//        _ip = smIP;
//        _sessionID = sID;
//    }

    QList<PAAS_LIST>& getList()
    {
        return _paasList;
    }



    void downloadIcon(QUrl url, QString filePath)
    {
        myGet(url, filePath);
    }

    void close();
};

#endif // PAASCOMMUINICATION_H
