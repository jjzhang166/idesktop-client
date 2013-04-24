#ifndef COMMUI_H
#define COMMUI_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

#include <QTimer>
#include <QByteArray>
#include <QThread>
#include <QEventLoop>
#include <QtNetwork/QNetworkReply>

#include "common.h"
extern _Commui Commui;

class commuinication:public QObject
{
    Q_OBJECT
public:
    enum AttriName{Name,Alias,Icon,Type,ExePath,WorkDir,IP,Aid,SrvType};

    enum POST_TYPE
    {
        INIT = 0,
        LOGIN,
        LOGOFF,
        GETAPP,
        CHPWD,
        LDBALANCE,
        HEARTBEAT,
        END
    };
public:
    commuinication(QObject *parent = 0);
    ~commuinication();
    void myPost(const QUrl url, const QByteArray postData);
    void myGet(const QUrl url, const QString path);

    //--------------
public:
    void login(const QString ip, const QString name, const QString pwd, const QString sysInfo);
    void logoff();
    void getAppList();
    void changePwd(const QString strCurPwd, const QString strNewPwd, const QString strConfPwd);
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
    void appRun();

private:
    QNetworkAccessManager *_nam;
    QEventLoop*   qEvent;
    QTimer        *timeOut;
    QByteArray _buffer;
    QNetworkReply* _reply;

    bool _isPost;

    QUrl _url;
    QString _dest;
    POST_TYPE _type;
public:
    QString errID;
    QString errInfo;
    QList<APP_LIST> _appList;
    QList<APP_LIST*> appList;
    QString _ip;
    QString _sessionID;
    int isBalance;
    bool _isNetError;
public:
    QString& getSessionID()
    {
        return _sessionID;
    }

    QByteArray& buffer()
    {
        return _buffer;
    }

    int getCount()
    {
        return _appList.count();
    }

    void setInfo(QString smIP, QString sID)
    {
        _ip = smIP;
        _sessionID = sID;
    }

    QList<APP_LIST>& getList()
    {
        qDebug()<<"return applist"<<"\n";
        qDebug()<<"_appList count="<<_appList.count()<<"\n";
        return _appList;
    }

    void downloadIcon(QUrl url, QString filePath)
    {
        myGet(url, filePath);
    }

    void close();
};

#endif // COMMUI_H
