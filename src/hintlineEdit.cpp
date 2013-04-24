#include <QLineEdit>
#include <QPainter>
#include <QDebug>
#include <QRect>
#include "hintlineEdit.h"

HintLineEdit::HintLineEdit(QString hint, QString pixmap, QWidget *parent)
    : QLineEdit(parent), _hint(hint), _pixmap(pixmap)
{
    //setFixedSize(300, 35);
    //setTextMargins(32, 5, 2, 5);
    setStyleSheet("QLineEdit{border-width:0;border-style:outset;font:18px;}");
    //QPalette pal;
    //pal.setBrush(backgroundRole(), QBrush(QPixmap(":images/login_input_bg.png")));
    //setPalette(pal);
}

void HintLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
    QPainter painter(this);
    painter.drawPixmap(0, 2, QPixmap(_pixmap));
    //if (hasFocus())
    //    return;
    if (!text().isEmpty())
        return;

    QRect rt = rect();
	painter.setPen(Qt::gray);
    painter.drawText(rt.translated(32,2), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextHideMnemonic, _hint);
}
