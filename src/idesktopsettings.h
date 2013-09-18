#ifndef IDESKTOPSETTINGS_H
#define IDESKTOPSETTINGS_H

#include <QObject>
//vac
#include "commuinication.h"
//paas
#include "paascommuinication.h"
#include "iconwidget.h"

#define FONTSIZE 10
#define SMALLSLIDERWIDTH 38

#define HSPACING 4 // grid left/right spacing
#define VSPACING 0 // grid top spacing
#define BOTTOMSPACING 24 // grid bottom spacing

#define LARGESIZE QSize(143, 143)     //72*72
#define MEDIUMSIZE QSize(119, 119)     //48*48
#define SMALLSIZE QSize(103, 103)   //32*32

#define ICONHSPACING (88 - 40) // left + right side margin
#define ICONVSPACING (88 - 36) // top + bottom side margin

class IDesktopSettings : public QObject
{
    Q_OBJECT
public:
    static IDesktopSettings* instance();

    IconWidget::icon_size iconSize() const;
    void setIconSize(IconWidget::icon_size newSize);
    void initIconSize();


    QList<LOCAL_LIST>& remoteLocalList() { return _remoteLocalList; }
    QList<APP_LIST>& remoteAppList() { return _remoteAppList; }
    QList<PAAS_LIST>& remotePaasList() { return _remotePaasList; }
    QList<LOCAL_LIST>& localList() { return _localList; }
    QList<APP_LIST>& vappList() { return _vappList; }
    QList<PAAS_LIST>& paasList() { return _paasList; }

    void setRemoteLocalList(const QList<LOCAL_LIST>& list);
    void setRemoteAppList(const QList<APP_LIST>& list);
    void setRemotePaasList(const QList<PAAS_LIST>& list);
    void setLocalList(const QList<LOCAL_LIST>& list);
    void setVappList(const QList<APP_LIST>& list);
    void setPaasList(const QList<PAAS_LIST>& list);

    QVariant prop(const QString& propName);

private:
    static IDesktopSettings *_instance;

    IconWidget::icon_size _iconSize;
    QList<LOCAL_LIST> _remoteLocalList;
    QList<APP_LIST> _remoteAppList;
    QList<PAAS_LIST> _remotePaasList;
    QList<LOCAL_LIST> _localList;
    QList<APP_LIST> _vappList;
    QList<PAAS_LIST> _paasList;

    explicit IDesktopSettings(QObject *parent = 0);
    
signals:
    void iconSizeUpdated(IconWidget::icon_size old, IconWidget::icon_size now);
    
public slots:
    
};

#endif // IDESKTOPSETTINGS_H
