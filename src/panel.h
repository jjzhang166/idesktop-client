#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
//#include "iconarrangedlg.h"
#include "dynamicbutton.h"

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
    void setAutoHide(bool hide)
    {
        autoHide = hide;
    }
    void animationShow();
    void animationHide();
signals:
    void quit();
    void desktop();
    void setEqual(int);
    void setMini();
    void pageChanged(int);    

    void showSwitcherDesktop();
    void showVacDesktop();
    void showLocalDesktop();
    void showDirDesktop();
    void showPerDesktop();

    //void showStore();
    void checkDirState();

public slots:
    void runCenter();

    void switcherBtnClicked();
    void vacBtnClicked();
    void localBtnClicked();
    void dirBtnClicked();
    void perBtnClicked();

private:
    QPixmap setTransparentPixmap(const QString &pix);

private:
    bool autoHide;
    bool visible;

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

    QPropertyAnimation *animation;
//    AppCenterWdg *_center;
//    TestWidget* _center;

    DynamicButton *_switcherButton;
    DynamicButton *_vacButton;
    DynamicButton *_localButton;
    DynamicButton *_dirButton;
    DynamicButton *_personButton;
    DynamicButton *_quitButton;

    bool _vacBtnHover;
    bool _localBtnHover;
    bool _skinBtnHover;
};

#endif
