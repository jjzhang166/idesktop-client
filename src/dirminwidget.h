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
class DirLineEdit;
class DirMinWidget;
class DirMWidget;

class DirMinShowWidget : public QWidget
{
    Q_OBJECT
public:
    DirMinShowWidget(QWidget *parent = 0);
    ~DirMinShowWidget();

    void setPage(int page)      { _page = page; }
    void setIndex(int index)    { _index = index; }
    void setId(int id)          { _id = id; }
//    void setHidden(bool hide);

    int page()                  { return _page; }
    int index()                 { return _index; }
    int id()                    { return _id; }

    const QString & getDirText();

    void setMinDragEnable( bool dragEnable);

    void removeDirMinItem(const QString &text);
    void addDirMinItem(const QString &text, const QString &icon, \
                       int page, int index, const QString &url);

signals:
//    void sendUrl(const QString &url);

    void iconEnter();
    void iconLeave();
    void iconMove();
//    void iconDrop(int id, const QString &text, const QString &iconPath, const QString &url);
    void iconDrop(const QString &text, const QString &iconPath, int page, int index,
                  const QString &url, int type);

//    void openItem(int id, int page, int index);
    void openItem();
    void dragEnterMinWidget();

//    void delIcon(const QString &text);

public slots:
    void mouseClicked();
    void iconDropEvent(const QString &text, const QString &iconPath, const QString &url);

    void editFocusIn();
    void editFocusOut();

protected:
    void paintEvent(QPaintEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
//    void dragLeaveEvent(QDragLeaveEvent *event);
//    void dropEvent(QDropEvent *event);


private:
    DirMWidget *_dirMWidget;
    DirLineEdit *_dirLineEdit;

//    QDrag *_drag;

//    QPoint dragStartPosition;
    QPropertyAnimation *_animation;

    int _page;
    int _index;
    int _id;

    int _width;
    int _height;

//    QImage *_editLeft;
//    QImage *_editCenter;
//    QImage *_editRight;

//    QImage *_editLeftNormal;
//    QImage *_editCenterNormal;
//    QImage *_editRightNormal;

//    QImage *_left;
//    QImage *_center;
//    QImage *_right;

    QPixmap _editLeft;
    QPixmap _editCenter;
    QPixmap _editRight;
    QPixmap _left;
    QPixmap _center;
    QPixmap _right;
    QPixmap _editLeftNormal;
    QPixmap _editCenterNormal;
    QPixmap _editRightNormal;

//    void setImgs(QImage *strLeft, QImage *strCenter, QImage *strRight);
    void setImgs(QPixmap strLeft, QPixmap strCenter, QPixmap strRight);
//    QImage setTransparentPixmap(const QString &pix);
    QPixmap setTransparentPixmap(const QString &pix);

    int _iconWidth;
    int _iconHeight;

//    bool _dragEnable;

};

//
class DirMWidget : public QWidget
{
    Q_OBJECT

public: 
    DirMWidget(QWidget *parent = 0);
    ~DirMWidget();

//    void setPage(int page)      { _page = page; }
//    void setIndex(int index)    { _index = index; }
//    void setId(int id)          { _id = id; }
////    void setHidden(bool hide);

//    int page()                  { return _dirMWidget->page(); }
//    int index()                 { return _dirMWidget->index(); }
//    int id()                    { return _dirMWidget->id(); }
    void setMinDragEnable( bool dragEnable);

    void removeDirMinItem(const QString &text);
    void addDirMinItem(const QString &text, const QString &icon, \
                       int page, int index, const QString &url);


signals:
//    void sendUrl(const QString &url);

    void iconEnter();
    void iconLeave();
    void iconMove();
//    void iconDrop(int id, const QString &text, const QString &iconPath, const QString &url);
    void iconDrop(const QString &text, const QString &iconPath,
                  int page, int index,
                  const QString &url, int type);

//    void mouseClicked(int id, int page, int index);
    void mouseClicked();

public slots:

protected:
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
//    void dragLeaveEvent(QDragLeaveEvent *event);
//    void dropEvent(QDropEvent *event);
//    void mousePressEvent(QMouseEvent *);
//    void mouseDoubleClickEvent(QMouseEvent *);


private:
    DirMinWidget *_dirMinWidget;

//    int _page;
//    int _index;
//    int _id;

    int _width;
    int _height;
};

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
    void animationMove(const QRect &start, const QRect &end);

    void changeSize();
    void setDragEnable(bool dragEnable)  { _dragEnable = dragEnable; }

    void removeDirMinItem(const QString &text);
    void moveBackIcons(int page, int index);

signals:
    void pageIncreased();
    void pageDecreased();
    void pageChanged(int i);
    void sendUrl(const QString &url);

    void iconEnter();
    void iconLeave();
    void iconMove();
    void iconDrop(const QString &text, const QString &iconPath, int page, int index,
                  const QString &url, int type);

    void mouseClicked();

protected:
    void paintEvent(QPaintEvent *);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
//    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *event);

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

    int _spacing;

    bool _dragEnable;

//    QPropertyAnimation *_animation;

};

class DirLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    DirLineEdit(QString hint = QString(""),
                 QWidget *parent = NULL); // QString pixmap = QString(""),
    ~DirLineEdit() {}

signals:
    void focusIn();
    void focusOut();

public slots:
//    void setHint(const QString &hint)
//    {
//        _hint = hint;
//        repaint();
//    }
protected:
    virtual void paintEvent(QPaintEvent* event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

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

    int _width;
    int _height;

    bool _mousePress;

};

//
class IconMinItem : public QWidget
{
    Q_OBJECT
public:
    IconMinItem(QWidget *parent = 0);
    ~IconMinItem();

    void setPixmap(const QString &icon);
    void setText(const QString &text);
    void setPage(int page);
    void setIndex(int index);
    void setHidden(bool hide);
    void setIsRmote(bool isRemote);
    void setUrl(const QString &url);
    void setId(int id);
    void setType(int type);

    void setSLargeSize();
    void setSMediumSize();
    void setSSmallSize();

    int page()                  { return _page; }
    int index()                 { return _index; }
    int id()                    { return _id; }
    int type()                  { return _type; }
    int getWidth()              { return _width; }
    int getHeight()             { return _height; }
    bool isRmote()              { return _isRemote; }
    QString url()               { return _url; }
    QString pix()               { return _pixText; }

    const QString & text()      { return _text; }
    const QPixmap & pixmap()    { return _pixmap;}

    int _icontype;/*³ÌÐòÍ¼±ê*/

    void animationMove(const QRect &start, const QRect &end);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPropertyAnimation *_animation;

    QString _text;
    QString _url;
    QString _pixText;

    QPixmap _pixmap;

    int _type;


    int _page;
    int _index;

    int _id;

    int _width;
    int _height;
    int _iconWidth;
    int _iconHeight;

    bool _isRemote;
};


#endif // DIRMINWIDGET_H
