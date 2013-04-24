#ifndef SCROLLUPWARDBUTTON_H
#define SCROLLUPWARDBUTTON_H

#include "custombtn.h"

class ScrollUpwardButton : public CustomBtn
{
    Q_OBJECT
public:
    explicit ScrollUpwardButton(QString image, QWidget *parent = 0);

protected:

    void paintEvent(QPaintEvent *event);
    void leaveEvent(QEvent *event);
    void enterEvent ( QEvent * event );
signals:
    
public slots:

private:

    bool _bEnter;
    QString _image;
};

#endif // SCROLLUPWARDBUTTON_H
