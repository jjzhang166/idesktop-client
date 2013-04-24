/********************************************************************************
** Form generated from reading UI file 'dlgchangepassword.ui'
**
** Created: Fri Apr 12 10:48:16 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGCHANGEPASSWORD_H
#define UI_DLGCHANGEPASSWORD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgChangePassword
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLineEdit *txt_oldPwd;
    QLineEdit *txt_newPwd;
    QLineEdit *txt_confirmPwd;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;

    void setupUi(QDialog *DlgChangePassword)
    {
        if (DlgChangePassword->objectName().isEmpty())
            DlgChangePassword->setObjectName(QString::fromUtf8("DlgChangePassword"));
        DlgChangePassword->setWindowModality(Qt::NonModal);
        DlgChangePassword->resize(370, 255);
        layoutWidget = new QWidget(DlgChangePassword);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(90, 60, 261, 131));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        txt_oldPwd = new QLineEdit(layoutWidget);
        txt_oldPwd->setObjectName(QString::fromUtf8("txt_oldPwd"));

        verticalLayout->addWidget(txt_oldPwd);

        txt_newPwd = new QLineEdit(layoutWidget);
        txt_newPwd->setObjectName(QString::fromUtf8("txt_newPwd"));

        verticalLayout->addWidget(txt_newPwd);

        txt_confirmPwd = new QLineEdit(layoutWidget);
        txt_confirmPwd->setObjectName(QString::fromUtf8("txt_confirmPwd"));

        verticalLayout->addWidget(txt_confirmPwd);

        layoutWidget1 = new QWidget(DlgChangePassword);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(170, 210, 181, 25));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(layoutWidget1);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setCursor(QCursor(Qt::ArrowCursor));

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(layoutWidget1);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);

        layoutWidget2 = new QWidget(DlgChangePassword);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(10, 70, 71, 111));
        verticalLayout_2 = new QVBoxLayout(layoutWidget2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(layoutWidget2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);

        label_3 = new QLabel(layoutWidget2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_2->addWidget(label_3);


        retranslateUi(DlgChangePassword);
        QObject::connect(pushButton_2, SIGNAL(clicked()), DlgChangePassword, SLOT(close()));

        QMetaObject::connectSlotsByName(DlgChangePassword);
    } // setupUi

    void retranslateUi(QDialog *DlgChangePassword)
    {
        DlgChangePassword->setWindowTitle(QApplication::translate("DlgChangePassword", "\344\277\256\346\224\271\345\257\206\347\240\201", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        txt_newPwd->setToolTip(QApplication::translate("DlgChangePassword", "\345\257\206\347\240\201\345\272\224\350\257\245\346\230\257\347\224\261\346\225\260\345\255\227\343\200\201\345\255\227\346\257\215\343\200\201'_','.'\347\273\204\346\210\220\347\232\2043-16\344\275\215\345\255\227\347\254\246\344\270\262", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        txt_confirmPwd->setToolTip(QApplication::translate("DlgChangePassword", "\345\257\206\347\240\201\345\272\224\350\257\245\346\230\257\347\224\261\346\225\260\345\255\227\343\200\201\345\255\227\346\257\215\343\200\201'_','.'\347\273\204\346\210\220\347\232\2043-16\344\275\215\345\255\227\347\254\246\344\270\262", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton->setText(QApplication::translate("DlgChangePassword", "\344\277\256\346\224\271", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("DlgChangePassword", "\345\205\263\351\227\255", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DlgChangePassword", "\346\227\247\345\257\206\347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DlgChangePassword", "\346\226\260\345\257\206\347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DlgChangePassword", "\347\241\256\350\256\244\350\276\223\345\205\245\357\274\232", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DlgChangePassword: public Ui_DlgChangePassword {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGCHANGEPASSWORD_H
