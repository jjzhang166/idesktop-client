#ifndef TABLABEL_H
#define TABLABEL_H

#include <QLabel>
#include "custombtn.h"

class QImage;
class TabLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TabLabel(QWidget *parent = 0);


    void current(const bool &bCurrent);
signals:


    void Chosen(TabLabel*);

public slots:

protected:

    void enterEvent ( QEvent * event );
    void leaveEvent ( QEvent * event );
    void paintEvent(QPaintEvent* event);
    void mousePressEvent ( QMouseEvent * event );
private:

    bool _isMouseIn;
    bool _isCurrent;
    QImage* _image;
    QImage* _enterImage;
    QImage* _chosenImage;
    QImage* _disableImage;
};

#endif // TABLABEL_H
