#ifndef APPNODE
#define APPNODE
#include <iostream>
#include <QString>
#include <QPixmap>
#include <QDebug>
#include <QVariant>
#define NAMELENTH 128
#define DESCRIPTION 256
#define ICONWIDTH 32
#define ICONHEIGH 32
#define ICONCOLUMNWIDTH 42
#define BUTTONCOLUMNWIDTH 75
#define SIZECOLUMNWIDTH 120
#define VERTICALHEIGHT 40
#define ICONPATH "./AppIcon/%1.png"
using namespace std;
enum PackageStatus { ALLSTATUS, UNINSTALLED, DOWNLOADED, INSTALLED, OLD };
enum PackageCategory { ALLCATEGORY, PROFESSIONAL, NETWORK, MEDIA, DRIVER, DEVELOP, OTHER }; 

class QueryThread;

class Node : public QObject
{
  Q_OBJECT
public:
    Node()
    {
    }

    ~Node()
    {
    } 

    QString name() 
    { 
        return _name; 
    }

    void setName(QString name)
    {
        _name = name;
    }

    QString version()
    {
        return _version;
    }

    void setVersion(QString ver)
    {
        _version = ver;
    } 

    QString brief() 
    { 
        return _brief; 
    }

    void setBrief(QString brief)
    {
        _brief= brief;
    }

    bool recommended() 
    { 
        return _recommended; 
    }

    void setRecommended(bool recom)
    {
        _recommended= recom;
    }

    PackageCategory category() 
    { 
        return _category; 
    }

    void setCategory(PackageCategory category)
    {
        _category = category;
    }

    int lastUpdate()
    {
        return _lastUpdate;
    }

    void setLastUpdate(int date)
    {
        _lastUpdate = date;
    }

    QString icon() 
    { 
        return _icon; 
    }

	void setIcon(QString pathname) 
    { 
        _icon = pathname;
    }

    QString uninstName()
    {
        return _uninstName;
    }

    void setUninstName(QString uninst) 
    {
        _uninstName = uninst;
    }

    QString execName()
    {
        return _execName;
    }

    void setExecName(QString ename)
    {
        _execName = ename;
    }

    int privilege()
    {
        return _privilege;
    }

    void setPrivilege(int priv)
    {
        _privilege = priv;
    }

    QString os()
    {
        return _os;
    }

    void setOS(QString os)
    {
        _os = os;
    }

    QString updateInfo()
    {
        return _updateInfo;
    }

    void setUpdateInfo(QString info)
    {
        _updateInfo = info;
    }

    QString comments()
    {
        return _comments;
    }

    void setComments(QString com)
    {
        _comments = com;
    }

    QString developer()
    {
        return _developer;
    }

    void setDeveloper(QString dev)
    {
        _developer = dev;
    }

    QString source()
    {
        return _source;
    }

    void setSource(QString source)
    {
        _source = source;
    }

    void setStatus(PackageStatus stat)
    {
        _oldstat = _status;
        _status = stat;
    }

    PackageStatus status() 
    { 
        return _status; 
    }

    PackageStatus  oldStatus() 
    { 
        return _oldstat; 
    }

private:
    QString         _name;
    QString         _version;
    QString         _brief;
    bool            _recommended;
    PackageCategory _category;
    int             _lastUpdate;
    QString         _icon;
    QString         _uninstName;
    QString         _execName;
    int             _privilege;
    QString         _os;
    QString         _updateInfo; 
    QString         _comments;
    QString         _developer;
    QString         _source;
    PackageStatus   _status;
    PackageStatus   _oldstat;
};

/* A singleton class */
class NodeList:public QObject {
    Q_OBJECT 
public:
    typedef QVector<Node *> T_NODES;
protected:
    NodeList();
public:
    static NodeList *instance();
    void flush();
    virtual ~ NodeList();
    void clear();
    int count() 
    { 
        return _nodes.count(); 
    }

    Node * at(int idx) 
    { 
        return _nodes[idx]; 
    }

    Node * getNode(QString name)
    {
        for (int i = 0; i< _nodes.count(); i++) {
           if (_nodes[i]->name() == name)
               return _nodes[i];
        }
        return NULL;
    }
public slots:
    void onQueryFinished(int);
private:
    T_NODES _nodes;
    QueryThread *thread;
    bool _finished;
    int _count;
    static NodeList *S_NODELIST;
    void initial();
};

#endif
