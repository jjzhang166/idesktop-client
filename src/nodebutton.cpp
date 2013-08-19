#include <QImage>
#include <QPainter>
#include <QFontMetrics>
#include <QMimeData>
#include <QPainter>
#include <QtDebug>

#include "nodebutton.h"


const int NODE_WIDTH = 42;
const int NODE_SPACING = 5;

NodeButton::NodeButton(const QString &image, const QString &hoverImage,
                       const QString &currentImage, const QString &disableImage,
                       const style &stl, QWidget * parent)
    : QWidget(parent)
    , _image(image)
    , _hoverImage(hoverImage)
    , _currentImage(currentImage)
    , _style(stl)
    , _font("", 9, QFont::Black)
    , _textPos(right)
    , _textColor(0,0,0)
    , _current(false)
    , _enable(true)
    , _hovered(false)
{
    if (!image.isEmpty())
        _pixmap.load(image);

    if (!hoverImage.isEmpty())
        _hoverPixmap.load(hoverImage);

    if (!currentImage.isEmpty())
        _currentPixmap.load(currentImage);

    if (!disableImage.isEmpty())
        _disablePixmap.load(disableImage);

    _metrics = new QFontMetrics(_font);

    _width = 42;
    _height = 42;

    setFixedSize( _width, _height);

    _textColor_hover = _textColor;
    _textColor_selected = _textColor;

//    _viewSwitch_dropDown_hover.load(":/resources/images/viewSwitch_dropDown_hover.png");
//    _viewSwitch_dropDown.load(":/resources/images/viewSwitch_dropDown.png");
}

NodeButton::~NodeButton()
{

}

void NodeButton::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    QColor textColor;

//    if (!_enable) {
//        textColor = QColor("#9ca3a5");
//        painter.drawPixmap(0, 0, 42, 42, _disablePixmap.scaled(42, 42));
//    }
//    else if(_current && _currentImage != "") {
//        textColor = _textColor_selected;
//        painter.drawPixmap(0, 0, 42, 42, _currentPixmap.scaled(42, 42));
//    }
//    else if ( _hovered) {
//        textColor = _textColor_hover;
//        painter.drawPixmap(0, 0, 42, 42, _hoverPixmap.scaled(42, 42));
//    }
//    else {
//        textColor = _textColor;
//        painter.drawPixmap(0, 0, 42, 42,  _pixmap.scaled(42, 42));
//    }

    if (!_enable) {
        textColor = QColor("#9ca3a5");
        painter.drawPixmap(_disablePixmap.rect(), _disablePixmap);
    }
    else if(_current && _currentImage != "") {
        textColor = _textColor_selected;
        painter.drawPixmap(_currentPixmap.rect(), _currentPixmap);
    }
    else if ( _hovered) {
        textColor = _textColor_hover;
        painter.drawPixmap(_hoverPixmap.rect(), _hoverPixmap);
    }
    else {
        textColor = _textColor;
        painter.drawPixmap(_pixmap.rect(), _pixmap);
    }

    //text
    if (_text == "")
        return;

    painter.setFont(_font);
    painter.setPen(QPen(textColor,1));

    QRect textR;
    if (_textPos == right) {
        if (_hovered)
            painter.drawLine(_pixmap.rect().right(), _height, _width,_height );
        textR = QRect(_pixmap.rect().right() , (height() - _metrics->height()) / 2, _metrics->width(_text), _metrics->height());
    }
    else if (_textPos == bottom) {
        int left = (_width - _metrics->width(_text)) / 2;
        left = left > 0 ? left : 0;
        textR = QRect(left, _height - _metrics->height(), _width, _metrics->height());
    }
    else if (_textPos == center)
    {
        int center = (_width - _metrics->width(_text)) / 2;
        center = center > 0 ? center : 0;
        textR = QRect(center + 1, (_height - _metrics->height()) / 2, _width, _metrics->height()); //_height - _metrics->height()
    }

    painter.drawText(textR, _text);
}

void NodeButton::enterEvent (QEvent *event)
{
    if (_enable)
    {
         _hovered = true;
         this->update();
    }
}

void NodeButton::mousePressEvent (QMouseEvent * event)
{
    if (!_enable)
        return;

//    if (_menus.size() > 0 && _menuRect.contains(event->pos().x(), event->pos().y())) {
//        return;
//    }
    if (_style == current_stay && _current)
        return;

    emit clicked();
    emit clicked(this);

    if (_style == current_stay) {
        _current = true;
        update();
    }
}

void NodeButton::leaveEvent(QEvent *event)
{
    _hovered = false;
    this->update();
}

void NodeButton::setCurrent(const bool &cur)
{
    if (_current == cur)
        return;

    _current = cur;

    emit clicked();
    emit clicked(this);

    update();
}

void NodeButton::setEnable(const bool &en)
{
    _enable = en;
    update();
}

void NodeButton::setTextColor(const QColor &tColor)
{
    _textColor = tColor;
    update();
}

void NodeButton::setFont(const QFont &font)
{
    delete _metrics;
    _metrics = new QFontMetrics(font);

    _font = font;
    update();
}

void NodeButton::setText(const QString &text)
{
    _text = text;
    update();
}

void NodeButton::setSize(const int width, const int height)
{
    _width = width;
    _height = height;

    setFixedSize(_width, _height);

    update();

}

void NodeButton::setTextPos(const text_pos &pos)
{
    _textPos = pos;
}

void NodeButton::setImage(const QString &image, const QString &hoverImage,
              const QString &currentImage, const QString &disableImage)
{
    if (!image.isEmpty())
        _pixmap.load(image);
    if (!hoverImage.isEmpty())
        _hoverPixmap.load(hoverImage);
    if (!currentImage.isEmpty())
        _currentPixmap.load(currentImage);
    if (!disableImage.isEmpty())
        _disablePixmap.load(disableImage);
    update();
}

void NodeButton::setTextColorHover(const QColor &tColor)
{
    _textColor_hover = tColor;
}

void NodeButton::setTextColorSelected(const QColor &tColor)
{
    _textColor_selected = tColor;
}

//PageNodes
PageNodes::PageNodes(QWidget *parent)
    : QWidget(parent)
    , _sum(1)
    , _current(0)
{
    setMaximumHeight(42);
}

PageNodes::~PageNodes()
{

}

void PageNodes::update(const int &sum , const int &cur)
{
    _sum = sum;
    _current = cur;

    for (int i = 0;i < _nodes.size();i++) {
        _nodes.at(i)->setParent(NULL);
        delete _nodes.at(i);
    }
    _nodes.clear();

    for (int i = 0;i < sum;i++) {
        NodeButton* node = new NodeButton(":images/bottomPage_icon.png",
                                                          ":images/bottomPage_icon.png",
                                                          ":images/bottomPage_current_icon.png","",
                                                          NodeButton::current_stay, this);
        node->setSize(42,42);
        node->move((NODE_WIDTH + NODE_SPACING) * i , 0);
        if (i == cur)
            node->setCurrent(true);
        _nodes.push_back(node);
        connect(node, SIGNAL(clicked(NodeButton *)), this, SLOT(clickNode(NodeButton *)));

        node->setText(QString("%1").arg(i + 1));
        node->setTextColor(QColor(Qt::white));
        node->setTextColorHover(QColor(Qt::white));
        node->setTextColorSelected(QColor(Qt::white));
        node->setTextPos(NodeButton::center);


    }
    resize((NODE_WIDTH + NODE_SPACING) * _sum, 42);
}

void PageNodes::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void PageNodes::clickNode(NodeButton* node)
{

    for (int i = 0; i < _nodes.size();i++) {
        if (_nodes.at(i)->isVisible()) {
            if (_nodes.at(i) == node) {
                emit choosePage(i);
                _current = i;
            }
            else {
                _nodes.at(i)->setCurrent(false);
            }
        }
    }
}

void PageNodes::setCurrent(const int &cur)
{
    if (_current == cur)
        return;

    if (cur >= 0 && cur < _nodes.size()) {
        if (_current >= 0 && _current < _nodes.size())
            _nodes.at(_current)->setCurrent(false);
        _current = cur;
        _nodes.at(_current)->setCurrent(true);
    }
/*
    int count = 0;
    for (int i = 0;i < _nodes.size();i++) {
        if (_nodes.at(i)->isVisible()) {
            if (count == cur)
                _nodes.at(i)->setCurrent(true);
            else
                _nodes.at(i)->setCurrent(false);

            count++;
        }
    }*/
}
