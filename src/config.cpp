#include <QString>
#include <QSettings>
#include <QMap>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "config.h"

#define GET_VALUE(x,y) x.value(y).toString()
#define FOLDERS_KEY "\\Windows\\CurrentVersion\\Explorer\\Shell Folders\\"

extern QString serverip;

QMap<QString, QString> Config::configDict;

void Config::initiate()
{
    QSettings commonReg(QSettings::NativeFormat, \
                  QSettings::SystemScope, \
                  "Microsoft", \
                  FOLDERS_KEY);

    QSettings userReg(QSettings::NativeFormat, \
                  QSettings::UserScope, \
                  "Microsoft", \
                  FOLDERS_KEY);

    set("CommonDesktopDir",GET_VALUE(commonReg, "Common Desktop"));
    set("DataDir", GET_VALUE(userReg, "AppData"));
    set("AppDir", get("DataDir").append("\\App Center"));
    set("DesktopDir", GET_VALUE(userReg, "Desktop"));
    set("UserDir",GET_VALUE(userReg, "Personal"));
    set("CommonProgramDir", GET_VALUE(commonReg, "Common Programs"));
    set("UserProgramDir", GET_VALUE(userReg, "Programs"));
    set("LoginDb", get("AppDir").append("\\data"));
    set("IconDir", get("AppDir").append("\\Licons"));
    set("InstDir", get("AppDir").append("\\downloads"));
    set("Profile", get("UserDir").append("\\profile"));
    set("Wallpaper", ":images/shadow.png");
    set("User", "");
    set("Server", "");
    set("SystemManage", "./system_manage/system_manage.html");
    set("UserNameDir", "");
    set("UserData", "");

    QString str = GET_VALUE(userReg, "Programs");

    /*
    set("CommonDesktopDir",GET_VALUE(commonReg, "Common Desktop"));
    set("DataDir", "C:\\");
    set("AppDir", "C:\\AppDir");
    set("DesktopDir", GET_VALUE(userReg, "Desktop"));
    set("UserDir",GET_VALUE(userReg, "Personal"));
    set("LocalDb", get("AppDir").append("\\data"));
    set("IconDir", get("AppDir").append("\\icons"));
    set("InstDir", get("AppDir").append("\\downloads"));
    set("Profile", get("UserDir").append("\\profile"));
    */

   
   /*
    set("DbUser", ts.readLine());
    set("DbPassword", ts.readLine());
    set("DbName", "appcenter");
    */
}

void Config::setServerIp()
{

//    /****************************************************************/
//    //wangyaoli

//    printf("server ip: %s\n", serverip.toAscii().data());
//    QFile f1(get("Profile"));
//    f1.open(QIODevice::WriteOnly | QIODevice::Text);
//    QTextStream txtOutput(&f1);
//    //txtOutput << "http://" <<serverip << "/"<<endl;
//    txtOutput << serverip <<endl;
//    f1.close();

//    /****************************************************************/

//    set("Server", "http://" + serverip);
//    qDebug() << "setServerIP : "<<get("Server");
}

void Config::getServerIp()
{
//    QFile fip(get("Profile"));
//    fip.open(QIODevice::ReadOnly);
//    QTextStream tsip(&fip);
//    serverip = tsip.readLine();
//    if (false == serverip.isEmpty())
//    {
//      set("Server", "http://" + serverip);
//      qDebug() <<"getServerIp() : "<<get("Server");
//    }
//    fip.close();
}

QString Config::get(QString key)
{
    return configDict.value(key, "");
}

void Config::set(QString key, QString value)
{ 
    configDict[key] = value;
}
