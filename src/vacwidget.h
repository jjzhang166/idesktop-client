#ifndef VACWIDGET_H
#define VACWIDGET_H

#include <QWidget>
#include <QTabBar>
#include <QStackedWidget>
#include <QDrag>
#include <QTimeLine>
#include <QPropertyAnimation>
#include <QAction>
#include <QMouseEvent>
#include <QScrollBar>

#include "localapps.h"
#include "hovericonitem.h"

class VacItem;
class VacWidget;

#define SPACING 30
#define ICONWIDTH 96
#define ICONHEIGHT 96

class VacShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VacShowWidget(QSize pageSize, QWidget *parent = 0);
    ~VacShowWidget();

    void showApp(bool localApp);

public slots:
    void scrollBarValueChanged(int val);

signals:
    void addVacApp(const QString &text, const QString &pix, const QString &url);

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

    VacWidget *_vacWidget;

    QScrollBar *_scrollBar;
    QPropertyAnimation *_animation;

};

class VacWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VacWidget(QSize pageSize, QWidget *parent = 0);
    ~VacWidget();


    int addIcon(QString text, const QString &icon, \
                int page, int index);

    QString addLocalApp(QString appPath);
    QString getAppImage(QString appPath);

    void showApp(bool localApp);

    int iconCount()             { return _nextIdx[_current]; }
    int currentPage()           { return _current; }
    int count()                 { return _count; }
    int iconsPerPage()          {  return _iconsPerPage; }
    QSize pageSize()            { return _pageSize; }
signals:
    void addVacApp(const QString &text, const QString &pix, const QString &url);

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
    QList<QList<VacItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QMap<QString, VacItem*> _iconDict;

    int _iconsPerPage;

    QPropertyAnimation *_animation;
    QString _url;

};

class VacItem : public QWidget
{
    Q_OBJECT
public:
    VacItem(const QString &text, QWidget *parent = 0);
    ~VacItem();
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
    QString getUrl()    { return _url; }

signals:
    void clicked();
    void runItem(const QString &text);
    void delItem(const QString &text);
    void addVacApp(const QString &text, const QString &pix, const QString &url);

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
//    HoverIconItem *_hoverVacItem;

    bool _equal;
    QString _url;
    QString _pix;

};

#endif // VACWIDGET_H
