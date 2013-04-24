#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QRadioButton>
#include <QDialog>
#include <QLabel>
#include <QEvent>
#include <QPaintEvent>
#include <QPixmap>

#define CLOSE_WIDTH 47
#define CLOSE_HEIGHT 35

class DynamicButton;
class QRadioButton;
class QLabel;
class QSpinBox;

class SettingDialog : public QDialog
{
    Q_OBJECT
public:
    bool equal;
    int pCount;
    SettingDialog(QWidget *parent = NULL);
    ~SettingDialog() {}
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
public slots:
    void updateUI(bool);
    void accept();
private:
    QRadioButton *equalRadio;
    QRadioButton *miniRadio;
    QLabel *lb1;
    QLabel *lb2;
    QLabel *lb3;
    QSpinBox *pCountSpin;
    QPoint startDrag;
    bool _titlePressed;
    bool _finished;
};

#endif
