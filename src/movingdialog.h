#ifndef MOVINGDIALOG_H
#define MOVINGDIALOG_H
#include <QRadioButton>
#include <QEvent>
#include <QDialog>
#include <QPixmap>

class DynamicButton;
class VirtualDesktop;
class IconItem;
class MovingDialog : public QDialog
{
    Q_OBJECT
public:
    MovingDialog(VirtualDesktop *vdesk, IconItem *item, QWidget *parent = 0);
    ~MovingDialog() {}
    //bool event(QEvent *event);
public slots:
    void moveTo(int index);
private:
    QPixmap getDark(const QString &icon);
    QPixmap getCurrent(const QString &icon);
    VirtualDesktop *_vdesk;
    IconItem *_item;
};

#endif // MOVINGDIALOG_H
