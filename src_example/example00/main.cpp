/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief Super fast tree sorting and filtering tree widget.
 */

// C++
#include <iostream>
#include <vector>

/* sqlite3 3.33.0
 * License: PublicDomain
 */
#include <sqlite3.h>

/* QT 5.13.2
 * License: LGPLv3
 */
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>

// Bookfiler Libraries
#include <BookFiler-Lib-Sort-Filter-Tree-Widget/Interface.hpp>

std::string gen_random(const int len);
int populateDatabase(std::shared_ptr<sqlite3> database);

std::string testName = "Sort Filter Tree Widget Example 00";

int main(int argc, char *argv[]) {
  std::cout << testName << " BEGIN" << std::endl;

  // Create a QT application
  QApplication qtApp(argc, argv);
  QMainWindow qtMainWindow;

  // create a test database
  sqlite3 *dbPtr = nullptr;
  int rc = sqlite3_open(":memory:", &dbPtr);
  if (rc) {
    std::cout << "sqlite3_open ERROR:\n" << sqlite3_errmsg(dbPtr) << std::endl;
    return -1;
  }

  std::shared_ptr<sqlite3> database(nullptr);
  database.reset(dbPtr, sqlite3_close);

  rc = populateDatabase(database);
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
  bookfiler::widget::SqliteModel *sqlModelPtr =
      new bookfiler::widget::SqliteModel(database, "testTable", columnMap);
  sqlModelPtr->setRoot("*");

  // Setup View
  bookfiler::widget::TreeView *treeViewPtr = new bookfiler::widget::TreeView();
  treeViewPtr->setModel(sqlModelPtr);
  treeViewPtr->update();
  treeViewPtr->setSortingEnabled(true);

  // Set up window
  QWidget *centralWidgetPtr = new QWidget();
  QVBoxLayout *layout = new QVBoxLayout;
  QPushButton *btn = new QPushButton();
  btn->setText("Sort");
  QObject::connect(btn, &QPushButton::clicked, [sqlModelPtr, treeViewPtr](bool) {
      std::vector<std::pair<std::string, std::string>> sortOrder;
      sortOrder.emplace_back(std::pair<std::string, std::string>("name", "ASC"));
      sqlModelPtr->setSort(sortOrder);
      treeViewPtr->update();
  });

  layout->addWidget(treeViewPtr);
  layout->addWidget(btn);
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

int populateDatabase(std::shared_ptr<sqlite3> database) {
  char *zErrMsg = 0;
  int rc = 0;
  /* CREATE TABLE - Create SQL statement */
  const char *creatTableSql = "CREATE TABLE testTable(guid text(32) PRIMARY "
                              "KEY NOT NULL, parent_guid text(32), name "
                              "text(2048) NOT NULL, value text(2048) NOT NULL)";

  /* Execute SQL statement */
  rc = sqlite3_exec(
      database.get(), creatTableSql,
      [](void *NotUsed, int argc, char **argv, char **azColName) -> int {
        int i;
        for (i = 0; i < argc; i++) {
          printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
      },
      0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  /* INSERT TEST DATA - Create SQL statement */
  // keep track of guid so that new children can randomly be added
  std::vector<std::string> guidList{"*", "*"};
  std::string insertSql;
  srand((unsigned)time(NULL) * getpid());
  /* Randomly generate an SQL statement to generate a tree with randomly
   * generated children, names, and values
   */
  for (int i = 0; i < 1000; i++) {
    std::string parentGuid = guidList[rand() % (guidList.size() - 1)];
    std::string newGuid = gen_random(32);
    insertSql.append("INSERT INTO testTable (guid,parent_guid,name,value) "
                     "VALUES ('");
    insertSql.append(newGuid);
    insertSql.append("', ");
    if (parentGuid == "*") {
      insertSql.append("NULL");
    } else {
      insertSql.append("'").append(parentGuid).append("'");
    }
    insertSql.append(", '")
        .append(gen_random(100))
        .append("', '")
        .append(gen_random(1000))
        .append("');");
    guidList.push_back(newGuid);
  }

  std::cout << insertSql.substr(0, 5000) << "..." << std::endl;

  /* Execute SQL statement */
  rc = sqlite3_exec(
      database.get(), insertSql.c_str(),
      [](void *NotUsed, int argc, char **argv, char **azColName) -> int {
        int i;
        for (i = 0; i < argc; i++) {
          printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
      },
      0, &zErrMsg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return -2;
  }
  return 0;
}
