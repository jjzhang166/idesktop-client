#include <QApplication>
#include <QTranslator>
#include <QRect>
#include <QDesktopWidget>
#include <QTextCodec>
#include <QPlastiqueStyle>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

#include <windows.h>
#include<Tlhelp32.h>

#include "dashboard.h"
#include "panel.h"
#include "config.h"
#include "appmessagebox.h"
#include "src/qtsingleapplication/qtsingleapplication.h"
#include <QMessageBox>

//#define one "0xbe8e2ce1, 0xdab6, 0x11d6, 0xad, 0xd0, 0x0, 0xe0, 0x4c, 0x53, 0xf6, 0xe6"
HANDLE handle;
/****************************************************************/
//wangyaoli
QString serverip;
/****************************************************************/

int main(int argc, char *argv[])
{
    serverip = "in put server ip";

    Config::initiate();
    QtSingleApplication app(argc, argv);

    QTranslator translator(0);
    translator.load("idesktop-client.qm",".");
    app.installTranslator(&translator);

#if 1
    if (app.isRunning())
    {
        AppMessageBox box(false, NULL);
        box.setText(QString(QObject::tr("A running instance already exists")));
        box.raise();
        box.exec();
       //     app.sendMessage("raies");

//        QMessageBox::information(0, QObject::tr("i-desktop"),
//                                         QString(QObject::tr("\n A running instance already exists             ")),
//                                         QString(QObject::tr("OK")));

            return EXIT_SUCCESS;
    }
#endif

    app.setQuitOnLastWindowClosed(false);
    app.setStyle(new QPlastiqueStyle);

    QFont font = app.font();
    font.setPointSize(10);
    app.setFont(font);

    /*
    app.setStyleSheet(//"QAbstractButton{border:1px solid #515151; "
        "border-bottom:2px solid #515151; border-radius: 0px;"
        " font-weight:450; height: 28px;}"
        //" QAbstractButton:checked{ background-color: #515151; color: white;}"
        //" QAbstractButton:pressed { background-color: #515151; color: white;}"
        " QLineEdit{border:1px solid #515151;border-radius: 3px;}");
    */

    QTextCodec *codec = QTextCodec::codecForName("System"); //utf-8
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    /* create a config object */
    /* check whether appdir exists */

    bool dbExists = true;

    QFileInfo aInfo(Config::get("AppDir"));
    if (!aInfo.exists() || !aInfo.isDir())
    {
       QDir appDir(Config::get("DataDir"));
       appDir.mkdir("App Center");
       dbExists = false;
    }

    QFileInfo dbInfo(Config::get("LocalDb"));
    if (!dbInfo.exists())
        dbExists = false;

    QFileInfo iInfo(Config::get("IconDir"));
    if (!iInfo.exists() || !iInfo.isDir())
    {
        QDir appDir(Config::get("AppDir"));
        appDir.mkdir("Licons");
    }

//    QFileInfo dInfo(Config::get("InstDir"));
//    if (!dInfo.exists() || !dInfo.isDir())
//    {
//        QDir appDir(Config::get("AppDir"));
//        appDir.mkdir("downloads");
//    }

    QSqlDatabase localDb = QSqlDatabase::addDatabase("QSQLITE", "local");
    localDb.setDatabaseName(Config::get("LocalDb"));

    if (!dbExists) {
       QString createAppTable = \
                      "CREATE TABLE localapps " \
                      "(name nvarchar not null, " \
                      "version nvarchar not null, " \
                      "execname nvarchar not null, " \
                      "icon nvarchar not null, " \
                      "uninstall nvarchar not null, "\
                      "lastupdate int not null, " \
                      "page int not null, " \
                      "idx int not null, " \
                      "hidden int not null default 0, " \
                      "id nvarchar not null," \
                      "type nvarchar not null," \
                      "isRemote int not null default 0, " \
                      "url nvarchar not null, " \
                      "dirId int not null, "\
                      "uniquename nvarchar not null);";
       QString createUserTable = \
                      "CREATE TABLE users " \
                      "(name nvarchar not null primary key, " \
                      "password nvarchar not null);";
       QString createAddrTable = \
                      "CREATE TABLE addrs " \
                      "(addr nvarchar not null primary key, " \
                      "count not null);";
       QString createWallpaperTable = \
                      "CREATE TABLE wallpapers " \
                      "(id int not null primary key, " \
                      "wallpaper nvarchar not null);";
       QString createVacServerTable = \
                      "CREATE TABLE vacservers " \
                      "(id int not null primary key, " \
                      "server nvarchar not null, " \
                      "port int not null, " \
                      "name nvarchar not null, " \
                      "password nvarchar not null);";
       QString createPaasServerTable = \
                      "CREATE TABLE paasservers " \
                      "(id int not null primary key, " \
                      "verifyServer nvarchar not null, "\
                      "server nvarchar not null);";
       QString createIconSizeTable = \
                      "CREATE TABLE sizetype " \
                      "(id int not null primary key, " \
                      "type int not null);";

       QSqlDatabase::database("local").exec(createAppTable);
       QSqlDatabase::database("local").exec(createUserTable);
       QSqlDatabase::database("local").exec(createAddrTable);
       QSqlDatabase::database("local").exec(createWallpaperTable);
       QSqlDatabase::database("local").exec(createVacServerTable);
       QSqlDatabase::database("local").exec(createPaasServerTable);
       QSqlDatabase::database("local").exec(createIconSizeTable);

       QString qstrLapp = QString("insert into localapps ("\
                                  "name, version, execname, icon, uninstall, "\
                                  "lastupdate, page, idx, hidden, id, type, isRemote, url, dirId, uniqueName) values ( " \
                                  "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', \'%8\', "\
                                  "\'%9\', \'%10\',\'%11\',\'%12\',\'%13\',\'%14\', \'%15\');")\
                                   .arg(QString(QObject::tr("dustbin"))).arg("1.0")\
                                   .arg(QString(QObject::tr("dustbin"))).arg(":images/dustbin_normal.png")\
                                   .arg(QString(QObject::tr("dustbin"))).arg(1)\
                                   .arg(0).arg(0)\
                                   .arg(int(false)).arg(1000)\
                                   .arg("4").arg(int(false))\
                                    .arg("").arg(-2).arg("4_dustbin");  //废纸篓

       QSqlQuery query(QSqlDatabase::database("local"));

       if(!query.exec(qstrLapp))
       {
           qDebug() <<"query failed";
       }

       QString qstrWp = QString("insert into wallpapers ("\
                              "id, wallpaper) values ( "\
                              "\'%1\', \'%2\');")\
                              .arg(1).arg("./images/wallpager/wp_0.jpg");

        if(!query.exec(qstrWp)) {
            qDebug() <<"query failed";
        }

         QString qstrSizeType = QString("insert into sizetype ("\
                                        "id, type) values ( "\
                                        "\'%1\', \'%2\');")\
                                        .arg(1).arg(0);
         if(!query.exec(qstrSizeType)) {
             qDebug() <<"query failed";
         }
    }

    /*获取localdb的数据*/

    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry();

    Dashboard board;
    board.setFixedSize(rect.width(), rect.height());

    board.show();
//    qInstallMsgHandler(crashingMessageHandler); //
    return app.exec();
}
