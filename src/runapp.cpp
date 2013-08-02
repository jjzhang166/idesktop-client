#include "runapp.h"

#include <QFileInfo>
#include <QMessageBox>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include "strings.h"
using namespace std;
#include <QMessageBox>

#include "appmessagebox.h"
#include "paascommuinication.h"



extern QList<APP_LIST> g_myVappList;
extern QList<LOCAL_LIST> g_RemotelocalList;
extern QList<APP_LIST> g_RemoteappList;
extern QList<PAAS_LIST> g_RemotepaasList;
RunApp * RunApp::_runApp = NULL;

RunApp * RunApp::getRunApp()
{
    if (!RunApp::_runApp)
        RunApp::_runApp = new RunApp();
    return _runApp;
}

RunApp::RunApp(QObject *parent)
    :QObject(parent)
{
    /*****app******/
#ifdef Q_WS_WIN
    mylib = new QLibrary("DllClientEngineMain.dll");   //
    if (mylib->load())              //check whether load successfully
    {
        //QMessageBox::information(NULL,"OK","DLL load is OK!");

        m_dllInitClass=(Dll_InitClass)mylib->resolve("DllCEM_InitClass");    //get function add()
        m_dllCloseClass=(Dll_CloseClass)mylib->resolve("DllCEM_CloseClass");
        m_dllCloseAppAll=(Dll_CloseAppAll)mylib->resolve("DllCEM_closeAppAll");
        m_dllStartAppEx=(Dll_StartAppEx)mylib->resolve("DllCEM_startAppEx");
    }
    else
        QMessageBox::information(NULL,"NO","DllClientEngineMain is not loaded!");
#endif

    _local = LocalAppList::getList();

    connect(&_communi, SIGNAL(appRun()), this, SLOT(runServerApp()));
}


RunApp::~RunApp()
{
}

void RunApp::runApp(const QString &uniqueName)
{

    if (uniqueName.startsWith("0_"))
    {
        LocalApp *la = _local->getAppByUniqueName(uniqueName);
        if (!la)
            return;

        char command[2048];

        QFileInfo link(la->execname());

        qDebug() << "la->execname():" << la->execname();
        if (!link.exists())
        {
            AppMessageBox box(true, NULL);
            box.setText("指向的应用或快捷方式已不存在\n点击\"确定\"删除图标");
            if (box.exec())
                LocalAppList::getList()->delApp(la->uniqueName());
            return;
        }
        sprintf(command, "\"%s\"", la->execname().toLocal8Bit().data());

        ShellExecute(NULL, "open", command, "", "", SW_SHOW);

        return;
    }
    else if(uniqueName.startsWith("1_"))
    {
        bool isRmote = false;
        for (int i = 0; i < g_myVappList.count(); i++)
        {
//                qDebug() << "g_myVappList.at(i).name" << g_myVappList.at(i).name;
//                qDebug() << "text" << text;
            if(g_myVappList.at(i).id == uniqueName.right(uniqueName.length() - 2))
            {
                _appid = g_myVappList.at(i).id;
                Commui._type = g_myVappList.at(i).type;
                isRmote = true;
                appText = g_myVappList.at(i).name;
            }
        }
        if(isRmote)
        {
            _communi.loadBalance(Commui._name,_appid);
            if(_communi._isNetError)
            {
                QMessageBox::warning(NULL, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
            }
            return;
        }

        return;
    }
    else if (uniqueName.startsWith("2_"))
    {
        for (int i = 0; i < g_RemotepaasList.count(); i++)
        {
            if ("2_" + g_RemotepaasList.at(i).name == uniqueName)
            {
                QDesktopServices::openUrl(QUrl(g_RemotepaasList.at(i).urls));
            }
        }

        return;
    }
}
void RunApp::runServerApp()
{

    QString tmp;
    QString pram;
    QString _appid;

    qDebug()<<appText;
    for (int i = 0; i < g_myVappList.count(); i++)
    {
        if(g_myVappList.at(i).name == appText)
        {
            qDebug() << g_myVappList.at(i).name;

            _appid = g_myVappList.at(i).id;
            Commui._type = g_myVappList.at(i).type;
        }
    }


    if(_communi.errID == "10000")
    {
        if (m_dllInitClass && m_dllCloseClass && m_dllCloseAppAll && m_dllStartAppEx )                  //??????? add() ??
        {
            char buf[100];
            DWORD len = GetLogicalDriveStringsA(sizeof(buf)/sizeof(char),buf);
            //
            //
            string strDiskVolume;
            string strFixed;
            string strCDROM;
            string strRemovable;

            for (char* s=buf; *s; s+=strlen(s)+1)
            {
                string strDrivePath = s;
                // GetDriveType
                //
                UINT uDriveType;
                uDriveType = GetDriveTypeA(strDrivePath.c_str());
                strDiskVolume.append(strDrivePath);

                if (uDriveType == DRIVE_CDROM)
                {
                    strCDROM.append(strDrivePath);
                    //qDebug() << "cdrom : " << strDrivePath.c_str();
                }
                else if (uDriveType == DRIVE_REMOVABLE)//mass storage
                {
                    strRemovable.append(strDrivePath);
                    //qDebug() << "removable : " << strDrivePath.c_str();
                }
                else if (uDriveType == DRIVE_FIXED)
                {
                    strFixed.append(strDrivePath);
                    //qDebug() << "fixed : " << strDrivePath.c_str();
                }
            }

            qDebug()<<"string:"<<QString::fromLocal8Bit(strDiskVolume.c_str())<<endl;
            QString diskpath;
            //            if(Commui._driver_loadClientDriverOnLogin == "1")
            //            {
            //                diskpath = QString::fromLocal8Bit(strDiskVolume.c_str());
            //            }
            //            else
            //            {
            //                if(Commui._driver_mapClientStorageOnLogin_check_harddisk == 0)
            //                    diskpath = QString::fromLocal8Bit(strFixed.c_str());
            //                if(Commui._driver_mapClientStorageOnLogin_check_CD == 0)
            //                    diskpath = diskpath + QString::fromLocal8Bit(strCDROM.c_str());
            //            }
            if(Commui._driver_mapClientStorageOnLogin_check_harddisk == "1")
            {
                diskpath = QString::fromLocal8Bit(strFixed.c_str());
                //qDebug() << "fixed : " << strFixed.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_CD == "1")
            {
                diskpath = diskpath + QString::fromLocal8Bit(strCDROM.c_str());
                //qDebug() << "CDROM : " << strCDROM.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_floppy == "1")
            {
                diskpath = diskpath + QString::fromLocal8Bit(strRemovable.c_str());
                //qDebug() << "Removable : " << strRemovable.c_str();
            }
            if(Commui._driver_mapClientStorageOnLogin_check_shareStorage == "1")
            {
                //diskpath = diskpath + QString::fromLocal8Bit(strRemovable.c_str());
            }

            qDebug()<<"diskpath:"<<diskpath;
            diskpath.replace("\\" , ",");
            qDebug()<<"count:"<<diskpath.count();
            diskpath.remove((diskpath.count() - 1),1);
            qDebug()<<"diskPath:"<<diskpath;
            qDebug()<<"cdrom:"<<QString::fromLocal8Bit(strCDROM.c_str());

            tmp = Commui._path;
            pram= Commui._param;
            const wchar_t* _param=reinterpret_cast<const wchar_t*>(pram.utf16());
            const wchar_t* ty = reinterpret_cast<const wchar_t*>(tmp.utf16());
            qDebug() << "passwd::" << Commui._pswd;
            //Commui._pswd = decryptRsa();
            qDebug() << "passwd::" << Commui._pswd;
            const wchar_t* pswd = reinterpret_cast<const wchar_t*>(Commui._pswd.utf16());
            const wchar_t* localime = reinterpret_cast<const wchar_t*>(Commui._redirect_inputMethodRedirect.utf16());

            if(Commui._userType == "1" || Commui._userType == "2")
            {
                Commui._applogin = Commui._domain +  "\\" + Commui._name;
            }
            else
            {
                Commui._applogin = Commui._name;
            }
            const wchar_t* name = reinterpret_cast<const wchar_t*>(Commui._applogin.utf16());
            const wchar_t* appName = reinterpret_cast<const wchar_t*>(appText.utf16());
            const wchar_t* ipPort = reinterpret_cast<const wchar_t*>(Commui._rdpIpPort.utf16());
            const wchar_t* audio = reinterpret_cast<const wchar_t*>(Commui._audio_clientAudioOutput.utf16());
            const wchar_t* _diskpath = reinterpret_cast<const wchar_t*>(diskpath.utf16());
            qDebug()<<"name:"<<Commui._name;
            qDebug()<<"ipPort:"<<Commui._rdpIpPort<<endl;
            bool tray;
            if("1" == Commui._type)
            {
                tray = true;
            }
            else
            {
                tray = false;
            }

            bool ok;
            m_dllInitClass(L"", L"");
            qDebug()<<"audio::"<<Commui._audio_clientAudioOutput.toULong(&ok,10);
            int _audio;
            if(Commui._audio_clientAudioOutput.toULong(&ok,10)==1)
            {
                _audio = 3;
            }
            else
            {
                _audio = 1;
            }


            DWORD colorDepth=24;
            switch (Commui._viewLimited_colorDepth.toUInt(&ok,10))
            {
            case 8:
                colorDepth = 8;
                break;
            case 16:
                colorDepth = 16;
                break;
            case 24:
                colorDepth = 24;
                break;
            case 32:
                colorDepth = 32;
                break;
            default:
                colorDepth = 24;
                break;
            }

            //    qDebug()<<"_param"<<pram;
            BOOL FontSmooth;
            if(Commui._FontSmooth=="1")
                FontSmooth=true;
            else
                FontSmooth=false;


            int _DesktopWidth,_DesktopHeight;
            /*if(Commui._viewLimited_resolution_width=="0")
             {
                 _DesktopWidth=qApp->desktop()->rect().width();
                 _DesktopHeight=qApp->desktop()->rect().height();
             }
            else
            {*/
            _DesktopWidth=Commui._DesktopWidth.toULong(&ok,10);
            _DesktopHeight=Commui._DesktopHeight.toULong(&ok,10);
            //}

            DWORD dwForbidApps = 0x00;
            if(Commui._applicationForbid_cmd == "1")
            {
                dwForbidApps |= 0x0001;
            }
            if(Commui._applicationForbid_explorer == "1")
            {
                dwForbidApps |= 0x0002;
            }
            if(Commui._applicationForbid_SysManage == "1")
            {
                dwForbidApps |= 0x0004;
            }
            if(Commui._applicationForbid_help == "1")
            {
                dwForbidApps |= 0x0008;
            }

            if(tray)
                qDebug()<<"tray program";
            else
                qDebug()<<"no tray program.";

            qDebug()<<"-----dwForbidApps-" << dwForbidApps;
            m_dllStartAppEx(0, 0, Commui._redirect_inputMethodRedirect.toULong(&ok,10), _audio, \
                            colorDepth, 0, Commui._type.toULong(&ok, 10), 0, L"{123654789}",pswd, \
                            name, ipPort, appName, ty,\
                            L"", _diskpath, _param , Commui._interface_COM.toULong(&ok,10),\
                            Commui._printer_supportVirtualPrint.toULong(&ok,10) , tray,\
                            _DesktopHeight ,_DesktopWidth,Commui._other_closeClipboard.toULong(&ok,10),\
                            FontSmooth,Commui._RdpPort.toULong(&ok,10),dwForbidApps);
        }
        else
        {
            QMessageBox::information(NULL,"NO","Linke to Function is not OK!!!!");
            return;  //???????
        }
    }
    else
    {
        QMessageBox::warning(NULL, tr("Get load balance info failed"), _communi.errInfo, tr("OK"));
        return;
    }
}
