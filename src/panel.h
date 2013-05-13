#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include "iconarrangedlg.h"

class QPropertyAnimation;
class AppCenter;
class SettingDialog;
class AppCenterWdg;
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
    void iconDialog();
    void pageChanged(int);    
    //void showDesktop();
    //void showStore();

public slots:
    void runCenter();
    void settings();
    //void addApp();
    void showDesktop();
    //void showVappDesktop();
    void showBsDesktop();
    //void showToolDesktop();
    void showManageDesktop();

private:
    bool autoHide;
    bool visible;
    QPropertyAnimation *animation;
    AppCenterWdg *_center;
//    TestWidget* _center;
    IconArrangeDlg *_settingDialog;
};

#endif
