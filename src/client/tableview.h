#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include<QTableView>
#include <QModelIndex>
#include<QPixmap>
#include <iostream>
#include <QtGui/QSortFilterProxyModel>
#include "appnode.h"
#include "itemmodel.h"
//#include "itemdelegate.h"

//class ItemDelegate;
class ItemModel;

class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget* parent = NULL);
    virtual ~TableView(){}
    int viewHeight();
    void showRecommended();
    void showNormal();
    void showUpdates();
    void showInstalled();
    void refresh();
public slots:
    void search(const QString&);
    void categoryChange(int);
    void onActivated(const QModelIndex &);
signals:
    void detail(const QString &);
protected:
    void mouseMoveEvent(QMouseEvent * event);
    void rearrange();
private:
    SoftwareProxyModel *_proxy;
    //ItemDelegate* _delegate;
    ItemModel * _model;
    PackageCategory _category;
};
#endif
