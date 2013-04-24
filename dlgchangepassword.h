#ifndef DLGCHANGEPASSWORD_H
#define DLGCHANGEPASSWORD_H

#include <QDialog>
#include "commuinication.h"

namespace Ui {
    class DlgChangePassword;
}

class DlgChangePassword : public QDialog
{
    Q_OBJECT

public:
    explicit DlgChangePassword(QString ip, QString sessionID, QWidget *parent = 0);
    ~DlgChangePassword();

private slots:
    void onDone();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::DlgChangePassword *ui;
    QString _smIP;
    QString _sID;
    commuinication *_commui;
    bool _finished;
};

#endif // DLGCHANGEPASSWORD_H
