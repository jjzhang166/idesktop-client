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
#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QBitmap>
#include <QByteArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QDir>
#include "logindialog.h"
#include "hintlineEdit.h"
#include "dynamicbutton.h"
#include "config.h"
#include "qtipaddressedit/qipaddressedit.h"
#include "qtipaddressedit/qipaddressedititem.h"
#include "../vappvirtualdesktop.h"
#include <windows.h>
#include <shellapi.h>
#include "ShlObj.h "
#ifdef Q_WS_WIN
QLibrary *mylib;   //
Dll_CloseAppAll m_dllCloseAppAll;
#endif

/****************************************************************/
//wangyaoli
extern QString serverip;
/****************************************************************/
QList<APP_LIST> g_myVappList;
QString  WIN_VAPP_IconPath;

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
    closeButton->setGeometry(351, 11, CLOSE_WIDTH, CLOSE_HEIGHT);

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

    userEdit = new HintLineEdit("请输入您的用户名", ":images/user_icon.png", this);
    passEdit = new HintLineEdit("请输入您的密码", ":images/password_icon.png", this);
    passEdit->setEchoMode(QLineEdit::Password);
    userEdit->setTextMargins(32, 5, 2, 5);
    passEdit->setTextMargins(32, 5, 2, 5);
    userEdit->setGeometry(75, 105, 208, 35);
    passEdit->setGeometry(75, 165, 208, 35);

    userEdit->setText(QString("admin"));
    passEdit->setText(QString("abc123"));

    QPixmap loginButton(":images/login_btn.png");
    QPixmap loginButtonHover(":images/login_btn_hover.png");

    submit = new DynamicButton(loginButton, loginButtonHover, this);
    submit->setGeometry(78, 230, 85, 35);

    remoteAuth = new QCheckBox(this);
    remoteAuth->setChecked(false);
    remoteAuth->setText("远程验证");
    remoteAuth->setGeometry(181, 230, 80, 35);

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
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(userEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(passEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoginFinished(QNetworkReply*)));
}

void LoginDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg(":images/bg_loginBox.png");
    painter.drawPixmap(0, 0, bg);
    painter.drawPixmap(22, 17, QPixmap(":images/logo_login.png"));
    painter.setPen(Qt::gray);
    painter.setFont(QFont("", 11, QFont::Black));
    painter.drawText(55, 33, "软件中心");
    painter.drawPixmap(74, 104, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(74, 164, QPixmap(":images/login_input_bg.png"));

    painter.drawPixmap(74, 54, QPixmap(":images/login_input_bg.png"));
    if (!_uerror.isEmpty()) {
        painter.setPen(Qt::red);
        painter.drawText(74, 160, _uerror);
    }
    if (!_perror.isEmpty()) {
        painter.setPen(Qt::red);
        painter.drawText(74, 220, _perror);
    }
    if (!_cerror.isEmpty()) {
        painter.setPen(Qt::red);
        painter.drawText(74, 280, _cerror);
    }
    if (!_cmsg.isEmpty()) {
        painter.setPen(Qt::blue);
        painter.drawText(74, 280, _cmsg);
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

void LoginDialog::onLoginFinished(QNetworkReply *reply)
{
    _finished = true;
    serverAddr->setEnabled(true);
    userEdit->setEnabled(true);
    passEdit->setEnabled(true);
    submit->setEnabled(true);
    remoteAuth->setEnabled(true);
    connMsg("");

    QString result = reply->readAll();
    _authSuccess = false;
    if (result == "APPSTORE SUCCESS") {
        _authSuccess = true;
    }
    else if (result == "USER NOT EXISTS") {
        userError("该用户不存在");
    }
    else if (result == "WRONG PASSWORD") {
        passError("密码验证失败");
    }
    else {

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
        QString loginUrl = Config::get("Server") + "/api/login";
        qDebug() << loginUrl;
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
	//login ivapp sm

//QString DlgLogin::GetSystemInfo()
//{
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
//    return sysInfo;
//}

    //_commui->login("192.168.31.196:80", "demo", "123456",QString("%1").arg(sysInfo));
    //_commui->login("192.168.49.253:80", "test", "1357.com",QString("%1").arg(sysInfo));
    _commui->login("192.168.30.63:80", "test", "1357.com",QString("%1").arg(sysInfo));

    // heart beat.5s timer
    heartbeat_timer=new QTimer(this);
    connect( heartbeat_timer, SIGNAL(timeout()), this, SLOT(heartbeat()));
    heartbeat_timer->start(5000);
    _retryTimes = 3;
    Isheartbeat=true;
    qDebug()<<"login end";


    char folder[MAX_PATH] = {0};
    SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
    QString WIN_TtempPath = QString(folder);
    WIN_VAPP_IconPath=WIN_TtempPath+"/App Center/Vicons/";

    //get vapp list
    _commui->getAppList();
    while (!_vacfinished)
        QApplication::processEvents();
    _vacfinished = false;

    g_myVappList = _commui->getList();
    qDebug()<<"g_myList.count()="<<g_myVappList.count();

    #ifdef Q_WS_WIN
    QString iconDirPath = WIN_VAPP_IconPath ;
    #else
    QString iconDirPath =  xmlPath + "/App Center/Vicons/";
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
//    _updateVappTimer = new QTimer(this);
//    connect(_updateVappTimer, SIGNAL(timeout()), this, SLOT(updateVappIcon()));
//    _updateVappTimer->start(1000 * 6);
}

void LoginDialog::onDone()
{
    //get the Application List;
    _vacfinished = true;
}

void LoginDialog::heartbeat()
{
    if(!Isheartbeat)
        return;
    _commui->heartBeat();

    if(_commui->_isNetError)
    {
        if( _retryTimes > 0)
        {
            heartbeat_timer->start(10);
            _retryTimes--;
        }
        else
        {
            heartbeat_timer->start(5000);
        }
    }
    else
    {
        _retryTimes = 3;
        heartbeat_timer->start(5000);
    }

    while (!_vacfinished)
        QApplication::processEvents();
    _vacfinished = false;

    //qDebug()<<"errID"<<_commui->errID;
    if(_commui->errID=="10036")
    {
#ifdef Q_WS_WIN
        m_dllCloseAppAll();
#endif
        Isheartbeat=false;
        QMessageBox::warning(this, tr("your seesion has to be ticked out."),tr("Your session has to be kicked out by the administrator, please contact the administrator"), tr("ok"));
        qApp->quit();
    }
}

//void LoginDialog::updateVappIcon()
//{

//    //get vapp list
//    _commui->getAppList();
//    while (!_vacfinished)
//        QApplication::processEvents();
//    _vacfinished = false;

//    g_myVappList = _commui->getList();
//    qDebug()<<"g_myList.count()="<<g_myVappList.count();

//    #ifdef Q_WS_WIN
//    QString iconDirPath = WIN_VAPP_IconPath ;
//    #else
//    QString iconDirPath =  xmlPath + "/App Center/Vicons/";
//    #endif

//    //qDebug()<<"############## WIN_IconPath"<<WIN_IconPath;
//    //qDebug()<<"############## iconDirPath"<<iconDirPath;

//    QDir iconDir(iconDirPath);
//    if(!iconDir.exists())
//    {
//        iconDir.mkdir(iconDirPath);
//    }
//    //store ico file locally
//    for(int i = 0; i < g_myVappList.count(); i++)
//    {
//        QString iconPath = QString("%1%2.ico")
//                .arg(iconDirPath)
//                .arg(g_myVappList[i].id);
//        //qDebug()<<"iconPath="<<iconPath;
//        //check if ico file is existed, or dont donwload
//        QFile chkFile(iconPath);
//        if(chkFile.exists())
//        {
//            chkFile.close();
//            continue;
//        }
//        chkFile.close();

//        //qDebug()<<"iconPath"<<iconPath;
//        _commui->downloadIcon(QUrl(g_myVappList[i].icon), iconPath);
//        while (!_vacfinished)
//            QApplication::processEvents();
//        _vacfinished = false;

//        //ico 95 * 95
//         QString newApp = iconPath;

//         if (newApp.isEmpty())
//             return;

//         QImage image = QImage(newApp).scaled(48, 48);
//         QImage normal = QImage(":images/app_bg.png");

//         for (int i = 0; i < normal.width(); i++) {
//             for (int j = 0; j < normal.height(); j++) {
//                 QRgb pixel = normal.pixel(i,j);
//                 int a = qAlpha(pixel);
//                 QRgb lightPixel = qRgba(qRed(pixel), qGreen(pixel),
//                                         qBlue(pixel), a * 0 / 255);
//                 normal.setPixel(i, j, lightPixel);
//             }
//         }

//         QPainter pt1(&normal);
//         pt1.setCompositionMode(QPainter::CompositionMode_SourceOver);
//         pt1.drawImage(17, 8, image);
//         pt1.end();
//         QPixmap pix = QPixmap::fromImage(normal);
//         pix.save(newApp, "ICO", -1);
//    }
//}

void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    if (QRect(10, 10, 350, 35).contains(event->pos())) {
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
