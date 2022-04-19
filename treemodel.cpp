#include "treemodel.h"

TreeModel::TreeModel(QObject *parent, QSqlDatabase db)
	: QSqlTableModel (parent,db)
{

}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	quint64 pid = 0;
	QString queryTxt;
	if (parent.isValid()) {
		pid = parent.internalId();
	}
	queryTxt = "SELECT * FROM " + tableName() + " WHERE parent=" + QString::number(pid) + " ORDER BY id";
	QSqlQuery query(database());
	query.exec(queryTxt);
	int existRows = query.size();

	//проверка диапазона
	if (position < 0 || position > existRows)
		return false;

	QSqlQuery insQuery(database());
	bool execResult;
	bool success = true;

	beginInsertRows(parent, position, position + rows - 1);
	for (int i = 0; i < rows; ++i)
	{
		queryTxt = "INSERT INTO " + tableName() + " (parent) VALUES (" + QString::number(pid) + ")";
		execResult = insQuery.exec(queryTxt);
		if (!execResult) {
			success = false;
		}
	}
	endInsertRows();

	return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	if (!parent.isValid())
		return false;

	quint64 pid = 0;
	quint64 id;
	QString queryTxt;
	if (parent.isValid()) {
		pid = parent.internalId();
	}
	queryTxt = "SELECT * FROM " + tableName() + " WHERE parent=" + QString::number(pid) + " ORDER BY id";
	QSqlQuery query(database());
	query.exec(queryTxt);
	int existRows = query.size();

	//проверка диапазона
	if (position < 0 || position + rows > existRows)
		return false;

	QSqlQuery delQuery(database());
	bool execResult;
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	query.seek(position);
	for (int i = 0; i < rows; ++i)
	{
		id = query.value("id").toULongLong();
		queryTxt = "DELETE FROM " + tableName() + " WHERE id=" + QString::number(id);
		execResult = delQuery.exec(queryTxt);
		if (!execResult) {
			success = false;
		}
	}
	endRemoveRows();

	return success;
}

bool TreeModel::insertRow(int position, const QModelIndex &parent)
{
	const bool success = insertRows(position,1,parent);
	return success;
}

bool TreeModel::removeRow(int position, const QModelIndex &parent)
{
	const bool success = removeRows(position,1,parent);
	return success;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	quint64 pid = 0;
	quint64 id;
	QString queryTxt;
	if (parent.isValid()) {
		pid = parent.internalId();
	}
	queryTxt = "SELECT * FROM " + tableName() + " WHERE parent=" + QString::number(pid) + " ORDER BY id";
	QSqlQuery query(database());
	query.exec(queryTxt);

	if (!parent.isValid() && row >= query.size())
		return QModelIndex();

	query.seek(row);
	id = query.value("id").toULongLong();
	return createIndex(row, column, id);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	quint64 id, pid;
	int row = 0;
	QString queryTxt;
	id = index.internalId();
	queryTxt = "SELECT * FROM " + tableName() + " WHERE id=" + QString::number(id);
	QSqlQuery query(database());
	query.exec(queryTxt);
	query.first();
	pid = query.value("parent").toULongLong();

	//если это индекс root item'а
	if (!pid)
		return QModelIndex();

	queryTxt = "SELECT * FROM " + tableName() + " WHERE parent=" + QString::number(pid) + " ORDER BY id";
	query.exec(queryTxt);
	while (query.next()) {
		if (query.value("id") == id) {
			break;
		}
		++row;
	}
	return createIndex(row, 0, pid);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	quint64 id;
	QString queryTxt;
	id = index.internalId();
	queryTxt = "SELECT name, id, parent FROM " + tableName() + " WHERE id=" + QString::number(id);
	QSqlQuery query(queryTxt,database());
	query.first();
	return query.value(index.column());
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return false;

	quint64 id;
	QString queryTxt;
	id = index.internalId();
	queryTxt = "UPDATE tree SET name = '" + value.toString() + "' WHERE id=" + QString::number(id);
	QSqlQuery query(database());
	bool result = query.exec(queryTxt);
	if (result) {
		emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
	}
	return result;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() > 0)
		return 0;

	quint64 pid = 0;
	QString queryTxt;
	if (parent.isValid()) {
		pid = parent.internalId();
	}
	queryTxt = "SELECT * FROM " + tableName() + " WHERE parent=" + QString::number(pid) + " ORDER BY id";
	QSqlQuery query(queryTxt,database());
	return query.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 3;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	if (!index.column())
		return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | QSqlTableModel::flags(index);

	return Qt::ItemIsEnabled | QSqlTableModel::flags(index);
}
