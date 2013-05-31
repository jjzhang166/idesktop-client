#include <QtDebug>
#include <QPainter>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "vacserverwidget.h"

VacServerWidget::VacServerWidget(QWidget *parent)
    : QWidget(parent)
{
    resize(395, 300);

    _vacLabel = new QLabel(tr("��������"), this);
    _vacLabel->move(20, height() / 9);

    _portLabel = new QLabel(tr(" ��"), this);
    _portLabel->move(295, height() / 9);

    _nameLabel = new QLabel(tr("�û�����"), this);
    _nameLabel->move(20, height() / 3);

    _pwdLabel = new QLabel(tr("���룺"), this);
    _pwdLabel->move(20, height() / 9 * 5);

    _vacEdit = new QLineEdit(this);
    _vacEdit->setGeometry(150, height() / 9 - 10, 148, 35);
    _vacEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:2px;");

    _portEdit = new QLineEdit(this);
    _portEdit->setGeometry(313, height() / 9 - 10, 45, 35);
    _portEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0); margin-left:2px;");

    _nameEdit = new QLineEdit(this);
    _nameEdit->setGeometry(150, height() / 3 - 10, 208, 35);
    _nameEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0);margin-left:2px;");

    _pwdEdit = new QLineEdit(this);
    _pwdEdit->setGeometry(150, height() / 9 * 5 - 10, 208, 35);
    _pwdEdit->setStyleSheet("border: 0px solid gray; background:rgba(255,255,255,0);margin-left:2px;");

    _saveBtn = new QPushButton(tr("Ӧ��"), this);
    _saveBtn->move(220, height() / 9 * 7);

    _cancelBtn = new QPushButton(tr("ȡ��"), this);
    _cancelBtn->move(300, height() / 9 * 7);


    QSqlQuery query = \
            QSqlDatabase::database("local").exec(QString("SELECT server,port,name,password FROM vacservers where id=\'%1\';").arg(1));
    while (query.next())
    {
        _vacEdit->setText(query.value(0).toString());
        _portEdit->setText(query.value(1).toString());
        _nameEdit->setText(query.value(2).toString());
        _pwdEdit->setText(query.value(3).toString());
    }

    connect(_saveBtn, SIGNAL(clicked()), this, SLOT(saveServer()));
    connect(_cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
}

VacServerWidget::~VacServerWidget()
{

}

void VacServerWidget::saveServer()
{
    qDebug() << "1232132312321";
    QSqlQuery query(QSqlDatabase::database("local"));

    QString qstr = QString("update vacservers "\
                           "set server=\'%1\', port=\'%2\', name=\'%3\', password=\'%4\' where id=1;")\
                           .arg(_vacEdit->text()).arg(_portEdit->text()).arg(_nameEdit->text()).arg(_pwdEdit->text());
    if(!query.exec(qstr)) {
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
    painter.drawPixmap(150, height() / 9 - 10, 148, 35, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(313, height() / 9 - 10, 45, 35, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(150, height() / 3 - 10, QPixmap(":images/login_input_bg.png"));
    painter.drawPixmap(150, height() / 9  * 5 - 10, QPixmap(":images/login_input_bg.png"));

    QWidget::paintEvent(event);
}
