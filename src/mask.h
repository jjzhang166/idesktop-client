#ifndef MASK_H
#define MASK_H

#include <QDialog>
#include <QPainter>
//#include <QtGui>

#include "rotatewarnningitem.h"

class Mask : public QDialog
{
        Q_OBJECT

public:
        Mask(int width, int height, QWidget *parent = 0, Qt::WFlags flags = 0);
        ~Mask();

        void setLoadStart();
        void setLoadStop();
        void setText(const QString &text);

protected:
        void paintEvent(QPaintEvent *event);

private:
        int _width;
        int _height;

        RotateWarnningLabel *_loadItem;

        QString _text;
};

#endif // MASK_H
