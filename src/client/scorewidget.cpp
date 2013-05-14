#include "scorewidget.h"

#include <QPainter>
#include <QImage>
#include <QRect>
#include <QMouseEvent>
#include <QPoint>
#include <QCursor>

ScoreWidget::ScoreWidget(QWidget *parent) :
    OpacityWidget(parent, Qt::FramelessWindowHint)
  , _score(0)
  , _toMark(0)
  , _canMark(false)
{
    setMouseTracking(true);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCursor(QCursor(Qt::PointingHandCursor));

    _image = new QImage(":images/full.png");
    _offImage = new QImage(":images/null.png");
}

void ScoreWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    int h = height();
    for (int i = 0;i < 5;i++) {
        QRect rImage(h*i, 0, h,h);
        if (i < _toMark)
            painter.drawImage(rImage, *_image);
        else
            painter.drawImage(rImage, *_offImage);
    }
}

void ScoreWidget::setScore(int s, const bool &isCanMark, const bool &showTooTip)
{
    s = s > 5 ? 5 : s;
    s = s < 0 ? 0 : s;

    _score = s;
    _toMark = s;
    _canMark = isCanMark;
    if (!_canMark && showTooTip)
        setToolTip("安装后才能评分");
    else
        setToolTip("");
    repaint();
}

int ScoreWidget::score()
{
    return _score;
}

void ScoreWidget::mousePressEvent ( QMouseEvent * event)
{
    if (!_canMark || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    _score = _toMark;
    emit appscored(_score);
}

void ScoreWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!_canMark) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    QPoint p = event->pos();

    if (p.x() > height() * 5) {
        _toMark = _score;
        repaint();
        return;
    }
    _toMark = p.x() / height() + 1;
    repaint();
}

void ScoreWidget::leaveEvent ( QEvent * event )
{
    Q_UNUSED(event);

    _toMark = _score;
    repaint();
}
