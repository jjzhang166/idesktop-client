#ifndef APPMESSAGEBOX_H
#define APPMESSAGEBOX_H

#include <QtCore>
#include <QtGui>
#include <QDialog>

class AppMessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit AppMessageBox(bool hasCancel, QWidget *parent = 0);
    void setText(const QString &text);

protected:

    void paintEvent(QPaintEvent *e);
    void mousePressEvent ( QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    
public slots:

private:
    QLabel *label;
    bool _bPressTitle;//бЁжа
    QPoint _beginDrag;
};

#endif // APPMESSAGEBOX_H
