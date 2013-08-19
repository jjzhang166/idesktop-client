#include <QApplication>
#include <QPixmap>
#include <QCheckBox>
#include <QDialog>
#include <QDebug>
#include <QPushButton>
#include <QCryptographicHash>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QBitmap>
#include <QByteArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QDir>
#include <QMenu>
#include <QTransform>
//#include <QApplication>
#include <QLibrary>
#include <QTextCodec>
#include <QSettings>
#include <QProcess>
#include <QFile>

#include "logindialog.h"
#include "hintlineEdit.h"
#include "dynamicbutton.h"
#include "config.h"
#include "qtipaddressedit/qipaddressedit.h"
#include "qtipaddressedit/qipaddressedititem.h"
//#include "vappvirtualdesktop.h"
#include <windows.h>
#include <shellapi.h>
#include "ShlObj.h "

#include "appmessagebox.h"

#include "public.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include <QtScript/QScriptValue>

//#ifdef Q_WS_WIN
//QLibrary *mylib;   //
//Dll_CloseAppAll m_dllCloseAppAll;
//#endif

#define FONTSIZE 10
#define KEY "\\Windows\\CurrentVersion\\App Paths\\"

#define PAASURL "http://192.168.30.128/dongfang.php"
//#define PAASURL "http://192.168.49.243:8080/CloudManage/rest/service/getUserAllService"
/****************************************************************/
//wangyaoli
extern QString serverip;
/****************************************************************/
QString VacServer;
QString VacPort;
QString VacUser;
QString VacPassword;

QString PaasServer;

QString USERNAME;

QList<APP_LIST> g_myVappList;
QList<PAAS_LIST> g_myPaasList;

QList<APP_LIST> g_RemoteappList;
QList<PAAS_LIST> g_RemotepaasList;
QList<LOCAL_LIST> g_RemotelocalList;

QString WIN_LOCAL_IconPath;
QString WIN_VAPP_IconPath;
QString WIN_PAAS_IconPath;
//QString iconDirPath;
QString WIN_TtempPath;
QString xmlPath;


typedef void (*DLL_getApp2)();
DLL_getApp2 my_getApp2;

int ICON_TYPE;

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint)
    , _titlePressed(false)
    , _vacfinished(false)
    , _paasFinished(false)
    , _flip(false)
    ,_isSetting(false)
{

    QTextCodec *codec = QTextCodec::codecForName("utf-8"); //System
    QTextCodec::setCodecForCStrings(codec);

    setAttribute(Qt::WA_TranslucentBackground, true);
    //    setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
    setAutoFillBackground(true);
    setFixedSize(413, 315);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0x00));
    setPalette(pal);

    QPixmap closeIcon(":images/dlgCloseIcon.png");
    QPixmap closeIconHover(":images/dlgCloseIconHover.png");
    DynamicButton *closeButton = new DynamicButton(closeIcon, closeIconHover, this);
    closeButton->setGeometry(width() - 10 - CLOSE_WIDTH, 10, CLOSE_WIDTH, CLOSE_HEIGHT);

    QPixmap minIcon(":images/dlgMinIcon.png");
    QPixmap minIconHover(":images/dlgMinIconHover.png");
    DynamicButton *minButton = new DynamicButton(minIcon, minIconHover, this);
    minButton->setGeometry(width() - 10 * 2 - CLOSE_WIDTH * 2, 10, CLOSE_WIDTH, CLOSE_HEIGHT);

    QPixmap configurationIcon = QPixmap(":images/setup_normal.png").scaled(10,10);
    QPixmap configurationIconHover = QPixmap(":images/setup_hover.png").scaled(10,10);
    configurationButton = new DynamicButton(configurationIcon, configurationIconHover, this);
    configurationButton->setGeometry(width() - 10 * 3 - CLOSE_WIDTH * 3, 10, 10, 10);
    configurationButton->setVisible(true);


    ipAddrEdit = new QIpAddressEdit(serverip, this);
    ipAddrEdit->setGeometry(75, 55, 208, 35);
    ipAddrEdit->setVisible(false);

    userEdit = new HintLineEdit(tr("请输入您的用户名"), ":images/user_icon.png", this);
    passEdit = new HintLineEdit(tr("请输入您的密码"), ":images/password_icon.png", this);
    userEdit->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
    passEdit->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));

    passEdit->setEchoMode(QLineEdit::Password);
    userEdit->setTextMargins(5, 5, 2, 5);
    passEdit->setTextMargins(5, 5, 2, 5);
    userEdit->setGeometry((width() - 208) / 2, 125, 208, 27);
    passEdit->setGeometry((width() - 208) / 2, 165, 208, 27);

    userEdit->setText(QString("demo"));
    passEdit->setText(QString("abc_123"));

    QPixmap loginButton(":images/login_btn.png");
    QPixmap loginButtonHover(":images/login_btn_hover.png");

    submit = new DynamicButton(loginButton, loginButtonHover, this);
    submit->setGeometry((width() - passEdit->width()) / 2, passEdit->pos().y() + passEdit->height() + 20, \
                        89, 34);
    //    submit->setVisible(false);
    QPixmap cancelButton(":images/cancel_btn.png");
    QPixmap cancelButtonHover(":images/cancel_btn_hover.png");

    cancelBtn = new DynamicButton(cancelButton, cancelButtonHover, this);
    cancelBtn->setGeometry(submit->pos().x() + submit->width() + 20, passEdit->pos().y() + passEdit->height() + 20,\
                           89, 34);
    //    cancelBtn->setVisible(false);
    //setting slot by zj
    connect(configurationButton, SIGNAL(clicked()), this, SLOT(settingSlot()));
    settingUi();


    remoteAuth = new QCheckBox(this);
    remoteAuth->setChecked(true);  //true
    remoteAuth->setText(tr("远程验证"));
    remoteAuth->setGeometry(181, 230, 80, 35);
    remoteAuth->setVisible(false);

    serverAddr = new QComboBox(this);
    serverAddr->setGeometry(75, 60, 208, 25);
    serverAddr->setEditable(true);
    serverAddr->hide();

    QSqlQuery query = QSqlDatabase::database("local").exec("SELECT addr FROM addrs ORDER BY count DESC;");
    while (query.next())
        serverAddr->addItem(query.value(0).toString());
    _nam = new QNetworkAccessManager(this);
    //vac
    _commui = new commuinication();
    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));

    //paas
    _paasCommui = new PaasCommuinication();
    connect(_paasCommui, SIGNAL(done()), this, SLOT(onPaasDone()));

//    QDir dir(Config::get("UserDir"));
//    QFileInfo fi(Config::get("Profile"));
//    if (dir.exists(fi.fileName()))
//    {
//        /*判断文件是否存在*/
//        Config::getServerIp();
//        qDebug()<<serverip;
//        ipAddrEdit->setText(serverip);
//    }

    //get serverip by zj


    _lShowAction = new QAction(tr("显示"), this);
    _lShowAction->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
    _lQuitAction = new QAction(tr("退出"), this);
    _lQuitAction->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));

    _loginMenu = new QMenu(this);
    _loginMenu->addAction(_lShowAction);
    _loginMenu->addSeparator();
    _loginMenu->addAction(_lQuitAction);

    _dQuitAction = new QAction(tr("退出"), this);
    _dShowAction = new QAction(tr("显示"), this);
    _dHideAction = new QAction(tr("隐藏"), this);
//    _dVacServer = new QAction(tr("设置服务器地址"), this);
    _dQuitAction->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
    _dShowAction->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
    _dHideAction->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
//    _dVacServer->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));

    _dashboardMenu = new QMenu(this);
//    _dashboardMenu->addAction(_dVacServer);
//    _dashboardMenu->addSeparator();
    _dashboardMenu->addAction(_dShowAction);
    _dashboardMenu->addAction(_dHideAction);
    _dashboardMenu->addSeparator();
    _dashboardMenu->addAction(_dQuitAction);

    _tray = new QSystemTrayIcon(this);
    QIcon icon(":images/icon.png");
    _tray->setIcon(icon);
    _tray->setContextMenu(_loginMenu);
    _tray->show();

    _timeLine = new QTimeLine(500, this);

    connect(_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(updateFlip(qreal)));
    connect(_lShowAction, SIGNAL(triggered()), this, SLOT(normal()));
    connect(_lQuitAction, SIGNAL(triggered()), this, SLOT(reject()));
    connect(_dQuitAction, SIGNAL(triggered()), this, SIGNAL(dQuit()));
    connect(_dShowAction, SIGNAL(triggered()), this, SIGNAL(dShow()));
    connect(_dHideAction, SIGNAL(triggered()), this, SIGNAL(dHide()));
    connect(_dShowAction, SIGNAL(triggered()), this, SIGNAL(pShow()));
    connect(_dHideAction, SIGNAL(triggered()), this, SIGNAL(pHide()));
    //    connect(showAction, SIGNAL(triggered()), panel, SLOT(show()));
    //    connect(hideAction, SIGNAL(triggered()), panel, SLOT(hide()));
//    connect(_dVacServer, SIGNAL(triggered()), this, SIGNAL(dVacServer()));
    connect(_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SIGNAL(dActivated(QSystemTrayIcon::ActivationReason)));

    connect(submit, SIGNAL(clicked()), this, SLOT(auth()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));
    connect(minButton, SIGNAL(clicked()), this, SLOT(minimized()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
//    connect(configurationButton, SIGNAL(clicked()), this, SLOT(flip()));
    connect(userEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(passEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoginFinished(QNetworkReply*)));
    connect(this, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

}

void LoginDialog::paintEvent(QPaintEvent *event)
{
    //    QPainter painter(this);
    painter.begin(this);

    //    QTransform transform;
    //    transform.translate(width() / 2, height() / 2);
    //    transform.rotate(_finalyrot,Qt::YAxis);
    //    transform.translate(-1 * width() / 2, -1 * height() / 2);
    //    painter.setWorldTransform(transform);

    QPixmap bg(":images/bg_loginBox.png");
    painter.drawPixmap(0, 0, width(), height(), bg.scaled(width(), height()));
    //    painter.drawPixmap(22, 17, QPixmap(":images/logo_login.png"));
    //setting
    if(!_isSetting)
    {
        painter.setPen(Qt::gray);
        painter.setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 10, QFont::Normal));
        //    painter->setFont(QFont(QString::fromLocal8Bit("宋体"),13,-1,false));
        painter.drawText(43, 143, tr("用户名"));
        painter.drawText(43, 183, tr("密  码"));

    }else{
        painter.setPen(Qt::gray);
        painter.setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 10, QFont::Normal));
        painter.drawText(13, 133, tr("验证服务器地址"));
        painter.drawText(13, 163, tr("应用服务器地址"));
        painter.drawText(13, 193, tr("平台服务器地址"));

    }

    painter.setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 9, QFont::Normal));
    //    painter.drawPixmap(74, 104, QPixmap(":images/login_input.png"));
    //    painter.drawPixmap(74, 164, QPixmap(":images/login_input.png"));

    //    painter.drawPixmap(74, 54, QPixmap(":images/login_input.png"));
    if(!_vserverError.isEmpty())
    {
            painter.setPen(QPen(QColor("#ff0000")));
            painter.drawText((width() - 188) / 2, 155, _vserverError);
    }
    if(!_paasError.isEmpty())
    {
            painter.setPen(QPen(QColor("#ff0000")));
            painter.drawText((width() - 188) / 2, 213, _paasError);
    }
    if (!_uerror.isEmpty()) {
        painter.setPen(QPen(QColor("#ff0000")));
        painter.drawText((width() + userEdit->width()) / 2 + 2, 140, _uerror);
    }
    if (!_perror.isEmpty()) {
        painter.setPen(QPen(QColor("#ff0000")));
        painter.drawText((width() + passEdit->width()) / 2 + 7, 180, _perror);
    }
    if (!_cerror.isEmpty()) {
        painter.setPen(QPen(QColor("#ff0000")));
        painter.drawText((width() - userEdit->width()) / 2, submit->pos().y() + submit->height() + 25, _cerror);
    }
    if (!_cmsg.isEmpty()) {
        painter.setPen(QPen(QColor("#299cfd")));
        painter.drawText((width() - userEdit->width()) / 2, submit->pos().y() + submit->height() + 25, _cmsg);
    }

    //    painter.setTransform(_tran);

    //    painter.setTransform(_t,true);

    painter.end();
    QDialog::paintEvent(event);
}

void LoginDialog::userError(QString text)
{
    _uerror = text;
    repaint();
}

void LoginDialog::passError(QString text)
{
    _perror = text;
    repaint();
}

void LoginDialog::connMsg(QString text)
{
    _cmsg = text;
    repaint();
}

void LoginDialog::connError(QString text)
{
    _cerror = text;
    repaint();
}

void LoginDialog::debugPrintVariant (QVariant const & v)
{
    if (! v. isValid ())
    {
        qDebug () << "invalid";
    }
    if (QVariant:: List == v. type ())
    {
        qDebug () << "list";
        for (QList <QVariant>:: const_iterator i = v.toList (). begin (); i != v. toList (). end (); ++ i)
            debugPrintVariant (*i);
    }
    else if (QVariant:: Map == v. type ())
    {
        qDebug () << "map";
        for (QMap <QString, QVariant>:: const_iterator i = v. toMap (). begin (); i != v. toMap (). end (); ++ i)
        {
            qDebug () << "Key: " << i. key ();
            debugPrintVariant (i. value ());
        }
    }
    else
    {
        qDebug () << "type: " << v. type ();
        qDebug () << v. toString ();
    }
}
void LoginDialog::onLoginFinished(QNetworkReply *reply)
{
    QScriptValue sc;
    QScriptEngine engine;
    QString result;

    _finished = true;
    serverAddr->setEnabled(true);
    userEdit->setEnabled(true);
    passEdit->setEnabled(true);
    submit->setEnabled(true);
    remoteAuth->setEnabled(true);
    configurationButton->setEnable(true);
    connMsg("");

    QString jsonResult = reply->readAll();
    qDebug() << "jsonResult" << jsonResult;
    _authSuccess = false;

    sc = engine.evaluate ("JSON.parse").call(QScriptValue(), QScriptValueList() << jsonResult);

    if(sc.isObject())
    {
        result = sc.property("status").toVariant().toString();  //1
        qDebug() << result;

    }
    else
    {
        qDebug()<<"Invalid response: " << jsonResult;
        // fatal error
    }

    if (result == "1")   //  APPSTORE SUCCESS
    {
        _authSuccess = true;
    }
    else if (result == "-1")//USER NOT EXISTS
    {
        userError(tr("用户名或密码错误"));  //
    }
    else if (result == "-2")//USER IS EXISTS
    {
        userError(tr("其他用户已登录"));  //
    }
    else
    {
        connError(tr("连接服务器失败..."));

    }

    //    else if (result == "WRONG PASSWORD")
    //    {
    //        passError(tr("密码验证失败"));
    //    }
}

void GetAppList()
{
#if 0
    QLibrary dllLib("GetApp.dll");
    if(!dllLib.load())
    {
        qDebug()<<"load failed!";
        return;
    }
    else
    {
        qDebug()<<"load succeed!";
        my_getApp = (DLL_getApp)dllLib.resolve("GetApp");
        my_getApp2 = (DLL_getApp2)dllLib.resolve("GetApp2");
        if(my_getApp == NULL)
        {
            qDebug()<<"resolve failed!";
            return;
        }
        else
        {
            qDebug()<<"resolve succeed!";
            MY_APP_StructList* mylist = new MY_APP_StructList;
            char p[20];
            sprintf(p, "list add:%x", mylist);
            qDebug()<<"before my_getApp"<< p;
#if 1
            *mylist = my_getApp2();
#else
            my_getApp(mylist);
#endif
            sprintf(p, "list add:%x", mylist);
            qDebug()<<"after my_getApp"<< p;
            FILE* hf = NULL;
            hf = fopen("c:\\Temp\\log.txt", "wb");
            fputs("hello..", hf);
            fclose(hf);
            hf = NULL;
            for(MY_APP_StructIter it = mylist->begin(); it != mylist->end(); ++it)
            {
                hf = fopen("c:\\Temp\\log.txt", "a");
                //qDebug()<< "app name:" << (char*)it->strAppName;
                fputs("app name:", hf);
                fputs(it->strAppName, hf);
                fputs("\n", hf);
                fputs("app path:", hf);
                fputs(it->strExePath, hf);
                fputs("\n", hf);

                fclose(hf);
            }

        }
    }
#endif
}

void LoginDialog::auth()
{
    qDebug() << "****************************************";
    _uerror = "";
    _perror = "";
    _cerror = "";
    _cmsg = "";
    if (userEdit->text().isEmpty()) {
        userError(tr("用户名不能为空"));
        return;
    }
    if (passEdit->text().isEmpty()) {
        passError(tr("密码不能为空"));
        return;
    }
//    QString md5;
//    QByteArray bb;
//    bb = QCryptographicHash::hash(passEdit->text().toAscii(), \
//                                  QCryptographicHash::Md5);
//    md5.append(bb.toHex());

    QString statement = QString("select password from users where name='%1'")\
            .arg(userEdit->text());
    QSqlDatabase localDb = QSqlDatabase::database("local");

    if (remoteAuth->isChecked()) {
        /****************************************************************/
        //wangyaoli
        //serverip = ipEdit->text();
        serverip = ipAddrEdit->text();
        //        serverip = "192.168.30.64";
        if(true == serverip.isEmpty())
        {
            return;
        }
        Config::setServerIp();
        //Config::initiate();

        /****************************************************************/
        _finished = false;
        _authSuccess = false;
        //QString loginUrl = "http://" + serverAddr->currentText() + "/api/login";
        QString loginUrl = "http://" + verifyLEdit->text() + ":8080/idesktop/login.action";
        qDebug() <<"loginUrl"<<loginUrl;
        QString data = "username=" + userEdit->text() + "&password=" + passEdit->text() + "&tenant=0";
        _nam->post(QNetworkRequest(QUrl(loginUrl)), data.toAscii());
        connMsg(tr("正在连接服务器..."));
        serverAddr->setEnabled(false);
        userEdit->setEnabled(false);
        passEdit->setEnabled(false);
        submit->setEnabled(false);
        remoteAuth->setEnabled(false);
        configurationButton->setEnable(false);
        while(!_finished)
            QApplication::processEvents();
        if (!_authSuccess)
            return;

        QString replaceNamePwd = QString("REPLACE INTO users(name, password) values('%1', '%2');")\
                .arg(userEdit->text()).arg(passEdit->text());
        localDb.exec(replaceNamePwd);

        USERNAME = userEdit->text();

        //        QString statement = QString("select name from users");
        //        QSqlDatabase localDb = QSqlDatabase::database("local");

        //        QSqlQuery query = localDb.exec(statement);
        //        if (!query.next()) {
        //            QString replaceNamePwd = QString("insert into users ("\
        //                                             "name, password) values ( " \
        //                                             "\'%1\', \'%2\');")
        //                                    .arg(userEdit->text()).arg(passEdit->text());
        //            localDb.exec(replaceNamePwd);
        //        }
        //        else            // if(query.value(0).toString() != userEdit->text())
        //        {
        //            QString replaceNamePwd = QString("update users set name=%1, password=%2;")
        //                                    .arg(userEdit->text()).arg(passEdit->text());
        //            localDb.exec(replaceNamePwd);
        //        }


        /* TODO: check the return value */
    }
    else {
        QSqlQuery query = localDb.exec(statement);
        if (!query.next()) {
            userError(tr("该用户不存在"));
            return;
        }
        if (query.value(0).toString() != passEdit->text()) {
            passError(tr("密码验证失败"));
            return;
        }
    }

    //    _tray->setVisible(false);

    int count = 0;
    QString selectCount = QString("SELECT count FROM addrs where addr='%1';")\
            .arg(serverAddr->currentText());
    QSqlQuery query = localDb.exec(selectCount);
    if (query.next())
        count = query.value(0).toInt();
    QString replaceAddr = QString("REPLACE INTO addrs(addr, count) values('%1', %2);")\
            .arg(serverAddr->currentText()).arg(count + 1);
    localDb.exec(replaceAddr);
    //Config::set("Server", "http://" + serverAddr->currentText() + "/");
    //Config::set("password",md5);
    Config::set("password",passEdit->text());
    Config::set("User", userEdit->text());
    QDialog::accept();

    char folder[MAX_PATH] = {0};
    SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
    WIN_TtempPath = QString(folder);

    WIN_LOCAL_IconPath=WIN_TtempPath+"\\App Center\\Licons\\";
    WIN_VAPP_IconPath=WIN_TtempPath+"\\App Center\\Vicons\\";
    WIN_PAAS_IconPath=WIN_TtempPath+"\\App Center\\Picons\\";
    iniPath = WIN_TtempPath + "\\App Center\\app.ini";
    qDebug()<<"icon path:"<<WIN_VAPP_IconPath;

    getIconType();
    qDebug()<<"after getIconType";
    //local
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////



    //   GetAppList();


    ///////////////////////////////////////////////////
#if 1
    saveVacUserInfo();

    QLibrary dllLib("GetApp.dll");
    if(!dllLib.load())
    {
        qDebug()<< "GetLastError :" << GetLastError();
        qDebug()<<"load failed!";
    }
    else
    {
        qDebug()<<"load succeed!";
        my_getApp2 = (DLL_getApp2)dllLib.resolve("GetApp2");
        if(my_getApp2 == NULL)
        {
            qDebug()<< "GetLastError :" <<GetLastError();
            qDebug()<<"resolve failed!";

        }
        else
        {
            my_getApp2();
        }
    }
//    qDebug()<<"inipath:"<<iniPath;
    //vc ini
    char  chSectionNames[8192]={0};       //所有节名组成的字符数组
    char *pSectionName; //保存找到的某个节名字符串的首地址
    int i;       //i指向数组chSectionNames的某个位置，从0开始，顺序后移
    int j=0;      //j用来保存下一个节名字符串的首地址相对于当前i的位置偏移量
    int _id = 0;
    //int count=0;      //统计节的个数
    char _folder[MAX_PATH] = {0};
    char _iniPath[MAX_PATH] = {0};
    SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,_folder);
    sprintf(_iniPath, "%s\\App Center\\app.ini", _folder);
    DWORD m_lRetValue =::GetPrivateProfileSectionNames(chSectionNames,8192,_iniPath);
    qDebug()<<"num:"<<m_lRetValue;
    for(i=0;i<m_lRetValue;i++,j++)
    {
        if(chSectionNames[0]=='\0')
            break;       //如果第一个字符就是0，则说明ini中一个节也没有
        if(chSectionNames[i]=='\0')
        {
            pSectionName=&chSectionNames[i-j]; //找到一个0，则说明从这个字符往前，减掉j个偏移量，
            //就是一个节名的首地址
            j=-1;         //找到一个节名后，j的值要还原，以统计下一个节名地址的偏移量
            //赋成-1是因为节名字符串的最后一个字符0是终止符，不能作为节名
            //的一部分
            char strBuff[256];
            //在获取节名的时候可以获取该节中键的值，前提是我们知道该节中有哪些键。
            ::GetPrivateProfileString(pSectionName,"ExePath",NULL,strBuff,256,_iniPath);
            //qDebug()<<"name:"<<QString::fromLocal8Bit(pSectionName);     //把找到的显示出来
            //qDebug()<<"name:"<<QString(pSectionName).toLocal8Bit();
            //qDebug()<<"path:"<<QString::fromLocal8Bit(strBuff);
            //QString path = QString::fromLocal8Bit(strBuff);
            QString path = strBuff;

            QFileInfo fio(path);
            if (!fio.exists())
                continue;

//            qDebug()<<"path"<<strBuff;
            QFileInfo fi = QFileInfo(path);
            QString iPath(Config::get("IconDir"));
            iPath = iPath + fi.baseName();
            iPath += ".png"; //png

            QString localIconPath;

            QFile chkFile(iPath);
            if(!chkFile.exists())
            {
                chkFile.close();

                localIconPath = getLocalIcon(path);
                setIcon(WIN_LOCAL_IconPath, localIconPath);
                //            continue;
            }
            else
            {
                chkFile.close();
                localIconPath = iPath;
            }

            LOCAL_LIST tempLocalList;
            tempLocalList.id = _id;
            tempLocalList.iconPath = localIconPath;
            //qDebug() << "name-->pSectionName" << pSectionName;
//            tempLocalList.name = QString::fromLocal8Bit(pSectionName);


            //QTextCodec *codec = QTextCodec::codecForName("utf-8"); //utf-8

            //tempLocalList.name = codec->toUnicode(pSectionName);
            QString temp = pSectionName;
            temp.replace("]", "");
            tempLocalList.name = temp;
//            qDebug() << "tempLocalList.name" << tempLocalList.name;

            //tempLocalList.name = QString(pSectionName);
            tempLocalList.execname = path;

            QString md5;
            QByteArray bb;
            bb = QCryptographicHash::hash(path.toAscii(), \
                                          QCryptographicHash::Md5);
            md5.append(bb.toHex());

            tempLocalList.uniqueName = md5;

            g_RemotelocalList.append(tempLocalList);
            _id++;
            if(chSectionNames[i+1]==0)
            {
                break;      //当两个相邻的字符都是0时，则所有的节名都已找到，循环终止
            }
        }
    }
    //qDebug()<<"id"<<_id;
    QFile b(iniPath);
    b.remove();
    //end
#else
    QSettings reg(QSettings::NativeFormat, \
                  QSettings::SystemScope, "Microsoft", KEY);
    for (int i = 0; i < reg.childGroups().count(); i++) {
        QSettings tmp(QSettings::NativeFormat, \
                      QSettings::SystemScope, "Microsoft", \
                      KEY + reg.childGroups().at(i));
        //        if (tmp.value("DisplayName").toString() == display) {
        //            return tmp.value("UninstallString").toString();
        //        }
        QString path = tmp.value(".").toString();

        if (path.isEmpty())
        {
            //            _addAppState = false;
            continue;
        }

        path.replace("\"\"", "\"");

        if (path.startsWith("%"))
            continue;

        //        QStringList environment = QProcess::systemEnvironment();
        //        QString str;
        //        foreach(str,environment)
        //        {
        //            if (str.startsWith("PATH="))
        //            {
        //                ui.textEdit->append(str);
        //                break;
        //            }
        //        }

        //        qDebug() << path;
        QFileInfo fi = QFileInfo(path);
        QString iPath(Config::get("IconDir"));
        iPath = iPath + fi.baseName();
        iPath += ".png"; //png

        QString localIconPath;

        QFile chkFile(iPath);
        if(!chkFile.exists())
        {
            chkFile.close();

            localIconPath = getLocalIcon(path);
            setIcon(WIN_LOCAL_IconPath, localIconPath);
            //            continue;
        }
        else
        {
            chkFile.close();
            localIconPath = iPath;
        }

        LOCAL_LIST tempLocalList;
        tempLocalList.id = i;
        tempLocalList.iconPath = localIconPath;
        QFileInfo info = QFileInfo(localIconPath);
        tempLocalList.name = info.baseName();
        tempLocalList.execname = path;

        g_RemotelocalList.append(tempLocalList);

    }
#endif
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //vac
    //    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));

    updateVacServer();
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, GetSystemInfo());
    while (!_vacfinished)
        QApplication::processEvents();
    _vacfinished = false;
    qDebug()<<"login3";

    getVac();

    getPaas(true);

}

void LoginDialog::onDone()
{
    //get the Application List;
    _vacfinished = true;
}

void LoginDialog::onPaasDone()
{
    //get the Application List;
    _paasFinished = true;
}

void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    if (QRect(10, 10, width(), height()).contains(event->pos())) {
        _titlePressed = true;
        startDrag = event->pos();
    }
}

void LoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
    _titlePressed = false;
    event->ignore();
}

void LoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (_titlePressed) {
        QPoint newPoint = pos() + event->pos() - startDrag;
        move(newPoint);
    }
}

QString LoginDialog::GetSystemInfo()
{
    //login ivapp sm
    QString sysInfo;
#ifdef Q_WS_WIN
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_2000:
        qDebug()<<"System info:windows 2000 \n";
        sysInfo="windows 2000";
        break;
    case QSysInfo::WV_2003:
        qDebug()<<"System info:windows 2003 \n";
        sysInfo="windows 2003";
        break;
    case QSysInfo::WV_XP:
        qDebug()<<"System info:windows xp \n";
        sysInfo="windows xp";
        break;
    case QSysInfo::WV_VISTA:
        qDebug()<<"System info:windows vista \n";
        sysInfo="windows vista";
        break;
    case QSysInfo::WV_WINDOWS7:
        qDebug()<<"System info:windows 7 \n";
        sysInfo="windows 7";
        break;
    default:
        qDebug()<<"System info:windows 7 \n";
        sysInfo="windows 7";
        break;

    }
#endif
#ifdef Q_WS_X11
    sysInfo = "linux";
#endif
    return sysInfo;
}


void LoginDialog::getIconType()
{
    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT type FROM sizetype where id=1;"));
    while (query.next())
        ICON_TYPE = query.value(0).toInt();

}

//setting by zj
void LoginDialog::settingUi()
{
    //setting by zj
    verifyLEdit = new HintLineEdit(tr("请输入验证服务器地址"), ":images/user_icon.png", this);
    vacLEdit = new HintLineEdit(tr("请输入应用服务器地址"), ":images/user_icon.png", this);
    paasLEdit = new HintLineEdit(tr("请输入平台服务器地址"), ":images/password_icon.png", this);
    vacPortLEdit = new HintLineEdit(tr("端口"), ":images/password_icon.png", this);
    verifyLEdit->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
    vacLEdit->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
    paasLEdit->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));
    vacPortLEdit->setFont(QFont(QString::fromLocal8Bit("微软雅黑"), FONTSIZE, QFont::Normal));


    verifyLEdit->setTextMargins(5, 5, 2, 5);
    vacLEdit->setTextMargins(5, 5, 2, 5);
    paasLEdit->setTextMargins(5, 5, 2, 5);
    vacPortLEdit->setTextMargins(5, 5, 2, 5);
    verifyLEdit->setGeometry((width() - 188) / 2, 115, 188, 27);
    vacLEdit->setGeometry((width() - 188) / 2, 145, 188, 27);
    paasLEdit->setGeometry((width() - 188) / 2, 175, 188, 27);
    vacPortLEdit->setGeometry((width() - 188) / 2 + 195, 145, 50, 27);
    QPixmap loginButton(":images/login_save_normal.png");
    QPixmap loginButtonHover(":images/login_save_hover.png");
    QPixmap cancelButton(":images/cancel_btn.png");
    QPixmap cancelButtonHover(":images/cancel_btn_hover.png");

    saveButton = new DynamicButton(loginButton, loginButtonHover, this);
    saveButton->setGeometry((width() - passEdit->width()) / 2, passEdit->pos().y() + passEdit->height() + 30, \
                            89, 34);
    //saveButton->hide();
    returnButton = new DynamicButton(cancelButton, cancelButtonHover, this);
    returnButton->setGeometry(submit->pos().x() + submit->width() + 20, passEdit->pos().y() + passEdit->height() + 30,\
                              89, 34);
    //returnButton->hide();
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveSettingSlot()));
    connect(returnButton, SIGNAL(clicked()), this, SLOT(returnSlot()));
    VacServer = "";
    VacPort = "";
    VacUser = "";
    VacPassword = "";
    PaasServer = "";
    _vserverError = "";
    _paasError = "";
    readSetting();
    //updateUi();
    //end
}

void LoginDialog::readVerifyServer()
{
    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT verifyServer FROM paasservers where id=1;"));
    while (query.next())
    {
        serverip = query.value(0).toString();

    }
}

void LoginDialog::readSetting()
{
    updateVacServer();
    qDebug()<<VacServer.isEmpty()<<VacServer;
    if(VacServer.isEmpty() || VacPort.isEmpty() || PaasServer.isEmpty() || serverip.isEmpty())
    {
        settingSlot();
    }else
    {
        updateUi();
        verifyLEdit->setText(serverip);
        vacLEdit->setText(VacServer);
        vacPortLEdit->setText(VacPort);
        paasLEdit->setText(PaasServer);
    }
}

void LoginDialog::updateUi()
{
    if(_isSetting)
    {

        userEdit->hide();
        passEdit->hide();
        submit->hide();
        cancelBtn->hide();

        _uerror = "";
        _perror = "";
        _cerror = "";
        _cmsg = "";

        _tempVerifyIp = verifyLEdit->text();
        _tempVacIp = vacLEdit->text();
        _tempPaasIp = paasLEdit->text();
        _tempVacPort = vacPortLEdit->text();

        verifyLEdit->show();
        vacLEdit->show();
        paasLEdit->show();
        vacPortLEdit->show();
        saveButton->show();
        returnButton->show();
        verifyLEdit->setFocus();
    }else{

        verifyLEdit->setText(_tempVerifyIp);
        vacLEdit->setText(_tempVacIp);
        paasLEdit->setText(_tempPaasIp);
        vacPortLEdit->setText(_tempVacPort);

        _tempVerifyIp = "";
        _tempVacIp = "";
        _tempPaasIp = "";
        _tempVacPort = "";

        verifyLEdit->hide();
        vacLEdit->hide();
        paasLEdit->hide();
        vacPortLEdit->hide();
        saveButton->hide();
        returnButton->hide();
        passEdit->show();
        userEdit->show();
        submit->show();
        cancelBtn->show();
    }

}

void LoginDialog::VserverError(QString error)
{
    _vserverError = error;
    repaint();
}

void LoginDialog::PaasError(QString error)
{
    _paasError = error;
    repaint();
}

void LoginDialog::saveVacUserInfo()
{
    QSqlQuery query(QSqlDatabase::database("local"));
    QString updateVacData = QString("update vacservers set name=\'%1\',password=\'%2\' where id=1;")\
            .arg(userEdit->text()).arg(passEdit->text());
    if(!query.exec(updateVacData)) {
        qDebug() <<"saveVacUserInfo query failed";
    }
}

void LoginDialog::settingSlot()
{
    _isSetting = !_isSetting;
    _vserverError = "";
    _paasError = "";
    updateUi();
    repaint();
}

void LoginDialog::saveSettingSlot()
{
    qDebug()<<"saveSettingSlot";
    _vserverError = "";
    _paasError = "";
    if(verifyLEdit->text().isEmpty())
    {
        PaasError(tr("验证服务器地址不能为空"));
        return;
    }
    if (vacLEdit->text().isEmpty()) {
        PaasError(tr("应用服务地址不能为空"));
        return;
    }
    if (vacPortLEdit->text().isEmpty()) {
        PaasError(tr("应用服务端口号不能为空"));
        return;
    }
    if (paasLEdit->text().isEmpty()) {
        PaasError(tr("平台服务器地址不能为空"));
        return;
    }

    QSqlQuery query(QSqlDatabase::database("local"));
    if(VacServer.isEmpty() || PaasServer.isEmpty())
    {

        QString qstrVacS = QString("insert into vacservers ("\
                                   "id, server, port, name, password) values ("\
                                   "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\');")\
                .arg(1).arg(vacLEdit->text()).arg(vacPortLEdit->text().toInt()).arg("").arg("");
        if(!query.exec(qstrVacS)) {
            qDebug() <<"qstrVacS query failed";
        }
        QString qstrPaasS = QString("insert into paasservers ("\
                                    "id, server, verifyServer) values ("\
                                    "\'%1\', \'%2\', \'%3\');")\
                .arg(1).arg(paasLEdit->text()).arg(verifyLEdit->text());
        if(!query.exec(qstrPaasS)) {
            qDebug() <<"qstrPaasS query failed";
        }
    }else{
        QString updateVacData = QString("update vacservers set server=\'%1\',port=\'%2\' where id=1;")\
                .arg(vacLEdit->text()).arg(vacPortLEdit->text().toInt());
        QString updatePaasData = QString("update paasservers set server=\'%1\',verifyServer=\'%2\' where id=1;")\
                .arg(paasLEdit->text()).arg(verifyLEdit->text());
        if(!query.exec(updateVacData)) {
            qDebug() <<"updateVacData query failed";
        }
        if(!query.exec(updatePaasData)) {
            qDebug() <<"updatePaasData query failed";
        }

    }

    updateVacServer();
   // returnSlot();

    verifyLEdit->setText(serverip);
    vacLEdit->setText(VacServer);
    paasLEdit->setText(PaasServer);
    vacPortLEdit->setText(VacPort);

    verifyLEdit->hide();
    vacLEdit->hide();
    paasLEdit->hide();
    vacPortLEdit->hide();
    saveButton->hide();
    returnButton->hide();
    passEdit->show();
    userEdit->show();
    submit->show();
    cancelBtn->show();
    _isSetting = false;

    repaint();
}

void LoginDialog::returnSlot()
{
    qDebug()<<"return";

    verifyLEdit->setText(_tempVerifyIp);
    vacLEdit->setText(_tempVacIp);
    paasLEdit->setText(_tempPaasIp);
    vacPortLEdit->setText(_tempVacPort);

    verifyLEdit->hide();
    vacLEdit->hide();
    paasLEdit->hide();
    vacPortLEdit->hide();
    saveButton->hide();
    returnButton->hide();
    passEdit->show();
    userEdit->show();
    submit->show();
    cancelBtn->show();
    _isSetting = false;


    _tempVerifyIp = "";
    _tempVacIp = "";
    _tempPaasIp = "";
    _tempVacPort = "";

    repaint();
}
//end

void LoginDialog::updateVacServer()
{
    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT server,port,name,password FROM vacservers where id=1;"));
    while (query.next())
    {
        VacServer = query.value(0).toString();
        VacPort = query.value(1).toString();
        VacUser = query.value(2).toString();
        VacPassword = query.value(3).toString();
    }
    QSqlQuery query2 = \
            QSqlDatabase::database("local").exec(QString("SELECT verifyServer,server FROM paasservers where id=1;"));
    while (query2.next())
    {
        //by zj
        serverip = query2.value(0).toString();
        PaasServer = query2.value(1).toString();
        ipAddrEdit->setText(serverip);
    }
}



void LoginDialog::dialogAccepted()
{
    _tray->setContextMenu(_dashboardMenu);
}

void LoginDialog::normal()
{
    setWindowOpacity(1);
}

void LoginDialog::minimized()
{
    setWindowOpacity(0);
}

void LoginDialog::flip()
{
    //    if (_flip) {
    //        if (_timeLine->state() == QTimeLine::Running)
    //         return;
    //        if (_timeLine->currentValue() == 0) {
    //            _timeLine->setDirection(QTimeLine::Forward);
    //            _timeLine->start();
    //        }
    //        else {
    //            _timeLine->setDirection(QTimeLine::Backward);
    //            _timeLine->start();
    //        }
    //    }
}

void LoginDialog::updateFlip(qreal val)
{
    qDebug() << val;
    _finalyrot = qreal(0) - qreal(0) * val - 180 *val;
    qDebug() << _finalyrot;
    //    QTransform tran;
    //    _tran.translate(88,88);
    //    _tran.rotate(finalyrot,Qt::YAxis);
    //   painter.setTransform(tran);
    //    QTransform t;
    //    _t.translate(-88,-88);
    //    painter.setTransform(t,true);

    repaint();
}

QString LoginDialog::getLocalIcon(QString localPath)
{
    typedef HICON (*tempFuc)(CONST TCHAR *filePath);
    QLibrary myLib;
#ifdef DEBUG
    myLib.setFileName("IconGetD.dll");
#else
    myLib.setFileName("IconGet.dll");
#endif

    tempFuc myFunction = (tempFuc) myLib.resolve("getJumbIcon");
    if (myFunction) {
        HICON jumbIcon = myFunction((CONST TCHAR *)localPath.utf16 ());

        QPixmap picon = QPixmap::fromWinHICON(jumbIcon);

        QFileInfo info = QFileInfo(localPath);
        QString path(Config::get("IconDir"));
        path = path + "\\" + info.baseName();
        path += ".png"; //png
        QPixmap newicon =  picon.scaled(59, 59, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        newicon.save(path, "PNG",-1);
        return path;
    }
    return "";

}

void LoginDialog::setIcon(const QString &dirPath, const QString &iconPath)
{
    QString newApp = iconPath;

    if (newApp.isEmpty())
        return;

    QImage image = QImage(newApp).scaled(59, 59, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage normal = QImage(":images/icon_shadow.png");
    QImage middle = QImage(":images/icon_middle_shadow.png");

    QPainter pt1(&normal);
    pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
    pt1.drawImage(QRect(35, 36, 72, 72), middle.scaled(72, 72, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    pt1.drawImage(QRect(35 + 7, 36 + 3, 59, 59), image);
    pt1.end();

    QFileInfo info = QFileInfo(newApp);
    if (newApp.right(4) == ".ico")
    {
        if (info.exists())
            QFile::remove(newApp);//刪除 .ico 文件

        QString path = dirPath + info.baseName();
        path += ".png";

        QPixmap pix = QPixmap::fromImage(normal);
        pix.save(path, "PNG", -1);
    }
    else
    {
        QPixmap pix = QPixmap::fromImage(normal);
        pix.save(newApp, "PNG", -1);
    }

}

void LoginDialog::getPaas(bool isLogin)
{

    //paas
    //    g_myPaasList.clear();

    //    QString::SectionFlag flag = QString::SectionSkipEmpty;
    //    QString url("");

    //get paas list
    _paasCommui->login(PaasServer);
    while (!_paasFinished)
        QApplication::processEvents();
    _paasFinished = false;

    g_myPaasList = _paasCommui->getList();

    if (g_myPaasList.count() == 0)
        return;


    QDir iconDir(WIN_PAAS_IconPath);
    if(!iconDir.exists())
    {
        iconDir.mkdir(WIN_PAAS_IconPath);
    }
    //store ico file locally
    for(int i = 0; i < g_myPaasList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(WIN_PAAS_IconPath)
                .arg(g_myPaasList[i].cnName);
        QString tempPath = QString("%1%2.png")
                .arg(WIN_PAAS_IconPath)
                .arg(g_myPaasList[i].cnName);
        //        qDebug()<<"iconPath="<<iconPath;
        g_myPaasList[i].iconPath = iconPath;

        if (isLogin)
        {
            g_RemotepaasList.insert(i, g_myPaasList[i]);
            g_RemotepaasList[i].iconPath = iconPath;
        }
        //check if ico file is existed, or dont donwload

        QFile chkFile(iconPath);
        if(chkFile.exists())
        {
            chkFile.close();
            continue;
        }
        chkFile.close();

        //qDebug()<<"iconPath"<<iconPath;
        if (g_myPaasList[i].logoURL.isEmpty())
        {
            //url = g_myPaasList.at(i).urls.section('/', 1, 1, flag);
            //url = QString("http://" + url + "/Favicon.ico");

            //_paasCommui->downloadIcon(QUrl(url), tempPath);


            QPixmap pix(":images/url_normal.png");
            pix.save(iconPath, "PNG", -1);

            continue;
        }
        else
        {
            _paasCommui->downloadIcon(QUrl(g_myPaasList[i].logoURL), tempPath);

            while (!_paasFinished)
                QApplication::processEvents();
            _paasFinished = false;
        }

        setIcon(WIN_PAAS_IconPath, tempPath);
    }
}

void LoginDialog::getVac()
{
    if(_commui->errID == "10000")
    {
        //        char folder[MAX_PATH] = {0};
        //        SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
        //        WIN_TtempPath = QString(folder);
        //        WIN_VAPP_IconPath=WIN_TtempPath+"\\App Center\\Vicons\\";

        //get vapp list

        _commui->getAppList();
        while (!_vacfinished)
            QApplication::processEvents();
        _vacfinished = false;

        g_myVappList = _commui->getList();
        qDebug()<<"g_myList.count()="<<g_myVappList.count();

#ifdef Q_WS_WIN
//        iconDirPath = WIN_VAPP_IconPath ;
#else
        WIN_VAPP_IconPath =  xmlPath + "\\App Center\\Vicons";
#endif

        QDir iconDir(WIN_VAPP_IconPath);
        if(!iconDir.exists())
        {
            iconDir.mkdir(WIN_VAPP_IconPath);
        }
        //store ico file locally
        for(int i = 0; i < g_myVappList.count(); i++)
        {
            QString iconPath = QString("%1%2.png")
                    .arg(WIN_VAPP_IconPath)
                    .arg(g_myVappList[i].id);
            QString tempPath = QString("%1%2.ico")
                    .arg(WIN_VAPP_IconPath)
                    .arg(g_myVappList[i].id);

//            g_myVappList[i].icon = iconPath;

            g_RemoteappList.insert(i, g_myVappList[i]);
            g_RemoteappList[i].icon = iconPath;

            //check if ico file is existed, or dont donwload
            QFile chkFile(iconPath);
            if(chkFile.exists())
            {
                chkFile.close();
                continue;
            }
            chkFile.close();

            //qDebug()<<"iconPath"<<iconPath;
            _commui->downloadIcon(QUrl(g_myVappList[i].icon), tempPath);
            while (!_vacfinished)
                QApplication::processEvents();
            _vacfinished = false;

            setIcon(WIN_VAPP_IconPath, tempPath);
        }
    }
    else
    {
        if ((_commui->errInfo == "会话已存在") || (_commui->errID == "10045"))
        {
            _commui->logoff();

            _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, GetSystemInfo());

            while (!_finished)
                QApplication::processEvents();
            _finished = false;

            getVac();

        }
        else
        {
//            QMessageBox::warning(this, tr("vapp login failed"), _commui->errInfo, tr("OK"));

            AppMessageBox box(false, NULL);
            box.setText(tr("vapp login failed, please contact the administrator."));
            box.exec();
        }

    }

}
