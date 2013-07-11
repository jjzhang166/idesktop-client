#ifndef ICONITEM_H
#define ICONITEM_H

#include <QWidget>
#include <QDrag>
#include <QTimeLine>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QPixmap>
#include <QAction>

#include "dirminwidget.h"
#include "localapps.h"


/* #############################################
 * Declaration of IconItem
 */

class IconItem : public QWidget
{
    Q_OBJECT
public:

    enum icon_size
    {
        large_size = 0, medium_size, small_size
    };

    IconItem(QWidget *parent = 0);
    ~IconItem();

    void setPixmap(const QString &icon);
    void setText(const QString &text);
    void setPage(int page);
    void setIndex(int index);
    void setHidden(bool hide);
    void setIsRmote(bool isRemote);
    void setUrl(const QString &url);
    void setEqualIcon(bool equal);
    void setId(int id);
    void setType(int type);
    void setDirId(int dirId);

    void setTimeLine(bool timeLine);
    void setPropertyAnimation(bool propertyAnimation);
    void setRunAction(bool runItem);
    void setDelAction(bool delItem);

    void setLargeSize();
    void setMediumSize();
    void setSmallSize();

    void setSLargeSize();
    void setSMediumSize();
    void setSSmallSize();

    void setClosePixBool(bool closePixBool)             { _closePixBool = closePixBool; }
    void setSelectPixBool(bool selectePixBool)          { _selectePixBool = selectePixBool; }
    void setDoubleClickBool(bool doubleClickeBool)      { _doubleClickeBool = doubleClickeBool; }
    void setContextMenuBool(bool contextMenuBool)       { _contextMenuBool = contextMenuBool; }
    void setTrembleBool(bool trembleBool)               { _trembleBool = trembleBool; }
    void setDragEventBool(bool dragEventBool)           { _dragEventBool = dragEventBool; }
    void setEnterEventBool(bool enterEventBool)         { _enterEventBool = enterEventBool; }
    void setLeaveEventBool(bool leaveEventBool)         { _leaveEventBool = leaveEventBool; }
    void setSaveDataBool(bool saveDataBool)             { _saveDataBool = saveDataBool; }

    int page()                  { return _page; }
    int index()                 { return _index; }
    int id()                    { return _id; }
    int type()                  { return _type; }
    int getWidth()              { return _width; }
    int getHeight()             { return _height; }
    bool isRmote()              { return _isRemote; }
    QString url()               { return _url; }
    virtual QString pix()       { return _pixText; }
    int dirId()                 { return _dirId; }

    const QString & text();
    virtual const QPixmap & pixmap();
    const QPixmap & originPixmap();
    const QPixmap & grayPixmap();
    const QPixmap & darkPixmap();
    const QPixmap & maskPixmap();

//    void setEnabled(bool);
    int _icontype;/*����ͼ��*/

    bool isUserType();

    void animationMove(const QRect &start, const QRect &end);

    void addMinWidget();
    void setMinWidgetDragEnable(bool enable);

    //dirMin
    void removeDirMinItem(const QString &text);

signals:
//    void clicked();
    void runItem(const QString &text);
    void delItem(const QString &text);

    void iconLeave();
    void iconEnter();
    void iconMove();
    void openDir(int id, int page, int index);
    void iconDrop(int id, const QString &text, const QString &iconPath, const QString &url);
    void addApp(const QString &text, const QString &pix, const QString &url, int type);

    void showContextMenu(QPoint pos, const QString &text);

    void dragEnterMinWidget();

public slots:
    void startTremble();
    void stopTremble();
    void doTremble(qreal);

    void runClicked();
    void delClicked();

    void openDirWidget();
    void iconDropEvent(const QString &text, const QString &iconPath, const QString &url);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void contextMenuEvent(QContextMenuEvent *event);

    void paintEvent(QPaintEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    LocalApp *_app;
    QDrag *_drag;
    QTimeLine *_timeline;
    QPoint _dragStartPosition;
    QPropertyAnimation *_animation;

    QString _text;
    QString _texticon;
    QString _texticon_firstrow;
    QString _texticon_secondrow;
    QString _texticon_thirdrow;

    QString _url;
    QString _pixText;

    QPixmap _pixmap;
    QPixmap _normalPixmap;
    QPixmap _grayPixmap;
    QPixmap _closePixmap;
    QPixmap _darkPixmap;
    QPixmap _originPixmap;
    QPixmap _shadowPixmap;
    QPixmap _selectPixmap;

    QAction *_runAction;
    QAction *_delAction;

    int _type;

//    DirMinShowWidget *_dirMinShowWidget;

    int _textWidth;
    int _textWidth_firstrow;
    int _textWidth_secondrow;
    int _textWidth_thirdrow;
    int _textHeight;

    int _page;
    int _index;
    int _dirId;

    int _trembling;

    int _id;

    int _width;
    int _height;
    int _iconWidth;
    int _iconHeight;

    bool _isRemote;

    bool _trembleBool;
    bool _dragEventBool;
    bool _equalBool;

    bool _runBool;
    bool _delBool;
    bool _propertyAnimationBool;
    bool _timeLineBool;
    bool _closePixBool;
    bool _selectePixBool;
    bool _doubleClickeBool;
    bool _contextMenuBool;
    bool _enterEventBool;
    bool _leaveEventBool;
    bool _saveDataBool;

    DirMinShowWidget *_dirMinShowWidget;
    QList<DirMinShowWidget*> _dirMinShowWidgets;

};

#endif // ICONITEM_H