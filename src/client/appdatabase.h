#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <QList>

class AppData;
class AppDatabase
{
public:

    static AppDatabase* Instance();
    static void destroy();

    //添加，保存复本
    void addData(AppData* data);
    AppData* getData(const int &index);
    AppData* getData(const QString &name);
    int size();
    void removeAll();

    //
    int totalCount();
    void totalCount(const int &count);

private:
    AppDatabase();

private:

    QList<AppData*> _dataBase;
    int _totalCount;//全部app数量

    static AppDatabase* _pInstance;
};

#endif // APPDATABASE_H
