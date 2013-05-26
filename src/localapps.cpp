#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QDesktopServices>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "localapps.h"
#include "constants.h"
#include "../common.h"
#define SQL_STATEMENT "select * from localapps;"

#define NAME    0
#define VERSION 1 
#define EXEC    2
#define ICON    3
#define UNINST  4
#define STATUS  5
#define PAGE    6
#define IDX     7
#define HIDDEN  8
#define ID      9
#define TYPE    10
#define ISREMOTE 11
extern QList<APP_LIST> g_RemoteappList;
LocalAppList * LocalAppList::_l = NULL;

LocalApp::LocalApp()
    : _date(0),
      _page(0),
      _index(0),
      _hidden(false)
{
}

LocalApp::~LocalApp()
{
}

LocalAppList * LocalAppList::getList()
{
    if (!LocalAppList::_l)
        LocalAppList::_l = new LocalAppList();
    return _l;
}

LocalAppList::LocalAppList(QObject *parent)
    :QObject(parent)
{
    updateQList();
    updateAppList();


}


LocalAppList::~LocalAppList()
{
    //TODO: delete every LocalApp
}

void LocalAppList::updateQList()
{
    QSqlQuery query = QSqlDatabase::database("local").exec(SQL_STATEMENT);
    while (query.next()) {
        LocalApp *app = new LocalApp();
        app->setName(query.value(NAME).toString());
        app->setVersion(query.value(VERSION).toString());
        app->setExecname(query.value(EXEC).toString());
        app->setIcon(query.value(ICON).toString());
        app->setUninstName(query.value(UNINST).toString());
        app->setDate(query.value(STATUS).toInt());
        app->setPage(query.value(PAGE).toInt());
        app->setIndex(query.value(IDX).toInt());
        app->setHidden(query.value(HIDDEN).toBool());
        app->setId(query.value(ID).toString());
        app->setType(query.value(TYPE).toString());
        app->setIsRemote(query.value(ISREMOTE).toBool());
//        qDebug()<<"index"<<query.value(IDX).toInt();
//        qDebug()<<"name:"<<query.value(NAME).toString();
//        qDebug()<<"isRemote"<<query.value(ISREMOTE).toInt();
        _list.append(app);
    }
}

void LocalAppList::updateAppList()
{
    bool isRemote = false;
    for(int i=0; i< _list.count(); i++)
    {
        if(_list[i]->isRemote())
        {
            isRemote = true;
            break;
        }
    }
    if(!isRemote)
    {
        qDebug()<<"remote add";
        QSqlQuery query1 = QSqlDatabase::database("local").exec("select max(idx) from localapps;");
        query1.next();
        qDebug()<<"max index value"<<query1.value(0).toInt();
        if(_list.count())
        {

            int j= query1.value(0).toInt();
            for(int i=0; i< g_RemoteappList.count(); i++)
            {
                LocalApp *RemoteApp = new LocalApp();
                RemoteApp->setName(g_RemoteappList[i].name);
                RemoteApp->setId(g_RemoteappList[i].id);
                RemoteApp->setIcon(g_RemoteappList[i].icon);
                RemoteApp->setPage(-1);
                RemoteApp->setIndex(i+j+1);
                RemoteApp->setType(g_RemoteappList[i].type);
                RemoteApp->setIsRemote(true);
                addRemoteApp(RemoteApp);
                //_list.append(RemoteApp);
            }
        }else{
            qDebug()<<"remoteList:"<<g_RemoteappList.count();
            for(int i=0; i< g_RemoteappList.count(); i++)
            {
                LocalApp *RemoteApp = new LocalApp();
                RemoteApp->setName(g_RemoteappList[i].name);
                RemoteApp->setId(g_RemoteappList[i].id);
                RemoteApp->setIcon(g_RemoteappList[i].icon);
                RemoteApp->setPage(-1);
                RemoteApp->setIndex(i);
                RemoteApp->setType(g_RemoteappList[i].type);
                RemoteApp->setIsRemote(true);
                addRemoteApp(RemoteApp);
                //_list.append(RemoteApp);
            }

        }
    }else{
        //
        QSqlQuery query = QSqlDatabase::database("local").exec("select name from localapps where isRemote=1");
        while (query.next()) {
            bool isExist = false;
            for(int i=0; i < g_RemoteappList.count(); i++)
            {
                if(query.value(0).toString() == g_RemoteappList[i].name)
                {
                    isExist = true;
                    break;
                }
            }
            if(!isExist)
            {
                //update idx and delte
                QString findName = QString("select idx from localapps where name=\"%1\"")\
                        .arg(query.value(0).toString());
                QSqlQuery query1 = QSqlDatabase::database("local").exec(findName);
                query1.next();
                QString updateIdx = QString("update localapps "\
                                            "set idx=idx-1 where idx > %1;")\
                                            .arg(query1.value(0).toInt());
                QSqlDatabase::database("local").exec(updateIdx);

                QString delteByName = QString("delete from localapps "\
                                              "where name=\"%1\";").arg(query.value(0).toString());
                QSqlDatabase::database("local").exec(delteByName);
            }
        }
        //
        QSqlQuery Rquery = QSqlDatabase::database("local").exec("select name from localapps where isRemote=1");
        for(int j=0; j < g_RemoteappList.count(); j++)
        {
            bool isRExist = false;
            while(Rquery.next()){
                if(g_RemoteappList[j].name == Rquery.value(0).toString())
                {
                    isRExist = true;
                    break;
                }
            }
            if(!isRExist)
            {
               QSqlQuery maxQuery = QSqlDatabase::database("local").exec("select max(idx) from localapps;");
               maxQuery.next();
               int index = maxQuery.value(0).toInt() + 1;
//               qDebug()<<"maxQuery:"<<index;
               LocalApp *RemoteApp = new LocalApp();
               RemoteApp->setName(g_RemoteappList[j].name);
               RemoteApp->setId(g_RemoteappList[j].id);
               RemoteApp->setIcon(g_RemoteappList[j].icon);
               RemoteApp->setPage(-1);
               RemoteApp->setIndex(index);
               RemoteApp->setType(g_RemoteappList[j].type);
               RemoteApp->setIsRemote(true);
               addRemoteApp(RemoteApp);
            }

        }
        //update _list
        _list.clear();
        updateQList();

    }
}

bool LocalAppList::addRemoteApp(LocalApp *app)
{
    // write to sqlite and emit datachange

    QString qstr = QString("insert into localapps ("\
                           "name, version, execname, icon, uninstall, "\
                           "lastupdate, page, idx, hidden, id, type, isRemote) values ( " \
                           "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', "\
                           "\'%6\', \'%7\', \'%8\', \'%9\',\'%10\',\'%11\',\'%12\');")\
            .arg(app->name()).arg("1.0")\
            .arg(app->name()).arg(app->icon())\
            .arg(app->name()).arg(1)\
            .arg(app->page()).arg(app->index())\
            .arg(int(false)).arg(app->id())\
            .arg(app->type()).arg(int(true));
    qDebug()<<"query:"<<qstr;
    QSqlQuery query(QSqlDatabase::database("local"));
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return false;
    }
    qDebug()<<"add Remote APP";
    _list.append(app);
    return true;
}

bool LocalAppList::addApp(LocalApp *app)
{ 
    // write to sqlite and emit datachange

    QString qstr = QString("insert into localapps ("\
                           "name, version, execname, icon, uninstall, "\
                           "lastupdate, page, idx, hidden, id, type, isRemote) values ( " \
                           "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', "\
                           "\'%6\', \'%7\', \'%8\', \'%9\',\'%10\',\'%11\',\'%12\');")\
            .arg(app->name()).arg(app->version())\
            .arg(app->execname()).arg(app->icon())\
            .arg(app->uninstName()).arg(app->date())\
            .arg(app->page()).arg(app->index())\
            .arg(int(app->hidden())).arg("111")\
            .arg("333").arg(int(false));
    QSqlQuery query(QSqlDatabase::database("local"));
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return false;
    }
    qDebug()<<"add APP";
    _list.append(app);
    emit appAdded(app->name(), app->icon());
    return true;
}

void LocalAppList::delApp(QString name)
{
    for (int i = 0; i < _list.count(); i++) {
        if (_list.at(i)->name() == name) {
            QString qstr = QString("delete from localapps "\
                                   "where name=\"%1\";").arg(name);
            QSqlQuery query = QSqlDatabase::database("local").exec(qstr);
            _list.remove(i);
            emit appRemoved(name);
            return;
        }
    }
}

LocalApp* LocalAppList::getAppByName(const QString &name)
{
    for (int i = 0; i < _list.count(); i++) {
        if (_list.at(i)->name() == name)
            return _list.at(i);
    }
    return NULL;
}

LocalApp* LocalAppList::at(int i)
{
    if (i >= _list.count() || i < 0)
        return NULL;
    return _list.at(i);
}

int LocalAppList::count()
{
    return _list.count();
}

void LocalAppList::save()
{
    for (int i = 0; i < _list.count(); i++) {
        qDebug() << _list.at(i)->name() << " -- " << _list.at(i)->page() << " : " << _list.at(i)->index();
    }
    QSqlQuery query(QSqlDatabase::database("local"));
    for (int i = 0; i < _list.count(); i++) {
        QString qstr = QString("update localapps "\
                               "set page=%1, idx=%2 where name=\'%3\';")\
                .arg(_list.at(i)->page())\
                .arg(_list.at(i)->index())\
                .arg(_list.at(i)->name());
        if(!query.exec(qstr)) {
            qDebug() <<"query failed";
            return;
        }
    }
}
