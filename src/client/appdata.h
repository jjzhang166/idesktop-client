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
    QString _brief;//���
    QString _strIconPath;
    QList<QString> _labels;

    bool _added;//�Ƿ������
    bool _isCanUpdated;//�Ƿ��и���
    int _recommended;//1�Ƽ�

    QString _intro;//��ϸ����
    QString _execname;//��ݷ�ʽ����
    int _lastupdate;//����������

    QString _updateinfo;//������ʾ
    QString _uninstall;//�����������ʾ������

    QString _developer;
    QString _category;
    QString _source;
    QString _version;
    QString _size;

    int _rate;
};

#endif // APPDATA_H
