#include <QString>
#include <QStringList>
#include <QObject>
#include <QDesktopServices>
#include <QFileInfoList>
#include <QDebug>
#include <QSettings>

#include "lnklist.h"
#include "config.h"

LnkList * LnkList::_l = NULL;
//LnkList * LnkList::_l = new LnkList();
LnkList * LnkList::getList()
{
    if (!LnkList::_l)
        LnkList::_l = new LnkList();
    return _l;
}

LnkList::LnkList(QObject *parent)
    : QObject(parent)
{
    _commonDesktop = Config::get("CommonDesktopDir");
    _desktop = Config::get("DesktopDir");
    _desktop.append("\\");
    _commonDesktop.append("\\");
    QStringList filters;
    filters << "*.lnk";
    dir.setPath(_desktop);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(filters);
    dirCommon.setPath(_commonDesktop);
    dirCommon.setFilter(QDir::Files);
    dirCommon.setNameFilters(filters);
    _fl = dir.entryInfoList();
    _flCommon = dirCommon.entryInfoList();
}

void LnkList::refresh()
{
    _fl.clear();
    _flCommon.clear();
    _fl = dir.entryInfoList();
    _flCommon = dirCommon.entryInfoList();
}

bool LnkList::has(QString name)
{
    for (int i = 0; i < _fl.count(); i++)
        if (_fl.at(i).fileName() == name)
            return true;
    for (int i = 0; i < _flCommon.count(); i++)
        if (_flCommon.at(i).fileName() == name)
            return true;
    return false;
}

QString LnkList::wholePath(QString name)
{
    for (int i = 0; i< _fl.count(); i++) {
        if (_fl.at(i).fileName() == name) 
            return _desktop + _fl.at(i).fileName();
    }
    for (int i = 0; i< _flCommon.count(); i++) {
        if (_flCommon.at(i).fileName() == name)  {
            QString result = _commonDesktop + _flCommon.at(i).fileName();
            return result;
        }
    }
    return QString("");
}

int LnkList::count()
{
    return _fl.count() + _flCommon.count();
}
