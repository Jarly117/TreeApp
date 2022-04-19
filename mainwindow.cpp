#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle("Just one more tree");

	//подключаемся к базе данных
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("treedb");
	db.setHostName("localhost");
	db.setPort(3306);
	db.setUserName("root");
	db.setPassword("Moon25!!");
	db.setConnectOptions("MYSQL_OPT_RECONNECT=TRUE;");

	db_opened = db.open();
	qDebug() << "db_opened =" << db_opened;
	if (!db_opened) {
		qDebug() << "Can't connect to database.";
		return;
	}

	//проверяем наличие таблицы с нодами дерева
	QString tableName("tree");
	bool needCreateTable = db.record(tableName).isEmpty();
	if (needCreateTable) {
		//создаем sql-запрос для создания таблицы
		QString query;
		query = "CREATE TABLE " + tableName + " (name VARCHAR(20), id INT NOT NULL AUTO_INCREMENT, parent INT, PRIMARY KEY (id))";
		db.exec(query);

		//создаем начальные ноды
		query = "INSERT INTO tree (name, parent) VALUES ('Node 1', 0)";
		db.exec(query);
		query = "INSERT INTO tree (name, parent) VALUES ('Node 2', 1)";
		db.exec(query);
		query = "INSERT INTO tree (name, parent) VALUES ('Node 3', 1)";
		db.exec(query);
	}

	//подключаемся к таблице тегов
	modelForTree = new TreeModel(this,db);
	modelForTree->setTable(tableName);
	modelForTree->setEditStrategy(QSqlTableModel::OnManualSubmit);
	selectModel = new QItemSelectionModel(modelForTree);
	modelForTree->select();

	ui->treeMain->setModel(modelForTree);
	ui->treeMain->setSelectionModel(selectModel);

	ui->treeMain->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

	//заполняем контекстное меню
	contextMenu = new QMenu(this);
	contextMenu->addAction("Добавить нод", this, &MainWindow::addNode);
	deleteRowAction = new QAction("Удалить нод");
	deleteRowAction->setShortcut(Qt::Key_Delete);
	deleteRowAction->setShortcutVisibleInContextMenu(true);
	contextMenu->addAction(deleteRowAction);
	connect(contextMenu, &QMenu::aboutToShow, this, &MainWindow::adaptContextMenu);
	connect(deleteRowAction, &QAction::triggered, this, &MainWindow::removeNode);
	deleteShortcut = new QShortcut(Qt::Key_Delete, ui->treeMain);
	connect(deleteShortcut, &QShortcut::activated, this, &MainWindow::removeNode);

	//устанавливаем счетчик id
	QString queryTxt;
	QSqlQuery tempQuery(db);
	queryTxt = "SELECT * FROM " + tableName + " ORDER BY id";
	tempQuery.exec(queryTxt);
	tempQuery.last();
	idCount = tempQuery.value("id").toInt();
}

MainWindow::~MainWindow()
{
	db.close();
	delete ui;
	delete deleteShortcut;
	delete deleteRowAction;
	delete contextMenu;
	delete selectModel;
	delete modelForTree;
}

//вызываем контекстное меню нода
void MainWindow::on_treeMain_pressed(const QModelIndex &index)
{
	if (!index.isValid())
		return;

	if (QGuiApplication::mouseButtons() == Qt::RightButton)
		contextMenu->exec(QCursor::pos());
}

void MainWindow::addNode()
{
	QModelIndex qmidx1 = selectModel->selectedRows().first();
	QModelIndex qmidx2;
	int rowCount = modelForTree->rowCount(qmidx1);
	QString name = "Node " + QString::number(++idCount);

	modelForTree->insertRow(rowCount,qmidx1);
	qmidx2 = modelForTree->index(rowCount,0,qmidx1);
	modelForTree->setData(qmidx2, name, Qt::EditRole);
}

void MainWindow::removeNode()
{
	QModelIndex qmidx1 = selectModel->selectedRows().first();
	modelForTree->removeRow(qmidx1.row(),qmidx1.parent());
}

//устанавливаем видимость опций меню
void MainWindow::adaptContextMenu()
{
	QModelIndex qmidx1 = selectModel->selectedRows(2).first();
	deleteRowAction->setEnabled(modelForTree->data(qmidx1).toInt());
}
