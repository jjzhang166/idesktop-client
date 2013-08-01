#ifndef DUSTBIN_H
#define DUSTBIN_H

#include <QWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QPropertyAnimation>
#include <QLineEdit>
#include <QtDebug>

class Dustbin : public QWidget
{
    Q_OBJECT
public:
    Dustbin(QWidget *parent = 0);
    ~Dustbin();

    void setPage(int page)      { _page = page; }
    void setIndex(int index)    { _index = index; }
    void setId(int id)          { _id = id; }
//    void setHidden(bool hide);

    int page()                  { return _page; }
    int index()                 { return _index; }
    int id()                    { return _id; }

signals:

    void iconEnter();
    void iconLeave();
    void iconMove();
    void iconDrop(const QString &text, const QString &iconPath, int page, int index,
                  const QString &url, int type, const QString &uniqueName);

    void openItem();

public slots:
    void mouseStatus();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

    int _page;
    int _index;
    int _id;

    int _width;
    int _height;

    int _iconWidth;
    int _iconHeight;

    bool _isMouseMove;
    bool _isMousePress;
    bool _skipMouseMove;

};
#endif // DUSTBIN_H
