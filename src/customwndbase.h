#ifndef CUSTOMWNDBASE_H
#define CUSTOMWNDBASE_H

#include <QWidget>
#include <QPushButton>
#include <QColor>
#include <QRect>
#include <QPoint>
#include <QSize>

//const int Title_Height = 25;

class CustomBtn;
class QPropertyAnimation;
class CustomWndBase : public QWidget
{
    Q_OBJECT
public:

    enum CustomStyle{
       WindowNormal = 0x01,
       WindowMaximizeButtonHint = 0x02,
       WindowMinimizeButtonHint = 0x04,
       WindowCloseButtonHint = 0x08,
       NotPainTitleImage = 0x10,
       DragWholeWindow = 0x20
    };

    Q_DECLARE_FLAGS(CustomFlags, CustomStyle);

    explicit CustomWndBase(QWidget *parent = 0, CustomFlags style = WindowNormal);

public:

    //strBackImg-  背景图片路径
    void setBkgImg(const QString &strBackImg);
    //strTitle-    标题栏内容
    void setTitle(const QString &strTitle);
    //strCloseOn-  关闭按钮,焦点图片路径
    void setCloseOn(const QString &strCloseOn);
    //strCloseOff- 关闭按钮,失去焦点图片路径
    void setCloseOff(const QString &strCloseOff);
    //strMaxOn-    最大化按钮,焦点图片路径
    void setMaxOn(const QString &strMaxOn);
    //strMaxOff-   最大化按钮,失去焦点图片路径
    void setMaxOff(const QString &strMaxOff);
    //strRtnOn-    还原按钮,焦点图片路径
    void setRtnOn(const QString &strRtnOn);
    //strRtnOff-   还原按钮,失去焦点图片路径
    void setRtnOff(const QString &strRtnOff);
    //strMinOn-    最小化按钮,焦点图片路径
    void setMinOn(const QString &strMinOn);
    //strMinOff-   最小化按钮,失去焦点图片路径
    void setMinOff(const QString &strMinOff);
    //nRed,nGreen,nBlue,nAlpha,线条颜色
    void setLineColor(const int &nRed, const int &nGreen, const int &nBlue, const int &nAlpha);


    ////////////////////////////////////
    //重写初始为最大化显示
    void showMaximized();

    void centerWidget();

    void setCustomStyle(const CustomFlags &style);


    void setHEdgeWidth(const int &w);
    void setTopEdgeHeight(const int &h);
    void setBottomEdgeHeight(const int &h);


    int titleHeight();
    void titleHeight(const int &h);

protected:

    virtual void maxWnd(const bool &bMax){}

public:
    void mousePressEvent ( QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void paintEvent(QPaintEvent *event);
    bool event ( QEvent * e );
    void resizeEvent( QResizeEvent * event);
    void closeEvent ( QCloseEvent * event );
    void showEvent ( QShowEvent * event );
signals:
    
public slots:

private:

    void construct();
    void calRect();

    void maxWnd();
    void minWnd();

    void DrawTitle(QPainter* painter);
    void DrawBody(QPainter* painter);
private:

    QColor _penColor;//color of background
    QString _strBackImg;//默认使用资源中的":images/background.png"
    QString _strBackLeft;//默认使用资源中的":images/widget/window_corner_lowerLeft.png"
    QString _strBackRight;//默认使用资源中的":images/widget/window_corner_lowerRight.png"
    QString _strBackLeftEdge;//默认使用资源中的":images/widget/window_side_left.png"
    QString _strBackRightEdge;//默认使用资源中的":images/widget/window_side_right.png"
    QString _strBackBottomEdge;//默认使用资源中的":images/widget/window_side_bottom.png"

    QString _strCloseOn;//默认使用资源中的":images/dlgCloseIconHover.png"
    QString _strCloseOff;//默认使用资源中的":images/dlgCloseIcon.png"

    QString _strMaxOn;//默认使用资源中的":images/max_on.png"
    QString _strMaxOff;//默认使用资源中的":images/max_off.png"

    QString _strReturnOn;//默认使用资源中的":images/rtn_on.png"
    QString _strReturnOff;//默认使用资源中的":images/rtn_off.png"

    QString _strMinOn;//默认使用资源中的":images/min_on.png"
    QString _strMinOff;//默认使用资源中的":images/min_off.png"

    QRect _closeRt;
    QRect _maxRt;
    QRect _titleRt;
    QRect _minRt;

    CustomBtn* _pCloseBtn;
    CustomBtn* _pMaxBtn;
    CustomBtn* _pMinBtn;

    bool _bMax;//是否最大化
    QSize _size;//最大化之前的大小
    QPoint _point;//最大化之前的坐标

    bool _bPressTitle;//选中标题栏
    QString _strTitle;//标题栏内容
    QString _strTitleIcon;//标题栏图标，//默认使用资源中的":images/appcenter.ico"
    QString _strTitleImg;//标题栏背景图，//默认使用资源中的":images/widget/window_side_top.png"
    QString _strTitleLeft;//标题栏左角图片，//默认使用资源中的":images/widget/window_corner_upperLeft.png"
    QString _strTitleRight;//标题栏右角图片，//默认使用资源中的":images/widget/window_corner_upperRight.png"

    QPropertyAnimation *_animation;

    QPoint _beginDrag;

    CustomFlags _style;

    int _HEdgeWidth;
    int _topEdgeHeight;
    int _bottomEdgeHeight;

    int _Title_Height;

};
Q_DECLARE_OPERATORS_FOR_FLAGS(CustomWndBase::CustomFlags)

#endif // CUSTOMWNDBASE_H
