/* applist.h
* header of list 
*/
#ifndef APPLIST_H
#define APPLIST_H

#include <QObject>


#include "commuinication.h"


typedef struct _APP_LIST_EX
{
    APP_LIST baseList;
    int page;
    int index;
    bool hidden;
} APP_LIST_EX, *PAPP_LIST_EX;

class AppList
{
public:
    AppList();
    ~AppList();

public:

    int count()
    {
        return _applicationListEx.count();
    }

    QString id()
    {
        return _id;
    }
    void setId(QString id)
    {
        _id = id;
    }

    QString name()
    {
        return _name;
    }
    void setName(QString name)
    {
        _name = name;
    }

    QString alias()
    {
        return _alias;
    }
    void setAlias(QString alias)
    {
        _alias = alias;
    }

    QString addr()
    {
        return _addr;
    }
    void setAddr(QString addr)
    {
        _addr = addr;
    }

    QString path()
    {
        return _path;
    }
    void setPath(QString path)
    {
        _path = path;
    }

    QString type()
    {
        return _type;
    }
    void setType(QString type)
    {
        _type = type;
    }

    QString icon()
    {
        return _icon;
    }
    void setIcon(QString icon)
    {
        _icon = icon;
    }

    QString param()
    {
        return _param;
    }
    void setParam(QString param)
    {
        _param = param;
    }

    //-----------
    int page()
    {
        return _page;
    }
    void setPage(int page)
    {
        qDebug()<< _page;
        _page = page;
    }
    int index()
    {
        qDebug()<< _index;

        return _index;
    }
    void setIndex(int index)
    {
        _index= index;
    }
    bool hidden()
    {
        return _hidden;
    }
    void setHidden(bool hidden)
    {
        _hidden= hidden;
    }

private:
    int _page;
    int _index;
    bool _hidden;

    QString _id;
    QString _name;
    QString _alias;
    QString _addr;
    QString _path;
    QString _type;
    QString _icon;
    QString _param;
    QList<APP_LIST_EX> _applicationListEx;
};

class VirutalAppList : public QObject
{
    Q_OBJECT
public:
    //explicit VirutalAppList(QObject *parent = 0);
    static VirutalAppList *getList();
    ~VirutalAppList();

    void addApp(AppList*);
    void delApp(QString name);
    AppList * getAppByName(const QString &name);
    AppList* at(int i);
    int count();
    void save();


    static QList<APP_LIST_EX>& getApplist();

    void setApplist(QList<APP_LIST> list)
    {
        _applicationList = list;
    }


public slots:

signals:
    void appAdded(QString name, QString icon);
    void appRemoved(const QString &name);
private:
    VirutalAppList(QObject *parent = NULL);
    //QSqlDatabase db;
    QVector<AppList*> _list;
    static VirutalAppList *_l;

    QList<APP_LIST> _applicationList;
};

#endif // APPLIST_H
