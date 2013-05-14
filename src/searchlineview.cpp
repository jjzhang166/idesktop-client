#include "searchlineview.h"
#include <QListView>
#include <QAbstractItemModel>
#include <QFontMetrics>
#include <QStringListModel>

SearchListView::SearchListView( QWidget * parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint |Qt::Tool);

    _listView = new QListView(this);
 //   _listView->setWindowFlags(Qt::ToolTip);
    _model = new  QStringListModel(this);
    _listView->setModel(_model);

    connect(_listView, SIGNAL(clicked(const QModelIndex &)), this, SIGNAL(listClicked(const QModelIndex &)));
}

void SearchListView::resizeEvent( QResizeEvent * event)
{

    _listView->setGeometry(rect());

    Q_UNUSED(event);
}

void SearchListView::setCurrentIndex(const QModelIndex &index)
{
    _listView->setCurrentIndex(index);
}

QAbstractItemModel* SearchListView::model()
{
    return _listView->model();
}

QModelIndex SearchListView::currentIndex()
{
    return _listView->currentIndex();
}

void SearchListView::setStringList(QStringList &strList)
{
    _model->setStringList(strList);
    _listView->setModel(_model);

/*    QFontMetrics metrics(this->font());
    int _TitleTextHeight =  metrics.height();
    this->setFixedHeight(_TitleTextHeight * strList.size());*/
}
