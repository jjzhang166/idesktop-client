#include "dlgchangepassword.h"
#include "ui_dlgchangepassword.h"
#include <QMessageBox>
#include <QDomDocument>
#include <QFile>
#include "dlglogin.h"

DlgChangePassword::DlgChangePassword(QString ip, QString sessionID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgChangePassword)
{
    ui->setupUi(this);
    int width = this->geometry().width();
    int height = this->geometry().height();
    this->setFixedSize(width,height); //设置窗体固定大小

    setWindowFlags(Qt::WindowTitleHint);
#ifdef Q_WS_X11
    QIcon icon(":images/VAC.ico");//for linux 
    setWindowIcon(icon);
#endif
    ui->txt_oldPwd->setEchoMode(QLineEdit::Password);
    ui->txt_newPwd->setEchoMode(QLineEdit::Password);
    ui->txt_confirmPwd->setEchoMode(QLineEdit::Password);

    _finished = false;
    _commui = new commuinication();
    connect(_commui, SIGNAL(done()), this, SLOT(onDone()));

    _smIP = ip;
    _sID = sessionID;
}

DlgChangePassword::~DlgChangePassword()
{
    delete ui;
}

void DlgChangePassword::onDone()
{
    qDebug()<<"onDone"<<"\n";
    _finished = true;
}


void DlgChangePassword::on_pushButton_clicked()
{

        ui->pushButton->setEnabled(false);
        if(ui->txt_oldPwd->text()=="" || ui->txt_newPwd->text()=="" || ui->txt_confirmPwd->text()=="")
        {
            QMessageBox::warning(this, tr("change password"),tr("please input all info"), tr("OK"));
            ui->txt_oldPwd->clear();
            ui->txt_newPwd->clear();
            ui->txt_confirmPwd->clear();
            ui->pushButton->setEnabled(true);
            return;
        }
        if(ui->txt_newPwd->text()!=ui->txt_confirmPwd->text())
        {
             QMessageBox::warning(this, tr("change password"),tr("please confirm your new password"), tr("OK"));
             ui->txt_oldPwd->clear();
             ui->txt_newPwd->clear();
             ui->txt_confirmPwd->clear();
             ui->pushButton->setEnabled(true);
             return;
        }

        _commui->setInfo(_smIP, _sID);
        _commui->changePwd(ui->txt_oldPwd->text(),ui->txt_newPwd->text(),ui->txt_confirmPwd->text());
        while (!_finished)
            QApplication::processEvents();
        _finished = false;
        if(_commui->errID == "10000")
        {
            DlgLogin cr;
            cr.cryptoRsa(ui->txt_newPwd->text());

            QMessageBox::warning(this, tr("change password"), _commui->errInfo, tr("OK"));

            ui->txt_oldPwd->clear();
            ui->txt_newPwd->clear();
            ui->txt_confirmPwd->clear();
	    this->close();
        }
        else
        {
            if(_commui->errID == "10063")
               QMessageBox::warning(this, tr("change password"), tr("old password error"), tr("OK"));


            else
                QMessageBox::warning(this, tr("change password"), _commui->errInfo, tr("OK"));

            ui->txt_oldPwd->clear();
            ui->txt_newPwd->clear();
            ui->txt_confirmPwd->clear();
        }
         ui->pushButton->setEnabled(true);
}

void DlgChangePassword::on_pushButton_2_clicked()
{
    //this->reject();
}
