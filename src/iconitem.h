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
#include "dustbin.h"


/* #############################################
 * Declaration of IconItem
 */
class QItemManager;
//class Dustbin;
class IconItem : public QWidget
{
    Q_OBJECT
public:

    enum icon_size
    {
        large_size = 0, medium_size, small_size
    };
    enum status{
        normal = 1, hover = 2, selected = 3
    };

    enum icon_class{
        local = 0, vapp, paas
    };
    IconItem(QWidget *parent = 0);
    ~IconItem();

    void setPixmap(const QString &icon, const QString &text);
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
    void setLineEditBg(QPixmap strLeft, QPixmap strCenter, QPixmap strRight);
    void setTimeLine(bool timeLine);
    void setPropertyAnimation(bool propertyAnimation);
    void setRunAction(bool runItem);
    void setDelAction(bool delItem);
    void init();
    void setCurrentStatus(status state) { _currentStatus = state ; \
                                          update();}
    QPixmap setTransparentPixmap(const QString &pix);
    void setUniqueName(const QString &uniqueName);
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

    void chopText(QFontMetrics fm, int width);

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
    QString uniqueName();//        { return _uniqueName; }

    void setIconSize(int iconSize);
    int iconSize()              {  return _iconSize; }

    const QString & text();
    virtual const QPixmap & pixmap();
    const QPixmap & originPixmap();
    const QPixmap & grayPixmap();
    const QPixmap & darkPixmap();
    const QPixmap & maskPixmap();

//    void setEnabled(bool);
    int _icontype;/*³ÌÐòÍ¼±ê*/

    bool isUserType();

    void animationMove(const QRect &start, const QRect &end);

    //dirMin
    void addMinWidget(int type);
    void setMinWidgetDragEnable(bool enable);

    void removeDirMinItem(const QString &uniqueName);
    void addDirMinItem(const QString &text, const QString &icon, \
                       int page, int index, const QString &url, const QString &uniqueName);
    void refreshDirMinWidgetIcon();

    int getMinIconNum();
    void setDirMinItemId(int id);
    void setMouseMoveEnable(bool enable);

    void addDustbin();
    void setIconClass(int iconClass);
    void setLineEditReadOnly(bool readOnly);
    void setDustbinDragEnable(bool enable);

signals:
//    void clicked();
    void runItem(const QString &uniqueName);
    void delItem(const QString &uniqueName);

    void iconLeave();
    void iconEnter();
    void iconMove();
    void openDir(int id, int page, int index);
    void iconDrop(int id, const QString &text,
                  const QString &iconPath, int page,
                  int index, const QString &url,
                  int type, const QString &uniqueName);
    void addApp(const QString &text, const QString &pix,
                const QString &url, int type,
                const QString &uniqueName);

    void showContextMenu(bool ,QPoint pos, QPoint mPos, IconItem *iconItem);

    void dragEnterMinWidget();

    void dirMinLineEditFocusOut(int i, const QString &text);
//dustbin
    void dustbinIconEnter();
    void dustbinIconMove();
    void dustbinIconLeave();

    void iconItemNameChanged(const QString &uniqueName, const QString &name);

    void dragOut();

public slots:
    void startTremble();
    void stopTremble();
    void doTremble(qreal);
    void LineEditFocusIn();
    void LineEditFocusOut();
    void changeItemName(QString);
    void setDirBackground();
    void runClicked();
    void delClicked();
//dir
    void openDirWidget();
    void iconDropEvent(const QString &text, const QString &iconPath, int page, int index,
                       const QString &url, int type, const QString &uniqueName);
    void dirMinLineEditChanged(int i, const QString &text);
//dustbin
//    void dustbinOpenDirWidget();
//    void dustbinIconDropEvent(const QString &text, const QString &iconPath, int page, int index,
//                              const QString &url, int type);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *);

    void contextMenuEvent(QContextMenuEvent *event);

    void paintEvent(QPaintEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void focusOutEvent(QFocusEvent *);
    void resizeEvent(QResizeEvent *event);
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
    QPixmap _hoverBackgroud;
    QPixmap _selectedBackgroud;
    QPixmap _normalBackground;

    QPixmap _editLeft;
    QPixmap _editCenter;
    QPixmap _editRight;
    QPixmap _left;
    QPixmap _center;
    QPixmap _right;
    QPixmap _editLeftNormal;
    QPixmap _editCenterNormal;
    QPixmap _editRightNormal;
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
    status _currentStatus;
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
    int _iconSize;

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
    DirLineEdit* _lineEdit;
    Dustbin *_dustbin;

    QPixmap _iconClassPixmap;
    QString _uniqueName;

    int _iconX;
    int _iconY;

};

#endif // ICONITEM_H
