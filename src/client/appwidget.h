#ifndef APPWIDGET_H
#define APPWIDGET_H

#include <QWidget>
#include <QString>
#include <QRect>
#include <QFont>

const int App_Widget_Width = 272;
const int App_Widget_Height = 93;

class ScoreWidget;
class InstallButton;
class UpdateButton;
class UnInstallButton;
class AppData;
class QImage;
class CustomBtn;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class QProgressBar;
class QTextEdit;
class AppWidget : public QWidget// , public WidgetRefreshBase
{
    Q_OBJECT
public:
    explicit AppWidget(QWidget *parent = 0);

    void setData(AppData* appData);

    void UpdateScore(QString appName, int score);

public:
    void enterEvent ( QEvent * event );
    void leaveEvent ( QEvent * event );
    void paintEvent( QPaintEvent *event );
    void resizeEvent ( QResizeEvent * event );
    void mousePressEvent ( QMouseEvent * event);


signals:
    
public slots:
    
    void beginInstall();
    void appScored(int);

private:

    void CalRect();

private:

    bool _bEnter;//鼠标是否在窗口区域内

 //   QString _IconPath;
    QString _Title;
    QString _BriefTitle;
    QString _Desription;

    QRect _rIcon;
    QRect _rProgressBar;

    QImage* _tabbg;

    CustomBtn* _AddUpdate;
//    QLabel* _DesriptionLabel;

    QFont _TitleFont;//
    QFont _DespFont;//

    int _TitleTextHeight;

    /////////////////////////

    QVBoxLayout *_mainVerticalLayout;
    QHBoxLayout *_spacerHorizontalLayout;
    QSpacerItem *_horizontalSpacer;
    QVBoxLayout *_labelVerticalLayout;
    QLabel *_TitleLabel;
    QLabel *_briefLabel;
  //  QLabel *_DescriptionLabel;
    QTextEdit* _DescriptionLabel;

    InstallButton* _InstallBtn;//添加
//    UpdateButton* _UpdateBtn;//更新
//    UnInstallButton* _UnInstallBtn;//卸载

    QProgressBar *_progressBar;

    AppData* _appData;


//    ScoreWidget* _scoreWdg;
};

#endif // APPWIDGET_H
