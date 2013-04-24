#include "appdatabase.h"
#include "appdata.h"


AppDatabase* AppDatabase::_pInstance = NULL;

AppDatabase::AppDatabase()
    : _totalCount(0)
{
}

AppDatabase* AppDatabase::Instance()
{
    if (_pInstance == NULL)
        _pInstance = new AppDatabase;
    return _pInstance;
}

void AppDatabase::destroy()
{
    if (_pInstance) {
        delete _pInstance;
        _pInstance = NULL;
    }
}

void AppDatabase::addData(AppData* data)
{
    if (data)
    {
        for (int i = 0;i < _dataBase.size();i++)
            if (_dataBase.at(i)->_name == data->_name) {
                *_dataBase.at(i) = *data;
                return;
            }
        _dataBase.push_back(new AppData(*data));
    }
}

AppData* AppDatabase::getData(const int &index)
{
    if (index >= 0 && index < _dataBase.size())
        return _dataBase.at(index);
    return NULL;
}

int AppDatabase::size()
{
    return _dataBase.size();
}

void AppDatabase::removeAll()
{
    for (int i = 0;i < _dataBase.size();i++)
        if (_dataBase.at(i) != NULL)
            delete _dataBase.at(i);

    _dataBase.clear();
}

AppData* AppDatabase::getData(const QString &name)
{
    for (int i = 0;i < _dataBase.size();i++)
        if (_dataBase.at(i)->_name == name)
            return _dataBase.at(i);
    return NULL;
}

int AppDatabase::totalCount()
{
    return _totalCount;
}

void AppDatabase::totalCount(const int &count)
{
    _totalCount = count;
}
