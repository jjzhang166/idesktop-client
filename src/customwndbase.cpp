#include "customwndbase.h"
#include <QPainter>
#include <QPen>
#include <QByteArray>
#include <QPixmap>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QFont>
#include <QPropertyAnimation>
#include <windows.h>
#include <QEasingCurve>

#include <windowsx.h>


#include "custombtn.h"

#define FONTSIZE 9

const int Edge_Width = 8 * 0.8;
const int Pen_Width = 1;
const int Color_Dodge = 25;

const int Title_Corner_Width = 39 * 0.8;
const int Btn_Width = 46 * 0.8;
const int Other_Btn_Width = 31 * 0.8;

//QColor CustomWndBase::_penColor = QColor(130,130,130,255);

CustomWndBase::CustomWndBase(QWidget *parent, CustomFlags style) :
    QWidget(parent, Qt::FramelessWindowHint)
  , _penColor(130,130,130,255)
  , _strBackImg(":images/background.png")
  , _strBackLeft(":images/widget/window_corner_lowerLeft.png")
  , _strBackRight(":images/widget/window_corner_lowerRight.png")
  , _strBackLeftEdge(":images/widget/window_side_left.png")
  , _strBackRightEdge(":images/widget/window_side_right.png")
  , _strBackBottomEdge(":images/widget/window_side_bottom.png")
  , _strCloseOn(":images/widget/window_close_hover.png")
  , _strCloseOff(":images/widget/window_close.png")
  , _strMaxOn(":images/widget/window_maximize_hover.png")
  , _strMaxOff(":images/widget/window_maximize.png")
  , _strReturnOn(":images/widget/window_restoreDown_hover.png")
  , _strReturnOff(":images/widget/window_restoreDown.png")
  , _strMinOn(":images/widget/window_minimize_hover.png")
  , _strMinOff(":images/widget/window_minimize.png")
  , _bMax(false)
  , _bPressTitle(false)
  , _strTitleIcon(":images/appstore.ico")
  , _strTitleImg(":images/widget/window_side_top.png")
  , _strTitleLeft(":images/widget/window_corner_upperLeft.png")
  , _strTitleRight(":images/widget/window_corner_upperRight.png")
  , _style(style)
  , _HEdgeWidth(Edge_Width)
  , _topEdgeHeight(0)
  , _bottomEdgeHeight(Edge_Width)
  , _Title_Height(28)
{
    construct();
}

void CustomWndBase::construct()
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);

 //   setObjectName("customwidget");

//    setStyleSheet("CustomWndBase#customwidget{background-color:rgba(0,0,0,0)}" );

 //   setStyleSheet("CustomWndBase{background: transparent; border: 0px solid white;}");

 //   setStyleSheet("background:transparent");


    _pCloseBtn = new CustomBtn(this);
    _pMaxBtn = new CustomBtn(this);
    _pMinBtn = new CustomBtn(this);

    _animation = new QPropertyAnimation(this,"geometry");
    //_animation->setEasingCurve(QEasingCurve::QEasingCurve::OutQuint);
    _animation->setEasingCurve(QEasingCurve::OutQuint);

    _pCloseBtn->SetOnImg(_strCloseOn);
    _pCloseBtn->SetOffImg(_strCloseOff);

    _pMaxBtn->SetOnImg(_strMaxOn);
    _pMaxBtn->SetOffImg(_strMaxOff);

    _pMinBtn->SetOnImg(_strMinOn);
    _pMinBtn->SetOffImg(_strMinOff);

    setCustomStyle(_style);
}

void DrawImg(QPainter* pPainter, const QString &strImgPath, const QRect &rect)
{
    QPixmap pixmap;
    pixmap.load(strImgPath);
    pPainter->drawPixmap(rect.left(), rect.top(),pixmap.scaled(rect.size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)); // 绘制不规则背景
}

void DrawImg(QPainter* pPainter, const QString &strImgPath, const int &x, const int &y, const QSize &size)
{
    QPixmap pixmap;
    pixmap.load(strImgPath);
    pPainter->drawPixmap(x,y,pixmap.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)); // 绘制不规则背景
}

const char* convertToChar(const QString &strVal)
{
    QByteArray strTmp = strVal.toAscii();
    return strTmp.data();
}
void CustomWndBase::DrawTitle(QPainter* painter)
{
    QSize iconS(Title_Corner_Width, _Title_Height);

    //left corner
    DrawImg(painter, _strTitleLeft,0,0,iconS);
    //right corner
    DrawImg(painter, _strTitleRight,width() - Title_Corner_Width,0,iconS);

    //body
    DrawImg(painter, _strTitleImg,Title_Corner_Width ,0, QSize(width() - 2* Title_Corner_Width, _Title_Height));

    //draw icon
    DrawImg(painter, _strTitleIcon, _Title_Height / 6 + _HEdgeWidth , _Title_Height / 6,
            QSize(_Title_Height /3 * 2, _Title_Height /3 * 2));
}

void CustomWndBase::DrawBody(QPainter* painter)
{
    QSize iconS(Title_Corner_Width, _Title_Height);

    int y = height() - _Title_Height;
    //left corner
    DrawImg(painter, _strBackLeft,0,y,iconS);
    //right corner
    DrawImg(painter, _strBackRight,width() - Title_Corner_Width,y,iconS);

    QSize edgeVS(Title_Corner_Width, height() - _Title_Height * 2);
    //left edge
    DrawImg(painter, _strBackLeftEdge,0,_Title_Height, edgeVS);
    //right edge
    DrawImg(painter, _strBackRightEdge,width() - Title_Corner_Width,_Title_Height, edgeVS);

    QSize edgeHS(width() - 2 * Title_Corner_Width, _Title_Height);
    //bottom edge
    DrawImg(painter, _strBackBottomEdge,Title_Corner_Width,
            height() - _Title_Height, edgeHS);

    //body
    painter->setPen(Qt::white);
    painter->fillRect(QRect(Title_Corner_Width, _Title_Height, width() - 2 * Title_Corner_Width, height() - 2 * _Title_Height)
                            , QBrush(QColor(255, 255, 255, 255)));
}

void CustomWndBase::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    //draw bg
    if ((_style & NotPainTitleImage) || _strTitleImg == "") {
        DrawImg(&painter, _strBackImg,0 ,0 , size());
    }
    else {
        DrawBody(&painter);
        DrawTitle(&painter);
    }

    //draw bg edge
    QPen pen;
    pen.setColor(_penColor);
    pen.setWidth(Pen_Width);
    painter.setPen(pen);
    painter.setFont(QFont("", FONTSIZE, QFont::Black));

    ////draw title text
    QRect rTitle(_titleRt.left() + 10, _titleRt.top() + 8, _titleRt.width() - 20, _titleRt.height() - 10);
    if(_strTitleIcon != "")
        rTitle.setLeft(_titleRt.left() + 5 + _Title_Height);

    QByteArray strTmp = _strTitle.toAscii();
    painter.drawText(rTitle,QString::fromLocal8Bit(strTmp.data()));

    if (this->isMaximized())
        maxWnd();

    QWidget::paintEvent(event);
}
void CustomWndBase::mousePressEvent ( QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (!_pCloseBtn->isHidden() && _closeRt.contains(event->pos())) {
        close();
    }
    else if (!_pMaxBtn->isHidden() &&  _maxRt.contains(event->pos())) {
        maxWnd();
    }
    else if (!_pMinBtn->isHidden() && _minRt.contains(event->pos())) {
        minWnd();
    }
    else if (_titleRt.contains(event->pos()) || _style & DragWholeWindow) {
        _bPressTitle = true;
        _beginDrag = event->pos();
    }

    QWidget::mousePressEvent(event);
}
void CustomWndBase::mouseMoveEvent(QMouseEvent *event)
{
    if (!_bPressTitle || _bMax) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (!(event->buttons() & Qt::LeftButton)) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    //convert to relative pos
    QPoint relaPos(QCursor::pos() - _beginDrag);
    move(relaPos);
  //  event->accept();

//    repaint();
    QWidget::mouseMoveEvent(event);

}
void CustomWndBase::mouseReleaseEvent(QMouseEvent *event)
{
    _bPressTitle = false;
    QWidget::mouseReleaseEvent(event);
}

void CustomWndBase::mouseDoubleClickEvent ( QMouseEvent * event )
{
    if (event->button() != Qt::LeftButton)
        return;

    if (_pMaxBtn->isHidden())
        return;

    if (_titleRt.contains(event->pos()))
        maxWnd();
}

bool CustomWndBase::event ( QEvent * e )
{
     return QWidget::event(e);
}
void CustomWndBase::closeEvent ( QCloseEvent * event )
{
    _bPressTitle = false;

    _pCloseBtn->close();
    _pMaxBtn->close();
    _pMinBtn->close();

    QWidget::closeEvent(event);
    if (_bMax)
        maxWnd();
    _bMax = false;
}

void CustomWndBase::calRect()
{
    _titleRt.setLeft(_HEdgeWidth);
    _titleRt.setTop(_topEdgeHeight);
    _titleRt.setRight(width() - 2*_HEdgeWidth);
    _titleRt.setBottom(_topEdgeHeight + _Title_Height);

    QRect r1;
    r1.setRight(width() - _HEdgeWidth);
    r1.setLeft(r1.right() - Btn_Width);
    r1.setTop(_topEdgeHeight);
    r1.setBottom(_Title_Height - Pen_Width + _topEdgeHeight);

    QRect r2 = r1;
    r2.setRight(r1.left() - Pen_Width);
    r2.setLeft(r2.right() - Other_Btn_Width);

    QRect r3 = r1;
    r3.setRight(r2.left() - Pen_Width);
    r3.setLeft(r3.right() - Other_Btn_Width);

    QList<QRect> rList;
    rList.push_back(r1);
    rList.push_back(r2);
    rList.push_back(r3);

    int nCount = 0;
    if (_pCloseBtn->ID() != -1) {
        _closeRt = rList.at(_pCloseBtn->ID());
        nCount++;
    }
    if (_pMaxBtn->ID() != -1) {
        _maxRt = rList.at(_pMaxBtn->ID());
        nCount++;
    }
    if (_pMinBtn->ID() != -1) {
        _minRt = rList.at(_pMinBtn->ID());
        nCount++;
    }
    //////change the right of title
    _titleRt.setRight(_titleRt.right() - r3.width() * nCount - Pen_Width );
}

void CustomWndBase::showMaximized()
{
    show();
    _bMax = false;
    maxWnd();
}

void CustomWndBase::maxWnd()
{
    if (_pMaxBtn->isHidden())
        return;

    if (_bMax) {
        _bMax = false;
        _pMaxBtn->SetOnImg(_strMaxOn);
        _pMaxBtn->SetOffImg(_strMaxOff);
        QRect rect(_point, _size);
        setGeometry(rect);
    }
    else {
        _point = pos();
        _size = size();

        QDesktopWidget* desktopWidget = QApplication::desktop();
        QRect clientRect = desktopWidget->availableGeometry();

        _pMaxBtn->SetOnImg(_strReturnOn);
        _pMaxBtn->SetOffImg(_strReturnOff);
        //
        QRect maxRect(clientRect.left() - _HEdgeWidth, clientRect.top() - _topEdgeHeight ,
                      clientRect.width() + 2 * _HEdgeWidth, clientRect.height() + _topEdgeHeight + _bottomEdgeHeight);
        setGeometry(maxRect);
        _bMax = true;
    }
    maxWnd(_bMax);
}

void CustomWndBase::minWnd()
{
    _pMinBtn->SetOn(false);
    _pMinBtn->repaint();
    this->showMinimized();
}
//strBackImg-  背景图片路径
void CustomWndBase::setBkgImg(const QString &strBackImg)
{
    _strBackImg = strBackImg;
}

//strTitle-    标题栏内容
void CustomWndBase::setTitle(const QString &strTitle)
{
    _strTitle = strTitle;
}

//strCloseOn-  关闭按钮,焦点图片路径
void CustomWndBase::setCloseOn(const QString &strCloseOn)
{
    _strCloseOn = strCloseOn;
    if (_pCloseBtn)
        _pCloseBtn->SetOnImg(strCloseOn);
}

//strCloseOff- 关闭按钮,失去焦点图片路径
void CustomWndBase::setCloseOff(const QString &strCloseOff)
{
    _strCloseOff = strCloseOff;
    if (_pCloseBtn)
        _pCloseBtn->SetOffImg(strCloseOff);
}

//strMaxOn-    最大化按钮,焦点图片路径
void CustomWndBase::setMaxOn(const QString &strMaxOn)
{
    _strMaxOn = strMaxOn;
    if (_pMaxBtn)
        _pMaxBtn->SetOnImg(strMaxOn);
}

//strMaxOff-   最大化按钮,失去焦点图片路径
void CustomWndBase::setMaxOff(const QString &strMaxOff)
{
    _strMaxOff = strMaxOff;
    if (_pMaxBtn)
        _pMaxBtn->SetOffImg(strMaxOff);
}

//strRtnOn-    还原按钮,焦点图片路径
void CustomWndBase::setRtnOn(const QString &strRtnOn)
{
    _strReturnOn = strRtnOn;
}

//strRtnOff-   还原按钮,失去焦点图片路径
void CustomWndBase::setRtnOff(const QString &strRtnOff)
{
    _strReturnOff = strRtnOff;
}

//strMinOn-    最小化按钮,焦点图片路径
void CustomWndBase::setMinOn(const QString &strMinOn)
{
    _strMinOn = strMinOn;
    if (_pMinBtn)
        _pMinBtn->SetOnImg(strMinOn);
}

//strMinOff-   最小化按钮,失去焦点图片路径
void CustomWndBase::setMinOff(const QString &strMinOff)
{
    _strMinOff = strMinOff;
    if (_pMinBtn)
        _pMinBtn->SetOffImg(strMinOff);
}

//nRed,nGreen,nBlue,nAlpha,线条颜色
void CustomWndBase::setLineColor(const int &nRed, const int &nGreen, const int &nBlue, const int &nAlpha)
{
    _penColor.setRed(nRed);
    _penColor.setBlue(nBlue);
    _penColor.setGreen(nGreen);
    _penColor.setAlpha(nAlpha);
}

void CustomWndBase::resizeEvent( QResizeEvent * event)
{
    calRect();
    if (!_pCloseBtn->isHidden()) {
        _pCloseBtn->setFixedSize(_closeRt.width(), _closeRt.height());
        _pCloseBtn->move(_closeRt.left(), _closeRt.top());
    }
    if (!_pMaxBtn->isHidden()) {
    _pMaxBtn->setFixedSize(_maxRt.width(), _maxRt.height());
    _pMaxBtn->move(_maxRt.left(), _maxRt.top());

    }
    //draw min btn
    if (!_pMinBtn->isHidden()) {
        _pMinBtn->setFixedSize(_minRt.width(), _minRt.height());
        _pMinBtn->move(_minRt.left(), _minRt.top());
    }
}

void CustomWndBase::centerWidget()
{
    QPoint pos;
    if (parentWidget() == NULL) {
        pos.setX(QApplication::desktop()->width()/2 - width()/2);
        pos.setY(QApplication::desktop()->height()/2 - height()/2);
    }
    else {
        pos.setX(parentWidget()->width()/2 - width()/2);
        pos.setY(parentWidget()->height()/2 - height()/2);
    }
    move(pos);
}

void CustomWndBase::setCustomStyle(const CustomFlags &style)
{
    _style = style;

    _pCloseBtn->setID(0);
    _pMaxBtn->setID(1);
    _pMinBtn->setID(2);
    if (_style & WindowMaximizeButtonHint) {
        _pMaxBtn->hide();
        _pMaxBtn->setID(-1);
        _pMinBtn->setID(_pMinBtn->ID() - 1);
    }
    else {
        _pMaxBtn->show();
    }

    if (_style & WindowMinimizeButtonHint) {
        _pMinBtn->hide();
        _pMinBtn->setID(-1);
    }
    else {
        _pMinBtn->show();
    }

    if (_style & WindowCloseButtonHint) {
        _pCloseBtn->hide();
        _pMaxBtn->setID(_pMaxBtn->ID() - 1);
        _pMinBtn->setID(_pMinBtn->ID() - 1);
        _pCloseBtn->setID(-1);
    }
    else {
        _pCloseBtn->show();
    }

    if (_style & WindowNormal) {
        _pCloseBtn->show();
        _pMinBtn->show();
        _pMaxBtn->show();
    }
}

void CustomWndBase::setHEdgeWidth(const int &w)
{
    _HEdgeWidth = w;
}

void CustomWndBase::setTopEdgeHeight(const int &h)
{
    _topEdgeHeight = h;
}

void CustomWndBase::setBottomEdgeHeight(const int &h)
{
    _bottomEdgeHeight = h;
}

void CustomWndBase::showEvent ( QShowEvent * event )
{
    setCustomStyle(_style);

    QWidget::showEvent(event);
}

int CustomWndBase::titleHeight()
{
    return _Title_Height;
}

void CustomWndBase::titleHeight(const int &h)
{
    _Title_Height = h;
}
