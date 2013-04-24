#include "dlglogin.h"
#include "ui_dlglogin.h"
#include <QMessageBox>
#include <QPixmap>
#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QDir>
#include <QXmlStreamWriter>
#include <QDomDocument>
#include <QSslSocket>
#include <QRegExp>

#include <QDesktopWidget>

#include "commuinication.h"
#include "applist.h"

//#include "src/dashboard.h"

#include "common.h"

#ifdef Q_WS_WIN
#include "ShlObj.h "
#endif

int temp;
QString xmlPath;
QString WIN_IconPath;
QString WIN_TtempPath;
bool g_isAutoLogin;
QList<APP_LIST> g_myList;

DlgLogin::DlgLogin(QWidget *parent) :
    QDialog(parent),ui(new Ui::DlgLogin),
    _finished(false)
{
    ui->setupUi(this);

    //setAttribute(Qt::WA_TranslucentBackground, true);
    //setWindowOpacity(1);
    //setAttribute(Qt::WA_TranslucentBackground);
    //setWindowOpacity(0.7);
    //QPalette pal = palette();
    //pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    //setPalette(pal);
    this->setWindowFlags(Qt::FramelessWindowHint);
    GetSystemInfo();
    setWindowTitle(tr("VAC Client"));
    //设置文字颜色；
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);

    //ui->label->setPalette(pa);
    //ui->label_2->setPalette(pa);
    //ui->label_3->setPalette(pa);
    ui->chkRember->setPalette(pa);
    ui->chkAuto->setPalette(pa);

    ui->txtPwd->setEchoMode(QLineEdit::Password);

    _commui = new commuinication();
#ifdef Q_WS_X11
    QIcon icon(":images/logo.ico");//for linux
    setWindowIcon(icon);
    xmlPath=QDir::homePath() + "/.VAC";//.VAC create in install.sh
    QDir *temp=new QDir;
    temp->mkdir(xmlPath);
#else
     char folder[MAX_PATH] = {0};
     SHGetFolderPathA(NULL, CSIDL_APPDATA , 0,0,folder);
     WIN_TtempPath = QString(folder);
     WIN_IconPath=WIN_TtempPath+"/icon/";

     xmlPath = QDir::currentPath();
#endif
     //generate info.xml;
     #ifdef Q_WS_WIN
     QFile xmlFile(WIN_TtempPath+XML_NAME);
#else
     QFile xmlFile(xmlPath + XML_NAME);
 #endif
     if(!xmlFile.exists())
     {
         qDebug()<<"xmlPath:"<<WIN_TtempPath+XML_NAME;

         QString xmlData("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
                         "<login>"\
                             "<smip></smip>"\
                             "<port></port>"\
                             "<username></username>"\
                             "<password></password>"\
                             "<remember></remember>"\
                             "<auto></auto>"\
                         "</login>");

         if (!xmlFile.open(QIODevice::WriteOnly))
             qDebug() << "open file failed when writing";

         xmlFile.write(xmlData.toUtf8());

         xmlFile.close();
     }
    //rsaTest();
     _login=false;

     if(ui->chkRember->checkState() == Qt::Unchecked)
     {
        ui->chkAuto->setCheckState(Qt::Unchecked);
        ui->chkAuto->setCheckable(false);
     }
     else
         ui->chkAuto->setCheckable(true);
     connect(ui->chkRember, SIGNAL(stateChanged(int)), this, SLOT(onRemCheckerStateChange(int)));

}

void DlgLogin::onRemCheckerStateChange(int state)
{
    if (state == Qt::Unchecked)
    {
        ui->chkAuto->setCheckState(Qt::Unchecked);
        ui->chkAuto->setCheckable(false);
    }
    else
        ui->chkAuto->setCheckable(true);
}

QString DlgLogin::GetSystemInfo()
{
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
            break;

    }
#endif
#ifdef Q_WS_X11
    sysInfo = "linux";
#endif
    return sysInfo;
}
DlgLogin::~DlgLogin()
{
    delete ui;
}

void DlgLogin::onDone()
{
    qDebug()<<"onDone"<<"\n";
    _finished = true;
}

void DlgLogin::checkautologin()     //
{
 //qDebug()<<"WIN_TtempPath:"<<WIN_TtempPath<<endl;
#ifdef Q_WS_WIN
   QFile xmlFile(WIN_TtempPath + XML_NAME);
#else
   QFile xmlFile(xmlPath + XML_NAME);
#endif
    qDebug()<<"xmlFile:"<<xmlPath + XML_NAME<<endl;
    QDomDocument doc;
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "open file failed when writing";
    }
    if(!doc.setContent(&xmlFile))
    {
        xmlFile.close();
    }

    QDomNodeList nodelist;
    nodelist= doc.elementsByTagName("port");
    _port = nodelist.item(0).toElement().text();
    //login automatically;
    nodelist= doc.elementsByTagName("auto");
    if(nodelist.item(0).toElement().text() == "1")
    {
        qDebug()<<"login automatically";
        //this->setVisible(false);
        _isRememberInfo = true;
        ui->chkRember->setChecked(_isRememberInfo);

        _isAutoLogin = true;
        ui->chkAuto->setChecked(_isAutoLogin);

        nodelist= doc.elementsByTagName("smip");
        ui->txtSmAddress->setText(nodelist.item(0).toElement().text());

        nodelist= doc.elementsByTagName("username");
        ui->txtUserName->setText(nodelist.item(0).toElement().text());

        nodelist= doc.elementsByTagName("password");
        //_input=crypto(nodelist.item(0).toElement().text(),0);//


        //ui->txtPwd->setText(nodelist.item(0).toElement().text());
        QString password = decryptRsa();
	if (QString("") == password)
	{
            _isAutoLogin = false;
	    ui->chkAuto->setChecked(_isAutoLogin);
            ui->txtPwd->setFocus();
	}
	else
	{
	    ui->txtPwd->setText(password);
            temp = 1;
            doLogin();
            qDebug()<<"successfully";
	}

    }
    else{
        qDebug()<<"Normal login";
        //this->setVisible(true);
        //remember current input
        temp = 0;
        nodelist= doc.elementsByTagName("remember");
        if(nodelist.item(0).toElement().text() == "1")
        {
            _isRememberInfo = true;
            ui->chkRember->setChecked(_isRememberInfo);

            nodelist= doc.elementsByTagName("smip");
            ui->txtSmAddress->setText(nodelist.item(0).toElement().text());

            nodelist= doc.elementsByTagName("username");
            ui->txtUserName->setText(nodelist.item(0).toElement().text());

            nodelist= doc.elementsByTagName("auto");
            if(nodelist.item(0).toElement().text() == "1")
            {
                _isAutoLogin = true;
            }
            else
            {
                _isAutoLogin = false;
            }
            ui->chkAuto->setChecked(_isAutoLogin);

            ui->txtPwd->setFocus();
        }
    }
}


void DlgLogin::cryptoRsa(QString input)
{
    //generate_key();
    //int n;
    RSA *rsa=NULL;
    qDebug("generating RSA key...\n");
    OpenSSL_add_all_algorithms();
    rsa = RSA_generate_key(1024, RSA_F4, NULL, NULL);
    if(rsa==NULL)
    {
            qDebug("gen rsa error\n");
             exit(-1);
    }

    BIO *bp;
    /*
    bp=BIO_new_file("public.pem", "w");
    if(PEM_write_bio_RSAPublicKey(bp, rsa)!=1)
    {
            qDebug("write public key error\n");
            exit(-1);
    }
    BIO_free_all(bp);
    */
    unsigned char passwd[]="abc";
    // key

#ifdef Q_WS_WIN
    QString path=WIN_TtempPath+"/private.pem";
    qDebug()<<path<<endl;
    QByteArray ba = path.toUtf8();
    const char *c_str2 = ba.data();
    printf("str2: %s", c_str2);
    bp = BIO_new_file(c_str2, "w");
#endif
#ifdef Q_WS_X11
    QString fullPath = QString(xmlPath +"/private.pem");
    QByteArray byteArray = fullPath.toLocal8Bit();
    const char *cstr_path = byteArray.data();
    qDebug()<<QString("XMLPath: ")<< xmlPath <<endl;
    bp = BIO_new_file(cstr_path , "w");
#endif
    if(bp == NULL)
	return;
    //BIO_write_filename(bp, (void *)("private.pem"));
    if(PEM_write_bio_RSAPrivateKey(bp, rsa, EVP_des_ede3_ofb(), passwd,3, NULL,NULL )!=1)
    {
            qDebug("write public key error\n");
            exit(-1);
    }
    qDebug("store key successfully\n");
    BIO_free_all(bp);
    qDebug()<<input<<"\n";
    flen=RSA_size(rsa);
   // EVP_PKEY* pkey=NULL;
   /*
    RSA *rsaK1=RSA_new();

    OpenSSL_add_all_algorithms();
    BIO *BP;
    char FileName[]="public.pem";
    //BIO_read_filename(BP,FileName);
    BP=BIO_new_file(FileName,"rb");
    rsaK1=PEM_read_bio_RSAPublicKey(BP,&rsaK1,NULL,NULL);
    flen=RSA_size(rsaK1);
    unsigned char cipher[flen];
    qDebug("%d \n",flen);
    if(rsaK1==NULL)
    {
          BIO_free(BP);
          RSA_free(rsaK1);
          exit(-1);
     }
    */
    //QByteArray ba = input.toLatin1();
    unsigned char cipher[flen];
    QByteArray inputByteArray = input.toUtf8();
    unsigned char *in =(unsigned char *) inputByteArray.data();
    flen = RSA_public_encrypt(flen, in, cipher, rsa, RSA_NO_PADDING );
    qDebug( "RSA_public_encrypt: %d\ncipher: %s\n", flen,cipher);
    //BIO_free(BP);
    RSA_free(rsa);
    #ifdef Q_WS_WIN
    QFile file(WIN_TtempPath + "/file.dat");
    #else
    QFile file(xmlPath + "/file.dat");
    #endif

    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << (quint32)0xA0B0C0D0;
    for(int i=0; i<flen; i++)
    {
        quint8 tmmm=cipher[i];

        out <<tmmm;   // serialize a string
       // qDebug()<<tmmm;
     }
    out.setVersion(QDataStream::Qt_4_6);
    qDebug("cRsa::%s",cipher);

}

QString DlgLogin::decryptRsa()
{

    RSA *rsaK=RSA_new();
    OpenSSL_add_all_algorithms();
   // BIO *BP=BIO_new(BIO_s_file());
    BIO *BP;
    //char FileName[]="private.pem";
    //const char *path = (xmlPath + "/private.pem").toLocal8Bit().data();
    //qDebug("private:%s", path);
    //BP=BIO_new_file(path, "rb");
#ifdef Q_WS_WIN
    QString path=WIN_TtempPath+"/private.pem";
    QByteArray ba = path.toLocal8Bit();
    const char *c_str2 = ba.data();
    printf("str2: %s", c_str2);
    BP=BIO_new_file(c_str2, "rb");
#endif
#ifdef Q_WS_X11
    QString fullPath = QString(xmlPath + "/private.pem");
    QByteArray byteArray = fullPath.toLocal8Bit();
    const char *cstr_path = byteArray.data();
    printf("str path is %s\n", cstr_path);
    BP=BIO_new_file(cstr_path, "rb");

#endif
    if (NULL == BP)
	return QString("");

    char PSW[]="abc";

    rsaK=PEM_read_bio_RSAPrivateKey(BP,&rsaK,0,PSW);
    qDebug()<<rsaK;
    flen=RSA_size(rsaK);
    if(rsaK==NULL)
    {
          qDebug("read failed!\n");
          BIO_free(BP);
          RSA_free(rsaK);
          return QString("");
    }
    qDebug("get key successfully\n");
    #ifdef Q_WS_WIN
    QFile file1(WIN_TtempPath + "/file.dat");
    #else
    QFile file1(xmlPath + "/file.dat");
    #endif
    if(file1.open(QIODevice::ReadOnly))
        qDebug()<<"open!";
    QDataStream in(&file1);    // read the data serialized from the file
    in.setVersion(QDataStream::Qt_4_6);
    quint32 magic;
     in >> magic;
     qDebug()<<magic;
     unsigned char tmpp[flen];
     unsigned char cipher[flen];
    for(int j=0;j<flen;j++)
    {
        quint8 tm;
        in>>tm;           // extract "the answer is" and 42
        tmpp[j]=tm;
        //qDebug()<<tm;
        //qDebug()<<"cc:"<<cc<<endl;
   }
    qDebug("dRsa::%s",tmpp);

    flen = RSA_private_decrypt( flen, tmpp, cipher, rsaK , RSA_NO_PADDING );
    qDebug( "RSA_public_encrypt: %d\ncipher: %s\n", flen,cipher );
    RSA_free(rsaK);
    BIO_free(BP);
    QString out=QString::fromUtf8(QByteArray((char*)cipher));
    qDebug()<<out<<"\n";
    return out;

}

void DlgLogin::on_btnCancel_clicked()
{
    _commui->close();
    this->close();
}

void DlgLogin::on_btnLogin_clicked()
{

    _login=true;
    if(ui->txtSmAddress->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Caution"), tr("Please input server address"), tr("OK"));
        return;
    }
    QRegExp rx2("^(1?\\d\\d?|2[0-4]\\d|25[0-5])\.(1?\\d\\d?|2[0-4]\\d|25[0-5])\.(1?\\d\\d?|2[0-4]\\d|25[0-5])\.(1?\\d\\d?|2[0-4]\\d|25[0-5])$");
    qDebug()<<"address:"<<ui->txtSmAddress->text();
    if( !rx2.exactMatch(ui->txtSmAddress->text()))
    {
        QMessageBox::information(this, tr("Error"), tr("ip address error"), tr("OK"));
          return;
    }

    if(ui->txtUserName->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Caution"), tr("Please input user name"), "OK");
        return;
    }
    if(ui->txtPwd->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Caution"), tr("Please input password"),tr("OK"));
        return;
    }
    ui->btnLogin->setEnabled(false);
    ui->btnCancel->setEnabled(false);
    doLogin(); 
    ui->btnLogin->setEnabled(true);
    ui->btnCancel->setEnabled(true);
    _login=false;
}

void DlgLogin::doLogin()
{
    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));
    Commui._ip = ui->txtSmAddress->text();
    _commui->login(ui->txtSmAddress->text()+ ":" + _port, ui->txtUserName->text(), ui->txtPwd->text(),GetSystemInfo());
    while (!_finished)
        QApplication::processEvents();
    _finished = false;
    g_isAutoLogin = ui->chkAuto->isChecked();
    _isRememberInfo = ui->chkRember->isChecked();

    qDebug()<<(_commui->errID)<<(_commui->errInfo)<<"\n";

    if(_commui->errID == "10000")
    {
        qDebug()<<"temp"<<temp;
        if(!temp)
       {
            qDebug()<<"XML";
            //write to xml
            qDebug()<<"isRemeber:"<<_isRememberInfo;

#ifdef Q_WS_WIN
            QFile xmlFile(WIN_TtempPath + XML_NAME);
            qDebug()<<"XMLFILE:"<<(WIN_TtempPath + XML_NAME);
#else
            QFile xmlFile(xmlPath + XML_NAME);
            qDebug()<<"XMLFILE:"<<(xmlPath + XML_NAME);
#endif
            if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            {
                qDebug() << "open file failed when writing";
            }
            if(_isRememberInfo)
            {
                QXmlStreamWriter xmlWriter(&xmlFile);
                xmlWriter.setAutoFormatting(true);
                xmlWriter.writeStartDocument();
                xmlWriter.writeStartElement("login");
                xmlWriter.writeTextElement("smip", ui->txtSmAddress->text());
                xmlWriter.writeTextElement("port","80");
                xmlWriter.writeTextElement("username", ui->txtUserName->text());

                cryptoRsa(ui->txtPwd->text());
                //xmlWriter.writeTextElement("password", ui->txtPwd->text());

                if(ui->chkRember->isChecked())
                {
                    xmlWriter.writeTextElement("remember", "1");
                }
                else
                {
                    xmlWriter.writeTextElement("remember", "0");
                }

                if(ui->chkAuto->isChecked())
                {
                    xmlWriter.writeTextElement("auto", "1");
                }else
                {
                    xmlWriter.writeTextElement("auto", "0");
                }
                xmlWriter.writeEndElement();
            }else{
                QXmlStreamWriter xmlWriter(&xmlFile);
                xmlWriter.setAutoFormatting(true);
                xmlWriter.writeStartDocument();
                xmlWriter.writeStartElement("login");
                xmlWriter.writeTextElement("smip", ui->txtSmAddress->text());
                xmlWriter.writeTextElement("port","80");
                xmlWriter.writeTextElement("username", ui->txtUserName->text());
                xmlWriter.writeTextElement("remember", "0");
                xmlWriter.writeTextElement("auto", "0");
                xmlWriter.writeEndElement();
            }
            //---write complete----
        }

        _sessionID = _commui->getSessionID();
        _smIP = ui->txtSmAddress->text() + ":" + _port;

        //get the Application List;
        _commui->getAppList();
        while (!_finished)
            QApplication::processEvents();
        _finished = false;

        g_myList = _commui->getList();

        //mkdir that store icon files

        #ifdef Q_WS_WIN
        QString iconDirPath = WIN_IconPath ;
        #else
        QString iconDirPath =  xmlPath + "/icon/";
        #endif

        qDebug()<<"############## iconDirPath"<<iconDirPath;

        QDir iconDir(iconDirPath);
        if(!iconDir.exists())
        {
            iconDir.mkdir(iconDirPath);
        }

        //store ico file locally
        for(int i = 0; i < g_myList.count(); i++)
        {
            QString iconPath = QString("%1%2.ico")
                    .arg(iconDirPath)
                    .arg(g_myList[i].id);

            //check if ico file is existed, or dont donwload
            QFile chkFile(iconPath);
            if(chkFile.exists())
            {
                chkFile.close();
                continue;
            }
            chkFile.close();

            qDebug()<<g_myList[i].icon<<" = ";
            _commui->downloadIcon(QUrl(g_myList[i].icon), iconPath);
            while (!_finished)
                QApplication::processEvents();
            _finished = false;
        }
        //

        //start a heart beat;
       /* _timerHeartBeat = new QTimer(this);
        _timerHeartBeat->setInterval(5000);
        _timerHeartBeat->start();
        _timerHeartBeat->connect(_timerHeartBeat, SIGNAL(timeout()), this, SLOT(HeartBeat()));
    */
        this->accept();
    }
    else
    {
        QMessageBox::warning(this, tr("login failed"), _commui->errInfo, tr("OK"));

        temp = 0;
        return;
    }
}

void DlgLogin::HeartBeat()
{
    _commui->heartBeat();
    while (!_finished)
        QApplication::processEvents();
    _finished = false;

    qDebug()<<"timer heartbeat"<<"\n";

    if(_commui->errID == "10024")
    {
        QMessageBox::warning(this, tr("Caution"), tr("Session has been by administrator"), tr("OK"));
    }
}

void DlgLogin::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) //click left mouse
    {
         dragPosition = event->globalPos() - frameGeometry().topLeft();
         //globalPos() get root windows's position，frameGeometry().topLeft() get the frame position relative to left (0, 0)
         event->accept();   
    }
//    if (event->button() == Qt::RightButton)
//    {
//         close();
//    }
}


void DlgLogin::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton && dragPosition.x()!=0) //
    {
         move(event->globalPos() - dragPosition);//move windows
         event->accept();
    }
}

void DlgLogin::keyPressEvent(QKeyEvent *e)
{
    //login when enter pressed
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        if(!_login)
            on_btnLogin_clicked();
    }
}

void DlgLogin::on_btnLogin_pressed()
{
    dragPosition = QPoint(0,0);
}

void DlgLogin::on_btnCancel_pressed()
{
    dragPosition = QPoint(0,0);
}

void DlgLogin::on_hideButton_pressed()
{
    dragPosition = QPoint(0,0);
}

void DlgLogin::on_quitButton_pressed()
{
    dragPosition = QPoint(0,0);
}
