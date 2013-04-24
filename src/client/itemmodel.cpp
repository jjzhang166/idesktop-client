#include "itemmodel.h"

bool SoftwareProxyModel::filterAcceptsRow(int sourceRow, \
        const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex;
    QString name;
    QString brief;
    PackageStatus status;
    PackageCategory category;
    int recom;
    bool TEXT, STAT, CLASS, RECOM;

    sourceIndex = sourceModel()->index(sourceRow, NameColumn, sourceParent);
    name = sourceIndex.data(Qt::UserRole + 1).toString();
    brief = sourceIndex.data(Qt::UserRole + 2).toString();
    status = static_cast<PackageStatus>(sourceIndex.data(Qt::UserRole + 3).toInt());
    category = static_cast<PackageCategory>(sourceIndex.data(Qt::UserRole + 4).toInt());
    recom = sourceIndex.data(Qt::UserRole + 5).toInt();

    TEXT = name.append(brief).contains(_pkg_search);
    STAT = (status == _pkg_stat);
    CLASS = (category == _category); 
    RECOM = true;
    if (_pkg_recom) //just show recommended
        RECOM = recom; 

    if (_pkg_both)
        STAT = (status == INSTALLED || status == OLD);

    if (_pkg_stat == ALLSTATUS)
        STAT = true;
    if (_category == ALLCATEGORY) {
        CLASS = true;
    }
    return STAT && CLASS && TEXT && RECOM;
}

ItemModel::ItemModel(QObject * parent)
	:QAbstractTableModel(parent)
{
    _nodeList = NodeList::instance();
}

QVariant ItemModel::data(
		const QModelIndex & index,
		int role) const
{
    int column;
    Node *node;
    QString name;

	column = index.column();
	node = (Node *)(index.internalPointer());
    name = node->name();

	if (NULL == node)
		return QVariant();

    if (role == Qt::UserRole + 1)
        return name;

    if (role == Qt::UserRole + 2)
        return node->brief();

    if (role == Qt::UserRole + 3)
        return node->status();

    if (role == Qt::UserRole + 4)
        return node->category();

    if (role == Qt::UserRole + 5)
        return node->recommended();

    if (column == FavorColumn) {
        /*
        if(role == Qt::ToolTipRole)
            return tr("Recommended");
        if (role == Qt::DecorationRole) {
            if (node->recommended())
                return QPixmap(node->icon())\
                    .scaled(ICONWIDTH,ICONHEIGH);
            else
                return QPixmap();
        }
        */
    }

    if (column == NameColumn)
        if (role == Qt::DisplayRole)
            return name + " (" + node->brief() + ")"; 

    if (column == StatusColumn) {
        /*
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignRight;
        */
        if (role == Qt::DisplayRole) {
            switch(node->status()) {
            case UNINSTALLED:
                return "未安装";
            case INSTALLED:
                return "已安装";
            case DOWNLOADED:
                return "已下载";
            case OLD:
                return "需更新";
            }
        }
    }
    return QVariant();
}

bool ItemModel::setData(const QModelIndex & index, const QVariant & value, int role )
{
	Q_UNUSED(role)
	return true;
}

QVariant ItemModel::headerData(int section,
		Qt::Orientation orientation,
		int role) const
{
    return "";
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex & parent)const
{
	Q_UNUSED(parent)
	if(!hasIndex(row, column, parent))
		return QModelIndex();
    return createIndex(row, column, _nodeList->at(row));
}

int ItemModel::rowCount(const QModelIndex &parent)const
{
	Q_UNUSED(parent)
	return  _nodeList->count();

}

int ItemModel::columnCount(const QModelIndex &parent)const {
	Q_UNUSED(parent)
	return 4;
}

bool ItemModel::insertRows ( int row,int count, const QModelIndex & parent )
{
	Q_UNUSED(parent)
	beginInsertRows(QModelIndex(), row, row + count - 1);
	endInsertRows();
	return true;
}
