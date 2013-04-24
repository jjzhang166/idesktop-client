#include "updatebutton.h"

UpdateButton::UpdateButton(QWidget *parent) :
    CustomBtn(parent)
  , _state(noUpdate)
{
    SetOnImg(":images/icon_install_hover.png");
    SetOffImg(":images/icon_install.png");
    SetEnabledImg(":images/icon_install_end.png");
}

void UpdateButton::mousePressEvent ( QMouseEvent * event)
{
    if (!this->isEnabled())
        return;

    CustomBtn::mousePressEvent(event);

    if (_state == canUpdate) {
        emit update();
    }
    else if (_state == loading) {
        emit cancelUpdate();
    }
}

void UpdateButton::resetState()
{
    setState(noUpdate);
    enabled(true);
}

UpdateButton::update_state UpdateButton::getState()
{
    return _state;
}
void UpdateButton::setState(const UpdateButton::update_state &state)
{
    _state = state;
    if (_state == noUpdate) {
    //    this->setText("�Ѱ�װ");
        this->setText("���");
        enabled(false);
    }
    else if (_state == updating) {
        this->setText("������..");
        enabled(false);
    }
    else if (_state == loading) {
        this->setText("ȡ��");
        enabled(true);
    }
    else if (_state == canUpdate) {
        this->setText("����");
        enabled(true);
    }
    repaint();
}

