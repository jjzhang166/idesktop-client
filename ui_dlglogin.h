/********************************************************************************
** Form generated from reading UI file 'dlglogin.ui'
**
** Created: Fri Apr 12 17:20:18 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGLOGIN_H
#define UI_DLGLOGIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DlgLogin
{
public:
    QLineEdit *txtSmAddress;
    QLineEdit *txtUserName;
    QLineEdit *txtPwd;
    QPushButton *btnLogin;
    QPushButton *btnCancel;
    QCheckBox *chkRember;
    QCheckBox *chkAuto;
    QPushButton *hideButton;
    QPushButton *quitButton;

    void setupUi(QDialog *DlgLogin)
    {
        if (DlgLogin->objectName().isEmpty())
            DlgLogin->setObjectName(QString::fromUtf8("DlgLogin"));
        DlgLogin->setEnabled(true);
        DlgLogin->resize(462, 309);
        DlgLogin->setFocusPolicy(Qt::NoFocus);
        DlgLogin->setAutoFillBackground(false);
        DlgLogin->setStyleSheet(QString::fromUtf8(""));
        txtSmAddress = new QLineEdit(DlgLogin);
        txtSmAddress->setObjectName(QString::fromUtf8("txtSmAddress"));
        txtSmAddress->setGeometry(QRect(141, 83, 228, 21));
        QFont font;
        font.setPointSize(9);
        txtSmAddress->setFont(font);
        txtSmAddress->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border: 2px solid gray;\n"
"    border-radius: 5px;\n"
"    padding: 0 4px; \n"
"	\n"
"	image: url(:/images/lineedit.png);\n"
"	selection-background-color: darkgray;\n"
" }\n"
""));
        txtUserName = new QLineEdit(DlgLogin);
        txtUserName->setObjectName(QString::fromUtf8("txtUserName"));
        txtUserName->setGeometry(QRect(141, 120, 228, 21));
        txtUserName->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border: 2px solid gray;\n"
"    border-radius: 5px;\n"
"    padding: 0 4px; \n"
"	\n"
"	image: url(:/images/lineedit.png);\n"
"	selection-background-color: darkgray;\n"
" }\n"
""));
        txtPwd = new QLineEdit(DlgLogin);
        txtPwd->setObjectName(QString::fromUtf8("txtPwd"));
        txtPwd->setGeometry(QRect(141, 156, 228, 21));
        txtPwd->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border: 2px solid gray;\n"
"    border-radius: 5px;\n"
"    padding: 0 4px; \n"
"	\n"
"	image: url(:/images/lineedit.png);\n"
"	selection-background-color: darkgray;\n"
" }\n"
""));
        btnLogin = new QPushButton(DlgLogin);
        btnLogin->setObjectName(QString::fromUtf8("btnLogin"));
        btnLogin->setGeometry(QRect(140, 230, 84, 41));
        btnLogin->setBaseSize(QSize(0, 0));
        btnLogin->setMouseTracking(false);
        btnLogin->setAcceptDrops(false);
        btnLogin->setStyleSheet(QString::fromUtf8("QPushButton \n"
"	{\n"
"		border-image:url(:/images/login_normal.png); 		} \n"
"QPushButton:hover:pressed\n"
"	{\n"
"		border-image:url(:/images/login_hover.png);\n"
"	} \n"
"  QPushButton:hover:!pressed \n"
"	{\n"
"		border-image:url(:/images/login_hover.png);\n"
"	}"));
        btnLogin->setIconSize(QSize(400, 40));
        btnLogin->setCheckable(false);
        btnLogin->setChecked(false);
        btnLogin->setAutoRepeat(false);
        btnLogin->setAutoExclusive(false);
        btnLogin->setAutoRepeatDelay(300);
        btnLogin->setDefault(false);
        btnCancel = new QPushButton(DlgLogin);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
        btnCancel->setGeometry(QRect(250, 230, 84, 41));
        btnCancel->setMouseTracking(false);
        btnCancel->setFocusPolicy(Qt::StrongFocus);
        btnCancel->setStyleSheet(QString::fromUtf8("QPushButton \n"
"	{\n"
"		border-image:url(:/images/cancel_normal.png); 		} \n"
"QPushButton:hover:pressed\n"
"	{\n"
"		border-image:url(:/images/cancel_hover.png);\n"
"	} \n"
"  QPushButton:hover:!pressed \n"
"	{\n"
"		border-image:url(:/images/cancel_hover.png);\n"
"	}"));
        btnCancel->setIconSize(QSize(200, 40));
        btnCancel->setAutoExclusive(true);
        btnCancel->setAutoDefault(false);
        chkRember = new QCheckBox(DlgLogin);
        chkRember->setObjectName(QString::fromUtf8("chkRember"));
        chkRember->setGeometry(QRect(149, 188, 81, 16));
        chkRember->setStyleSheet(QString::fromUtf8("QCheckBox {\n"
"     spacing: 5px;\n"
"	 color: white ;\n"
" }\n"
"\n"
" QCheckBox::indicator {\n"
"     width: 13px;\n"
"     height: 13px;\n"
" }\n"
"\n"
" QCheckBox::indicator:unchecked {\n"
"     image: url(:/images/check_unselected.png);\n"
" }\n"
"QCheckBox::indicator:unchecked:hover {\n"
"     image: url(:/images/check_skipped_unselected.png);\n"
" }\n"
"QCheckBox::indicator:checked {\n"
"     image: url(:/images/check_selected.png);\n"
" }\n"
"QCheckBox::indicator:checked:hover {\n"
"     image: url(:/images/check_skipped_selected.png);\n"
" }"));
        chkAuto = new QCheckBox(DlgLogin);
        chkAuto->setObjectName(QString::fromUtf8("chkAuto"));
        chkAuto->setGeometry(QRect(240, 188, 81, 16));
        chkAuto->setStyleSheet(QString::fromUtf8("QCheckBox {\n"
"     spacing: 5px;\n"
"	color:white;\n"
" }\n"
"\n"
" QCheckBox::indicator {\n"
"     width: 13px;\n"
"     height: 13px;\n"
" }\n"
"\n"
" QCheckBox::indicator:unchecked {\n"
"     image: url(:/images/check_unselected.png);\n"
" }\n"
"QCheckBox::indicator:unchecked:hover {\n"
"     image: url(:/images/check_skipped_unselected.png);\n"
" }\n"
"QCheckBox::indicator:checked {\n"
"     image: url(:/images/check_selected.png);\n"
" }\n"
"QCheckBox::indicator:checked:hover {\n"
"     image: url(:/images/check_skipped_selected.png);\n"
" }"));
        hideButton = new QPushButton(DlgLogin);
        hideButton->setObjectName(QString::fromUtf8("hideButton"));
        hideButton->setGeometry(QRect(410, 10, 16, 16));
        hideButton->setStyleSheet(QString::fromUtf8("QPushButton \n"
"	{\n"
"		border-image:url(:/images/minize_normal.png); 		} \n"
"QPushButton:hover:pressed\n"
"	{\n"
"		border-image:url(:/images/minize_hover.png);\n"
"	} \n"
"  QPushButton:hover:!pressed \n"
"	{\n"
"		border-image:url(:/images/minize_hover.png);\n"
"	}"));
        quitButton = new QPushButton(DlgLogin);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));
        quitButton->setGeometry(QRect(430, 10, 16, 16));
        quitButton->setStyleSheet(QString::fromUtf8("QPushButton \n"
"	{\n"
"		border-image:url(:/images/shutdown_normal.png); 		} \n"
"QPushButton:hover:pressed\n"
"	{\n"
"		border-image:url(:/images/shutdown_hover.png);\n"
"	} \n"
"  QPushButton:hover:!pressed \n"
"	{\n"
"		border-image:url(:/images/shutdown_hover.png);\n"
"	}"));

        retranslateUi(DlgLogin);
        QObject::connect(btnLogin, SIGNAL(clicked()), DlgLogin, SLOT(showMinimized()));
        QObject::connect(quitButton, SIGNAL(clicked()), DlgLogin, SLOT(close()));

        QMetaObject::connectSlotsByName(DlgLogin);
    } // setupUi

    void retranslateUi(QDialog *DlgLogin)
    {
        DlgLogin->setWindowTitle(QApplication::translate("DlgLogin", "change password", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        txtSmAddress->setToolTip(QApplication::translate("DlgLogin", "\346\234\215\345\212\241\345\231\250\345\234\260\345\235\200", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        txtSmAddress->setInputMask(QString());
        txtSmAddress->setText(QString());
#ifndef QT_NO_TOOLTIP
        txtUserName->setToolTip(QApplication::translate("DlgLogin", "\347\224\250\346\210\267\345\220\215", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        txtUserName->setText(QString());
#ifndef QT_NO_TOOLTIP
        txtPwd->setToolTip(QApplication::translate("DlgLogin", "\345\257\206\347\240\201", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        txtPwd->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        txtPwd->setText(QString());
        btnLogin->setText(QString());
        btnCancel->setText(QString());
        chkRember->setText(QApplication::translate("DlgLogin", "\350\256\260\344\275\217\350\256\276\347\275\256", 0, QApplication::UnicodeUTF8));
        chkAuto->setText(QApplication::translate("DlgLogin", "\350\207\252\345\212\250\347\231\273\345\275\225", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        hideButton->setToolTip(QApplication::translate("DlgLogin", "\346\234\200\345\260\217\345\214\226", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        hideButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        quitButton->setToolTip(QApplication::translate("DlgLogin", "\345\205\263\351\227\255", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        quitButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DlgLogin: public Ui_DlgLogin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGLOGIN_H
