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

    QFileInfo dbInfo(Config::get("LoginDb"));
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

    QSqlDatabase localDb = QSqlDatabase::addDatabase("QSQLITE", "loginData");
    localDb.setDatabaseName(Config::get("LoginDb"));

    if (!dbExists) {

       QString createUserTable = \
                      "CREATE TABLE users " \
                      "(name nvarchar not null primary key, " \
                      "password nvarchar not null);";
       QString createAddrTable = \
                      "CREATE TABLE addrs " \
                      "(addr nvarchar not null primary key, " \
                      "count not null);";
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

       QSqlDatabase::database("loginData").exec(createUserTable);
       QSqlDatabase::database("loginData").exec(createAddrTable);
       QSqlDatabase::database("loginData").exec(createVacServerTable);
       QSqlDatabase::database("loginData").exec(createPaasServerTable);
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
