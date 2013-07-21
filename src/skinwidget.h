#ifndef SKINWIDGET_H
#define SKINWIDGET_H

#include <QWidget>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPixmap>
#include <QPainter>
#include <QLabel>

#include "dynamicbutton.h"

//class PixmapWidget;
//class SkinPixWidget;
class PixItem;
class PixWidget;

class SkinShowWidget : public QWidget
{
    Q_OBJECT
public :
    explicit SkinShowWidget(QWidget *parent = 0);
    ~SkinShowWidget() {}

signals:
    void setBgPixmap(const QString &pixText);
    void skinCloseBtnClicked();

public slots:
    void scrollBarValueChanged(int val);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);

private:
//    PixmapWidget *_pixmapWidget;
    PixWidget *_pixWidget;
    QScrollBar *_scrollBar;

//    SkinPixWidget *_skinPixWidget;

    QPropertyAnimation *_animation;

//    QPixmap _rightTopPix;
//    QPixmap _rightCenterPix;
//    QPixmap _rightBottomPix;

    QPixmap _bgPix;

//    QLabel *_skinLabel;

    int _width;
    int _height;
    int _oldPagePos;
    int _newPagePos;
    QPixmap _closePix;
    QPixmap _closeHoverPix;
    DynamicButton *_closeBtn;
};


class PixWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PixWidget(QSize pageSize, QWidget *parent = 0);
    ~PixWidget();


    int addIcon(const QString &icon, \
                int page, int index);

    int iconCount()             { return _nextIdx[_current]; }
    int currentPage()           { return _current; }
    int count()                 { return _count; }
    int iconsPerPage()          {  return _iconsPerPage; }
    QSize pageSize()            { return _pageSize; }


signals:
    void setBgPixmap(const QString &pixText);


public slots:
    void itemClicked(const QString &pixText);

protected:
//    void paintEvent(QPaintEvent *event);
//    void resizeEvent(QResizeEvent *event);

private:

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
    QList<QList<PixItem*> > _iconTable;
    QList<QList<QRect> > _gridTable;
    QList<int> _nextIdx;
    QMap<QString, PixItem*> _iconDict;

    int _iconsPerPage;

    QString _iconPath;
};

class PixItem : public QWidget
{
    Q_OBJECT
public:
    PixItem(QWidget *parent = 0);
    ~PixItem();
    void setPixmap(const QString &icon);
    const QPixmap & pixmap();

    void setPage(int page);
    void setIndex(int index);
    void setPenColor(bool yellow);

    int page()  { return _page; }
    int index() { return _index; }

    QString getBgPix() { return _pixText;}

signals:
    void mouseClicked(const QString &pixText);


public slots:
//    void openClicked();
//    void delClicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:

    int _page;
    int _index;

    QColor _color;

    QPixmap _pixmap;
    QString _pixText;

    bool _selectPix;

};
#endif // SKINWIDGET_H
