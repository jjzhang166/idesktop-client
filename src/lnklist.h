#ifndef LNKLIST_H
#define LNKLIST_H
#include <QDir>
#include <QFileInfoList>
class QObject;
class QString;

class LnkList : public QObject
{
    Q_OBJECT
public:
    static LnkList *getList();
    ~LnkList() {};
    bool has(QString name);
    QString wholePath(QString name);
    int count();
    void refresh();
private:
    LnkList(QObject *parent = NULL);
    QDir dir;
    QDir dirCommon;
    QFileInfoList _fl;
    QFileInfoList _flCommon;
    static LnkList *_l;
    QString _desktop;
    QString _commonDesktop;
};
#endif
