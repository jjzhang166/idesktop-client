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

    //strBackImg-  ����ͼƬ·��
    void setBkgImg(const QString &strBackImg);
    //strTitle-    ����������
    void setTitle(const QString &strTitle);
    //strCloseOn-  �رհ�ť,����ͼƬ·��
    void setCloseOn(const QString &strCloseOn);
    //strCloseOff- �رհ�ť,ʧȥ����ͼƬ·��
    void setCloseOff(const QString &strCloseOff);
    //strMaxOn-    ��󻯰�ť,����ͼƬ·��
    void setMaxOn(const QString &strMaxOn);
    //strMaxOff-   ��󻯰�ť,ʧȥ����ͼƬ·��
    void setMaxOff(const QString &strMaxOff);
    //strRtnOn-    ��ԭ��ť,����ͼƬ·��
    void setRtnOn(const QString &strRtnOn);
    //strRtnOff-   ��ԭ��ť,ʧȥ����ͼƬ·��
    void setRtnOff(const QString &strRtnOff);
    //strMinOn-    ��С����ť,����ͼƬ·��
    void setMinOn(const QString &strMinOn);
    //strMinOff-   ��С����ť,ʧȥ����ͼƬ·��
    void setMinOff(const QString &strMinOff);
    //nRed,nGreen,nBlue,nAlpha,������ɫ
    void setLineColor(const int &nRed, const int &nGreen, const int &nBlue, const int &nAlpha);


    ////////////////////////////////////
    //��д��ʼΪ�����ʾ
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
    QString _strBackImg;//Ĭ��ʹ����Դ�е�":images/background.png"
    QString _strBackLeft;//Ĭ��ʹ����Դ�е�":images/widget/window_corner_lowerLeft.png"
    QString _strBackRight;//Ĭ��ʹ����Դ�е�":images/widget/window_corner_lowerRight.png"
    QString _strBackLeftEdge;//Ĭ��ʹ����Դ�е�":images/widget/window_side_left.png"
    QString _strBackRightEdge;//Ĭ��ʹ����Դ�е�":images/widget/window_side_right.png"
    QString _strBackBottomEdge;//Ĭ��ʹ����Դ�е�":images/widget/window_side_bottom.png"

    QString _strCloseOn;//Ĭ��ʹ����Դ�е�":images/dlgCloseIconHover.png"
    QString _strCloseOff;//Ĭ��ʹ����Դ�е�":images/dlgCloseIcon.png"

    QString _strMaxOn;//Ĭ��ʹ����Դ�е�":images/max_on.png"
    QString _strMaxOff;//Ĭ��ʹ����Դ�е�":images/max_off.png"

    QString _strReturnOn;//Ĭ��ʹ����Դ�е�":images/rtn_on.png"
    QString _strReturnOff;//Ĭ��ʹ����Դ�е�":images/rtn_off.png"

    QString _strMinOn;//Ĭ��ʹ����Դ�е�":images/min_on.png"
    QString _strMinOff;//Ĭ��ʹ����Դ�е�":images/min_off.png"

    QRect _closeRt;
    QRect _maxRt;
    QRect _titleRt;
    QRect _minRt;

    CustomBtn* _pCloseBtn;
    CustomBtn* _pMaxBtn;
    CustomBtn* _pMinBtn;

    bool _bMax;//�Ƿ����
    QSize _size;//���֮ǰ�Ĵ�С
    QPoint _point;//���֮ǰ������

    bool _bPressTitle;//ѡ�б�����
    QString _strTitle;//����������
    QString _strTitleIcon;//������ͼ�꣬//Ĭ��ʹ����Դ�е�":images/appcenter.ico"
    QString _strTitleImg;//����������ͼ��//Ĭ��ʹ����Դ�е�":images/widget/window_side_top.png"
    QString _strTitleLeft;//���������ͼƬ��//Ĭ��ʹ����Դ�е�":images/widget/window_corner_upperLeft.png"
    QString _strTitleRight;//�������ҽ�ͼƬ��//Ĭ��ʹ����Դ�е�":images/widget/window_corner_upperRight.png"

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
