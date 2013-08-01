#ifndef QITEMMANAGER_H
#define QITEMMANAGER_H
#include "iconitem.h"
#include <QObject>
#include <QList>
class QItemManager : public QObject
{
    Q_OBJECT
public:
    static QItemManager* getManager();
    void savePressedKey(int key);
    void deleteRecordKey(int key);
    void saveSelectedIconItem(IconItem* iconitem);
    void deleteRecordIconItem(IconItem* iconitem);
    QList<int> getSelectKeyList() {return _selectedKeyList;}
    QList<IconItem*> getSelectIconItem() {return _selectedIconItem;}
    QList<QString> getSelectIconItemText() {return _selectedIconItemText;}
    int getKeyListSize() {return _selectedKeyList.size();}
    int getItemListSize() {return _selectedIconItem.size();}
    int keyAt(int i);
    IconItem* itemAt(int i);
    bool containsItem(IconItem *);
    void removeByUniqueName(QString uniqueName);
    void clearFocus();

    
signals:
    
public slots:
private:
    explicit QItemManager(QObject *parent = 0);
    QList<int> _selectedKeyList;
    QList<IconItem*> _selectedIconItem;
    static QItemManager* _instance;
    QList<QString> _selectedIconItemText;
};

#endif // QITEMMANAGER_H
