#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include "treemodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	//для базы данных
	bool db_opened;
	QSqlDatabase db;
	TreeModel *modelForTree;
	QItemSelectionModel *selectModel;

	QMenu *contextMenu;
	QAction *deleteRowAction;
	QShortcut *deleteShortcut;

	int idCount = 0;

private slots:
	void on_treeMain_pressed(const QModelIndex &index);
	void addNode();
	void removeNode();
	void adaptContextMenu();

};

#endif // MAINWINDOW_H
