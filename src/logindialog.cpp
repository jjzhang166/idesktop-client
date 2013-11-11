#include <QtGui>
#include <QtSql/QtSql>
#include <QVariant>
#include <QJSon/qjson.h>
#include <QJSon/serializer.h>

//#include "idesktopsettings.h"
#include "logindialog.h"
#include "hintlineEdit.h"
#include "dynamicbutton.h"
#include "config.h"
#include "qtipaddressedit/qipaddressedit.h"
#include "qtipaddressedit/qipaddressedititem.h"

#include <windows.h>
#include <shldisp.h>

#include "public.h"
#include "ShlObj.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include <QtScript/QScriptValue>

#define FONTSIZE 10
#define KEY "\\Windows\\CurrentVersion\\App Paths\\"

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

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint)
    , _titlePressed(false)
    , _flip(false)
    ,_isSetting(false)
    , _connecting(false)
{
//    _settings = IDesktopSettings::instance();

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
    closeButton = new DynamicButton(closeIcon, closeIconHover, this);
    closeButton->setGeometry(width() - 10 - CLOSE_WIDTH, 10, CLOSE_WIDTH, CLOSE_HEIGHT);

    QPixmap minIcon(":images/dlgMinIcon.png");
    QPixmap minIconHover(":images/dlgMinIconHover.png");
    minButton = new DynamicButton(minIcon, minIconHover, this);
    minButton->setGeometry(width() - 10 * 2 - CLOSE_WIDTH * 2, 10, CLOSE_WIDTH, CLOSE_HEIGHT);

    QPixmap configurationIcon = QPixmap(":images/setup_normal.png").scaled(10,10);
    QPixmap configurationIconHover = QPixmap(":images/setup_hover.png").scaled(10,10);
    configurationButton = new DynamicButton(configurationIcon, configurationIconHover, this);
    configurationButton->setGeometry(width() - 10 * 3 - CLOSE_WIDTH * 3, 10, 10, 10);
    configurationButton->setVisible(true);

    ipAddrEdit = new QIpAddressEdit(serverip, this);
    ipAddrEdit->setGeometry(75, 55, 208, 35);
    ipAddrEdit->setVisible(false);
    userEdit = new HintLineEdit(tr("�����������û���"), ":images/user_icon.png", this);
    passEdit = new HintLineEdit(tr("��������������"), ":images/password_icon.png", this);
    userEdit->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    passEdit->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));


    passEdit->setEchoMode(QLineEdit::Password);
    userEdit->setTextMargins(5, 0, 2, 3);
    passEdit->setTextMargins(5, 0, 2, 3);
    userEdit->setGeometry((width() - 208) / 2, 125, 208, 27);
    passEdit->setGeometry((width() - 208) / 2, 165, 208, 27);

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

//    remoteAuth = new QCheckBox(this);
//    remoteAuth->setChecked(true);  //true
//    remoteAuth->setText(tr("Զ����֤"));
//    remoteAuth->setGeometry(181, 230, 80, 35);
//    remoteAuth->setVisible(false);

    _nam = new QNetworkAccessManager(this);
    _namJson = new QNetworkAccessManager(this);
    _namOut = new QNetworkAccessManager(this);

    //get serverip by zj

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

    _dQuitAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    _dShowAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));
    _dHideAction->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));

    _dashboardMenu = new QMenu(this);
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

    _loadItem = new RotateWarnningLabel(this);
    _loadItem->move((width() - _loadItem->width()) / 2, height() / 3);
    _loadItem->start();
    _loadItem->setVisible(false);

    connect(_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(updateFlip(qreal)));
    connect(_lShowAction, SIGNAL(triggered()), this, SLOT(normal()));
    connect(_lQuitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(_dQuitAction, SIGNAL(triggered()), this, SIGNAL(dQuit()));
    connect(_dShowAction, SIGNAL(triggered()), this, SIGNAL(dShow()));
    connect(_dHideAction, SIGNAL(triggered()), this, SIGNAL(dHide()));

    connect(_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(lActivated(QSystemTrayIcon::ActivationReason)));
    connect(_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SIGNAL(dActivated(QSystemTrayIcon::ActivationReason)));

    connect(submit, SIGNAL(clicked()), this, SLOT(auth()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(quit()));
    connect(minButton, SIGNAL(clicked()), this, SLOT(minimized()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(quit()));
//    connect(configurationButton, SIGNAL(clicked()), this, SLOT(flip()));
    connect(userEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(passEdit, SIGNAL(returnPressed()), submit, SIGNAL(clicked()));
    connect(_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoginFinished(QNetworkReply*)));
    connect(_namJson, SIGNAL(finished(QNetworkReply*)), this, SLOT(jsonDownloadFinished(QNetworkReply*)));
    connect(this, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

}

LoginDialog::~LoginDialog()
{
    delete _timeLine;
    delete _tray;
    delete _dashboardMenu;
    delete _dHideAction;
    delete _dShowAction;
    delete _dQuitAction;
    delete _loginMenu;
    delete _lQuitAction;
    delete _lShowAction;
    delete _nam;
//    delete serverAddr;
    delete userEdit;
    delete passEdit;
    delete submit;
    delete cancelBtn;
//    delete remoteAuth;
    delete ipAddrEdit;
    delete _namOut;
//    delete configurationButton;
//    delete minButton;
//    delete closeButton;
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
    if (!_connecting)
    {
        if(!_isSetting)
        {
            painter.setPen(Qt::gray);
            painter.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), 10, QFont::Normal));
            //    painter->setFont(QFont(QString::fromLocal8Bit("����"),13,-1,false));
            painter.drawText(43, 143, tr("�û���"));
            painter.drawText(43, 183, tr("��  ��"));

        }else{
            painter.setPen(Qt::gray);
            painter.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), 10, QFont::Normal));
            painter.drawText(20, 143, tr("����˵�ַ"));

        }

        painter.setFont(QFont(QString::fromLocal8Bit("΢���ź�"), 9, QFont::Normal));

        if(!_vserverError.isEmpty())
        {
                painter.setPen(QPen(QColor("#ff0000")));
                painter.drawText((width() - 188) / 2, 155, _vserverError);
        }
        if(!_paasError.isEmpty())
        {
                painter.setPen(QPen(QColor("#ff0000")));
                //painter.drawText((width() - 188) / 2, 213, _paasError);   //verifyLEdit
                painter.drawText((width() - 208) / 2, verifyLEdit->pos().y() + verifyLEdit->height() + 22, _paasError);
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
    }
    else
    {
        QFont font(QString::fromLocal8Bit("Microsoft Yahei"), 13, QFont::Normal);
        painter.setFont(font);

        if (!_cmsg.isEmpty()) {

            QFontMetrics fm(font);
            int _textWidth = fm.width(_cmsg);
            painter.setPen(QPen(QColor("#299cfd")));
            painter.drawText((width() - _textWidth) / 2, height() /3 * 2, _cmsg);

        }
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
    QString userType;
    QString vappPort;
    QString vappUrl;

    _finished = true;
    userEdit->setEnabled(true);
    passEdit->setEnabled(true);
    submit->setEnabled(true);
//    remoteAuth->setEnabled(true);
    closeButton->setEnabled(true);
    configurationButton->setEnable(true);
    connMsg("");

    QString jsonResult = reply->readAll();
    if (jsonResult.isEmpty())
        return;
    qDebug() << "jsonResult" << jsonResult;
    _authSuccess = false;

    sc = engine.evaluate ("JSON.parse").call(QScriptValue(), QScriptValueList() << jsonResult);

    if(sc.isObject())
    {
        result = sc.property("status").toVariant().toString();  //1
        qDebug() << "result : " << result;
        userType = sc.property("userType").toVariant().toString();
        qDebug() << "userType : " << userType;
        vappUrl = sc.property("vappUrl").toVariant().toString();
        qDebug() << "vappUrl : " << vappUrl;
        vappPort = sc.property("vappPort").toVariant().toString();
        qDebug() << "vappPort : " << vappPort;

    }
    else
    {
        qDebug()<<"Invalid response: " << jsonResult;
        // fatal error
    }

    //FIXME: delete it!
//    _authSuccess = true;
    if (result == "1")   //  APPSTORE SUCCESS
    {
        if (vappUrl.isEmpty() || vappPort.isEmpty())
        {
            logout();

            connenting(false);
            connError(tr("��������Ӧ�÷�����ʧ�ܣ�����ϵ����Ա��"));
            return;
        }

        if (userType == "comm_user")
        {
            closeButton->setEnabled(false);
            _authSuccess = true;

            VacServer = vappUrl;
            VacPort = vappPort;
            VacUser = userEdit->text();
            VacPassword = passEdit->text();

            connMsg(tr("���ӳɹ������ڻ�ȡ�û���Ϣ..."));
        }
        else
        {
            logout();

            connenting(false);
            userError(tr("������ͨ�û���¼"));
        }
    }
    else if (result == "-1")//USER NOT EXISTS
    {
		connenting(false);
        userError(tr("�û������������"));  //
    }
    else if (result == "-2")//USER IS EXISTS
    {
		connenting(false);
        userError(tr("�����û��ѵ�¼"));  //
    }
    else
    {
		connenting(false);
        connError(tr("���ӷ�����ʧ��..."));
    }
}

void LoginDialog::connenting(bool connecting)
{
    if(connecting)
    {
        _connecting = true;
        _loadItem->setVisible(true);

        _uerror = "";
        _perror = "";
        _cerror = "";
        _cmsg = "";

        connMsg(tr("��������..."));

        userEdit->hide();
        passEdit->hide();
        submit->hide();
        cancelBtn->hide();
        configurationButton->hide();
    }
    else
    {
        _connecting = false;

        _loadItem->setVisible(false);
        userEdit->show();
        passEdit->show();
        submit->show();
        cancelBtn->show();
        configurationButton->show();
    }
}

void LoginDialog::jsonDownloadFinished(QNetworkReply *reply)
{

    char folder[MAX_PATH] = {0};
    SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
    QString WIN_TtempPath = QString(folder);
    QString iconPath;

    qDebug() << "------>jsonDownloadFinished";
    QByteArray buffer = reply->readAll();

    QJson::QJson parser;
    bool ok;
    QVariantMap result = parser.parse(buffer, &ok).toMap();
    qDebug() << "init icon "<< result["userInfoJsonStr"].toString();
    if (!ok) {
      qFatal("An error occurred during parsing by buffer");
      return;
    }

    qDebug() << "status:" << result["status"].toString();

    if (result["userInfoJsonStr"].toString() == "")
    {
        QDialog::accept();
        return;
    }

    QSqlQuery query(QSqlDatabase::database("local"));

    QString truncateStr("delete from localapps;");
    if(!query.exec(truncateStr))
    {
         qDebug() <<"query failed by clear table";
         return;
    }

    bool ok2;
    QVariantMap userInfoMap = parser.parse(result["userInfoJsonStr"].toString().toUtf8(), &ok2).toMap();
    if (!ok2) {
      qFatal("An error occurred during parsing by userInfoJsonStr");
      return;
    }

    foreach(QVariant pluginApp, userInfoMap["localapps"].toList())
    {
        QVariantMap appmap = pluginApp.toMap();

        QString uniquename = appmap["uniquename"].toString();
        QFileInfo info = QFileInfo(appmap["icon"].toString());
        iconPath = "";

        if (uniquename.startsWith("0_"))
        {
            iconPath = WIN_TtempPath+"\\App Center\\Licons\\" + info.baseName() + ".png";

        }
        else if (uniquename.startsWith("1_"))
        {
            iconPath = WIN_TtempPath+"\\App Center\\Vicons\\" + info.baseName() + ".png";
        }
        else if (uniquename.startsWith("2_"))
        {
            iconPath = WIN_TtempPath+"\\App Center\\Picons\\" + info.baseName() + ".png";

        }
        else if (uniquename.startsWith("3_"))
        {
            iconPath = appmap["icon"].toString();
        }
        else
        {
            iconPath = QString(":images/dustbin_normal.png");
        }

        QString insertStr = QString("insert into localapps ("\
                                    "name, version, execname, icon, uninstall, "\
                                    "lastupdate, page, idx, hidden, id, type, isRemote, url, dirId, uniquename) values ( " \
                                    "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', "\
                                    "\'%8\', \'%9\',\'%10\',\'%11\',\'%12\',\'%13\',\'%14\',\'%15\');")\
                                    .arg(appmap["name"].toString()).arg(appmap["version"].toString())\
                                    .arg(appmap["execname"].toString()).arg(iconPath)\
                                    .arg(appmap["uninstall"].toString()).arg(appmap["lastupdate"].toString())\
                                    .arg(appmap["page"].toString()).arg(appmap["idx"].toString())\
                                    .arg(appmap["hidden"].toString()).arg(appmap["id"].toString())\
                                    .arg(appmap["type"].toString()).arg(appmap["isRemote"].toString())\
                                    .arg(appmap["url"].toString()).arg(appmap["dirId"].toString())
                                    .arg(appmap["uniquename"].toString());

        if(!query.exec(insertStr))
        {
            qDebug() <<"query failed by insert";
            return;
        }
    }

    QVariantList wallpapersList = userInfoMap["wallpapers"].toList();
    foreach(QVariant pluginWallpapers, wallpapersList)
    {
        QVariantMap wallpapersMmap = pluginWallpapers.toMap();

        QString path = QCoreApplication::applicationDirPath();
        path.replace(QString("/"), QString("\\"));
        path += "\\images\\wallpager";
        QFileInfo info = QFileInfo(wallpapersMmap["wallpaper"].toString());
        path += ("\\" + info.baseName() + ".jpg");
        qDebug() << "jpg path : " << path;


        QString wallpapersStr = QString("update wallpapers "\
                               "set wallpaper=\'%1\' where id=\'%2\';")\
                               .arg(path)\
                               .arg(wallpapersMmap["id"].toString());
        if(!query.exec(wallpapersStr)) {
            qDebug() <<"query failed by update wallpapers";
            return;
        }

    }

    QVariantList sizetypeList = userInfoMap["sizetype"].toList();
    foreach(QVariant pluginSizetype, sizetypeList)
    {
        QVariantMap sizetypeMap = pluginSizetype.toMap();

        QString sizetypeStr = QString("update sizetype "\
                               "set type=\'%1\' where id=\'%2\';")\
                               .arg(sizetypeMap["type"].toString())\
                               .arg(sizetypeMap["id"].toString());
        if(!query.exec(sizetypeStr)) {
            qDebug() <<"query failed by update sizetype";
            return;
        }
    }

    bool isDustbin = false;
    QSqlQuery queryDustbin = QSqlDatabase::database("local").exec("select uniquename from localapps;");
    while (queryDustbin.next()) {
        qDebug() << "queryDustbin.value(0).toString()" << queryDustbin.value(0).toString();
        if ("4_dustbin" == queryDustbin.value(0).toString())
        {
           isDustbin = true;
           break;
        }
    }
    if (!isDustbin)
    {
        QString qstrLapp = QString("insert into localapps ("\
                                   "name, version, execname, icon, uninstall, "\
                                   "lastupdate, page, idx, hidden, id, type, isRemote, url, dirId, uniquename) values ( " \
                                   "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', \'%8\', "\
                                   "\'%9\', \'%10\',\'%11\',\'%12\',\'%13\',\'%14\', \'%15\');")\
                                    .arg(QString(QObject::tr("��ֽ¨"))).arg("1.0")\
                                    .arg(QString(QObject::tr("��ֽ¨"))).arg(":images/dustbin_normal.png")\
                                    .arg(QString(QObject::tr("��ֽ¨"))).arg(1)\
                                    .arg(0).arg(0)\
                                    .arg(int(false)).arg(1000)\
                                    .arg("4").arg(int(false))\
                                    .arg("").arg(-2).arg("4_dustbin");


        if(!query.exec(qstrLapp))
        {
            qDebug() <<"query failed";
        }
    }

    QDialog::accept();
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

//    QString statement = QString("select password from users where name='%1'")
//            .arg(userEdit->text());
    QSqlDatabase localDb = QSqlDatabase::database("loginData");

//    if (remoteAuth->isChecked()) {

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
        QString loginUrl = "http://" + verifyLEdit->text() + ":8080/idesktop/login.action";
        qDebug() <<"loginUrl"<<loginUrl;
        QString data = "username=" + userEdit->text() + "&password=" + passEdit->text() + "&tenant=0";
        QNetworkReply *_reply = _nam->post(QNetworkRequest(QUrl(loginUrl)), data.toUtf8());
        connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
		connMsg(tr("�������ӷ�����..."));
		
//        serverAddr->setEnabled(false);
        userEdit->setEnabled(false);
        passEdit->setEnabled(false);
        submit->setEnabled(false);
//        remoteAuth->setEnabled(false);
        closeButton->setEnabled(false);
        configurationButton->setEnable(false);
		connenting(true);
        while(!_finished)
            QApplication::processEvents();
        if (!_authSuccess)
            return;

        QString replaceNamePwd = QString("REPLACE INTO users(name, password) values('%1', '%2');")\
                .arg(userEdit->text()).arg(passEdit->text());
        localDb.exec(replaceNamePwd);

        USERNAME = userEdit->text();

        /* TODO: check the return value */
//    }
//    else {
//        QSqlQuery query = localDb.exec(statement);
//        if (!query.next()) {
//            userError(tr("���û�������"));
//            return;
//        }
//        if (query.value(0).toString() != passEdit->text()) {
//            passError(tr("������֤ʧ��"));
//            return;
//        }
//    }

    Config::set("Server", verifyLEdit->text());
    //Config::set("password",md5);
    Config::set("password",passEdit->text());
    Config::set("User", userEdit->text());
    Config::set("UserNameDir", Config::get("AppDir") + "\\" + Config::get("User"));
    Config::set("UserData", Config::get("AppDir") + "\\" + Config::get("User") + "\\data");

//    saveVacUserInfo();
    createDb();

    QString jsonUrl = "http://" + verifyLEdit->text() + ":8080/idesktop/getUserStatusData.action?username=" + userEdit->text() + "&ismanager=false";
    _namJson->get(QNetworkRequest(QUrl(jsonUrl)));

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

//setting by zj
void LoginDialog::settingUi()
{
    //setting by zj
    verifyLEdit = new HintLineEdit(tr("���������˵�ַ"), ":images/user_icon.png", this);
    verifyLEdit->setFont(QFont(QString::fromLocal8Bit("΢���ź�"), FONTSIZE, QFont::Normal));

    verifyLEdit->setTextMargins(5, 0, 2, 3);
    verifyLEdit->setGeometry((width() - 208) / 2, 125, 208, 27);

    QPixmap loginButton(":images/login_save_normal.png");
    QPixmap loginButtonHover(":images/login_save_hover.png");
    QPixmap cancelButton(":images/cancel_btn.png");
    QPixmap cancelButtonHover(":images/cancel_btn_hover.png");

    saveButton = new DynamicButton(loginButton, loginButtonHover, this);
    saveButton->setGeometry((width() - passEdit->width()) / 2, passEdit->pos().y() + passEdit->height() + 20, \
                            89, 34);
    //saveButton->hide();
    returnButton = new DynamicButton(cancelButton, cancelButtonHover, this);
    returnButton->setGeometry(submit->pos().x() + submit->width() + 20, passEdit->pos().y() + passEdit->height() + 20,\
                              89, 34);
    //returnButton->hide();
    returnButton->setEnable(false);
    returnButton->setEnabled(false);

    connect(verifyLEdit, SIGNAL(returnPressed()), saveButton, SIGNAL(clicked()));
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
            QSqlDatabase::database("loginData").exec(QString("SELECT verifyServer FROM servers where id=1;"));
    while (query.next())
    {
        serverip = query.value(0).toString();

    }
}

void LoginDialog::readSetting()
{
    updateVacServer();

    if(serverip.isEmpty())
    {
        settingSlot();
    }else
    {
        updateUi();
        verifyLEdit->setText(serverip);

        returnButton->setEnable(true);
        returnButton->setEnabled(true);
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
        configurationButton->hide();

        _uerror = "";
        _perror = "";
        _cerror = "";
        _cmsg = "";

        _tempVerifyIp = verifyLEdit->text();

        verifyLEdit->show();
        saveButton->show();
        returnButton->show();
        verifyLEdit->setFocus();
    }else{

        verifyLEdit->setText(_tempVerifyIp);
        _tempVerifyIp = "";


        verifyLEdit->hide();
        saveButton->hide();
        returnButton->hide();
        passEdit->show();
        userEdit->show();
        if (!userEdit->hasFocus())
            userEdit->setFocus();
        submit->show();
        cancelBtn->show();
        configurationButton->show();
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
    QSqlQuery query(QSqlDatabase::database("loginData"));
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
        PaasError(tr("����˵�ַ����Ϊ��"));   //��֤��������ַ����Ϊ��
        return;
    }

    QRegExp rx2("^(1?\\d\\d?|2[0-4]\\d|25[0-5])\.(1?\\d\\d?|2[0-4]\\d|25[0-5])\.(1?\\d\\d?|2[0-4]\\d|25[0-5])\.(1?\\d\\d?|2[0-4]\\d|25[0-5])$");
    if( !rx2.exactMatch(verifyLEdit->text()))
    {
        PaasError(tr("��������ȷ�ķ���˵�ַ"));  //��������ȷ����֤�����ַ
        return;
    }

    returnButton->setEnable(true);
    returnButton->setEnabled(true);

    QSqlQuery query(QSqlDatabase::database("loginData"));

    if(!query.exec(QString("select verifyServer from servers"))) {
        qDebug() <<"select verifyServer failed";
        return;
    }

    if (!query.next())
    {

        QString qstrPaasS = QString("insert into servers ("\
                                    "id, verifyServer) values ("\
                                    "\'%1\', \'%2\');")\
                .arg(1).arg(verifyLEdit->text());
        if(!query.exec(qstrPaasS)) {
            qDebug() <<"qstrPaasS query failed";
        }
    }else{

        QString updatePaasData = QString("update servers set verifyServer=\'%1\' where id=1;")\
                                 .arg(verifyLEdit->text());

        if(!query.exec(updatePaasData)) {
            qDebug() <<"updatePaasData query failed";
        }

    }

    serverip = verifyLEdit->text();
    ipAddrEdit->setText(serverip);

   // returnSlot();

    verifyLEdit->hide();
    saveButton->hide();
    returnButton->hide();
    passEdit->show();
    userEdit->show();
    if (!userEdit->hasFocus())
        userEdit->setFocus();
    submit->show();
    cancelBtn->show();
    configurationButton->show();
    _isSetting = false;

    repaint();
}

void LoginDialog::returnSlot()
{
    qDebug()<<"return";

    _vserverError = "";
    _paasError = "";

    verifyLEdit->setText(_tempVerifyIp);

    verifyLEdit->hide();
    saveButton->hide();
    returnButton->hide();
    passEdit->show();
    userEdit->show();
    if (!userEdit->hasFocus())
        userEdit->setFocus();
    submit->show();
    cancelBtn->show();
    configurationButton->show();
    _isSetting = false;

    _tempVerifyIp = "";

    repaint();
}
//end

void LoginDialog::updateVacServer()
{
    QSqlQuery query = \
            QSqlDatabase::database("loginData").exec(QString("SELECT verifyServer FROM servers where id=1;"));
    while (query.next())
    {
        //by zj
        serverip = query.value(0).toString();
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

void LoginDialog::createDb()
{
    bool dbExists = true;

    QFileInfo aInfo(Config::get("AppDir"));
    if (!aInfo.exists() || !aInfo.isDir())
    {
       QDir appDir(Config::get("DataDir"));
       appDir.mkdir("App Center");
       dbExists = false;
    }

    QFileInfo uInfo(Config::get("UserNameDir"));
    if (!uInfo.exists() || !uInfo.isDir())
    {
        QDir appDir(Config::get("AppDir"));
        appDir.mkdir(QString("%1").arg(Config::get("User")));
        dbExists = false;
    }
//    QString db = Config::get("AppDir") + "\\" + Config::get("User") + "\\data";
    QFileInfo dbInfo(Config::get("UserData"));
    if (!dbInfo.exists())
        dbExists = false;
    else
    {
        QDir toDir(Config::get("UserNameDir"));
        bool remove = toDir.remove(Config::get("UserData"));
        if (!remove)
        {
//            QSqlQuery query(QSqlDatabase::database("local"));

//            QString truncateStr("delete from localapps;");
//            if(!query.exec(truncateStr))
//            {
//                 qDebug() <<"query failed by clear table";
//                 return;
//            }
            qDebug() <<"query failed by remove data";
        }
        dbExists = false;
    }

    QSqlDatabase localDb = QSqlDatabase::addDatabase("QSQLITE", "local");
    localDb.setDatabaseName(Config::get("UserData"));

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
       QString createWallpaperTable = \
                      "CREATE TABLE wallpapers " \
                      "(id int not null primary key, " \
                      "wallpaper nvarchar not null);";
       QString createIconSizeTable = \
                      "CREATE TABLE sizetype " \
                      "(id int not null primary key, " \
                      "type int not null);";

       QSqlDatabase::database("local").exec(createAppTable);
       QSqlDatabase::database("local").exec(createWallpaperTable);
       QSqlDatabase::database("local").exec(createIconSizeTable);

       QString qstrLapp = QString("insert into localapps ("\
                                  "name, version, execname, icon, uninstall, "\
                                  "lastupdate, page, idx, hidden, id, type, isRemote, url, dirId, uniquename) values ( " \
                                  "\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', \'%8\', "\
                                  "\'%9\', \'%10\',\'%11\',\'%12\',\'%13\',\'%14\', \'%15\');")\
                                   .arg(QString(QObject::tr("��ֽ¨"))).arg("1.0")\
                                   .arg(QString(QObject::tr("��ֽ¨"))).arg(":images/dustbin_normal.png")\
                                   .arg(QString(QObject::tr("��ֽ¨"))).arg(1)\
                                   .arg(0).arg(0)\
                                   .arg(int(false)).arg(1000)\
                                   .arg("4").arg(int(false))\
                                    .arg("").arg(-2).arg("4_dustbin");  //��ֽ¨

       QSqlQuery query(QSqlDatabase::database("local"));

       if(!query.exec(qstrLapp))
       {
           qDebug() <<"query failed";
       }

       QString qstrWp = QString("insert into wallpapers ("\
                              "id, wallpaper) values ( "\
                              "\'%1\', \'%2\');")\
                              .arg(1).arg(".//images//wallpager//wp_0.jpg");

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
}
void LoginDialog::slotError(QNetworkReply::NetworkError)
{
    connenting(false);

    _uerror = "";
    _perror = "";
    _cerror = "";
    _cmsg = "";

    repaint();

    connError(tr("���ӷ�����ʧ��..."));
}

void LoginDialog::quit()
{
    hideTrayIcon();
    reject();
}

void LoginDialog::lActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (this->isHidden())
        return;

    if (reason == QSystemTrayIcon::DoubleClick)
    {
        if (windowOpacity() == qreal(0))
        {
            normal();
        }
    }
    else if (reason == QSystemTrayIcon::Context)
    {
        if (windowOpacity() == qreal(0))
        {
            _lShowAction->setEnabled(true);
        }
        else
        {
            _lShowAction->setEnabled(false);
        }
    }
}

void LoginDialog::logout()
{
    QString outUrl ="http://" + verifyLEdit->text() + ":8080/idesktop/logout.action?username=" + userEdit->text();
    _namOut->get(QNetworkRequest(QUrl(outUrl)));
}

void LoginDialog::setHideActionEnabled(bool enable)
{
    _dHideAction->setEnabled(enable);
}

void LoginDialog::setShowActionEnabled(bool enable)
{
    _dShowAction->setEnabled(enable);
}
