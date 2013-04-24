#ifndef ICONARRANGEDLG_H
#define ICONARRANGEDLG_H

#include <QtGui>
#include <QtCore>
#include "virtualdesktop.h"

class IconArrangeDlg : public QDialog
{
    Q_OBJECT
public:
    IconArrangeDlg(VirtualDesktop *vdesk, QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
public slots:
    void accept();

signals:
    
public slots:
private:
    QScrollArea *list;
    QWidget *listWidget;
    bool _titlePressed;
    QPoint startDrag;
    QList<IconItem*> iconList;
    QList<QComboBox*> boxList;
public:
    VirtualDesktop *vdesk;
    QList<int> iconCounter;
};

class TheComboBox : public QComboBox
{
    Q_OBJECT
public:
    TheComboBox(IconArrangeDlg *idlg, QWidget *parent = NULL)
        : QComboBox(parent), dlg(idlg), oldIndex(-1)
    {
        qDebug() << oldIndex;
        connect(this, SIGNAL(currentIndexChanged(int)), \
                this, SLOT(onCurrentChanged(int)));
    }
public slots:
    void setCurrentIndex(int index)
    {
        oldIndex = index;
        QComboBox::setCurrentIndex(index);
    }

    void onCurrentChanged(int index)
    {
        if (index >= dlg->iconCounter.count())
            return;

        if (oldIndex != -1) {
            if (dlg->iconCounter[index] >= dlg->vdesk->_iconsPerPage){
                setCurrentIndex(oldIndex);
            } else {
                dlg->iconCounter[oldIndex]--;
                dlg->iconCounter[index]++;
                oldIndex = index;
            }
        } else {
            oldIndex = index;
        }
    }
private:
    int oldIndex;
    IconArrangeDlg *dlg;
};

#endif // ICONARRANGEDLG_H
