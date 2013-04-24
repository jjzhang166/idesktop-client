#ifndef GLOBAL_H
#define GLOBAL_H
#include <QMap>
#include <QString>

class Config
{
public:
    static void initiate();
    static QString get(QString key);
    static void set(QString key, QString value);
    static void getServerIp();
    static void setServerIp();
private:
    static QMap<QString, QString> configDict;
};
#endif
