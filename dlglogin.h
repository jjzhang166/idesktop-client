#ifndef DLGLOGIN_H
#define DLGLOGIN_H

#include <QDialog>
#include <QPoint>
#include <QTimer>
#include <QSysInfo>
#include "commuinication.h"
#include "applist.h"

class QSysInfo;
extern int temp;
extern QString xmlPath;
extern QString WIN_IconPath;
extern QString WIN_TtempPath;
extern "C"
{
    #include "openssl/des.h"
    #include "openssl/rc4.h"
    #include "openssl/rsa.h"
    #include "openssl/sha.h"
    #include "openssl/pem.h"
    #include "openssl/evp.h"
    #include "openssl/bio.h"
}

namespace Ui {
class DlgLogin;
}
class QPoint;       //定义一个Qpoint类

class DlgLogin : public QDialog
{
    Q_OBJECT
public:
    explicit DlgLogin(QWidget *parent = 0);
    ~DlgLogin();
    QString sID()
    {
        return _sessionID;
    }
    void checkautologin();
    QString crypto(QString input,int i);//加密函数，des加密，input为要加密或解密的QString，i 为1加密，0为解密
    //int cryptoRsa();
    //void generate_key();
    void cryptoRsa(QString input);
    QString decryptRsa();
    QString smIP()
    {
        return _smIP;
    }
    QString GetSystemInfo();

private slots:
    void on_btnCancel_clicked();
    void on_btnLogin_clicked();
    void onDone();
    void HeartBeat();

    void doLogin();

    void on_btnLogin_pressed();

    void on_btnCancel_pressed();

    void on_hideButton_pressed();

    void on_quitButton_pressed();
    void onRemCheckerStateChange(int state);

signals:
    //void autoClick();

private:
    Ui::DlgLogin *ui;

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent * e);
private:
    QPoint dragPosition;
    commuinication *_commui;
    QList<APP_LIST *> _appList;
    bool _finished;
    QString _input;
    QString _output;
    QString _port;
    bool _isRememberInfo;
    bool _isAutoLogin;
    //RSA   *rsa;
    int flen;
    QString _sessionID;
    QString _smIP;
    QTimer * _timerHeartBeat;
    bool    _login;
};

#endif // DLGLOGIN_H
