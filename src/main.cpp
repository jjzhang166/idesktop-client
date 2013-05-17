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

int main(int argc, char **argv)
{
    serverip = "in put server ip";


    Config::initiate();
//    QApplication app(argc, argv);
    QtSingleApplication app(argc, argv);
    if (app.isRunning())
    {
   //     AppMessageBox box(false, NULL);
   //     box.setText("    已有一个程序在运行   ");
   //     box.exec();
       //     app.sendMessage("raies");
        QMessageBox::information(0, "appStore",
                                 QString::fromLocal8Bit("\n     已有一个程序在运行             "),
                                 QString::fromLocal8Bit("确定"));

            return EXIT_SUCCESS;
    }


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

    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    /* create a config object */
    /* check whether appdir exists */

    bool dbExists = true;
    QFileInfo aInfo(Config::get("AppDir"));
    if (!aInfo.exists() || !aInfo.isDir()) {
       QDir appDir(Config::get("DataDir"));

       appDir.mkdir("App Center");       
       appDir.mkdir("icons");
       appDir.mkdir("downloads");

       dbExists = false;
    }
    else
    {
      QFileInfo dbInfo(Config::get("LocalDb"));
      if (!dbInfo.exists())
          dbExists = false;

        QFileInfo iInfo(Config::get("IconDir"));
        if (!iInfo.exists() || !iInfo.isDir()) {
           QDir appDir(Config::get("AppDir"));
           appDir.mkdir("icons");
        }

        QFileInfo dInfo(Config::get("InstDir"));
        if (!dInfo.exists() || !dInfo.isDir()) {
           QDir appDir(Config::get("AppDir"));
           appDir.mkdir("downloads");
        }
    }

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
                      "isRemote int not null default 0);";
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
       QString createUrlTable = \
                      "CREATE TABLE urls " \
                      "(id int not null primary key, " \
                      "virtualurl nvarchar not null," \
                      "serviceurl nvarchar not null);";

       QSqlDatabase::database("local").exec(createAppTable);
       QSqlDatabase::database("local").exec(createUserTable);
       QSqlDatabase::database("local").exec(createAddrTable);
       QSqlDatabase::database("local").exec(createWallpaperTable);
       QSqlDatabase::database("local").exec(createUrlTable);

       QString qstrWp = QString("insert into wallpapers ("\
                              "id, wallpaper) values ( "\
                              "\'%1\', \'%2\');")\
                              .arg(1).arg(":images/shadow.png");
        QSqlQuery query(QSqlDatabase::database("local"));
        if(!query.exec(qstrWp)) {
            qDebug() <<"query failed";
        }

        QString qstrUrl = QString("insert into urls ("\
                                  "id, virtualurl, serviceurl) values ("\
                                  "\'%1\', \'%2\', \'%3\');")\
                                .arg(1).arg("welcome_manage.html").arg("welcome_manage.html");
         if(!query.exec(qstrUrl)) {
             qDebug() <<"query failed";
         }
    }

    /*获取localdb的数据*/

    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry();

    Dashboard board;
    board.setFixedSize(rect.width(), rect.height());

    board.show();
    return app.exec();
}
