#ifndef UPDATEBUTTON_H
#define UPDATEBUTTON_H

#include "../custombtn.h"

class UpdateButton : public CustomBtn
{
    Q_OBJECT
public:

    enum update_state{
        noUpdate, updating, loading, canUpdate
    };

    explicit UpdateButton(QWidget *parent = 0);

    void resetState();
    update_state getState();
    void setState(const update_state &state);

protected:

    void mousePressEvent ( QMouseEvent * event);

signals:

    void update();
    void cancelUpdate();

public slots:

private:

    update_state _state;
};

#endif // UPDATEBUTTON_H
