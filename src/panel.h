#ifndef PANEL_H
#define PANEL_H

#include <QWidget>

#include "dynamicbutton.h"
#include "switcher.h"

class QPropertyAnimation;
class AppCenter;
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

signals:
    void quit();
    void desktop();
    void pageChanged(int);    

    void showSkinWidget();
    void showSoftwareWidget();

    //void showStore();
    void checkDirState();

public slots:

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

    QPropertyAnimation *animation;


    Switcher *_switcherSkin;
    Switcher *_switcherSoftware;
    Switcher *_switcherQuit;
    DynamicButton *_quitButton;
    QPixmap _quitPix;
    QPixmap _quitPixHover;
};

#endif
