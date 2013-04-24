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
        _list.append(app);
    }
}

LocalAppList::~LocalAppList()
{
    //TODO: delete every LocalApp
}

bool LocalAppList::addApp(LocalApp *app)
{ 
    // write to sqlite and emit datachange

    QString qstr = QString("insert into localapps ("\
                   "name, version, execname, icon, uninstall, "\
                   "lastupdate, page, idx, hidden) values ( " \
		           "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', "\
		           "\'%6\', \'%7\', \'%8\', \'%9\');")\
		           .arg(app->name()).arg(app->version())\
		           .arg(app->execname()).arg(app->icon())\
		           .arg(app->uninstName()).arg(app->date())\
		           .arg(app->page()).arg(app->index())\
                   .arg(int(app->hidden()));
    QSqlQuery query(QSqlDatabase::database("local"));
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return false;
    }
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
