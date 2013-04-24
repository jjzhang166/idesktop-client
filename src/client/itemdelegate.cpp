#include "itemdelegate.h"
#include "itemmodel.h"
#include <QApplication>
#include <QPainter>
#include <QEvent>
ItemDelegate::ItemDelegate(QObject * parent):QItemDelegate(parent)
{
}

void ItemDelegate::paint(QPainter * painter,
		const QStyleOptionViewItem & option,
		const QModelIndex & index) const
{
    int column;
    int status;
    QString name;

	if (!index.isValid()) {
		return;
	}

    column = index.column();
    name = index.data(Qt::UserRole + 1).toString();
    status = index.data(Qt::UserRole + 2).toInt();

    /*
    QFont font = option.font;
    QFontMetrics fm(font);
    font.setPointSize(font.pointSize() - 1);

	if (column == NameColumn) {
		QRectF nameRectF(option.rect.x(), option.rect.y() + 5,
				option.rect.width(), option.rect.height() * 0.6);
		painter->drawText(nameRectF, Qt::AlignLeft | Qt::AlignTop, name);
        return;
	} 

    if (column == DescrColumn) {
		QRectF nameRectF(option.rect.x(), option.rect.y() + 5,
				option.rect.width(), option.rect.height() * 0.6);
		painter->drawText(nameRectF, Qt::AlignLeft | Qt::AlignTop, \
            index.data(Qt::DisplayRole, DescrColumn));
        return;
    }
    */

    QItemDelegate::paint(painter, option, index);
}
