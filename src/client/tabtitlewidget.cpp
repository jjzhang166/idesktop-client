#include "tabtitlewidget.h"

#include "appdatabase.h"
#include "appdatareadexe.h"
#include <QPainter>
#include <QPen>

TabTitleWidget::TabTitleWidget(QWidget *parent) :
    OpacityWidget(parent)
  , _TitleFont("", 10, QFont::Black)
{
}

void TabTitleWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    QPen pen;
    pen.setColor(QColor(73,73,73,255));
    painter.setPen(pen);
    painter.setFont(_TitleFont);

    painter.drawText(QRect(10,15, 500,30), _text);
}

void TabTitleWidget::OnRefresh()
{
    QByteArray tmp = AppDataReadExe::Instance()->getCurCategory().toAscii();

    _text = QString("%1%2%3%4").arg(AppDataReadExe::Instance()->getCurCategory()).arg('(').arg(AppDatabase::Instance()->totalCount()).arg(')');
    repaint();
}

void TabTitleWidget::clearWdg()
{
    _text = "";
    repaint();
}
