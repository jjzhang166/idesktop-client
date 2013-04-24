#ifndef LOCALAPPS_H
#define LOCALAPPS_H

//#define PAGENUM 4
#define PAGENUM 2
#include <QString>
#include <QObject>
#include <QVector>
#include <QtSql/QSqlQuery>

//enum Status {havenot, downloaded, installed, old};

class LocalApp
{
public:
    LocalApp();
    ~LocalApp();
    QString name()
        { return _name; }
    void setName(QString name) 
        { _name = name; }
    QString icon()
        { return _icon; }
    void setIcon(QString icon) 
        { _icon = icon; }
    QString version()
        { return _version; }
    void setVersion(QString version)
        { _version = version; }
    QString execname()
        { return _execname; }
    void setExecname(QString execname)
        { _execname = execname; }
    QString uninstName()
        { return _uninstName; }
    void setUninstName(QString uninstName)
        { _uninstName = uninstName;}
    int date()
        { return _date; }
    void setDate(int date)
        { _date= date; }
    int page()
        { return _page; }
    void setPage(int page)
        { _page = page; }
    int index()
        { return _index; }
    void setIndex(int index)
        { _index= index; }
    bool hidden()
    {
        return _hidden;
    }
    void setHidden(bool hidden)
    {
        _hidden= hidden;
    }
private:
    QString _name;
    QString _icon;
    QString _version;
    QString _execname;
    QString _uninstName;
    int _date;
    int _page;
    int _index;
    bool _hidden;
};

class LocalAppList : public QObject
{
    Q_OBJECT
public:
    static LocalAppList *getList();
    ~LocalAppList();
    bool addApp(LocalApp*);
    void delApp(QString name);
    LocalApp * getAppByName(const QString &name);
    LocalApp* at(int i);
    bool updateApp(LocalApp* app);
    int count();
    void save();
signals:
    void appAdded(QString name, QString icon);
    void appRemoved(const QString &name);
private:
    LocalAppList(QObject *parent = NULL);
    QVector<LocalApp*> _list;
    static LocalAppList *_l;
};

#endif
