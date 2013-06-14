#ifndef DIRMINWIDGET_H
#define DIRMINWIDGET_H

#include <QWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QPropertyAnimation>
#include <QLineEdit>
#include <QtDebug>

class IconMinItem;

class DirMinWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit DirMinWidget(QWidget *parent = 0);
    ~DirMinWidget();

    int addIcon(const QString &text, const QString &iconPath, int page, int index, const QString &url);
    void expand();
    void delPage(int page);
    void delIcon(const QString &text);
//    void animationMove(const QRect &start, const QRect &end);

signals:
    void pageIncreased();
    void pageDecreased();
    void pageChanged(int i);
    void sendUrl(const QString &url);

    void iconEnter();
    void iconLeave();
    void iconMove();
    void iconDrop(const QString &text, const QString &iconPath, const QString &url);

    void mouseClicked();

protected:
    void paintEvent(QPaintEvent *);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *);
    
private:
    int _width;
    int _height;

    int _gridWidth;
    int _gridHeight;

    int _col;
    int _row;
    int _iconsPerPage;
    QSize _pageSize;

    int _count;
    int _current;

    int _iconNum;
    IconMinItem *_inDrag;

    QList<int> _pages;
    QList<QList<IconMinItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QMap<QString, IconMinItem*> _iconDict;

//    QPropertyAnimation *_animation;

};

class IconMinItem : public QWidget
{
    Q_OBJECT
public:
    IconMinItem(QWidget *parent = NULL);
    ~IconMinItem();

    void animationMove(const QRect &start, const QRect &end);
    void setPixmap(const QString &icon);
    void setText(const QString &text);
    const QString & text();
    const QPixmap & pixmap();

//    void animationMove(const QRect &start, const QRect &end);
    void setPage(int page);
    void setIndex(int index);


    int page() { return _page; }
    int index() { return _index; }

    int _icontype;/*����ͼ��*/

    void setUrl(const QString &url);
    QString getUrl() { return _url; }

signals:
    void clicked();
    void sendUrl(const QString &url);

public slots:
//    void startTremble();
//    void stopTremble();
//    void doTremble(qreal);
protected:
    void paintEvent(QPaintEvent *event);

private:

//    QTimeLine *_timeline;
//    QPropertyAnimation *_animation;
//    int _trembling;

    int _page;
    int _index;

    QPixmap _pixmap;
    QString _url;
    QString _pixText;
    QString _text;
    QPropertyAnimation *_animation;
};

class DirLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    DirLineEdit(QString hint = QString(""), \
                 QWidget *parent = NULL); // QString pixmap = QString(""), \
    ~DirLineEdit() {}


public slots:
    void setHint(const QString &hint)
    {
        _hint = hint;
        repaint();
    }
protected:
    virtual void paintEvent(QPaintEvent* event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:

    void setImgs(QImage *strLeft, QImage *strCenter, QImage *strRight);

private:
    QString _hint;
//    QPixmap _pixmap;

    QImage *_editLeft;
    QImage *_editCenter;
    QImage *_editRight;

    QImage *_left;
    QImage *_center;
    QImage *_right;

    QColor _color;

};

class DirMinShowWidget : public QWidget
{
    Q_OBJECT
public:
    DirMinShowWidget(QWidget *parent = 0);
    ~DirMinShowWidget();

    void animationMove(const QRect &start, const QRect &end);

    void setPage(int page)      { _page = page; }
    void setIndex(int index)    { _index = index; }
    void setId(int id)          { _id = id; }
//    void setHidden(bool hide);

    int page()                  { return _page; }
    int index()                 { return _index; }
    int id()                    { return _id; }


signals:
    void sendUrl(const QString &url);

    void iconEnter();
    void iconLeave();
    void iconMove();
//    void iconDrop(int id, const QString &text, const QString &iconPath, const QString &url);
    void iconDrop(const QString &text, const QString &iconPath, const QString &url);

//    void openItem(int id, int page, int index);
    void openItem();


public slots:
    void mouseClicked();
    void iconDropEvent(const QString &text, const QString &iconPath, const QString &url);

private:
    DirMinWidget *_dirMinWidget;
    DirLineEdit *_dirLineEdit;

//    QDrag *_drag;

//    QPoint dragStartPosition;
    QPropertyAnimation *_animation;

    int _page;
    int _index;
    int _id;
};
#endif // DIRMINWIDGET_H