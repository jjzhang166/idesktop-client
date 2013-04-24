#include<QPainter>
#include<QMouseEvent>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QtGui>
//#include <QStyledItemDelegate>
#include "tableview.h"
//#include "itemdelegate.h"
#include "itemmodel.h"

#ifndef NDEBUG
#include<QDebug>
#endif

TableView::TableView(QWidget * parent) 
    : QTableView(parent), _category(ALLCATEGORY)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::NoSelection);
    setFocusPolicy(Qt::NoFocus);
    setAlternatingRowColors(true);
    setStyleSheet("alternate-background-color: rgb(227, 255, 214);");
    //setSelectionMode(QAbstractItemView::NoSelection);
    setMouseTracking(true);//important
    installEventFilter(this);
    //_delegate = new ItemDelegate;
    _proxy = new SoftwareProxyModel;
    _model = new ItemModel(this);
    _proxy->setSourceModel(_model);
    _proxy->setDynamicSortFilter(true);
    setModel(_proxy);
    //setItemDelegate(_delegate);
    //resizeColumnsToContents();
    rearrange();
    connect(this, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onActivated(const QModelIndex &)));
}

void TableView::onActivated(const QModelIndex &index)
{
    QString name = index.data(Qt::UserRole + 1).toString();
    emit detail(name);
}

void TableView::refresh()
{
    NodeList *l = NodeList::instance();
    _model->beginRefresh();
    l->flush();
    _model->endRefresh();
}

void TableView::categoryChange(int id)
{
    _category = static_cast<PackageCategory>(id);
    _proxy->filterPkgCategory(_category);
}

void TableView::search(const QString &text)
{
    _proxy->filterPkgSearch(text);
}

void TableView::showRecommended()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _proxy->filterPkgCategory(_category);
    _proxy->filterPkgStatus(ALLSTATUS);
    _proxy->getRecommended(true);
    _proxy->getAllInstalled(false);
}

void TableView::showNormal()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _proxy->filterPkgCategory(_category);
    _proxy->filterPkgStatus(ALLSTATUS);
    _proxy->getRecommended(false);
    _proxy->getAllInstalled(false);
}

void TableView::showUpdates()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _proxy->filterPkgCategory(ALLCATEGORY);
    _proxy->filterPkgStatus(OLD);
    _proxy->getRecommended(false);
    _proxy->getAllInstalled(false);
}

void TableView::showInstalled()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _proxy->filterPkgCategory(ALLCATEGORY);
    _proxy->filterPkgStatus(INSTALLED);
    _proxy->getRecommended(false);
    _proxy->getAllInstalled(true);
}

void TableView::rearrange()
{
    setShowGrid(false);
    horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    setColumnWidth(0, 1);
    setColumnWidth(2, 60);
    verticalHeader()->setDefaultSectionSize(VERTICALHEIGHT);
}

void TableView::mouseMoveEvent(QMouseEvent * event)
{
    setCursor(Qt::PointingHandCursor);
}

int TableView::viewHeight()
{
    return _model->rowCount() * VERTICALHEIGHT;
}
