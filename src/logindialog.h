#include <QRadioButton>
#include <QDialog>
#include <QLabel>
#include <QEvent>
#include <QPaintEvent>
#include <QComboBox>
#include <QPixmap>
#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "qtipaddressedit/qipaddressedit.h"
#include "qtipaddressedit/qipaddressedititem.h"
//vac
#include "../commuinication.h"
#define CLOSE_WIDTH 47
#define CLOSE_HEIGHT 35

class HintLineEdit;
class QCheckBox;
class DynamicButton;
extern QList<APP_LIST> g_RemoteappList;
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(QWidget *parent);
    ~LoginDialog() {}
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
public slots:
    void auth();
    void onLoginFinished(QNetworkReply *reply);
	//vac
    void heartbeat();
    void onDone();

    //void updateVappIcon();
private:
    void userError(QString text);
    void passError(QString text);
    void connError(QString text);
    void connMsg(QString text);
    QNetworkAccessManager *_nam;
    QComboBox *serverAddr;
    QCheckBox *remoteAuth;
    HintLineEdit *userEdit;
    HintLineEdit *passEdit;
    QIpAddressEdit *ipAddrEdit;
    /****************************************************************/
    //wangyaoli
    //HintLineEdit *ipEdit;
    /****************************************************************/
    DynamicButton *submit;
    QPoint startDrag;
    QString _uerror;
    QString _perror;
    QString _cerror;
    QString _cmsg;
    bool _titlePressed;
    bool _authSuccess;
    bool _finished;
	//ivapp
    commuinication *_commui;
    int _retryTimes;
    QTimer *heartbeat_timer;  //heart beat.
    bool  Isheartbeat;    // check if send the heart beat packet
    bool _vacfinished;

    //QTimer *_updateVappTimer;
};
