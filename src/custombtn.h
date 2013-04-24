#ifndef CUSTOMBTN_H
#define CUSTOMBTN_H

#include <QWidget>

class CustomBtn : public QWidget
{
    Q_OBJECT
public:
    explicit CustomBtn(QWidget *parent = 0);
    
public:

    void SetOnImg(const QString &strOn);
    void SetOffImg(const QString &strOff);
    void SetEnabledImg(const QString &strEnabled);
    void setText(const QString &strText);
    QString text();

    void SetOn(const bool &bOn);

    void enabled(const bool &bEnable);

    int ID();
    void setID(const int &id);

signals:
    
    void clicked();

public slots:

public:
    void mousePressEvent ( QMouseEvent * event);
    void enterEvent ( QEvent * event );
    void leaveEvent ( QEvent * event );
    void paintEvent(QPaintEvent *event);

    void closeEvent ( QCloseEvent * event );
private:

    QString _strOn;
    QString _strOff;
    QString _strEnabled;

    bool _bOn;//是否在窗口上
    bool _bEnabled;//是否可用

    QString _text;
    int _id;
};

#endif // CUSTOMBTN_H
