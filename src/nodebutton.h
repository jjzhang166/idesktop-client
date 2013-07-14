#ifndef NODEBUTTON_H
#define NODEBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QObject>
#include <QFont>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMoveEvent>
#include <QEvent>
#include <QImage>

class NodeButton : public QWidget
{
    Q_OBJECT
    
public:
    
    enum style {
        current_stay, normal
    };

    enum text_pos {
        left, right, center, top, bottom
    };

    explicit NodeButton(const QString &image, const QString &hoverImage,
                       const QString &currentImage, const QString &disableImage,
                       const style &stl = normal,
                       QWidget* parent = 0);
    ~NodeButton();

    void setImage(const QString &image, const QString &hoverImage,
                  const QString &currentImage, const QString &disableImage);

//    void setDragEnterImg(const QString &image);

    void setCurrent(const bool &cur);

    void setEnable(const bool &en);

    void setText(const QString &text);
    QString getText() {return  _text;}

    void setTextColor(const QColor &tColor);
    void setTextColorHover(const QColor &tColor);
    void setTextColorSelected(const QColor &tColor);
    void setFont(const QFont &font);

    void setSize(const int width, const int height);

    void setTextPos(const text_pos &pos);

signals:

    void clicked();
    void clicked(NodeButton* btn);

//    void clickMenu(const QString &menu);
//    void clickMenu(const int &menu);

//    void drop(const QMimeData* mimeData, VmDeviceButton* btn);

protected:

    void paintEvent(QPaintEvent *event);
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dragLeaveEvent(QDragLeaveEvent *event);
//    void dropEvent(QDropEvent *event);

   void enterEvent (QEvent *event);
//    void moveEvent(QMoveEvent *event);
    void mousePressEvent (QMouseEvent * event);
    void leaveEvent(QEvent *event);

private:

    QString _image;
    QString _hoverImage;
    QString _currentImage;
    QString _dragEnterImage;

    QPixmap _pixmap;
    QPixmap _hoverPixmap;
    QPixmap _currentPixmap;
    QPixmap _disablePixmap;

    QPixmap _viewSwitch_dropDown_hover;
    QPixmap _viewSwitch_dropDown;

    style _style;

    QFont _font;
    text_pos _textPos;

//    void* _data;

protected:

    QFontMetrics* _metrics;
    QColor _textColor;
    QColor _textColor_hover;
    QColor _textColor_selected;
    bool _current;
    bool _enable;
    QString _text;
    bool _hovered;
    int _width;
    int _height;

    QList<QString> _menus;
//    QRect _menuRect;
    bool _menuHover;

    bool _dragOver;
};

//PageNodes

class PageNodes : public QWidget
{
    Q_OBJECT

public:
    PageNodes(QWidget *parent = 0);
    ~PageNodes();

    void update(const int &sum, const int &cur = 0);

signals:

    void choosePage(const int &page);

public slots:

    void clickNode(NodeButton* node);
    void setCurrent(const int &cur);

protected:

    void paintEvent(QPaintEvent *event);

private:


    QList<NodeButton*> _nodes;

    int _sum;
    int _current;
};

#endif // NODEBUTTON_H
