#ifndef PERSONALIZATIONWIDGET_H
#define PERSONALIZATIONWIDGET_H

#include <QWidget>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPixmap>
#include <QPainter>
#include <QLabel>

class PixmapWidget;
class SkinPixWidget;

class SkinWidget;

class PersonalizationWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PersonalizationWidget(QWidget *parent = 0);
    ~PersonalizationWidget();

signals:
    void setBgPixmap(const QString &pixText);
    
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    QPixmap _pixmap;
    SkinWidget *_skinWidget;
};

class SkinWidget : public QWidget
{
    Q_OBJECT
public :
    explicit SkinWidget(QWidget *parent = 0);
    ~SkinWidget() {}

signals:
    void setBgPixmap(const QString &pixText);

public slots:
    void scrollBarValueChanged(int val);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    PixmapWidget *_pixmapWidget;
    QScrollBar *_scrollBar;

    SkinPixWidget *_skinPixWidget;

    QPropertyAnimation *_animation;

    QPixmap _rightTopPix;
    QPixmap _rightCenterPix;
    QPixmap _rightBottomPix;

    QLabel *_skinLabel;

    int _pageNum;

    int _width;
    int _height;

};

class SkinPixWidget : public QWidget
{
    Q_OBJECT
public :
    explicit SkinPixWidget(QWidget *parent = 0);
    ~SkinPixWidget() {}

    void setSize(int width, int height, int pixNum);
    int getWidth();
    int getHeight();

signals:
    void setBgPixmap(const QString &pixText);

private:
    PixmapWidget *_pixmapWidget;

    int _column;
    int _rowNum;

    int _width;
    int _height;
    int _pixNum;
    int _bgHeight;
};

class PixmapWidget : public QWidget
{
    Q_OBJECT
public :
    explicit PixmapWidget(QString pixText, QWidget *parent = 0);
    ~PixmapWidget() {}

    void setSize(int w, int h);
    QString getPixmapText();
    int getWidth();
    int getHeight();

signals:
    void mouseClicked(const QString &pixText);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    int _width;
    int _height;
    QString _pixText;
    QPixmap _pixmap;

};
#endif // PERSONALIZATIONWIDGET_H
