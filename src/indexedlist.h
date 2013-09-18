#ifndef INDEXEDLIST_H
#define INDEXEDLIST_H

#include <QObject>
#include <QtCore>

class IconWidget;

class IndexedList : public QObject
{
    Q_OBJECT
public:
    typedef QPair<int, int> Change;
    enum ChangeReason {
        Delete, MoveAround, Insert
    };

    typedef IconWidget* U;
    explicit IndexedList(QObject *parent = 0);
    IndexedList(const IndexedList& other);

    IndexedList& operator=(const IndexedList& other);

    int indexOf(const U& t) const;
    const U& at(int i) const;
    void insert(int i, const U& t);
    void append(const U& t);
    void remove(int i);
    void remove(const U& t);
    void moveAround(int from, int to);
    int size() const;
    void clear();

signals:
    void affectedIndices(const QSet<IndexedList::Change>& changes,
                         IndexedList::ChangeReason reason = MoveAround); // emitted by insert/delete
    
private:
    QList<U> _data;
};

#endif // INDEXEDLIST_H
