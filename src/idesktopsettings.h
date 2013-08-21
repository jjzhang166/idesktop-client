#ifndef IDESKTOPSETTINGS_H
#define IDESKTOPSETTINGS_H

#include <QObject>
//vac
#include "commuinication.h"
//paas
#include "paascommuinication.h"

class IDesktopSettings : public QObject
{
    Q_OBJECT
public:
    static IDesktopSettings* instance();

    int iconSize() const;
    void setIconSize(int newSize);


    QList<LOCAL_LIST>& remoteLocalList() { return _remoteLocalList; }
    QList<APP_LIST>& remoteAppList() { return _remoteAppList; }
    QList<PAAS_LIST>& remotePaasList() { return _remotePaasList; }
    QList<APP_LIST>& vappList() { return _vappList; }
    QList<PAAS_LIST>& paasList() { return _paasList; }

    void setRemoteLocalList(const QList<LOCAL_LIST>& list);
    void setRemoteAppList(const QList<APP_LIST>& list);
    void setRemotePaasList(const QList<PAAS_LIST>& list);
    void setVappList(const QList<APP_LIST>& list);
    void setPaasList(const QList<PAAS_LIST>& list);

private:
    static IDesktopSettings *_instance;

    int _iconSize;
    QList<LOCAL_LIST> _remoteLocalList;
    QList<APP_LIST> _remoteAppList;
    QList<PAAS_LIST> _remotePaasList;
    QList<APP_LIST> _vappList;
    QList<PAAS_LIST> _paasList;

    explicit IDesktopSettings(QObject *parent = 0);
    
signals:
    void iconSizeUpdated(int old, int now);
    
public slots:
    
};

#endif // IDESKTOPSETTINGS_H
