/*
 * @name BookFiler Widget w/ QT - Task Manager
 * @version 1.00
 * @license MIT
 * @brief Task Manager Widget tracks task progress
 */

// C++
#include <iostream>

/* QT 5.13.2
 * License: LGPLv3
 */
#include <QApplication>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QWidget>

// Bookfiler Libraries
#include <BookFiler-Lib-Sort-Filter-Tree-Widget/Interface.hpp>

std::string gen_random(const int len);
int populateDatabase(QSqlDatabase &database);

std::string testName = "Sort Filter Tree Widget Example 01";

int main(int argc, char *argv[]) {
  std::cout << testName << " BEGIN" << std::endl;

  // Create a QT application
  QApplication qtApp(argc, argv);
  QMainWindow qtMainWindow;

  // create a test database
  QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
  database.setDatabaseName(":memory:");

  if (!database.open()) {
    std::cout << "Database Error Code: " << database.lastError().number()
              << ", Message: " << database.lastError().text().toStdString()
              << std::endl;
    return -1;
  }

  int rc = populateDatabase(database);
  if (rc < 0) {
    std::cout << "error" << std::endl;
    return rc;
  }

  // columnMap
  std::map<std::string, std::string> columnMap{{"id", "guid"},
                                               {"parentId", "parent_guid"},
                                               {"name", "name"},
                                               {"value", "value"}};

  // Setup Model
  bookfiler::widget::SqliteModelTest *sqlModelPtr =
      new bookfiler::widget::SqliteModelTest(database, "testTable", columnMap);
  sqlModelPtr->setRoot("*");

  // Setup View
  bookfiler::widget::TreeView *treeViewPtr = new bookfiler::widget::TreeView();
  treeViewPtr->setModel(sqlModelPtr);
  treeViewPtr->update();

  // Set up window
  QWidget *centralWidgetPtr = new QWidget();
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(treeViewPtr);
  centralWidgetPtr->setLayout(layout);

  qtMainWindow.setCentralWidget(centralWidgetPtr);
  qtMainWindow.show();

  // Start the application loop
  qtApp.exec();

  std::cout << testName << " END" << std::endl;
  return 0;
}

std::string gen_random(const int len) {
  std::string tmp_s;
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  tmp_s.reserve(len);
  for (int i = 0; i < len; ++i)
    tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  return tmp_s;
}

int populateDatabase(QSqlDatabase &database) {
  bool ret;
  /* CREATE TABLE - Create SQL statement */
  const char *creatTableSql = "CREATE TABLE testTable(guid text(32) PRIMARY "
                              "KEY NOT NULL, parent_guid text(32), name "
                              "text(2048) NOT NULL, value text(2048) NOT NULL)";

  QSqlQuery query(database);
  /* Execute SQL statement */
  ret = query.exec(creatTableSql);
  if (!ret) {
    std::cout << "Create Error Code: " << query.lastError().number()
              << ", Message: " << query.lastError().text().toStdString()
              << std::endl;
    return -1;
  }

  /* INSERT TEST DATA - Create SQL statement */
  // keep track of guid so that new children can randomly be added
  std::vector<std::string> guidList{"*", "*", "*", "*"};
  std::string insertSql;
  srand((unsigned)time(NULL) * getpid());
  /* Randomly generate an SQL statement to generate a tree with randomly
   * generated children, names, and values
   */
  for (int i = 0; i < 100; i++) {
    std::string parentGuid = guidList[rand() % (guidList.size() - 1)];
    std::string newGuid = gen_random(32);
    std::string singleQuerySql = "";
    singleQuerySql.append("INSERT INTO testTable (guid,parent_guid,name,value) "
                          "VALUES ('");
    singleQuerySql.append(newGuid);
    singleQuerySql.append("', ");
    if (parentGuid == "*") {
      singleQuerySql.append("NULL");
    } else {
      singleQuerySql.append("'").append(parentGuid).append("'");
    }
    singleQuerySql.append(", '")
        .append(gen_random(100))
        .append("', '")
        .append(gen_random(1000))
        .append("');");
    guidList.push_back(newGuid);

    /* Execute SQL statement
     * QSqlQuery can only execute one statement at a time.
     */
    ret = query.exec(QString().fromStdString(singleQuerySql));
    if (!ret) {
      std::cout << "Insert Error Code: " << query.lastError().number()
                << ", Message: " << query.lastError().text().toStdString()
                << std::endl;
      return -2;
    }

    insertSql.append(singleQuerySql);
  }

  std::cout << insertSql.substr(0, 5000) << "..." << std::endl;

  return 0;
}
