#include <QTimer>

#include "dustbin.h"

#define LARGESIZE QSize(143, 143)       //72*72
#define MEDIUMSIZE QSize(119, 119)      //48*48
#define SMALLSIZE QSize(103, 103)       //32*32

Dustbin::Dustbin(QWidget *parent)
    : QWidget(parent)
{

    setAcceptDrops(true);
#if 0
    switch(ICON_TYPE)
    {
        case 0 :

            _width = LARGESIZE.width();
            _height = LARGESIZE.height();

            _iconWidth = ICONLSIZE.width();
            _iconHeight = ICONLSIZE.height();

            _dirMWidget->move(35, 36);
            break;

        case 1 :

            _width = MEDIUMSIZE.width();
            _height = MEDIUMSIZE.height();

            _iconWidth = ICONMSIZE.width();
            _iconHeight = ICONMSIZE.height();

            _dirMWidget->move(35 - 24 + 15, 35 - 24 + 15);
            break;

        default:

            _width = SMALLSIZE.width();
            _height = SMALLSIZE.height();

            _iconWidth = ICONSSIZE.width();
            _iconHeight = ICONSSIZE.height();

            _dirMWidget->move(35 - 32 + 22, 35 - 32 + 22);
            break;
    }
#endif

    _width = LARGESIZE.width();
    _height = LARGESIZE.height();

    setFixedSize(_width, _height);

}

Dustbin::~Dustbin()
{

}

void Dustbin::dragEnterEvent(QDragEnterEvent *event)
{

    void *source = (Dustbin*)(event->source());
    void *dst = this->parent();
    if(source == dst)
    {
        event->ignore();
        return;
    }

    if ((event->source() != this) && (event->mimeData()->hasFormat("image/x-iconitem"))) {
        emit iconEnter();

        event->setDropAction(Qt::MoveAction);
        event->accept();

    } else {

        event->ignore();
    }


}

void Dustbin::dragMoveEvent(QDragMoveEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();


    emit iconMove();
}

void Dustbin::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit iconLeave();
}

void Dustbin::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("image/x-iconitem")) {
        QByteArray itemData = event->mimeData()->data("image/x-iconitem");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString nameText;
        QString pixText;
        QString urlText;
        int page;
        int index;
        int type;
        int id;
        QString uniqueName;
        dataStream >> nameText >> pixText >> page >> index >> urlText >> type >> id >> uniqueName;

        if (id == _id)
        {
            event->ignore();
            return;
        }

        event->setDropAction(Qt::MoveAction);
        event->accept();
        emit iconDrop(nameText ,pixText, page, index, urlText, type, uniqueName);
    } else {
        event->ignore();
    }
}

void Dustbin::mouseMoveEvent(QMouseEvent *event)
{
    /*
    if (_isMousePress)
        return;
 */
   if(_skipMouseMove)
        return;
    _isMouseMove = true;

    event->ignore();
}

void Dustbin::mousePressEvent(QMouseEvent *event)
{
//    QWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        _isMousePress = true;
        QTimer::singleShot(300, this, SLOT(mouseStatus()));
        _skipMouseMove=false;
    }

}

void Dustbin::mouseStatus()
{
//    _isMousePress = true;

    if (_isMouseMove)
    {
        _isMouseMove = false;
        return;
    }
    else
    {
        emit openItem();
        _skipMouseMove = true;
    }
}

void Dustbin::mouseReleaseEvent(QMouseEvent *)
{
    _isMousePress = false;
    _skipMouseMove=false;
}
