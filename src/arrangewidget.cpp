#include <QPainter>

#include "arrangewidget.h"

ArrangeWidget::ArrangeWidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
    , _titlePressed(false)
{
    resize(450,450);

    _topPix.load(":/images/widget_top.png");
    _centerPix.load(":/images/widget_center.png");
    _bottomPix.load(":/images/widget_bottom.png");

    _arrangeCenterWidget = new ArrangeCenterWidget(this);
    _arrangeCenterWidget->setGeometry(1, 35, _width - 2, _height - 35 -1);
    _arrangeCenterWidget->setVisible(true);

}

ArrangeWidget::~ArrangeWidget()
{

}

void ArrangeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _width, 9,\
                       _topPix.scaled(_width, 9));
    painter.drawPixmap(0, 9, _width, _height - 18,\
                       _centerPix.scaled(_width, _height - 18));
    painter.drawPixmap(0, _height - 9, _width, 9,\
                       _bottomPix.scaled(_width, 9));

    QWidget::paintEvent(event);
}

void ArrangeWidget::mousePressEvent(QMouseEvent *event)
{
    if (QRect(1, 0, width() - 2, 35).contains(event->pos())) {
        _titlePressed = true;
        _startDrag = event->pos();
    }
}

void ArrangeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _titlePressed = false;
    event->ignore();
}

void ArrangeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_titlePressed) {
        QPoint newPoint = pos() + event->pos() - _startDrag;
        emit setPos(newPoint);
    }
}

void ArrangeWidget::resizeEvent(QResizeEvent *event)
{
    _width = width();
    _height = height();

    QWidget::resizeEvent(event);
}

ArrangeCenterWidget::ArrangeCenterWidget(QWidget *parent) :
    QWidget(parent)
{

}

ArrangeCenterWidget::~ArrangeCenterWidget()
{

}

//int ArrangeCenterWidget::addIcon(const QString &text, \
//                            const QString &iconPath, \
//                            int page, \
//                            int index,\
//                            const iconStyle &iSt)
//{

//    IconItem *icon = new IconItem(text, iSt, this);
//    //icon->setText(text);
//    /* TODO deal with the icon is full */
////    if (page == -1) {
////        for (int i = 0; i < _count; i++) {
////            if (_nextIdx[i] < _row * _col) {
////                page = i;
////                index = _nextIdx[i];
////                break;
////            }
////        }
////    } else {
//////        if (index == -1) {
////            if ((page > 0) && (_nextIdx[page - 1] == _iconsPerPage) && (_nextIdx[page] < _iconsPerPage)) {
////                index = _nextIdx[page];
////            }
////            else if ((page == 0) && (_nextIdx[page] < _iconsPerPage)) {
////                index = _nextIdx[page];
////            }
////            else {
////                for (int i = 0; i < _count; i++) {
////                    if (_nextIdx[i] < _row * _col) {
////                        page = i;
////                        index = _nextIdx[i];
////                        move(_pages[page], y());
////                        break;
////                    }
////                }
////            }
//////        }
////    }

//    icon->setPixmap(iconPath);
//    //icon->setGeometry(_gridTable[page][index].translated(SPACING, SPACING));
//    icon->setGeometry(_gridTable[page][index].translated(SPACING, SPACING));
//    icon->setPage(page);
//    icon->setIndex(index);
////    _iconDict.insert(text, icon);
////    _iconTable[page][index] = icon;
////    _nextIdx[page]++;
//    icon->show();

//    connect(icon, SIGNAL(runItem(const QString&)), this, SIGNAL(runApp(const QString&)));
//    connect(icon, SIGNAL(delItem(const QString&)), this, SIGNAL(uninstall(const QString&)));

//    /*addicon add the last*/
////    showAddIcon(page, -1);

//     return page;
//    //LocalAppList::getList()->save();
//}
