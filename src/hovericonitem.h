#ifndef HOVERICONITEM_H
#define HOVERICONITEM_H

#include <QWidget>

/* ##############################################
 * Mouse Enter IconItem
 */

class HoverIconItem : public QWidget
{
    Q_OBJECT
public:
    HoverIconItem(int width, int height, QWidget *parent = 0);
    ~HoverIconItem();

protected:
    void paintEvent(QPaintEvent *event);

private:
    int _width;
    int _height;

};

#endif // HOVERICONITEM_H
