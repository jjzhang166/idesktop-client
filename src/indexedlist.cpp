#include "indexedlist.h"
#include "iconwidget.h"

IndexedList::IndexedList(QObject *parent) :
    QObject(parent)
{
}

IndexedList::IndexedList(const IndexedList& other)
    :QObject(0)
{
    _data = other._data;
}

IndexedList& IndexedList::operator=(const IndexedList& other)
{
    this->_data = other._data;
    return *this;
}

int IndexedList::indexOf(const IndexedList::U& t) const
{
    return _data.indexOf(t);
}

const IndexedList::U& IndexedList::at(int i) const
{
    return _data.at(i);
}

void IndexedList::insert(int i, const IndexedList::U& t)
{
    QSet<Change> changes;
    for (int j = i; j < _data.size(); j++) {
        changes.insert(qMakePair(j, j+1));
    }

    _data.insert(i, t);
    qDebug() << __PRETTY_FUNCTION__ << ": size " << _data.size();

    emit affectedIndices(changes, Insert);
}

void IndexedList::append(const IndexedList::U& t)
{
    _data.append(t);
}

void IndexedList::moveAround(int from, int to)
{
    if (from == to)
        return;


    QSet<Change> changes;
    if (from < to) {
        for (int j = from+1; j <= to; j++) {
            changes.insert(qMakePair(j, j-1));
        }

    } else {
        for (int j = to; j < from; j++) {
            changes.insert(qMakePair(j, j+1));
        }
    }
    _data.move(from, to);
    emit affectedIndices(changes, MoveAround);
}

void IndexedList::remove(const IndexedList::U& t)
{
    remove(indexOf(t));
}

void IndexedList::remove(int i)
{
    QSet<Change> changes;
    for (int j = i+1; j < _data.size(); j++) {
        changes.insert(qMakePair(j, j-1));
    }

    _data.removeAt(i);

    emit affectedIndices(changes, Delete);
}

int IndexedList::size() const
{
    return _data.size();
}

void IndexedList::clear()
{
    _data.clear();
}
