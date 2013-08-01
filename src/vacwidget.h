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
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QByteArray>
#include <QEventLoop>
#include <QTime>

#include "localapps.h"
#include "hovericonitem.h"
#include "commuinication.h"
#include "paascommuinication.h"
#include "iconitem.h"
#include "dynamicbutton.h"

//class VacItem;
class VacWidget;
class VacScrollBarWidget;

class VacShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VacShowWidget(QSize pageSize, QWidget *parent = 0);
    ~VacShowWidget();

    void showApp(bool localApp);

    void getIcon();

    int addIcon(QString text, const QString &icon, \
                int page, int index, const QString &url, \
                int type, const QString &uniqueName);

    void delIcon(const QString &uniqueName);

public slots:

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void desktopDelIcon(const QString &uniqueName);

signals:
    void addApp(const QString &text, const QString &pix, const QString &url, \
                int type, const QString &uniqueName);
    void vacCloseBtnClicked();
    void delItem(const QString &uniqueName);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int _width;
    int _height;

    QPixmap _bgPix;

    VacScrollBarWidget *_vacScrollBarWidget;

    QPixmap _closePix;
    QPixmap _closeHoverPix;
    DynamicButton *_closeBtn;
};

class VacScrollBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VacScrollBarWidget(QSize pageSize, QWidget *parent = 0);
    ~VacScrollBarWidget();

    void showApp(bool localApp);

    void getIcon();

    int addIcon(QString text, const QString &icon, \
                int page, int index, const QString &url, \
                int type, const QString &uniqueName);

    void delIcon(const QString &uniqueName);

    void desktopDelIcon(const QString &uniqueName);

public slots:
    void scrollBarValueChanged(int val);
    void vacWidgetCountChanged();

    void largeIcon();
    void mediumIcon();
    void smallIcon();
//    void desktopDelIcon(const QString &text);
signals:
    void addApp(const QString &text, const QString &pix, const QString &url, \
                int type, const QString &uniqueName);
    void delItem(const QString &uniqueName);


protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
//    void paintEvent(QPaintEvent *event);

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
    int _oldPagePos;
    int _newPagePos;

};

class VacWidget : public QWidget
{
    Q_OBJECT

public:

    enum iconStyle
    {
        localIcon = 0, vacIcon, paasIcon
    };

    explicit VacWidget(QSize pageSize, QWidget *parent = 0);
    ~VacWidget();

//    QString addVacApp(QString appPath);
//    void addVacApp(QString appPath);
    QString getAppImage(QString appPath);

    void showApp(bool localApp);

    int iconCount()             { return _nextIdx[_current]; }
    int currentPage()           { return _current; }
    int count()                 { return _count; }
    int iconsPerPage()          {  return _iconsPerPage; }
    QSize pageSize()            { return _pageSize; }

    void getLocalIcon();
    void getVacIcon();
    void getPaasIcon();

    void expand();
    void delPage(int page);

    int addIcon(QString text, const QString &icon, \
                int page, int index, const QString &url, \
                int type, const QString &uniqueName);
    void delIcon(const QString &uniqueName);
    void moveBackIcons(int page, int index);

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void refresh(QSize size);

    void movetoFirst();
    void deleteAllIconItem();
    void reloadApplist(QSize size);

    void unChecked(const QString &uniqueName);

signals:
    void addApp(const QString &text, const QString &pix, const QString &url, \
                int type, const QString &uniqueName);

    void pageIncreased();
    void pageDecreased();
    void delItem(const QString &uniqueName);

public slots:
//    void onPaasDone();
//    void onVacDone();
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
    QList<QList<IconItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QMap<QString, IconItem*> _iconDict;

    int _iconsPerPage;

    QPropertyAnimation *_animation;
    QString _url;

    bool _pFinished;
    PaasCommuinication *_paasCommui;

    bool _vFinished;
    commuinication *_commui;

    int _iconSize;
    int _iconHSpacing;
    int _iconVSpacing;

    void changeSpacing();

    LocalAppList *_local;

//    QList<PAAS_LIST> _paasList;


};

//class VacItem : public QWidget
//{
//    Q_OBJECT
//public:
//    VacItem(const QString &text, QWidget *parent = 0);
//    ~VacItem();
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
//    void setUrl(const QString &url);
//    QString getUrl()    { return _url; }

//signals:
//    void clicked();
//    void runItem(const QString &text);
//    void delItem(const QString &text);
//    void addVacApp(const QString &text, const QString &pix, const QString &url);

//public slots:
////    void startTremble();
////    void stopTremble();
////    void doTremble(qreal);

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
//    QPixmap _selectPixmap;

//    QAction *_openAction;
//    QAction *_delAction;
////    HoverIconItem *_hoverVacItem;

//    bool _equal;
//    QString _url;
//    QString _pix;

//    int _width;
//    int _height;

//};

#endif // VACWIDGET_H
