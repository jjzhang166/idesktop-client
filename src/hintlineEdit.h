#ifndef HINTLINEEDIT_H
#define HINTLINEEDIT_H
#include <QLineEdit>

class HintLineEdit : public QLineEdit
{
public:
    HintLineEdit(QString hint = QString(""), \
                 QString pixmap = QString(""), \
                 QWidget *parent = NULL);
    ~HintLineEdit() {}
    void setHint(const QString &hint)
    {
        _hint = hint;
        repaint();
    }
protected:
    virtual void paintEvent(QPaintEvent* event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    QString _hint;
    QPixmap _pixmap;

};

#endif
