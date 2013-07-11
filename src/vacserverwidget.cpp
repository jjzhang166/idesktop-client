#include <QtDebug>
#include <QPainter>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "vacserverwidget.h"

VacServerWidget::VacServerWidget(QWidget *parent)
    : QWidget(parent)
{
    resize(395, 300);

    _vacLabel = new QLabel(tr("V应用服务器："), this);
    _vacLabel->move(20, height() / 11);

    _portLabel = new QLabel(tr(" ："), this);
    _portLabel->move(2115, height() / 11);

    _nameLabel = new QLabel(tr("用户名："), this);
    _nameLabel->move(20, height() / 3);

    _pwdLabel = new QLabel(tr("密码："), this);
    _pwdLabel->move(20, height() / 11 * 5);

    _paasLabel = new QLabel(tr("P服务器"), this);
    _paasLabel->move(20, height() / 11 * 7);

    _vacEdit = new QLineEdit(this);
    _vacEdit->setGeometry(150, height() / 11 - 10, 148, 35);
    _vacEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:2px;");

    _portEdit = new QLineEdit(this);
    _portEdit->setGeometry(313, height() / 11 - 10, 45, 35);
    _portEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:2px;");

    _nameEdit = new QLineEdit(this);
    _nameEdit->setGeometry(150, height() / 3 - 20, 208, 35);
    _nameEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0);margin-left:2px;");

    _pwdEdit = new QLineEdit(this);
    _pwdEdit->setGeometry(150, height() / 11 * 5 - 10, 208, 35);
    _pwdEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0);margin-left:2px;");

    _paasEdit = new QLineEdit(this);
    _paasEdit->setGeometry(150, height() / 11 * 7 - 10, 208, 35);
    _paasEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0);margin-left:2px;");

    _saveBtn = new QPushButton(tr("应用"), this);
    _saveBtn->move(220, height() / 11 * 9);

    _cancelBtn = new QPushButton(tr("取消"), this);
    _cancelBtn->move(300, height() / 11 * 9);


    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT server,port,name,password FROM vacservers where id=\'%1\';").arg(1));
    while (query.next())
    {
        _vacEdit->setText(query.value(0).toString());
        _portEdit->setText(query.value(1).toString());
        _nameEdit->setText(query.value(2).toString());
        _pwdEdit->setText(query.value(3).toString());
    }

    QSqlQuery query2 = \
            QSqlDatabase::database("local").exec(QString("SELECT server FROM paasservers where id=1;"));
    while (query2.next())
    {
        _paasEdit->setText(query2.value(0).toString());
    }

    connect(_saveBtn, SIGNAL(clicked()), this, SLOT(saveServer()));
    connect(_cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
}

VacServerWidget::~VacServerWidget()
{

}

void VacServerWidget::saveServer()
{
    //qDebug() << "1232132312321";
    QSqlQuery query(QSqlDatabase::database("local"));

    QString qstr = QString("update vacservers "\
                           "set server=\'%1\', port=\'%2\', name=\'%3\', password=\'%4\' where id=1;")\
                           .arg(_vacEdit->text()).arg(_portEdit->text()).arg(_nameEdit->text()).arg(_pwdEdit->text());
    if(!query.exec(qstr)) {
        qDebug() <<"query failed";
        return;
    }

    QString qstrPaas = QString("update paasservers "\
                           "set server=\'%1\' where id=1;")\
                           .arg(_paasEdit->text());
    if(!query.exec(qstrPaas)) {
        qDebug() <<"query failed";
        return;
    }

    emit serverChanged();

    close();
}

void VacServerWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(0, 0, width(), height());
    painter.drawPixmap(150, height() / 11 - 10, 148, 35, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(313, height() / 11 - 10, 45, 35, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(150, height() / 3 - 20, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(150, height() / 11  * 5 - 10, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(150, height() / 11  * 7 - 10, QPixmap(":images/login_input_bg.png"));

    QWidget::paintEvent(event);
}
