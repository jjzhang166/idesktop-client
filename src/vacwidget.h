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
#include "commuinication.h"
#include "paascommuinication.h"
#include "iconitem.h"
#include "dynamicbutton.h"

//class VacItem;
class VacWidget;
class VacScrollBarWidget;
class IDesktopSettings;

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

    void initIconItem();

    void movetoFirst();

public slots:

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void checkedIcon(const QString &uniqueName, bool check);

    void iconItemNameChanged(const QString &uniqueName, const QString &name);

    void closeBtnClicked();
signals:
    void addApp(const QString &text, const QString &pix, const QString &url,
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

    void checkedIcon(const QString &uniqueName, bool check);

    void iconItemNameChanged(const QString &uniqueName, const QString &name);

    void initIconItem();

    void  movetoFirst();

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
//    void moveBackIcons(int page, int index);

    void largeIcon();
    void mediumIcon();
    void smallIcon();

    void refresh(QSize size);

    void movetoFirst();
    void deleteAllIconItem();
    void reloadApplist(QSize size);

    void checkedIcon(const QString &uniqueName, bool check);

    void iconItemNameChanged(const QString &uniqueName, const QString &name);

    void initIconItem();

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

    int _iconHSpacing;
    int _iconVSpacing;

    void changeSpacing();

    LocalAppList *_local;
    IDesktopSettings *_settings;
};

#endif // VACWIDGET_H
