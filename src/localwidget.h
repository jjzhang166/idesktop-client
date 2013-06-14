#ifndef LOCALWIDGET_H
#define LOCALWIDGET_H

#include <QWidget>
#include <QTabBar>
#include <QStackedWidget>
#include <QDrag>
#include <QTimeLine>
#include <QPropertyAnimation>
#include <QAction>
#include <QMouseEvent>
#include <QScrollBar>
#include <QUrl>

#include "localapps.h"
#include "hovericonitem.h"

class LocalItem;
class LocalWidget;

#define SPACING 30
#define ICONWIDTH 96
#define ICONHEIGHT 96

class LocalShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalShowWidget(QSize pageSize, QWidget *parent = 0);
    ~LocalShowWidget();

    void showApp(bool localApp);

signals:
    void addLocalApp(const QString &text, const QString &pix, const QString &url);
public slots:
    void scrollBarValueChanged(int val);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    int _width;
    int _height;

//    QPixmap _rightTopPix;
//    QPixmap _rightCenterPix;
//    QPixmap _rightBottomPix;
    QPixmap _bgPix;

    LocalWidget *_localWidget;

    QScrollBar *_scrollBar;
    QPropertyAnimation *_animation;


};

class LocalWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit LocalWidget(QSize pageSize, QWidget *parent = 0);
    ~LocalWidget();


    int addIcon(QString text, const QString &icon, \
                int page, int index);

    QString addLocalApp(QString appPath);
    QString getAppImage(QString appPath);

    void showApp(bool localApp);

    int iconCount()             { return _nextIdx[_current]; }
    int currentPage()           { return _current; }
    int count()                 { return _count; }
    int iconsPerPage()          { return _iconsPerPage; }
    QSize pageSize()            { return _pageSize; }

signals:
    void addLocalApp(const QString &text, const QString &pix, const QString &url);
protected:
//    void paintEvent(QPaintEvent *event);
//    void resizeEvent(QResizeEvent *event);
    
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
    QList<QList<LocalItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QMap<QString, LocalItem*> _iconDict;

    int _iconsPerPage;

    QPropertyAnimation *_animation;
    QString _url;

};

class LocalItem : public QWidget
{
    Q_OBJECT
public:
    LocalItem(const QString &text, QWidget *parent = 0);
    ~LocalItem();
    void setPixmap(const QString &icon);
    void setText(const QString &text);
    const QString & text();
    const QPixmap & pixmap();
    const QPixmap & originPixmap();
    const QPixmap & grayPixmap();
    const QPixmap & darkPixmap();

    bool isUserType();

    void animationMove(const QRect &start, const QRect &end);
    void setPage(int page);
    void setIndex(int index);
    void setHidden(bool hide);
    void setIsRmote(bool isRemote);
    int page()
    {
        return _page;
    }
    int index()
    {
        return _index;
    }
    bool isRmote()
    {
        return _isRemote;
    }
    int _icontype;/*³ÌÐòÍ¼±ê*/

    void setEqualIcon(bool equal);
    void setUrl(const QString &url);
    QString getUrl()            { return _url; }

signals:
    void clicked();
    void sendUrl(const QString &url);
    void delItem(const QString &text);
    void addLocalApp(const QString &text, const QString &pix, const QString &url);

public slots:
//    void startTremble();
//    void stopTremble();
//    void doTremble(qreal);

    void openClicked();
    void delClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//    void contextMenuEvent(QContextMenuEvent *event);
    void paintEvent(QPaintEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
//    LocalApp *_app;
    QDrag *_drag;
    QTimeLine *_timeline;
    QPoint dragStartPosition;
    QPropertyAnimation *_animation;
    int _textWidth;
    QString _text;

    QString _texticon;
    QString _texticon_firstrow;
    QString _texticon_secondrow;
    QString _texticon_thirdrow;
    int _textWidth_firstrow;
    int _textWidth_secondrow;
    int _textWidth_thirdrow;

    int _textHeight;
    int _page;
    int _index;
    int _trembling;
    bool _isRemote;
    QPixmap _pixmap;
    QPixmap _normalPixmap;
    QPixmap _grayPixmap;
    QPixmap _closePixmap;
    QPixmap _darkPixmap;
    QPixmap _originPixmap;

    QAction *_openAction;
    QAction *_delAction;
//    HoverIconItem *_hoverLocalItem;

    bool _equal;
    QString _url;
    QString _pix;

};


#endif // LOCALWIDGET_H
