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
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimeLine>
#include <QPainter>

#include "qtipaddressedit/qipaddressedit.h"
#include "qtipaddressedit/qipaddressedititem.h"

#define CLOSE_WIDTH 10
#define CLOSE_HEIGHT 10

extern QString serverip;
extern QString VacServer;
extern QString VacPort;
extern QString VacUser;
extern QString VacPassword;

extern QString PaasServer;

//extern QString xmlPath;
//extern QString iconDirPath;
//extern QString WIN_LOCAL_IconPath;
//extern QString WIN_VAPP_IconPath;
//extern QString WIN_PAAS_IconPath;
//extern QString WIN_TtempPath;
//extern QString iniPath;

extern QString USERNAME;
class HintLineEdit;
class QCheckBox;
class DynamicButton;
class IDesktopSettings;

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(QWidget *parent);
    ~LoginDialog();

//    void paintEvent(QPaintEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);

    void debugPrintVariant (QVariant const& v);

    void updateVacServer();

    QAction *_dHideAction;
    QAction *_dShowAction;

signals:
    void dQuit();
    void dShow();
    void dHide();
    void pShow();
    void pHide();

//    connect(showAction, SIGNAL(triggered()), panel, SLOT(show()));
//    connect(hideAction, SIGNAL(triggered()), panel, SLOT(hide()));
    void dVacServer();
    void dActivated(QSystemTrayIcon::ActivationReason);
public slots:
    void auth();
    void onLoginFinished(QNetworkReply *reply);

    void dialogAccepted();

    void minimized();
    void normal();
    void updateFlip(qreal val);
    void flip();

    //setting
    void settingSlot();
    void saveSettingSlot();
    void returnSlot();

//    void jsonDownloadFinished(QNetworkReply *reply);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
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

    DynamicButton *submit;
    DynamicButton *cancelBtn;
    QPoint startDrag;
    QString _uerror;
    QString _perror;
    QString _cerror;
    QString _cmsg;
    bool _titlePressed;
    bool _authSuccess;
    bool _finished;

    QSystemTrayIcon *_tray;
    QAction *_lShowAction;
    QAction *_lQuitAction;

    QAction *_dQuitAction;
//    QAction *_dHideAction;
//    QAction *_dShowAction;
//    QAction *_dVacServer;

    QMenu *_loginMenu;
    QMenu *_dashboardMenu;

    QTimeLine *_timeLine;
    QPainter painter;

    bool _flip;
    QTransform _tran;
    QTransform _t;
    qreal _finalyrot;
    // app ini
//    QString iniPath;


//    void setIcon(const QString &dirPath, const QString &iconPath);

    QString getLocalIcon(QString localPath);

    // setting line
    HintLineEdit *vacLEdit;
    HintLineEdit *paasLEdit;
    HintLineEdit *vacPortLEdit;
    HintLineEdit *verifyLEdit;
    DynamicButton *saveButton;
    DynamicButton *returnButton;
    bool _isSetting;
    void readVerifyServer();
    void updateUi();
    void settingUi();
    void readSetting();
    void VserverError(QString error);
    void PaasError(QString error);
    void saveVacUserInfo();
    QString _vserverError, _paasError;

    QString _tempVacIp;
    QString _tempPaasIp;
    QString _tempVerifyIp;
    QString _tempVacPort;

    DynamicButton *configurationButton;
    DynamicButton *minButton;
    DynamicButton *closeButton;
    IDesktopSettings *_settings;

//    QNetworkAccessManager *_namJson;
};
