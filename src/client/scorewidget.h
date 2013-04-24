#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include "../opacitywidget.h"

class QImage;
class ScoreWidget : public OpacityWidget
{
    Q_OBJECT
public:
    explicit ScoreWidget(QWidget *parent = 0);
    
    void setScore(int s, const bool &isCanMark, const bool &showTooTip = true);
    int score();

signals:
    
    void appscored(int);

public slots:
    
protected:

    void paintEvent(QPaintEvent *event);

    void mousePressEvent ( QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent ( QEvent * event );

private:

    QImage* _image;
    QImage* _offImage;
    int _score;
    int _toMark;
    bool _canMark;
};

#endif // SCOREWIDGET_H
