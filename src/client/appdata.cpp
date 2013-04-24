#include "appdata.h"

AppData::AppData()
    : _added(false)
    , _isCanUpdated(false)
    , _recommended(0)
    , _rate(0)
{
}
bool AppData::isInLabels(QString label)
{
    for (int i = 0;i < _labels.size();i++)
        if (label == _labels[i])
            return true;
    return false;
}
