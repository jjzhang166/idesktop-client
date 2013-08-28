#include <QLineEdit>
#include <QPainter>
#include <QDebug>
#include <QRect>
#include "hintlineEdit.h"

HintLineEdit::HintLineEdit(QString hint, QString pixmap, QWidget *parent)
    : QLineEdit(parent), _hint(hint)
{
    Q_UNUSED(pixmap);
    setStyleSheet("QLineEdit:focus{border-color: rgba(133,192,241,255);}");
}

void HintLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
    QPainter painter(this);
    //painter.drawPixmap(0, 0, 208, 27, _pixmap.scaled(208, 27));
    //if (hasFocus())
    //    return;
    if (!text().isEmpty())
        return;

    QRect rt = rect();
	painter.setPen(Qt::gray);
    painter.drawText(rt.translated(13,1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextHideMnemonic, _hint);
}

void HintLineEdit::focusInEvent(QFocusEvent *event)
{

    //_pixmap.load(":images/login_input_hover.png");

    QLineEdit::focusInEvent(event);
}

void HintLineEdit::focusOutEvent(QFocusEvent *event)
{

    //_pixmap.load(":images/login_input.png");

    QLineEdit::focusOutEvent(event);
}
