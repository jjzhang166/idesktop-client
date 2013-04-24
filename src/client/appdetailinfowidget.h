#ifndef APPDETAILINFOWIDGET_H
#define APPDETAILINFOWIDGET_H

#include <QDialog>
#include <QRect>
#include <QFont>
#include "widgetrefreshbase.h"

class ScoreWidget;
class InstallButton;
class UpdateButton;
class UnInstallButton;
class AppData;
class CustomBtn;
class QImage;
class QProgressBar;
class QLabel;
class QTextEdit;
class QMovie;
class QTimer;
class AppDetailInfoWidget : public QDialog , public WidgetRefreshBase
{
    Q_OBJECT
public:
    explicit AppDetailInfoWidget(QWidget *parent = 0);
    
    void OnUpdateScore(QString appName, int score);

    void showApp(AppData* data);
    void finishInstall();
    void finishUnInstall();

    void AppError(const QString &error);

    void Progress(qint64, qint64);
    void installFinished();
    void installDownloaded();
    void installCanceled();
    void loadFailed();

    void centerWidget();
    void clearWidget();

    void messageBox(const QString &msg,const QString &title,const QString &btn, QString state);

    void showMessage(QString msg);
signals:
    
public slots:

    void beginInstall();
    void cancelInstall();


    void beginUpdate();
    void cancelUpdate();

    void appScored(int);

    void timerUpDate();
    void textTimerUpDate();

protected:


    void closeEvent ( QCloseEvent * event );
    void paintEvent( QPaintEvent *event );
    void resizeEvent( QResizeEvent * event);

    void showEvent ( QShowEvent * event );

    void mousePressEvent ( QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

    void refreshBtn();
    QString convertByte(qint64 total);

private:

    bool _bPressTitle;//选中标题栏
    QPoint _beginDrag;

    CustomBtn* _pCloseBtn;
    QString _icon;
    QImage* _tabbg;
    QImage* _succes;

    QRect _rIcon;

    InstallButton* _InstallBtn;//添加
    UpdateButton* _UpdateBtn;//更新
    UnInstallButton* _UnInstallBtn;//卸载
    QProgressBar *_progressBar;

    QLabel *_TitleLabel;
    QLabel *_DescriptionLabel;
    QLabel* _detailTitle;

    QTextEdit* _detailInfo;

    int _TitleTextHeight;

    QFont _TitleFont;//
    QFont _DespFont;//
    QFont _DespTitleFont;//

    AppData* _appData;


    QMovie* _movie;
    QLabel* _movieLabel;

    QString _error;

    ScoreWidget* _scoreWdg;

    bool _scoreSucceed;
    QTimer* _timer;
    QTimer* _textTimer;
    int _alpha;
    int _textAlpha;

    QString _state;
};

#endif // APPDETAILINFOWIDGET_H
