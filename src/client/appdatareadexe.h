#ifndef APPDATAREADEXE_H
#define APPDATAREADEXE_H

#include <cstdlib>
#include <QObject>
#include <QList>
#include <QByteArray>
#include <qtnetwork/QNetworkReply>
#include <QProcess>


class Downloader;
class QProcess;
class QNetworkAccessManager;
class QNetworkReply;
class QScriptValueIterator;
class QNetworkCookieJar;
class AppData;
class AppDatabase;
class LocalAppList;
class LocalApp;
class AppDataReadExe : QObject
{
    Q_OBJECT
public:


    struct category {

        category(): _index(-1) {}
        int _index;
        QString _category;
    };

    static AppDataReadExe* Instance();
    static void destroy();

private slots:

    void onDone();
    void onIconDone();
    void onProgress(qint64, qint64);
    void onProcessFinished(int, QProcess::ExitStatus);
    void onProcessStarted();
    void onLoadError(QString error);

public:

    void installApp(AppData* data);

    void readSortData(const int &tabIndex, const int &start, const int &count);
//    void readSortData(const int &tabIndex);

    void readAppData(const QString &appName);
    void readCategory();
    void searchApp(const QString &keyword);

    category getCategory(const int &index);
    int size();

    QString getCurCategory(){ return _curCategory; }

    void readLocalApp(AppData* data, bool &bAdded, bool &bCanUpdated, const bool &checkOS);


    void InstallCanceled();

    void postData(QString appName, QByteArray data);
    void login();


    QString getUninstExec(QString display);
    QString getExec(QString execname);

    void addLocalApp(QString appPath);
    QString getAppImage(QString appPath);

public slots:

    void replyFinished(QNetworkReply*);
    void categoryFinished(QNetworkReply*);
    void searchFinished(QNetworkReply*);
    void appFinished(QNetworkReply*);

    void postFinished(QNetworkReply*);
    void loginFinished(QNetworkReply*);


protected:

 //   bool getExec(QString execname);
    bool findFile(QString path, QString fileName, QString &lnkPath);

    AppData* getAppData(QScriptValueIterator* it);
    QString convertError(QNetworkReply::NetworkError error);

    bool isInstalled(QString execName, QString uninstallName,
                     QString &exec, QString &uninstall);

    LocalApp* addLocalApp(AppData* data, const QString &execName = "");

private:
    AppDataReadExe();
    QString convertCategory(const int &index);

    static AppDataReadExe* _instance;

     QNetworkAccessManager *_manager;
     QNetworkAccessManager *_categoryManager;
     QNetworkAccessManager *_searchManager;
     QNetworkAccessManager *_appManager;


     QNetworkAccessManager *_postManager;
     QNetworkAccessManager *_loginManager;

  //   QByteArray _url;
     QString _url;
     QNetworkReply* _reply;

     QList<category> _categoryList;

     QByteArray _winVersion;
     Downloader* _loader;
     Downloader* _iconLoader;
     QString _curCategory;
     QProcess *_p;
     bool _finished;
//     bool _inProgress;
     bool _installCanceled;
     bool _loadError;

     bool _loadIcon;

     QNetworkCookieJar* _cookieJar;
 //    LocalAppList *_l;

     void setImgOpacity(QImage &img, const int &alpha);
};

#endif // APPDATAREADEXE_H
