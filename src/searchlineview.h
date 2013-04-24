#ifndef SEARCHLINEVIEW_H
#define SEARCHLINEVIEW_H

#include <QWidget>
#include <QModelIndex>

class QListView;
class QAbstractItemModel;
class QStringListModel;
class SearchListView : public QWidget
{
    Q_OBJECT
public:
    SearchListView( QWidget * parent = 0);

    QAbstractItemModel* model();
    QModelIndex currentIndex();
    void setCurrentIndex(const QModelIndex &index);
    void setStringList(QStringList &strList);

signals:

    void listClicked(const QModelIndex &);

protected:

    void resizeEvent( QResizeEvent * event);

private:


    QListView* _listView;
    QStringListModel *_model; // 完成列表的model

};

#endif // SEARCHLINEVIEW_H
