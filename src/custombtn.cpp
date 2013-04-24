#include "custombtn.h"
#include <QPainter>
#include <QFontMetrics>
#include <QByteArray>
#include <QTextCodec>

void DrawImg(QPainter* pPainter, const QString &strImgPath, const QRect &rect);
void DrawImg(QPainter* pPainter, const QString &strImgPath, const int &x, const int &y, const QSize &size);

CustomBtn::CustomBtn(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint)
  , _bOn(false)
  , _bEnabled(true)
  , _id(-1)
{

    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    hide();
}

void CustomBtn::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!_bEnabled)
        DrawImg(&painter, _strEnabled, 0, 0, size());
    else if(_bOn)
        DrawImg(&painter, _strOn, 0, 0, size());
    else
        DrawImg(&painter, _strOff, 0, 0, size());

    if (_text != "") {

       painter.setPen(Qt::white);

       QFontMetrics metrics(painter.font());
       int textWidth = metrics.width(_text);
       int textHeight = metrics.height();

       int left = (width() - textWidth)/2;
       int top = (height() - textHeight)/2;
       QByteArray data = _text.toAscii();
       painter.drawText(QRect(left ,top ,width() - 2*left, height() - 2 * top), QString::fromLocal8Bit(data.data()));
   }

    QWidget::paintEvent(event);
}

void CustomBtn::mousePressEvent ( QMouseEvent * event)
{
    emit clicked();
    QWidget::mousePressEvent(event);
}

void CustomBtn::SetOnImg(const QString &strOn)
{
    _strOn = strOn;
}

void CustomBtn::SetOffImg(const QString &strOff)
{
    _strOff = strOff;
}

void CustomBtn::enterEvent ( QEvent * event )
{
    _bOn = true;
    repaint();
}

void CustomBtn::leaveEvent ( QEvent * event )
{
    _bOn = false;
    repaint();
}

void CustomBtn::SetOn(const bool &bOn)
{
    _bOn = bOn;
}


void CustomBtn::closeEvent ( QCloseEvent * event )
{
    _bOn = false;
    repaint();
    QWidget::closeEvent(event);
}

void CustomBtn::setText(const QString &strText)
{
    _text = strText;
}

void CustomBtn::enabled(const bool &bEnable)
{
    setEnabled(bEnable);
    _bEnabled = bEnable;
}

void CustomBtn::SetEnabledImg(const QString &strEnabled)
{
    _strEnabled = strEnabled;
}

int CustomBtn::ID()
{
    return _id;
}

void CustomBtn::setID(const int &id)
{
    _id = id;
}

QString CustomBtn::text()
{
    return _text;
}
