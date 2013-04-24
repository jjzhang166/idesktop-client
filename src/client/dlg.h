#ifndef DLG_H
#define DLG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>

class Dialog : public QDialog
{
public:
    Dialog(QWidget *parent=NULL);
    ~Dialog(){}
    void setText(QString text);
    /*
public slots:
    void accept();
    void reject();
    */
private:
    QDialogButtonBox *buttonBox;
    QLabel *label;
};

#endif
