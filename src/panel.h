#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
//#include "iconarrangedlg.h"
#include "dynamicbutton.h"
#include "switcher.h"

class QPropertyAnimation;
class AppCenter;
//class AppCenterWdg;
class TestWidget;
class Panel : public QWidget
{
    Q_OBJECT
public:
    Panel(QWidget *parent = NULL);
    ~Panel() {}
//    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void setAutoHide(bool hide)
    {
        autoHide = hide;
    }
//    void animationShow();
//    void animationHide();
signals:
    void quit();
    void desktop();
    void setEqual(int);
    void setMini();
    void pageChanged(int);    
#if 0
    void showSwitcherDesktop();
    void showVacDesktop();
    void showLocalDesktop();
    void showDirDesktop();
    void showPerDesktop();
# endif

    void showSkinWidget();
    void showSoftwareWidget();

    //void showStore();
    void checkDirState();

public slots:
    void runCenter();
#if 0
    void switcherBtnClicked();
    void vacBtnClicked();
    void localBtnClicked();
    void dirBtnClicked();
    void perBtnClicked();
    void quitBtnClicked();
#endif

    void skinClicked();
    void softwareClicked();
    void quitClicked();

    void animationShow();
    void animationHide();

private:
    QPixmap setTransparentPixmap(const QString &pix);

private:
    bool autoHide;
    bool visible;
#if 0
    QPixmap _switcherPix;
    QPixmap _switcherPixHover;
    QPixmap _vacPix;
    QPixmap _vacPixHover;
//    QPixmap _localPix;
//    QPixmap _localPixHover;
    QPixmap _dirPix;
    QPixmap _dirPixHover;
    QPixmap _personPix;
    QPixmap _personPixHover;
    QPixmap _quitPix;
    QPixmap _quitPixHover;
#endif
    QPropertyAnimation *animation;
//    AppCenterWdg *_center;
//    TestWidget* _center;
#if 0
    DynamicButton *_switcherButton;
    DynamicButton *_vacButton;
    DynamicButton *_localButton;
    DynamicButton *_dirButton;
    DynamicButton *_personButton;
    DynamicButton *_quitButton;

    bool _vacBtnHover;
    bool _localBtnHover;
    bool _skinBtnHover;
#endif
    Switcher *_switcherSkin;
    Switcher *_switcherSoftware;
    Switcher *_switcherQuit;
        DynamicButton *_quitButton;
        QPixmap _quitPix;
        QPixmap _quitPixHover;
};

#endif
