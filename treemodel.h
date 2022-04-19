#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QtWidgets>
#include <QSqlTableModel>
#include <QSqlQuery>

class TreeModel : public QSqlTableModel
{
	Q_OBJECT

public:
	explicit TreeModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual bool insertRows(int position, int rows,
						const QModelIndex &parent = QModelIndex()) override;
	virtual bool removeRows(int position, int rows,
						const QModelIndex &parent = QModelIndex()) override;
	bool insertRow(int position, const QModelIndex &parent = QModelIndex());
	bool removeRow(int position, const QModelIndex &parent = QModelIndex());
	virtual bool setData(const QModelIndex &index, const QVariant &value,
					 int role = Qt::EditRole) override;

private:

};

#endif // TREEMODEL_H
