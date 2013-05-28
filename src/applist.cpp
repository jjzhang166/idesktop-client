/* applist.cpp
*  a list stored application that assigned to current user 
*/

#include "applist.h"


VirutalAppList * VirutalAppList::_l = NULL;

AppList::AppList() :
    _index(-1),_page(-1),_hidden(true)
{
}
AppList::~AppList()
{
}

//-----------------
VirutalAppList * VirutalAppList::getList()
{
    if (!VirutalAppList::_l)
        VirutalAppList::_l = new VirutalAppList();
    return _l;
}


VirutalAppList::VirutalAppList(QObject *parent) :
    QObject(parent)
{
    if(!_applicationList.isEmpty())
    {
        return;
    }

    for (int i = 0; i<_applicationList.count(); i++)
    {
        AppList *app = new AppList();
        app->setId(_applicationList.at(i).id);
        app->setName(_applicationList.at(i).name);
        app->setAlias(_applicationList.at(i).alias);
        app->setAddr(_applicationList.at(i).addr);   
        app->setIcon(_applicationList.at(i).icon);
        app->setType(_applicationList.at(i).type);
        _list.append(app);
    }
}

VirutalAppList::~VirutalAppList()
{
}

int VirutalAppList::count()
{
    return _list.count();
}

AppList* VirutalAppList::at(int i)
{
    if (i >= _list.count() || i < 0)
        return NULL;
    return _list.at(i);
}

void VirutalAppList::addApp(AppList *app)
{
    _list.append(app);
    emit appAdded(app->name(), app->icon());
}

AppList* VirutalAppList::getAppByName(const QString &name)
{
    for (int i = 0; i < _list.count(); i++) {
        if (_list.at(i)->name() == name)
            return _list.at(i);
    }
    return NULL;
}

