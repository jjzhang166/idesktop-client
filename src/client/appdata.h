#ifndef APPDATA_H
#define APPDATA_H

#include <QString>
#include <QList>


class AppData
{
public:
    AppData();

    bool isInLabels(QString label);


public:

    QString _name;
    QString _brief;//简介
    QString _strIconPath;
    QList<QString> _labels;

    bool _added;//是否已添加
    bool _isCanUpdated;//是否有更新
    int _recommended;//1推荐

    QString _intro;//详细介绍
    QString _execname;//快捷方式名称
    int _lastupdate;//最后更新日期

    QString _updateinfo;//更新提示
    QString _uninstall;//控制面板里显示的名称

    QString _developer;
    QString _category;
    QString _source;
    QString _version;
    QString _size;

    int _rate;
};

#endif // APPDATA_H
