#ifndef DYNAMICBUTTON_H
#define DYNAMICBUTTON_H
#include <QLabel>

class DynamicButton : public QLabel
{
    Q_OBJECT
public:
    DynamicButton(QPixmap normal, QPixmap hover, QWidget *parent = NULL)
        : QLabel(parent), _normal(normal), _hover(hover), \
          _entered(false), _value(0)
    {
        setPixmap(_normal);
    }
    ~DynamicButton() {}
    void setValue(int value)
    {
        _value = value;
    }

    void setCurrentPixmap(QPixmap normal)
    {
        _normal = normal;
        setPixmap(_normal);
        repaint();
    }

signals:
    void clicked();
    void valueClicked(int);
private:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QPixmap _normal;
    QPixmap _hover;
    bool _entered;
    int _value;
};

#endif // DYNAMICBUTTON_H
