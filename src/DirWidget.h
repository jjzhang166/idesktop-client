#ifndef DIRWIDGET_H
#define DIRWIDGET_H

#include <QWidget>
#include <QDrag>
#include <QTimeLine>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QMenu>
#include <QScrollBar>
#include <cassert>

#include "hovericonitem.h"
#include "localapps.h"
#include "iconitem.h"

//class DirItem;
class DirWidget;

class DirShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirShowWidget(QSize pageSize, QWidget *parent = 0);
    ~DirShowWidget();

    void showApp(bool localApp);

    void setId(int id);
    int id()                        { return _id; }

    void setMaxRow(int row);
    int getHeight()                 { return _height; }
    int getRow();
    int iconCount();
    int currentPage();
    int count();

    void removeIcon(const QString &text);

signals:


public slots:
//    void scrollBarValueChanged(int val);
    void addDirIcon(const QString &text, const QString &iconPath, const QString &url);
    void setSize();

    void largeIcon();
    void mediumIcon();
    void smallIcon();

protected:
//    void resizeEvent(QResizeEvent *event);
//    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *);

private:
    int _width;
    int _height;

    QPixmap _rightTopPix;
    QPixmap _rightCenterPix;
    QPixmap _rightBottomPix;

    DirWidget *_dirWidget;

    QScrollBar *_scrollBar;
    QPropertyAnimation *_animation;

    int _id;
    int _bgHeight;

    int _iconCount;
    int _currentPage;
    int _count;

};

class DirWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirWidget(QSize pageSize, QWidget *parent = 0);
    ~DirWidget();


    int addIcon(QString text, const QString &icon, \
                int page, int index);

    QString addLocalApp(QString appPath);
    QString getAppImage(QString appPath);

    void showApp(bool localApp);

    int iconCount()                     { return _nextIdx[_current]; }
    int currentPage()                   { return _current; }
    int count()                         { return _count; }
    int iconsPerPage()                  { return _iconsPerPage; }
    QSize pageSize()                    { return _pageSize; }
    int row()                           { return _row; }
    int getIconHeight()                 { return gridHeight; }
    void setUrl(const QString &url)     { _url = url; }

    void setId(int id)                  { _id = id; }
    int id()                            { return _id; }

    void expand();
    void delPage(int page);
    void delIcon(const QString &text);

    void setMaxPage(int page)   { _maxPage = page; }

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void refresh(QSize size);

    void movetoFirst();
    void deleteAllIconItem();
    void reloadApplist(QSize size);
    int getNearestIndex(const QRect &rect);

    void removeIcon(const QString &text);
    void moveBackIcons(int page, int index);

signals:
//    void sendUrl(const QString &url);
    void pageChanged(int i);
//    void sizeChanged();
    void pageIncreased();
    void pageDecreased();

    void dragLeave();

protected:

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *event);

private:


//    LocalAppList *_local;

    QSize _pageSize;

    int _col;
    int _row;

    int gridWidth;
    int gridHeight;

    int _width;
    int _height;

    int _current;
    int _count;

    int _localCount;

    QList<int> _pages;
    QList<QList<IconItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QMap<QString, IconItem*> _iconDict;

    int _iconsPerPage;

    QPropertyAnimation *_animation;
    QString _url;

    int _iconNum;
    IconItem *_inDrag;

    int _maxPage;

    int _iconSize;
    int _iconHSpacing;
    int _iconVSpacing;

    void changeSpacing();

    bool _dragEvent;
    int m2v;
    int gm2v;

    int _id;

};

//class DirItem : public QWidget
//{
//    Q_OBJECT
//public:
//    DirItem(const QString &text, QWidget *parent = 0);
//    ~DirItem();
//    void setPixmap(const QString &icon);
//    void setText(const QString &text);
//    const QString & text();
//    const QPixmap & pixmap();
//    const QPixmap & originPixmap();
//    const QPixmap & grayPixmap();
//    const QPixmap & darkPixmap();

//    bool isUserType();

//    void animationMove(const QRect &start, const QRect &end);
//    void setPage(int page);
//    void setIndex(int index);
//    void setHidden(bool hide);
//    void setIsRmote(bool isRemote);
//    int page()
//    {
//        return _page;
//    }
//    int index()
//    {
//        return _index;
//    }
//    bool isRmote()
//    {
//        return _isRemote;
//    }
//    int _icontype;/*³ÌÐòÍ¼±ê*/

//    void setEqualIcon(bool equal);

//    void setUrl(const QString &url) { _url = url;}
//    QString getUrl() { return _url; }
//signals:
//    void clicked();
//    void runItem(const QString &text);
//    void delItem(const QString &text);

//    void sendUrl(const QString &url);

//public slots:
////    void startTremble();
////    void stopTremble();
//   void doTremble(qreal);

//    void openClicked();
//    void delClicked();

//protected:
//    void mouseDoubleClickEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
////    void contextMenuEvent(QContextMenuEvent *event);
//    void paintEvent(QPaintEvent *event);

//    void enterEvent(QEvent *event);
//    void leaveEvent(QEvent *event);

//private:
////    LocalApp *_app;
//    QDrag *_drag;
//    QTimeLine *_timeline;
//    QPoint dragStartPosition;
//    QPropertyAnimation *_animation;
//    int _textWidth;
//    QString _text;

//    QString _texticon;
//    QString _texticon_firstrow;
//    QString _texticon_secondrow;
//    QString _texticon_thirdrow;
//    int _textWidth_firstrow;
//    int _textWidth_secondrow;
//    int _textWidth_thirdrow;

//    int _textHeight;
//    int _page;
//    int _index;
//    int _trembling;
//    bool _isRemote;
//    QPixmap _pixmap;
//    QPixmap _normalPixmap;
//    QPixmap _grayPixmap;
//    QPixmap _closePixmap;
//    QPixmap _darkPixmap;
//    QPixmap _originPixmap;

//    QAction *_openAction;
//    QAction *_delAction;
////    HoverIconItem *_hoverDirItem;

//    bool _equal;
//    QString _url;

//    int _width;
//    int _height;

//};


#endif // DIRWIDGET_H
