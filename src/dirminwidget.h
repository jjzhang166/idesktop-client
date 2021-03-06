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

#include "localapps.h"
#include "struct.h"

class IconMinItem;
class DirLineEdit;
class DirMinWidget;
class DirMWidget;

class DirMinShowWidget : public QWidget
{
    Q_OBJECT
public:
    DirMinShowWidget(int type, QWidget *parent = 0);
    ~DirMinShowWidget();

    void setPage(int page)      { _page = page; }
    void setIndex(int index)    { _index = index; }
    void setId(int id);
//    void setHidden(bool hide);
    void setText(const QString text);

    int page()                  { return _page; }
    int index()                 { return _index; }
    int id()                    { return _id; }

    const QString & getDirText();

    void removeDirMinItem(const QString &uniqueName);
    void addDirMinItem(const QString &text, const QString &icon, \
                       int page, int index, const QString &url, const QString &uniqueName);

    int getMinIconNum();
    void setUniqueName(const QString &uniqueName);
    QString uniqueName()          { return _uniqueName; }
    void refresh();

signals:

    void iconEnter();
    void iconLeave();
    void iconMove();
    void iconDrop(const QString &text, const QString &iconPath, int page, int index,
                  const QString &url, int type, const QString &uniqueName);

    void openItem();
    void dragEnterMinWidget();
    void dirMinLineEditFocusOut(int i, const QString &text);

public slots:

    void iconDropEvent(const QString &text, const QString &iconPath, const QString &url, const QString &uniqueName);

    void editFocusIn();
    void editFocusOut();

    void mouseStatus();

protected:
    void paintEvent(QPaintEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);


private:
    DirMWidget *_dirMWidget;
    DirLineEdit *_dirLineEdit;
    QPropertyAnimation *_animation;

    int _page;
    int _index;
    int _id;

    int _width;
    int _height;

    QPixmap _editLeft;
    QPixmap _editCenter;
    QPixmap _editRight;
    QPixmap _left;
    QPixmap _center;
    QPixmap _right;
    QPixmap _editLeftNormal;
    QPixmap _editCenterNormal;
    QPixmap _editRightNormal;

    void setImgs(QPixmap strLeft, QPixmap strCenter, QPixmap strRight);
    QPixmap setTransparentPixmap(const QString &pix);

    int _iconWidth;
    int _iconHeight;

    bool _isMouseMove;
    bool _isMousePress;
    bool _skipMouseMove;
    LocalAppList *_local;

    int _type;

    QString _uniqueName;

};

//
class DirMWidget : public QWidget
{
    Q_OBJECT

public: 
    DirMWidget(int type, QWidget *parent = 0);
    ~DirMWidget();

    void removeDirMinItem(const QString &uniqueName);
    void addDirMinItem(const QString &text, const QString &icon, \
                       int page, int index, const QString &url, const QString &uniqueName);

    int getMinIconNum();

    void setUniqueName(const QString &uniqueName);
    QString uniqueName()          { return _uniqueName; }
    void refresh();

signals:
    void iconEnter();
    void iconLeave();
    void iconMove();
    void iconDrop(const QString &text, const QString &iconPath,
                  int page, int index,
                  const QString &url, int type, const QString &uniqueName);

    void mouseClicked();

public slots:

protected:

private:
    DirMinWidget *_dirMinWidget;

    int _width;
    int _height; 

    int _type;
    QString _uniqueName;
};

class DirMinWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirMinWidget(int type, QWidget *parent = 0);
    ~DirMinWidget();

    int addIcon(const QString &text, const QString &iconPath,
                int page, int index, const QString &url, const QString &uniqueName);
    void expand();
    void delPage(int page);
    void delIcon(const QString &uniqueName);
    void animationMove(const QRect &start, const QRect &end);

    void changeSize();
    void setDragEnable(bool dragEnable);
    int getIconNum();

    void removeDirMinItem(const QString &uniqueName);
    void moveBackIcons(int page, int index);

    void setUniqueName(const QString &uniqueName);
    QString uniqueName()          { return _uniqueName; }

    void refresh();

signals:
    void pageIncreased();
    void pageDecreased();
    void pageChanged(int i);
    void sendUrl(const QString &url);

    void iconEnter();
    void iconLeave();
    void iconMove();
    void iconDrop(const QString &text, const QString &iconPath, int page, int index,
                  const QString &url, int type, const QString &uniqueName);

    void mouseClicked();

protected:
    void paintEvent(QPaintEvent *);

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

    int _type;

    QString _uniqueName;
};

class DirLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    DirLineEdit(QString hint = QString(""),
                 QWidget *parent = NULL); // QString pixmap = QString(""),
    ~DirLineEdit() {}
    void saveFullName(QString name) { _fullName = name;}
signals:
    void focusIn();
    void focusOut();

protected:
    virtual void paintEvent(QPaintEvent* event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void enterEvent(QEvent *);

private:

    void setImgs(QImage *strLeft, QImage *strCenter, QImage *strRight);

private:
    QString _hint;
    QString _fullName;
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
    void setUniqueName(const QString &uniqueName);

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
    QString uniqueName()        { return _uniqueName; }

    int _icontype;/*����ͼ��*/

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
    QString _uniqueName;
};


#endif // DIRMINWIDGET_H
