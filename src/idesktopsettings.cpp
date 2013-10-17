#include <QtSql/QtSql>
#include <QtGui>

#include "idesktopsettings.h"
#include "iconwidget.h"

IDesktopSettings *IDesktopSettings::_instance = NULL;

IDesktopSettings::IDesktopSettings(QObject *parent) :
    QObject(parent)
{
    QSqlQuery query =
            QSqlDatabase::database("local").exec(QString("SELECT type FROM sizetype where id=1;"));
    if (query.first()) {
        _iconSize = (IconWidget::icon_size)query.value(0).toInt();
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

IconWidget::icon_size IDesktopSettings::iconSize() const
{
    return _iconSize;
}

void IDesktopSettings::setIconSize(IconWidget::icon_size newSize)
{
    IconWidget::icon_size old = _iconSize;
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


void IDesktopSettings::setRemoteLocalList(const QList<LOCAL_LIST>& list)
{
    _remoteLocalList = list;
}

void IDesktopSettings::setRemoteAppList(const QList<APP_LIST>& list)
{
    _remoteAppList = list;
}

void IDesktopSettings::setRemotePaasList(const QList<PAAS_LIST>& list)
{
    _remotePaasList = list;
}

void IDesktopSettings::setLocalList(const QList<LOCAL_LIST>& list)
{
    _localList = list;
}

void IDesktopSettings::setVappList(const QList<APP_LIST>& list)
{
    _vappList = list;
}

void IDesktopSettings::setPaasList(const QList<PAAS_LIST>& list)
{
    _paasList = list;
}

QVariant IDesktopSettings::prop(const QString& propName)
{
    return QVariant();
}
