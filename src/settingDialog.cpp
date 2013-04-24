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
#include <QLabel>
#include <QSpinBox>
#include "hintlineEdit.h"
#include "dynamicbutton.h"
#include "settingDialog.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), \
      _titlePressed(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFixedSize(413, 315);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0x00));
    setPalette(pal);

    QPixmap closeIcon(":images/dlgCloseIcon.png");
    QPixmap closeIconHover(":images/dlgCloseIconHover.png");
    DynamicButton *closeButton = new DynamicButton(closeIcon, closeIconHover, this);
    closeButton->setGeometry(351, 11, CLOSE_WIDTH, CLOSE_HEIGHT);

    equalRadio = new QRadioButton(this);
    equalRadio->move(50, 80);
    lb1 = new QLabel(this);
    lb1->setText("将图标平均分布在几个页面上");
    lb1->move(70, 100);
    lb2 = new QLabel(this);
    lb2->setText("页面个数: ");
    lb2->move(70, 140);
    pCountSpin = new QSpinBox(this);
    pCountSpin->move(140, 135);
    pCountSpin->setMinimum(1);

    miniRadio = new QRadioButton(this);
    miniRadio->move(50, 180);
    lb3 = new QLabel(this);
    lb3->setText("将图标平均尽可能的放在前面的页面上");
    lb3->move(70, 200);

    QPushButton *okButton = new QPushButton(this);
    okButton->setText("确定");
    okButton->setGeometry(220, 240, 60, 30);

    QPushButton *clButton = new QPushButton(this);
    clButton->setText("取消");
    clButton->setGeometry(300, 240, 60, 30);

    equalRadio->setChecked(true);
    lb3->setEnabled(false);

    connect(equalRadio, SIGNAL(toggled(bool)), this, SLOT(updateUI(bool)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(clButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void SettingDialog::accept()
{
    if (equalRadio->isChecked()) {
        equal = true;
        pCount = pCountSpin->value();
    }
    else {
        equal = false;
    }
    QDialog::accept();
}

void SettingDialog::updateUI(bool checked)
{
    if (equalRadio->isChecked()) {
        lb1->setEnabled(true);
        lb2->setEnabled(true);
        pCountSpin->setEnabled(true);
        lb3->setEnabled(false);
    }
    if (miniRadio->isChecked()) {
        lb1->setEnabled(false);
        lb2->setEnabled(false);
        pCountSpin->setEnabled(false);
        lb3->setEnabled(true);
    }
}

void SettingDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap bg(":images/bg_loginBox.png");
    painter.drawPixmap(0, 0, bg);
    painter.drawPixmap(22, 17, QPixmap(":images/logo_login.png"));
    painter.setPen(Qt::gray);
    painter.setFont(QFont("", 11, QFont::Black));
    painter.drawText(55, 33, "设置");
    QDialog::paintEvent(event);
}

void SettingDialog::mousePressEvent(QMouseEvent *event)
{
    if (QRect(10, 10, 350, 35).contains(event->pos())) {
        _titlePressed = true;
        startDrag = event->pos();
    }
}

void SettingDialog::mouseReleaseEvent(QMouseEvent *event)
{
    _titlePressed = false;
}

void SettingDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (_titlePressed) {
        QPoint newPoint = pos() + event->pos() - startDrag;
        move(newPoint);
    }
}
