#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QWidget>
#include "dlg.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    resize(400, 160);
    setFixedSize(400, 160);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(30, 120, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    label = new QLabel(this);
    label->setGeometry(QRect(10, 0, 371, 111));
    label->setWordWrap(true);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setWindowTitle("");
    label->setText("");
}

void Dialog::setText(QString text)
{
    label->setText(text);
}
/*
void Dialog::accept()
{
}

void Dialog::reject()
{
}
*/
