#ifndef IDESKTOPSETTINGS_H
#define IDESKTOPSETTINGS_H

#include <QObject>

class IDesktopSettings : public QObject
{
    Q_OBJECT
public:
    static IDesktopSettings* instance();

    int iconSize() const;
    void setIconSize(int newSize);

private:
    static IDesktopSettings *_instance;

    int _iconSize;

    explicit IDesktopSettings(QObject *parent = 0);
    
signals:
    void iconSizeUpdated(int old, int now);
    
public slots:
    
};

#endif // IDESKTOPSETTINGS_H
