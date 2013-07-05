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

QList<APP_LIST> g_myVappList;
QList<PAAS_LIST> g_myPaasList;

QList<APP_LIST> g_RemoteappList;
QList<PAAS_LIST> g_RemotepaasList;
QList<LOCAL_LIST> g_RemotelocalList;

QString WIN_LOCAL_IconPath;
QString WIN_VAPP_IconPath;
QString WIN_PAAS_IconPath;
QString iconDirPath;
QString WIN_TtempPath;
QString xmlPath;

int ICON_TYPE;

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint)
    , _titlePressed(false)
    , _vacfinished(false)
    , _paasFinished(false)
    , _flip(false)
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

    QPixmap configurationIcon(":images/setup_normal.png");
    QPixmap configurationIconHover(":images/setup_hover.png");
    DynamicButton *configurationButton = new DynamicButton(configurationIcon, configurationIconHover, this);
    configurationButton->setGeometry(width() - 10 * 3 - CLOSE_WIDTH * 3, 8, 15, 16);
    configurationButton->setVisible(false);

    ipAddrEdit = new QIpAddressEdit(serverip, this);
    ipAddrEdit->setGeometry(75, 55, 208, 35);
    ipAddrEdit->setVisible(false);

    userEdit = new HintLineEdit(tr("�����������û���"), ":images/user_icon.png", this);
    passEdit = new HintLineEdit(tr("��������������"), ":images/password_icon.png", this);
    userEdit->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    passEdit->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));

    passEdit->setEchoMode(QLineEdit::Password);
    userEdit->setTextMargins(5, 5, 2, 5);
    passEdit->setTextMargins(5, 5, 2, 5);
    userEdit->setGeometry((width() - 208) / 2, 125, 208, 27);
    passEdit->setGeometry((width() - 208) / 2, 165, 208, 27);

    userEdit->setText(QString("admin"));
//    passEdit->setText(QString("root"));

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
    remoteAuth = new QCheckBox(this);
    remoteAuth->setChecked(true);  //true
    remoteAuth->setText(tr("Զ����֤"));
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
	
    QDir dir(Config::get("UserDir"));
    QFileInfo fi(Config::get("Profile"));
    if (dir.exists(fi.fileName()))
    {
        /*�ж��ļ��Ƿ����*/
        Config::getServerIp();
        qDebug()<<serverip;
        ipAddrEdit->setText(serverip);
    }

    _lShowAction = new QAction(tr("��ʾ"), this);
    _lShowAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    _lQuitAction = new QAction(tr("�˳�"), this);
    _lQuitAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));

    _loginMenu = new QMenu(this);
    _loginMenu->addAction(_lShowAction);
    _loginMenu->addSeparator();
    _loginMenu->addAction(_lQuitAction);

    _dQuitAction = new QAction(tr("�˳�"), this);
    _dShowAction = new QAction(tr("��ʾ"), this);
    _dHideAction = new QAction(tr("����"), this);
    _dVacServer = new QAction(tr("���÷�������ַ"), this);
    _dQuitAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    _dShowAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    _dHideAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    _dVacServer->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));

    _dashboardMenu = new QMenu(this);
    _dashboardMenu->addAction(_dVacServer);
    _dashboardMenu->addSeparator();
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
    connect(_dVacServer, SIGNAL(triggered()), this, SIGNAL(dVacServer()));
    connect(_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SIGNAL(dActivated(QSystemTrayIcon::ActivationReason)));

    connect(submit, SIGNAL(clicked()), this, SLOT(auth()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));
    connect(minButton, SIGNAL(clicked()), this, SLOT(minimized()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(configurationButton, SIGNAL(clicked()), this, SLOT(flip()));
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

    painter.setPen(Qt::gray);
    painter.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), 10, QFont::Normal));
//    painter->setFont(QFont(QString::fromLocal8Bit("����"),13,-1,false));
    painter.drawText(43, 143, tr("�û���"));
    painter.drawText(43, 183, tr("��  ��"));
    painter.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), 9, QFont::Normal));
//    painter.drawPixmap(74, 104, QPixmap(":images/login_input.png"));
//    painter.drawPixmap(74, 164, QPixmap(":images/login_input.png"));

//    painter.drawPixmap(74, 54, QPixmap(":images/login_input.png"));
    if (!_uerror.isEmpty()) {
        painter.setPen(QPen(QColor("#ff0000")));
        painter.drawText((width() + userEdit->width()) / 2 + 7, 140, _uerror);
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
    connMsg("");

    QString jsonResult = reply->readAll();
    qDebug() << "jsonResult" << jsonResult;
    _authSuccess = false;
	
    sc = engine.evaluate ("JSON.parse").call(QScriptValue(), QScriptValueList() << jsonResult);

    if(sc.isObject())
    {
        result = sc.property("data").toVariant().toString();
        qDebug() << result;

    }
    else
    {
        qDebug()<<"Invalid response: " << jsonResult;
		// fatal error
    }

    if (result == "APPSTORE SUCCESS")
    {
        _authSuccess = true;
    }
    else if (result == "USER NOT EXISTS")
    {
        userError(tr("���û�������"));
    }
    else if (result == "WRONG PASSWORD")
    {
        passError(tr("������֤ʧ��"));
    }
    else
    {
        connError(tr("���ӷ�����ʧ��..."));

    }
}

void LoginDialog::auth()
{
    qDebug() << "****************************************";
    _uerror = "";
    _perror = "";
    _cerror = "";
    _cmsg = "";
    if (userEdit->text().isEmpty()) {
        userError(tr("�û�������Ϊ��"));
        return;
    }
    if (passEdit->text().isEmpty()) {
        passError(tr("���벻��Ϊ��"));
        return;
    }
    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash(passEdit->text().toAscii(), \
                                  QCryptographicHash::Md5);
    md5.append(bb.toHex());

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
        QString loginUrl = Config::get("Server") + ":8080/idesktop/platform/service/ClientServiceLogin.action";
        qDebug() <<"loginUrl"<<loginUrl;
        QString data = "name=" + userEdit->text() + "&password=" + md5;
        _nam->post(QNetworkRequest(QUrl(loginUrl)), data.toAscii());
        connMsg(tr("�������ӷ�����..."));
        serverAddr->setEnabled(false);
        userEdit->setEnabled(false);
        passEdit->setEnabled(false);
        submit->setEnabled(false);
        remoteAuth->setEnabled(false);
        while(!_finished)
            QApplication::processEvents();
        if (!_authSuccess)
            return;
        QString replaceNamePwd = QString("REPLACE INTO users(name, password) values('%1', '%2');")\
                                .arg(userEdit->text()).arg(md5);
        localDb.exec(replaceNamePwd);
        /* TODO: check the return value */
    }
    else {
        QSqlQuery query = localDb.exec(statement);
        if (!query.next()) {
            userError(tr("���û�������"));
            return;
        }
        if (query.value(0).toString() != md5) {
            passError(tr("������֤ʧ��"));
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
    Config::set("password",md5);
    Config::set("User", userEdit->text());
    QDialog::accept(); 

    char folder[MAX_PATH] = {0};
    SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
    WIN_TtempPath = QString(folder);

    WIN_LOCAL_IconPath=WIN_TtempPath+"\\App Center\\Licons\\";
    WIN_VAPP_IconPath=WIN_TtempPath+"\\App Center\\Vicons\\";
    WIN_PAAS_IconPath=WIN_TtempPath+"\\App Center\\Picons\\";
    qDebug()<<"icon path:"<<WIN_VAPP_IconPath;

    getIconType();

    //local
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//vac
//    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));

    updateVacServer();
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, GetSystemInfo());
    while (!_vacfinished)
        QApplication::processEvents();
    _vacfinished = false;


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
        iconDirPath = WIN_VAPP_IconPath ;
        #else
        iconDirPath =  xmlPath + "\\App Center\\Vicons";
        #endif

        QDir iconDir(iconDirPath);
        if(!iconDir.exists())
        {
            iconDir.mkdir(iconDirPath);
        }
        //store ico file locally
        for(int i = 0; i < g_myVappList.count(); i++)
        {
            QString iconPath = QString("%1%2.png")
                    .arg(iconDirPath)
                    .arg(g_myVappList[i].id);
            QString tempPath = QString("%1%2.ico")
                    .arg(iconDirPath)
                    .arg(g_myVappList[i].id);

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

            setIcon(iconDirPath, tempPath);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("vapp login failed"), _commui->errInfo, tr("OK"));
//        return;
    }

//paas
    qDebug() << "!#@$!@#$@#^%*(&)(*%^$#%@#$!@#$#@!$#";
//    g_myPaasList.clear();

//    QString::SectionFlag flag = QString::SectionSkipEmpty;
//    QString url("");

    //get paas list
    _paasCommui->login(PAASURL);
    while (!_paasFinished)
        QApplication::processEvents();
    _paasFinished = false;

    g_myPaasList = _paasCommui->getList();
    qDebug() << "********" << g_myPaasList.count();

    if (g_myPaasList.count() == 0)
        return;


    QString iconDirPath = WIN_PAAS_IconPath ;


    QDir iconDir(iconDirPath);
    if(!iconDir.exists())
    {
        iconDir.mkdir(iconDirPath);
    }
    //store ico file locally
    for(int i = 0; i < g_myPaasList.count(); i++)
    {
        QString iconPath = QString("%1%2.png")
                .arg(iconDirPath)
                .arg(g_myPaasList[i].cnName);
        QString tempPath = QString("%1%2.ico")
                .arg(iconDirPath)
                .arg(g_myPaasList[i].cnName);
//        qDebug()<<"iconPath="<<iconPath;

        g_RemotepaasList.insert(i, g_myPaasList[i]);
        g_RemotepaasList[i].iconPath = iconPath;
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

        setIcon(iconDirPath, tempPath);
    }
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
            QFile::remove(newApp);//�h�� .ico �ļ�

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
