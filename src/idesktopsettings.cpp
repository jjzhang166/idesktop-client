#include "idesktopsettings.h"
#include <QtSql/QtSql>
#include <QtGui>

IDesktopSettings *IDesktopSettings::_instance = NULL;

IDesktopSettings::IDesktopSettings(QObject *parent) :
    QObject(parent)
{
    QSqlQuery query =
            QSqlDatabase::database("local").exec(QString("SELECT type FROM sizetype where id=1;"));
    if (query.first()) {
        _iconSize = query.value(0).toInt();
        qDebug() << "query iconSize = " << _iconSize;
    }
}

IDesktopSettings* IDesktopSettings::instance()
{
    if (!_instance) {
        _instance = new IDesktopSettings;
    }
    return _instance;
}


int IDesktopSettings::iconSize() const
{
    return _iconSize;
}

void IDesktopSettings::setIconSize(int newSize)
{
    int old = _iconSize;
    if (old == newSize) {
        return;
    }

    QSqlQuery query(QSqlDatabase::database("local"));
    if(!query.exec(QString("update sizetype set type=%1 where id=1;").arg(newSize))) {
        qDebug() <<"update icon size failed";
        return;
    }

    _iconSize = newSize;
    emit iconSizeUpdated(old, newSize);
}
