#ifndef VACSERVERWIDGET_H
#define VACSERVERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class VacServerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit VacServerWidget(QWidget *parent = 0);
    ~VacServerWidget();
    
signals:
    void serverChanged();

public slots:
    void saveServer();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLabel *_vacLabel;
    QLabel *_portLabel;
    QLabel *_nameLabel;
    QLabel *_pwdLabel;

    QLineEdit *_vacEdit;
    QLineEdit *_portEdit;
    QLineEdit *_nameEdit;
    QLineEdit *_pwdEdit;

    QPushButton *_saveBtn;
    QPushButton *_cancelBtn;
};

#endif // VACSERVERWIDGET_H
