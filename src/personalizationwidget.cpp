#include <QPalette>
#include <QBrush>

#include "personalizationwidget.h"
#include "appmessagebox.h"

PersonalizationWidget::PersonalizationWidget(QWidget *parent) :
    QWidget(parent)
{
    _pixmap.load(":/images/shadow.png");

    _skinWidget = new SkinWidget(this);
    _skinWidget->move(30, 10);

    connect(_skinWidget, SIGNAL(setBgPixmap(QString)), this, SIGNAL(setBgPixmap(QString)));
}

PersonalizationWidget::~PersonalizationWidget()
{
    delete _skinWidget;
}

void PersonalizationWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), _pixmap);

    QWidget::paintEvent(event);
}

void PersonalizationWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    _skinWidget->resize(width() - 70, height() - 110);
}

/************************** 换皮肤 *************************/
SkinWidget::SkinWidget(QWidget *parent)
    : QWidget(parent)
    , _pageNum(0)
{
    //resize(20, 20);

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    _skinLabel = new QLabel(tr("换皮肤"), this);
    _skinLabel->setStyleSheet("border: 0px solid gray;background:rgba(255,255,255,0); color:#FFFFFF");
    _skinLabel->move(20, 20);

    _skinPixWidget = new SkinPixWidget(this);
    _skinPixWidget->move(5, 45);

//    _scrollBar = new QScrollBar(this);

//    _scrollBar->setStyleSheet("QScrollBar:vertical{width:8px;border: 0px solid gray;background:rgba(255,255,255,0);}\
//                    QScrollBar::handle:vertical{ min-width: 8px;min-height: 249px; background-image:url(:/images/bs_scrollbar.png);\
//                    background-position: left; background-repeat:none; border-style:flat;}\
//                    QScrollBar::handle:vertical::disabled{background:#232329;}\
//                    QScrollBar::handle:vertical:hover{background-image:url(:/images/bs_scrollbar.png);}\
//                    QScrollBar::add-line:vertical{background-color: rgba(255,255,255,0);\
//                                                  subcontrol-position: bottom; subcontrol-origin: margin;}\
//                    QScrollBar::sub-line:vertical{background-color: rgba(255,255,255,0);\
//                                                  subcontrol-position: top; subcontrol-origin: margin;}\
//                    QScrollBar::add-page:vertical{background-color: rgba(255,255,255,0);}\
//                    QScrollBar::sub-page:vertical{background-color: rgba(255,255,255,0);}\
//                    QScrollBar::up-arrow:vertical{background-color: rgba(255,255,255,0);}\
//                    QScrollBar::down-arrow:vertical{background-color: rgba(255,255,255,0);}");

//    if (_skinPixWidget->getHeight() % height() == 0)
//        _pageNum = _skinPixWidget->getHeight() / height();
//    else
//        _pageNum = _skinPixWidget->getHeight() / height() + 1;

//    _scrollBar->setSingleStep(height() -50);
//    _scrollBar->setPageStep(height() - 50);

//    _animation = new QPropertyAnimation(_skinPixWidget, "geometry");

    _rightTopPix.load(":/images/bs_rightbg_top.png");
    _rightCenterPix.load(":/images/bs_rightbg_center.png");
    _rightBottomPix.load(":/images/bs_rightbg_bottom.png");

//    connect(_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
    connect(_skinPixWidget, SIGNAL(setBgPixmap(QString)), this, SIGNAL(setBgPixmap(QString)));
}

void SkinWidget::scrollBarValueChanged(int val)
{
    Q_UNUSED(val);
//    if ((_skinPixWidget->pos().y() + val) % (height() - 10) != 0)
//    {
//        return;
//    }

//    if (_animation->state() == QAbstractAnimation::Running)
//    {
//        return;
//    }

//    _animation->setDuration(80);
//    _animation->setStartValue(QRect(0, _skinPixWidget->pos().y(), _skinPixWidget->getWidth(), _skinPixWidget->getHeight()));
//    _animation->setEndValue(QRect(0, -val, _skinPixWidget->getWidth(), _skinPixWidget->getHeight()));

//    _animation->start();
}

void SkinWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    _width = width();
    _height = height();
    _skinPixWidget->setSize(_width - 10, _height - 45 - 10, 7);

//    int w = 15;
//    int x = width() - w;
//    x = x < 0 ? 0: x;
//    int h = height();
//    _scrollBar->setGeometry(x, 0, w, h);

//    _scrollBar->setRange(0, _pageNum * height() - h - 10);
    update();
}

void SkinWidget::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
//    _scrollBar->event(event);
}

void SkinWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, _width, 9,\
                       _rightTopPix.scaled(_width, 9));
    painter.drawPixmap(0, 9, _width, _height - 18,\
                       _rightCenterPix.scaled(_width, _height - 18));
    painter.drawPixmap(0, _height - 9, _width, 9,\
                       _rightBottomPix.scaled(_width, 9));

    painter.setPen(QPen(QBrush(Qt::white), 1, Qt::DashLine));
    painter.drawLine(15, 40, _width - 25, 40);

    QWidget::paintEvent(event);
}

SkinPixWidget::SkinPixWidget(QWidget *parent)
    : QWidget(parent)
    , _column(0)
    , _rowNum(0)
    , _pixNum(0)
    , _bgHeight(0)
{
   resize(180 * 3 + 4 * 10, 890);
}

void SkinPixWidget::setSize(int width, int height, int pixNum)
{
    _width = width;
    _height = height;
    _pixNum = pixNum;

    for (int i = 0; i < 30; i++)
    {
        _pixmapWidget = new PixmapWidget(QString(":/images/wallpager/wp_%1.jpg").arg(i), this);

        _pixmapWidget->setSize((_width - 10) / _pixNum - 10, 100);

        if ((_rowNum + 1) * (10 + _pixmapWidget->getWidth()) >= _width)
        {
            _rowNum = 0;
            _column++;
        }

        _pixmapWidget->move(((_rowNum + 1) *10 + _pixmapWidget->getWidth() * _rowNum),
                             ((_column + 1) * 10 + _pixmapWidget->getHeight() * _column));
        _rowNum++;

        _bgHeight = _column * (_pixmapWidget->getHeight() + 10) + 10;
        if (_bgHeight <= _height)
        {
            resize(_width, _height);
        }
        else {
            if (_bgHeight % _height == 0)
            {
                resize(_width, _bgHeight);
            }
            else
            {
                resize(_width, _bgHeight + _height);
            }
        }

        connect(_pixmapWidget, SIGNAL(mouseClicked(const QString&)), \
                        this, SIGNAL(setBgPixmap(const QString&)));
    }
}

int SkinPixWidget::getWidth()
{
    return _width;
}

int SkinPixWidget::getHeight()
{
    return _bgHeight;
}

PixmapWidget::PixmapWidget(QString pixText, QWidget *parent)
    : QWidget(parent)
    , _pixText(pixText)
    , _width(0)
    , _height(0)
{
    _pixmap.load(_pixText);
}

void PixmapWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _width, _height, _pixmap);

    QWidget::paintEvent(event);
}

void PixmapWidget::setSize(int w, int h)
{
    _width = w;
    _height = h;
    resize(_width, _height);
    update();
}

QString PixmapWidget::getPixmapText()
{
    return _pixText;
}

int PixmapWidget::getWidth()
{
    return _width;
}

int PixmapWidget::getHeight()
{
    return _height;
}

void PixmapWidget::mousePressEvent(QMouseEvent *event)
{
    AppMessageBox box(true, NULL);
    box.setText("是否将选中的图片设置为桌面背景\n点击\"确定\"更换背景");
    if (box.exec())
        emit mouseClicked(_pixText);

    QWidget::mousePressEvent(event);
}
