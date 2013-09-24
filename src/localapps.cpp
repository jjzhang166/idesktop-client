#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QDesktopServices>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <QPixmap>
#include <QRgb>
#include <QPainter>
#include <QFileInfo>
#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QFileInfoList>
#include <QLibrary>
#include <windows.h>
#include <stdio.h>
#include <QVariant>
#include <QJSon/qjson.h>
#include <QJSon/serializer.h>

#include "idesktopsettings.h"
#include "localapps.h"
//#include "constants.h"
#include "common.h"
#include "appmessagebox.h"
#include "config.h"

#define KEY "\\Windows\\CurrentVersion\\Uninstall\\"
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
#define URL     12
#define DIRID   13
#define UNIQUENAME 14

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

void LocalApp::setUniqueName(QString uniqueName)
{
    _uniqueName = uniqueName;
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
    addDustbinOnNeed();
    updateQList();
//    updateAppList();
//    qDebug() << uploadJson();

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
        app->setUrl(query.value(URL).toString());
        app->setDirId(query.value(DIRID).toInt());
        app->setUniqueName(query.value(UNIQUENAME).toString());
        _list.append(app);
    }
}

void LocalAppList::updateAppList()
{
    QList<APP_LIST>& remoteAppList = _settings->remoteAppList();
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
            for(int i=0; i< remoteAppList.count(); i++)
            {
                LocalApp *RemoteApp = new LocalApp();
                RemoteApp->setName(remoteAppList[i].name);
                RemoteApp->setId(remoteAppList[i].id);
                RemoteApp->setIcon(remoteAppList[i].icon);
                RemoteApp->setPage(-1);
                RemoteApp->setIndex(i+j+1);
                RemoteApp->setType(remoteAppList[i].type);
                RemoteApp->setIsRemote(true);
                addRemoteApp(RemoteApp);
                //_list.append(RemoteApp);
            }
        }else{
            qDebug()<<"remoteList:"<<remoteAppList.count();
            for(int i=0; i< remoteAppList.count(); i++)
            {
                LocalApp *RemoteApp = new LocalApp();
                RemoteApp->setName(remoteAppList[i].name);
                RemoteApp->setId(remoteAppList[i].id);
                RemoteApp->setIcon(remoteAppList[i].icon);
                RemoteApp->setPage(-1);
                RemoteApp->setIndex(i);
                RemoteApp->setType(remoteAppList[i].type);
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
            for(int i=0; i < remoteAppList.count(); i++)
            {
                if(query.value(0).toString() == remoteAppList[i].name)
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
        for(int j=0; j < remoteAppList.count(); j++)
        {
            bool isRExist = false;
            while(Rquery.next()){
                if(remoteAppList[j].name == Rquery.value(0).toString())
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
               RemoteApp->setName(remoteAppList[j].name);
               RemoteApp->setId(remoteAppList[j].id);
               RemoteApp->setIcon(remoteAppList[j].icon);
               RemoteApp->setPage(-1);
               RemoteApp->setIndex(index);
               RemoteApp->setType(remoteAppList[j].type);
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
                           "lastupdate, page, idx, hidden, id, type, isRemote, url, dirId, uniquename) values ( " \
                           "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', "\
                           "\'%8\', \'%9\',\'%10\',\'%11\',\'%12\',\'%13\',\'%14\',\'%15\');")\
            .arg(app->name()).arg("1.0")\
            .arg(app->name()).arg(app->icon())\
            .arg(app->name()).arg(1)\
            .arg(app->page()).arg(app->index())\
            .arg(int(false)).arg(app->id())\
            .arg(app->type()).arg(int(true))\
            .arg(app->url()).arg(app->dirId())
            .arg(app->uniqueName());
    qDebug()<<"query:"<<qstr;
    QSqlQuery query(QSqlDatabase::database("local"));
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return false;
    }
    qDebug()<<"add Remote APP";
    _list.append(app);
    emit appAdded(app->name(), app->icon(), app->url(), app->type().toInt(), app->id().toInt(), app->uniqueName());
    emit appAdded(app);
    return true;
}

bool LocalAppList::addApp(LocalApp *app)
{ 

//    qDebug() << "addApp----->app.name" << app->name();
//    qDebug() << "addApp----->app->execname()" << app->execname();
    // write to sqlite and emit datachange

    QString qstr = QString("insert into localapps ("\
                           "name, version, execname, icon, uninstall, "\
                           "lastupdate, page, idx, hidden, id, type, isRemote, url, dirId, uniquename) values ( " \
                           "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', "\
                           "\'%8\', \'%9\',\'%10\',\'%11\',\'%12\',\'%13\',\'%14\',\'%15\');")\
            .arg(app->name()).arg(app->version())\
            .arg(app->execname()).arg(app->icon())\
            .arg(app->uninstName()).arg(app->date())\
            .arg(app->page()).arg(app->index())\
            .arg(int(app->hidden())).arg(QString("%1").arg(app->id()))\
            .arg(app->type()).arg(int(false))\
            .arg(app->url()).arg(app->dirId())\
            .arg(app->uniqueName());
    QSqlQuery query(QSqlDatabase::database("local"));
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return false;
    }
    qDebug()<<"add APP";
    _list.append(app);
    emit appAdded(app->name(), app->icon(), app->url(), app->type().toInt(), app->id().toInt(), app->uniqueName());
    emit appAdded(app);
    return true;
}

//void LocalAppList::delApp(QString name)
//{
//    for (int i = 0; i < _list.count(); i++) {
//        if (_list.at(i)->name() == name) {
//            QString qstr = QString("delete from localapps "
//                                   "where name=\"%1\";").arg(name);
//            QSqlQuery query = QSqlDatabase::database("local").exec(qstr);
//            _list.remove(i);
//            emit appRemoved(name);
//            return;
//        }
//    }
//}

/**
 * @brief LocalAppList::getPages
 * here I assume that all page numbers are consective, or else I should return
 * a set of page numbers
 * @return
 */
int LocalAppList::getPages() const
{
    QSqlQuery qry = QSqlDatabase::database("local").exec("select count(distinct page) from localapps where dirid=-1;");
    if (qry.next()) {
        return qry.value(0).toInt();
    }
}

int LocalAppList::dirCounts() const
{
    QSqlQuery qry = QSqlDatabase::database("local").exec("select count(*) from localapps where cast (id as int) between 0 and 110;");
    if (qry.next()) {
        return qry.value(0).toInt();
    }

    return 0;
}

QList<LocalApp*> LocalAppList::appsInDir(int id) const
{
    QList<LocalApp*> apps;
    for (int i = 0; i < this->count(); i++) {
        if (this->at(i)->hidden())
            continue;

        if (this->at(i)->dirId() == id) {
            apps.append(this->at(i));
        }
    }
    return apps;
}

LocalApp* LocalAppList::getAppByName(const QString &name)
{
    for (int i = 0; i < _list.count(); i++) {
        if (_list.at(i)->name() == name)
            return _list.at(i);
    }
    return NULL;
}

void LocalAppList::delApp(QString uniqueName)
{
    for (int i = 0; i < _list.count(); i++) {
        if (_list.at(i)->uniqueName() == uniqueName) {
            QSqlQuery query(QSqlDatabase::database("local"));
            QString qstr = QString("delete from localapps "
                                   "where uniquename=\"%1\";").arg(uniqueName);

            if(!query.exec(qstr)) {
                qDebug() <<"query failed";
                return;
            }

            LocalApp *app = _list.value(i);
            _list.remove(i);
            emit appRemoved(uniqueName);
            emit appRemoved(app);
            delete app;
            return;
        }
    }
}

LocalApp* LocalAppList::getAppByUniqueName(const QString &uniqueName)
{
    for (int i = 0; i < _list.count(); i++) {
        if (_list.at(i)->uniqueName() == uniqueName)
            return _list.at(i);
    }
    return NULL;
}

LocalApp* LocalAppList::at(int i) const
{
    if (i >= _list.count() || i < 0)
        return NULL;
    return _list.at(i);
}

int LocalAppList::count() const
{
    return _list.count();
}

void LocalAppList::save()
{
    for (int i = 0; i < _list.count(); i++) {
        qDebug() << _list.at(i)->name() << " -- " << _list.at(i)->page() << " : " << _list.at(i)->index()
                 << "--" << _list.at(i)->dirId();
    }
    QSqlQuery query(QSqlDatabase::database("local"));
    for (int i = 0; i < _list.count(); i++) {
        QString qstr = QString("update localapps "\
                               "set page=%1, idx=%2, dirId=%3, id=\'%4\', name=\'%5\' where uniquename=\'%6\';")\
                .arg(_list.at(i)->page())\
                .arg(_list.at(i)->index())\
                .arg(_list.at(i)->dirId())\
                .arg(_list.at(i)->id())\
                .arg(_list.at(i)->name())\
                .arg(_list.at(i)->uniqueName());
        if(!query.exec(qstr)) {
            qDebug() <<"query failed";
            return;
        }
    }
}

QString LocalAppList::getAppImage(QString appPath)
{
    qDebug() << "appPath--->" << appPath;
    typedef HICON (*tempFuc)(CONST TCHAR *filePath);
    QLibrary myLib;
    #ifdef DEBUG
        myLib.setFileName("IconGetD.dll");
    #else
        myLib.setFileName("IconGet.dll");
    #endif

    tempFuc myFunction = (tempFuc) myLib.resolve("getJumbIcon");
    if (myFunction) {
         HICON jumbIcon = myFunction((CONST TCHAR *)appPath.utf16 ());

         QPixmap picon = QPixmap::fromWinHICON(jumbIcon);

         QFileInfo info = QFileInfo(appPath);
         QString path(Config::get("IconDir"));
         path = path + "\\" + info.baseName();      //"USER_ADDED_"
//         qDebug() << "info.baseName()--->" << info.baseName();
         path += ".png"; //png
         QPixmap newicon =  picon.scaled(59, 59, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
         newicon.save(path, "PNG",-1);
         qDebug() << newicon.width() << newicon.height();
         return path;
    }
    return "";
}

void LocalAppList::addLocalApp(QString appPath)
{
    QString newApp = getAppImage(appPath);

    if (newApp.isEmpty())
        return;
#if 0
    QImage image = QImage(newApp).scaled(72, 72);
    QImage normal = QImage(":images/icon_shadow.png");
    //setImgOpacity(normal, 0);

//    for (int i = 0; i < normal.width(); i++) {
//        for (int j = 0; j < normal.height(); j++) {
//            QRgb pixel = normal.pixel(i,j);
//            int a = qAlpha(pixel);
//            QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel),
//                                    qBlue(pixel), a * 117 / 255);
//            normal.setPixel(i, j, lightPixel);
//        }
//    }

    QPainter pt1(&normal);
    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
    pt1.drawImage(35, 36, image);
    pt1.end();
    QPixmap pix = QPixmap::fromImage(normal);
    pix.save(newApp, "PNG", -1);//
#endif

    QImage image = QImage(newApp).scaled(59, 59, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage normal = QImage(":images/icon_shadow.png");
    QImage middle = QImage(":images/icon_middle_shadow.png");

    QPainter pt1(&normal);
    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
    pt1.drawImage(QRect(35, 36, 72, 72), middle.scaled(72, 72, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    pt1.drawImage(QRect(35 + 7, 36 + 3, 59, 59), image);
    pt1.end();

    QPixmap pix = QPixmap::fromImage(normal);
    pix.save(newApp, "PNG", -1);//

    QFileInfo info = QFileInfo(appPath);
    LocalApp *app = new LocalApp();
    app->setName(info.baseName());        // "/" + info.baseName()
    app->setIcon(newApp);
    app->setExecname(appPath);

    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash(appPath.toLower().toAscii(), \
                                  QCryptographicHash::Md5);
    md5.append(bb.toHex());
    app->setUniqueName("0_" + md5);

    app->setType(QString("%1").arg(0));
    app->setUrl("");
    app->setDirId(-1);
    app->setId("111");


    if (LocalAppList::getList()->getAppByUniqueName(app->uniqueName())) {
        QApplication::processEvents();
        AppMessageBox box(false, NULL);
        box.setText("�����Ӹ�ͼ��"); //    �����Ӹ�ͼ��
        box.exec();
    } else {
        addApp(app);
    }
}

QString LocalAppList::getUninstExec(QString display)
{
    QSettings reg(QSettings::NativeFormat, \
        QSettings::SystemScope, "Microsoft", KEY);
    for (int i = 0; i < reg.childGroups().count(); i++) {
        QSettings tmp(QSettings::NativeFormat, \
            QSettings::SystemScope, "Microsoft", \
            KEY + reg.childGroups().at(i));
        if (tmp.value("DisplayName").toString() == display) {
            return tmp.value("UninstallString").toString();
        }
    }
    return QString("");
}

QString LocalAppList::uploadJson()
{
//localapps
    QVariantList localapps;
    for (int i = 0; i < _list.count(); i++) {
        QVariantMap localapp;
        localapp.insert("name",         QString("%1").arg(_list.at(i)->name())          );
        localapp.insert("version",      QString("%1").arg(_list.at(i)->version())       );
        localapp.insert("execname",     QString("%1").arg(_list.at(i)->execname())      );
        localapp.insert("icon",         QString("%1").arg(_list.at(i)->icon())          );
        localapp.insert("uninstall",    QString("%1").arg(_list.at(i)->uninstName())    );
        localapp.insert("lastupdate",   QString("%1").arg(0)                            );
        localapp.insert("page",         QString("%1").arg(_list.at(i)->page())          );
        localapp.insert("idx",          QString("%1").arg(_list.at(i)->index())         );
        localapp.insert("hidden",       QString("%1").arg(_list.at(i)->hidden())        );
        localapp.insert("id",           QString("%1").arg(_list.at(i)->id())            );
        localapp.insert("type",         QString("%1").arg(_list.at(i)->type())          );
        localapp.insert("isRemote",     QString("%1").arg(_list.at(i)->isRemote())      );
        localapp.insert("url",          QString("%1").arg(_list.at(i)->url())           );
        localapp.insert("dirId",        QString("%1").arg(_list.at(i)->dirId())         );
        localapp.insert("uniquename",   QString("%1").arg(_list.at(i)->uniqueName())    );

        localapps << localapp;
    }

    Serializer serializer;

    QString localappsJson = QString::fromUtf8(serializer.serialize(localapps).data());

    qDebug() << "localappsJson" << localappsJson;


//wallpapers
    QVariantList wallpapers;
    QSqlQuery wallpapersQuery = \
            QSqlDatabase::database("local").exec(QString("SELECT id,wallpaper FROM wallpapers;"));
    while (wallpapersQuery.next())
    {
        QVariantMap wallpaper;
        wallpaper.insert("id",          QString("%1").arg(wallpapersQuery.value(0).toString())  );
        wallpaper.insert("wallpaper",   QString("%1").arg(wallpapersQuery.value(1).toString())  );

        wallpapers << wallpaper;
    }

    QString wallpapersJson = QString::fromUtf8(serializer.serialize(wallpapers).data());
//    qDebug() << "wallpapersJson" << wallpapersJson;


//sizetype
    QVariantList sizetype;
    QSqlQuery sizetypeQuery = \
            QSqlDatabase::database("local").exec(QString("SELECT id,type FROM sizetype;"));
    while (sizetypeQuery.next())
    {
        QVariantMap type;
        type.insert("id",               QString("%1").arg(sizetypeQuery.value(0).toString())    );
        type.insert("type",             QString("%1").arg(sizetypeQuery.value(1).toString())    );

        sizetype << type;
    }
    QString sizetypeJson = QString::fromUtf8(serializer.serialize(sizetype).data());
//        qDebug() << "sizetypeJson" << sizetypeJson;


    return "{\"localapps\":" + localappsJson +","
           "\"wallpapers\":" + wallpapersJson +","
           "\"sizetype\":" + sizetypeJson + "}";
}

void LocalAppList::addDustbinOnNeed()
{
    qDebug() << __PRETTY_FUNCTION__ << "need to create dustbin";

    QSqlDatabase db = QSqlDatabase::database("local");
    QSqlQuery query(db);

    if (query.exec(QString("select count(*) from localapps where id = 1000;"))) {
        query.first();
        if (query.value(0).toInt() > 0) {
            return;
        }
        qDebug() << __PRETTY_FUNCTION__ << "need to create dustbin";
    }

    QString qstrLapp = QString("insert into localapps ("\
                               "name, version, execname, icon, uninstall, "\
                               "lastupdate, page, idx, hidden, id, type, isRemote, url, dirId, uniqueName) values ( " \
                               "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', \'%8\', "\
                               "\'%9\', \'%10\',\'%11\',\'%12\',\'%13\',\'%14\', \'%15\');")\
            .arg(QString(QObject::tr("��ֽ¨"))).arg("1.0")\
            .arg(QString(QObject::tr("��ֽ¨"))).arg(":images/dustbin_normal.png")\
            .arg(QString(QObject::tr("��ֽ¨"))).arg(1)\
            .arg(0).arg(0)\
            .arg(int(false)).arg(1000)\
            .arg("4").arg(int(false))\
            .arg("").arg(-2).arg("4_dustbin");  //��ֽ¨

    if(!query.exec(qstrLapp)) {
        qDebug() <<"query failed";
    }

}
