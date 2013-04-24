#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <QList>

class AppData;
class AppDatabase
{
public:

    static AppDatabase* Instance();
    static void destroy();

    //��ӣ����渴��
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
    int _totalCount;//ȫ��app����

    static AppDatabase* _pInstance;
};

#endif // APPDATABASE_H
