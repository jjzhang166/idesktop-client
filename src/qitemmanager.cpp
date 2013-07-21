#include "qitemmanager.h"
#include <QDebug>
QItemManager *QItemManager::_instance = NULL;
QItemManager::QItemManager(QObject *parent) :
    QObject(parent)
{
}
QItemManager* QItemManager::getManager()
{
    if(!_instance)
        _instance = new QItemManager();
    return _instance;
}

void QItemManager::savePressedKey(int key)
{
     _selectedKeyList << key;
}
void QItemManager::deleteRecordKey(int key)
{
    if(_selectedKeyList.size() == 0)
        return;
    for(int i = 0; i < _selectedKeyList.size(); i ++)
    {
        if(_selectedKeyList.at(i) == key)
            _selectedKeyList.removeAt(i);
    }
}
void QItemManager::saveSelectedIconItem(IconItem *iconitem)
{
    _selectedIconItem << iconitem;
    _selectedIconItemText << iconitem->text();
}
void QItemManager::deleteRecordIconItem(IconItem *iconitem)
{
    if(_selectedIconItem.size() == 0)
        return;
    for(int i = 0; i <= _selectedIconItem.size(); i++)
    {
        if(_selectedIconItem.at(i) == iconitem)
        {
            _selectedIconItem.removeAt(i);
            _selectedIconItemText.removeAt(i);
            break;
        }
    }
}
int QItemManager::keyAt(int i)
{
    if(_selectedKeyList.size() == 0)
        return 0;
    else
        return _selectedKeyList.at(i);
}
IconItem* QItemManager::itemAt(int i)
{
    if(_selectedIconItem.size() == 0)
        return NULL;
    else
        return _selectedIconItem.at(i);
}
bool QItemManager::containsItem(IconItem *item)
{
    if(_selectedIconItem.size() == 0)
        return false;
    for(int i = 0; i < _selectedIconItem.size(); i++)
    {
        if(_selectedIconItem.at(i) == item)
            return true;
    }
}
void QItemManager::clearFocus()
{
    if(_selectedIconItem.size() == 0)
        return;
    for(int i = 0; i < _selectedIconItem.size(); i++)
    {
        _selectedIconItem.at(i)->setCurrentStatus(IconItem::normal);
    }
    _selectedIconItem.clear();
    _selectedIconItemText.clear();
}
void QItemManager::removeByName(QString name)
{
    for(int i = 0; i < _selectedIconItem.size(); i++)
    {
        if(_selectedIconItem.at(i)->text() == name)
        {
            _selectedIconItem.removeAt(i);
            _selectedIconItemText.removeAt(i);
            break;
        }
    }
}
