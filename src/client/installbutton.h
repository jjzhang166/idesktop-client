#ifndef INSTALLBUTTON_H
#define INSTALLBUTTON_H

#include "../custombtn.h"

class InstallButton : public CustomBtn
{
    Q_OBJECT
public:

    enum add_state{
        unAdded, adding, loading
    };
    explicit InstallButton(QWidget *parent = 0);
    
    void resetState();
    add_state getState();

    void setState(const add_state &bAdd);


protected:

    void mousePressEvent ( QMouseEvent * event);

signals:

    void install();
    void cancelInstall();
    
public slots:

private:

    add_state _state;
    
};

#endif // INSTALLBUTTON_H
