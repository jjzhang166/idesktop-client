#include "graphicsloginitem.h"
/*
GraphicsLoginItem::GraphicsLoginItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    _hostInfoItem = new HostNormalItem(_host,this);
    _addHostWidget = new AddHostWidget;
    _proxyWidget = new QGraphicsProxyWidget(_hostInfoItem);
    _proxyWidget->setWidget(_addHostWidget);
    _proxyWidget->hide();
    _proxyWidget->setGeometry(boundingRect());
    const QRectF r = _proxyWidget->rect();
    _proxyWidget->setTransform(QTransform().rotate(180,Qt::YAxis).translate(-r.width(), 0));

    _timeline = new QTimeLine(500, this);

    connect(_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(updateFlip(qreal)));
    connect(_hostInfoItem, SIGNAL(clicked(Node*)), this, SLOT(hostHlicked(Node*)));
    connect(_hostInfoItem, SIGNAL(hostUpdated(Node*)), this, SIGNAL(hostUpdated(Node*)));
    connect(_hostInfoItem, SIGNAL(doubleClicked(Node*)), this, SIGNAL(doubleClicked(Node*)));
    connect(_hostInfoItem, SIGNAL(pressed()), this, SLOT(addHostflip()));
    connect(_addHostWidget,SIGNAL(cancelClicked()),this,SLOT(addHostCanceled()));
    connect(_addHostWidget,SIGNAL(enterPress()),this,SLOT(connectStart()));
}

void GraphicsLoginItem::hostHlicked(Node* host)
{
    emit clicked(host, this);
}

void GraphicsLoginItem::addHostflip()
{
    if (_flip) {
        if (_timeline->state() == QTimeLine::Running)
         return;
        if (_timeline->currentValue() == 0) {
            _timeline->setDirection(QTimeLine::Forward);
            _timeline->start();
        }
        else {
            _timeline->setDirection(QTimeLine::Backward);
            _timeline->start();
        }
    }
}

void GraphicsLoginItem::addHostCanceled()
{
    addHostflip();
    _addHostWidget->ipwClear();
}

void GraphicsLoginItem::addHostCommitted()
{
        addHostflip();
        _flip = false;
        connect(_host, SIGNAL(statusChanged(NodeStatus)), this, SLOT(newHostDaTaRead(NodeStatus)));
        _loadItem->setVisible(true);
}

void GraphicsLoginItem::updateFlip(qreal val)
{
    qreal finalyrot = qreal(0) - qreal(0) * val - 180 *val;
    QTransform tran;
    tran.translate(88,88);
    tran.rotate(finalyrot,Qt::YAxis);
    _hostInfoItem->setTransform(tran);
    QTransform t;
    t.translate(-88,-88);
    _hostInfoItem->setTransform(t,true);
}

QRectF GraphicsLoginItem::boundingRect() const
{
    return QRectF(0,0,176,176);
}

void GraphicsLoginItem::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
    if (!_hostInfoItem->freeFlip()){
        if (_proxyWidget && _proxyWidget->isVisible()){
            _proxyWidget->hide();
            _proxyWidget->setGeometry(boundingRect());
            if(_hostInfoItem && _hostInfoItem->isVisible())
                _hostInfoItem->setAcceptedMouseButtons(Qt::LeftButton);
        }
        return;
    }
    if (_proxyWidget && !_proxyWidget->isVisible()){
        _proxyWidget->show();
        _proxyWidget->setFocus();
        _hostInfoItem->setAcceptedMouseButtons(Qt::NoButton);
    }
    if (_proxyWidget && _proxyWidget->pos() != QPoint())
        _proxyWidget->setGeometry(boundingRect());

//    if ( _connectState)
//        connectState();
}

void GraphicsLoginItem::setData(Node* host)
{
    _hostInfoItem->setData(host);
}

void GraphicsLoginItem::addHost()
{
    QString ip = _addHostWidget->ip();
    QString psw = _addHostWidget->password();
    _host = new Node(ip, psw);

    if ( !QvManager::getManager()->getNode(ip)) {

        _addHostWidget->setFocus();
        _blackPixmap->show();
        _loadItem->setVisible(true);
        _addHostWidget->connecting(true);
        emit connecting();
        connect(_host, SIGNAL(statusChanged(NodeStatus)), this, SLOT(newHostStatusChanged(NodeStatus)));

        _host->connectToNode();
    }
    else {
        _host->deleteLater();
        _host = NULL;
    }
}


void GraphicsLoginItem::setCurrent(const bool &cur)
{
    _hostInfoItem->setCurrent(cur);
}

void GraphicsLoginItem::setCurPos(const QPointF &p)
{
    setPos(p);
    _pos = p;
}

void GraphicsLoginItem::addAvailable()
{
    _hostInfoItem->addAvailable();
    _flip = true;
}

void GraphicsLoginItem::connectStart()
{
    addHost();
}


void GraphicsLoginItem::setHighLight(const bool &highlight)
{
    _hostInfoItem->setHighLight(highlight);
}

void GraphicsLoginItem::newHostStatusChanged(NodeStatus status)
{
    if (status >= CONNECTED) {
        disconnect(_host, SIGNAL(statusChanged(NodeStatus)), this, SLOT(newHostStatusChanged(NodeStatus)));
        _loadItem->setVisible(false);
        _blackPixmap->hide();
        _addHostWidget->connecting(false);
        QvManager::getManager()->addNode(_host);
        setData(_host);
        _hostInfoItem->setCurrent(false);
        addHostCommitted();

        emit addTrue();
    }
    else if (status == DISCONNECTED){
        disconnect(_host, SIGNAL(statusChanged(NodeStatus)), this, SLOT(newHostStatusChanged(NodeStatus)));
        _host->deleteLater();
        _host = NULL;
        _loadItem->setVisible(false);
        _blackPixmap->hide();
        _addHostWidget->connecting(false);
        _addHostWidget->checkConnect();
        emit connectFailed();
    }
}

void GraphicsLoginItem::newHostDaTaRead(NodeStatus status)
{
    if (status == DATAREAD) {
        disconnect(_host, SIGNAL(statusChanged(NodeStatus)), this, SLOT(newHostDaTaRead(NodeStatus)));
        _loadItem->setVisible(false);
    }
}
*/
