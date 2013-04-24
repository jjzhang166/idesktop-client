#include "installbutton.h"

InstallButton::InstallButton(QWidget *parent) :
    CustomBtn(parent)
  , _state(unAdded)
{
    SetOnImg(":images/icon_install_hover.png");
    SetOffImg(":images/icon_install.png");
    SetEnabledImg(":images/icon_install_end.png");
}

void InstallButton::mousePressEvent ( QMouseEvent * event)
{
    if (!this->isEnabled())
        return;

    CustomBtn::mousePressEvent(event);

    if (_state == unAdded) {
        emit install();
    }
    else if (_state == loading) {
        emit cancelInstall();
    }
}

void InstallButton::resetState()
{
    setState(unAdded);
    enabled(true);
}


InstallButton::add_state InstallButton::getState()
{
    return _state;
}

void InstallButton::setState(const add_state &bAdd)
{
    _state = bAdd;
    if (_state == unAdded) {
        this->setText("安装");
        enabled(true);
    }
    else if (_state == adding) {
        this->setText("安装中..");
        enabled(false);
    }
    else if (_state == loading) {
        this->setText("取消");
        enabled(true);
    }
    repaint();
}
