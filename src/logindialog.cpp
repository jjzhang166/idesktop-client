#include <QPainter>
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
//#include <QApplication>

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


/****************************************************************/
//wangyaoli
extern QString serverip;
/****************************************************************/
QString VacServer;
QString VacPort;
QString VacUser;
QString VacPassword;

QList<APP_LIST> g_myVappList;
QString  WIN_VAPP_IconPath;
QString iconDirPath;
QString WIN_TtempPath;
QString xmlPath;

QList<APP_LIST> g_RemoteappList;
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), \
      _titlePressed(false),_vacfinished(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
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

#if 0
    /****************************************************************/
    //wangyaoli
    ipEdit = new HintLineEdit(serverip.toAscii().data(), "", this);
    ipEdit->setTextMargins(32, 5, 2, 5);
    ipEdit->setGeometry(75, 55, 208, 35);
    /****************************************************************/
#endif
    ipAddrEdit = new QIpAddressEdit(serverip, this);
    ipAddrEdit->setGeometry(75, 55, 208, 35);
    ipAddrEdit->setVisible(false);

    userEdit = new HintLineEdit("请输入您的用户名", ":images/user_icon.png", this);
    passEdit = new HintLineEdit("请输入您的密码", ":images/password_icon.png", this);
    passEdit->setEchoMode(QLineEdit::Password);
    userEdit->setTextMargins(5, 5, 2, 5);
    passEdit->setTextMargins(5, 5, 2, 5);
    userEdit->setGeometry((width() - 208) / 2, 125, 208, 27);
    passEdit->setGeometry((width() - 208) / 2, 165, 208, 27);

    userEdit->setText(QString("admin"));
    passEdit->setText(QString("root"));

    QPixmap loginButton(":images/login_btn.png");
    QPixmap loginButtonHover(":images/login_btn_hover.png");

    submit = new DynamicButton(loginButton, loginButtonHover, this);
    submit->setGeometry((width() - passEdit->width()) / 2, passEdit->pos().y() + passEdit->height() + 20, \
                        89, 34);

    QPixmap cancelButton(":images/cancel_btn.png");
    QPixmap cancelButtonHover(":images/cancel_btn_hover.png");

    cancelBtn = new DynamicButton(cancelButton, cancelButtonHover, this);
    cancelBtn->setGeometry(submit->pos().x() + submit->width() + 20, passEdit->pos().y() + passEdit->height() + 20,\
                           89, 34);

    remoteAuth = new QCheckBox(this);
    remoteAuth->setChecked(true);  //true
    remoteAuth->setText("远程验证");
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
	
    QDir dir(Config::get("UserDir"));
    QFileInfo fi(Config::get("Profile"));
    if (dir.exists(fi.fileName()))
    {
        /*判断文件是否存在*/
        Config::getServerIp();
        qDebug()<<serverip;
        ipAddrEdit->setText(serverip);
    }

    connect(submit, SIGNAL(clicked()), this, SLOT(auth()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));
    connect(minButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(userEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(passEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoginFinished(QNetworkReply*)));
}

void LoginDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg(":images/bg_loginBox.png");
    painter.drawPixmap(0, 0, width(), height(), bg.scaled(width(), height()));
//    painter.drawPixmap(22, 17, QPixmap(":images/logo_login.png"));

    painter.setPen(Qt::gray);
    painter.setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 10, QFont::Normal));
//    painter->setFont(QFont(QString::fromLocal8Bit("宋体"),13,-1,false));
    painter.drawText(43, 143, "用户名");
    painter.drawText(43, 183, "密  码");
    painter.setFont(QFont(QString::fromLocal8Bit("微软雅黑"), 9, QFont::Normal));
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
        userError("该用户不存在");
    }
    else if (result == "WRONG PASSWORD")
    {
        passError("密码验证失败");
    }
    else
    {
        connError("连接服务器失败...");

    }
}

void LoginDialog::auth()
{
    _uerror = "";
    _perror = "";
    _cerror = "";
    _cmsg = "";
    if (userEdit->text().isEmpty()) {
        userError("用户名不能为空");
        return;
    }
    if (passEdit->text().isEmpty()) {
        passError("密码不能为空");
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
        connMsg("正在连接服务器...");
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
            userError("该用户不存在");
            return;
        }
        if (query.value(0).toString() != md5) {
            passError("密码验证失败");
            return;
        }
    }


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

	//vac
//    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));
/*
    updateVacServer();
    _commui->login(VacServer + ":" + VacPort, VacUser, VacPassword, GetSystemInfo());
    while (!_vacfinished)
        QApplication::processEvents();
    _vacfinished = false;

//    // heart beat.5s timer
//    heartbeat_timer=new QTimer(this);
//    connect( heartbeat_timer, SIGNAL(timeout()), this, SLOT(heartbeat()));
//    heartbeat_timer->start(5000);
//    _retryTimes = 3;
//    _Isheartbeat=true;
//    qDebug()<<"login end";

    if(_commui->errID == "10000")
    {
        char folder[MAX_PATH] = {0};
        SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
        WIN_TtempPath = QString(folder);
        WIN_VAPP_IconPath=WIN_TtempPath+"\\App Center\\Vicons\\";

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

        //qDebug()<<"############## WIN_IconPath"<<WIN_IconPath;
        //qDebug()<<"############## iconDirPath"<<iconDirPath;

        QDir iconDir(iconDirPath);
        if(!iconDir.exists())
        {
            iconDir.mkdir(iconDirPath);
        }
        //store ico file locally
        for(int i = 0; i < g_myVappList.count(); i++)
        {
            QString iconPath = QString("%1%2.ico")
                    .arg(iconDirPath)
                    .arg(g_myVappList[i].id);
            //qDebug()<<"iconPath="<<iconPath;
            g_RemoteappList.insert(i, g_myVappList[i]);
//            qDebug()<<"g_mylist:"<<g_myVappList[i].icon;
            g_RemoteappList[i].icon = iconPath;
//            qDebug()<<"g_Rmote:"<<g_RemoteappList.at(i).icon;
            //check if ico file is existed, or dont donwload
            QFile chkFile(iconPath);
            if(chkFile.exists())
            {
                chkFile.close();
                continue;
            }
            chkFile.close();

            //qDebug()<<"iconPath"<<iconPath;
            _commui->downloadIcon(QUrl(g_myVappList[i].icon), iconPath);
            while (!_vacfinished)
                QApplication::processEvents();
            _vacfinished = false;

            //ico 95 * 95
             QString newApp = iconPath;

             if (newApp.isEmpty())
                 return;

             QImage image = QImage(newApp).scaled(48, 48);
             QImage normal = QImage(":images/app_bg.png");

             for (int i = 0; i < normal.width(); i++) {
                 for (int j = 0; j < normal.height(); j++) {
                     QRgb pixel = normal.pixel(i,j);
                     int a = qAlpha(pixel);
                     QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel), \
                                             qBlue(pixel), a * 0 / 255);
                     normal.setPixel(i, j, lightPixel);
                 }
             }

             QPainter pt1(&normal);
             pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
             pt1.drawImage(17, 8, image);
             pt1.end();
             QPixmap pix = QPixmap::fromImage(normal);
             pix.save(newApp, "ICO", -1);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("vapp login failed"), _commui->errInfo, tr("OK"));
        return;
    }
*/
}

void LoginDialog::onDone()
{
    //get the Application List;
    _vacfinished = true;
}

//void LoginDialog::heartbeat()
//{
//    if(!_Isheartbeat)
//        return;
//    _commui->heartBeat();

//    if(_commui->_isNetError)
//    {
//        if( _retryTimes > 0)
//        {
//            heartbeat_timer->start(10);
//            _retryTimes--;
//        }
//        else
//        {
//            heartbeat_timer->start(5000);
//        }
//    }
//    else
//    {
//        _retryTimes = 3;
//        heartbeat_timer->start(5000);
//    }

//    while (!_vacfinished)
//        QApplication::processEvents();
//    _vacfinished = false;

//    //qDebug()<<"errID"<<_commui->errID;
//    if(_commui->errID=="10036")
//    {
//#ifdef Q_WS_WIN
//        m_dllCloseAppAll();
//#endif
//        _Isheartbeat=false;
//        QMessageBox::warning(this, tr("your seesion has to be ticked out."),tr("Your session has to be kicked out by the administrator, please contact the administrator"), tr("ok"));
//        //qApp->quit();
//    }
//}

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
