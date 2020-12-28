/*
 * @name BookFiler Widget - Sqlite Model
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief QAbstractItemModel with a sqlite3 backend.
 */

#if DEPENDENCY_SQLITE

// C++
#include <algorithm> // std::find
#include <iostream>  // std::cout
#include <vector>    // std::vector

// Local Project
#include "SqliteModelIndex.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

SqliteModelIndex::SqliteModelIndex(
    std::shared_ptr<sqlite3> database_, std::string tableName_,
    std::shared_ptr<boost::bimap<std::string, std::string>> columnMap_,
    std::shared_ptr<boost::bimap<int, int>> columnNumMap_,
    std::shared_ptr<boost::bimap<std::string, int>> columnToNumMap_)
    : database(database_), tableName(tableName_), columnMap(columnMap_),
      columnNumMap(columnNumMap_), columnToNumMap(columnToNumMap_) {
  // Don't cache data yet because sortOrder and filterList have not been set
  // yet. Let the sqliteModel decide when to do full cache.
}

SqliteModelIndex::~SqliteModelIndex() {}

int SqliteModelIndex::setParent(SqliteModelIndex *parentIndex_) {
  parentIndex = parentIndex_;
  return 0;
}

SqliteModelIndex *SqliteModelIndex::getParent() { return parentIndex; }

int SqliteModelIndex::setSortOrder(
    std::shared_ptr<std::list<std::pair<std::string, std::string>>>
        sortOrder_) {
  sortOrder = sortOrder_;
  return 0;
}

int SqliteModelIndex::setFilterList(
    std::shared_ptr<
        std::list<std::tuple<std::string, std::string, std::string>>>
        filterList_) {
  filterList = filterList_;
  return 0;
}

std::string SqliteModelIndex::getParentId() { return parentId; }

int SqliteModelIndex::setParentId(std::string parentId_) {
  parentId = parentId_;
  return 0;
}

std::optional<std::string> SqliteModelIndex::getParentIdBackend() {
  std::string indexId = "";
  std::string *parentId;
  int rc = 0;

  // Get the parentID from the SELECT of the id
  std::string sqlQuery = "SELECT `" + columnMap->left.at("parentId") +
                         "` FROM `" + tableName + "` WHERE `" +
                         columnMap->left.at("id") + "`='" + indexId + "';";

  /* sqlite3_prepare_v2, sqlite3_step, sqlite3_finalize is used
   * instead of sqlite3_exec because it allows more control over the
   * extracted data type. This method also allows progressive handling of the
   * queried data rather and avoids sqlite3_exec callback that will fragment the
   * logic too much.
   */

  // sqlite3 prepare statement
  sqlite3_stmt *stmt = nullptr;
  rc = sqlite3_prepare_v2(database.get(), sqlQuery.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
    return std::optional<std::string>();

  // sqlite3 step loop
  rc = sqlite3_step(stmt);
  while (rc != SQLITE_DONE && rc != SQLITE_OK) {
    int colCount = sqlite3_column_count(stmt);
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      int type = sqlite3_column_type(stmt, colIndex);
      if (type == SQLITE_NULL) {
        break;
      }
      const unsigned char *valChar = sqlite3_column_text(stmt, colIndex);
      parentId = new std::string(reinterpret_cast<const char *>(valChar));
    }
    rc = sqlite3_step(stmt);
  }

  // sqlite3 finalize
  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    return std::optional<std::string>();
  }

  if (!parentId)
    return std::optional<std::string>();

  return *parentId;
}

QVariant SqliteModelIndex::getDataCell(int rowNum, int columnNum) {
#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_getDataCell
  std::cout << BOOST_CURRENT_FUNCTION << " rowNum: " << rowNum
            << ", columnNum: " << columnNum << std::endl;
  std::cout << "id: " << id << ", parentId: " << parentId
            << ", RowNum: " << getRowNum() << ", ColNum: " << getColNum()
            << std::endl;
#endif
  return data.at(rowNum).at(columnNum);
}

int SqliteModelIndex::setDataCell(int rowNum, int columnNum, QVariant value) {
  data[rowNum][columnNum] = value;
  return 0;
}

int SqliteModelIndex::getDataCellBackend(int rowNum, int columnNum) {
  QVariant value;

  std::string columnCodeName = columnToNumMap->right.at(columnNum);
  std::string columnActualName = columnMap->left.at(columnCodeName);
  // Get the fieldValue from the SELECT of the id
  std::string sqlQuery = "SELECT `" + columnActualName + "` FROM `" +
                         tableName + "` LIMIT " + std::to_string(rowNum) +
                         ",1;";

  /* sqlite3_prepare_v2, sqlite3_step, sqlite3_finalize is used
   * instead of sqlite3_exec because it allows more control over the
   * extracted data type. This method also allows progressive handling of the
   * queried data rather and avoids sqlite3_exec callback that will fragment the
   * logic too much.
   */

  // sqlite3 prepare statement
  sqlite3_stmt *stmt = nullptr;
  int rc =
      sqlite3_prepare_v2(database.get(), sqlQuery.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    return -1;
  }

  // sqlite3 step loop
  rc = sqlite3_step(stmt);
  while (rc != SQLITE_DONE && rc != SQLITE_OK) {
    int colCount = sqlite3_column_count(stmt);
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      /* Should the underying value data type be checked?
       * Does treating all value types as text save time?
       */
      const unsigned char *valChar = sqlite3_column_text(stmt, colIndex);
      if (valChar) {
        value = QVariant(QString().fromStdString(
            std::string(reinterpret_cast<const char *>(valChar))));
      }
    }
    rc = sqlite3_step(stmt);
  }

  // sqlite3 finalize
  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    return -1;
  }

  // cache
  data[rowNum][columnNum] = value;

  return 0;
}

int SqliteModelIndex::setDataCellBackend(int rowNum, int columnNum) {
  std::string columnCodeName = columnToNumMap->right.at(columnNum);
  std::string columnActualName = columnMap->left.at(columnCodeName);
  QVariant value = data.at(rowNum).at(columnNum);
  int rc = 0;

  auto childId = getRowId(rowNum);
  if (!childId) {
    return -1;
  }

  // Get the fieldValue from the SELECT of the id
  std::string sqlQuery = "UPDATE`" + tableName + "` SET `" + columnActualName +
                         "` = '" + value.toString().toStdString() +
                         "' WHERE `" + columnMap->left.at("id") + "` = '" +
                         *childId + "';";

  /* sqlite3_prepare_v2, sqlite3_step, sqlite3_finalize is used
   * instead of sqlite3_exec because it allows more control over the
   * extracted data type. This method also allows progressive handling of the
   * queried data rather and avoids sqlite3_exec callback that will fragment the
   * logic too much.
   */

  // sqlite3 prepare statement
  sqlite3_stmt *stmt = nullptr;
  rc = sqlite3_prepare_v2(database.get(), sqlQuery.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    return -1;
  }

  // sqlite3 step loop
  rc = sqlite3_step(stmt);
  while (rc != SQLITE_DONE && rc != SQLITE_OK) {
    int colCount = sqlite3_column_count(stmt);
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      // TODO
      // nothing for updates?
    }
    rc = sqlite3_step(stmt);
  }

  // sqlite3 finalize
  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    return -2;
  }

  return 0;
}

int SqliteModelIndex::getDataBackend() {
  int rc = 0;
  // Get the parentID from the SELECT of the id
  std::string sqlQuery = "SELECT * FROM `" + tableName + "`";
  sqlQuery.append(getWhereSQL(getParentId()));
  sqlQuery.append(getOrderBySQL());

  /* sqlite3_prepare_v2, sqlite3_step, sqlite3_finalize is used
   * instead of sqlite3_exec because it allows more control over the
   * extracted data type. This method also allows progressive handling of the
   * queried data rather and avoids sqlite3_exec callback that will fragment the
   * logic too much.
   */

  // sqlite3 prepare statement
  sqlite3_stmt *stmt = nullptr;
  rc = sqlite3_prepare_v2(database.get(), sqlQuery.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    return -1;
  }

  // wipe current data cache
  data.clear();

  // step through the SQL query and insert data into the data cache
  rc = sqlite3_step(stmt);
  int rowNum = 0;
  while (rc != SQLITE_DONE && rc != SQLITE_OK) {
    int colCount = sqlite3_column_count(stmt);
    std::unordered_map<int, QVariant> rowData;
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      const unsigned char *valChar = sqlite3_column_text(stmt, colIndex);
      rowData.insert({colIndex, reinterpret_cast<const char *>(valChar)});
    }
    data.insert({rowNum, rowData});
    rowNum++;
    rc = sqlite3_step(stmt);
  }

#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_getDataBackend
  std::cout << BOOST_CURRENT_FUNCTION << " data: " << std::endl;
  std::cout << "id: " << id << ", parentId: " << parentId
            << ", RowNum: " << getRowNum() << ", ColNum: " << getColNum()
            << std::endl;
  for (auto data1 : data) {
    std::cout << data1.first << ": ";
    for (auto data2 : data1.second) {
      if (data2.first == 0) {
        std::cout << data2.second.toString().toStdString();
      }
    }
    std::cout << std::endl;
  }
#endif

  // sqlite3 finalize
  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    return -2;
  }
  return 0;
}

std::optional<std::string> SqliteModelIndex::getRowId(int rowNum) {
#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_getRowId
  std::cout << BOOST_CURRENT_FUNCTION << " rowNum: " << rowNum
            << " columnToNumMap:" << std::endl;
  for (auto it : columnToNumMap->left) {
    std::cout << it.first << ": " << it.second << std::endl;
  }
#endif
  std::string columnRealName = columnMap->left.at("id");
  int columnCodeNum = columnToNumMap->left.at(columnRealName);
#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_getRowId
  std::cout << BOOST_CURRENT_FUNCTION << " columnCodeNum: " << columnCodeNum
            << " columnNumMap:" << std::endl;
  for (auto it : columnNumMap->left) {
    std::cout << it.first << ": " << it.second << std::endl;
  }
#endif
  int columnRealNum = columnNumMap->left.at(columnCodeNum);
#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_getRowId
  std::cout << BOOST_CURRENT_FUNCTION << " columnRealNum: " << columnRealNum
            << std::endl;
#endif
  auto rowFindIt = data.find(rowNum);
  if (rowFindIt == data.end()) {
    return std::optional<std::string>();
  }
  QVariant childIdVariant = rowFindIt->second.at(columnRealNum);
  if (!childIdVariant.isValid()) {
    return std::optional<std::string>();
  }
  std::string returnId = childIdVariant.toString().toStdString();
#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_getRowId
  std::cout << BOOST_CURRENT_FUNCTION << " returnId: " << returnId << std::endl;
#endif
  return returnId;
}

std::optional<std::string> SqliteModelIndex::getRowIdBackend(int rowNum) {
  std::string childId;
  // Get the fieldValue from the SELECT of the id
  std::string sqlQuery =
      "SELECT `" + columnMap->left.at("id") + "` FROM `" + tableName + "`";
  sqlQuery.append(getWhereSQL(getParentId()));
  sqlQuery.append(getOrderBySQL());
  sqlQuery.append(" LIMIT " + std::to_string(rowNum) + ",1;");

  /* sqlite3_prepare_v2, sqlite3_step, sqlite3_finalize is used
   * instead of sqlite3_exec because it allows more control over the
   * extracted data type. This method also allows progressive handling of the
   * queried data rather and avoids sqlite3_exec callback that will fragment the
   * logic too much.
   */

  // sqlite3 prepare statement
  sqlite3_stmt *stmt = nullptr;
  int rc =
      sqlite3_prepare_v2(database.get(), sqlQuery.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    return std::optional<std::string>();
  }

  // sqlite3 step loop
  rc = sqlite3_step(stmt);
  while (rc != SQLITE_DONE && rc != SQLITE_OK) {
    int colCount = sqlite3_column_count(stmt);
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      /* Should the underying value data type be checked?
       * Does treating all value types as text save time?
       */
      const unsigned char *valChar = sqlite3_column_text(stmt, colIndex);
      if (valChar) {
        childId = std::string(reinterpret_cast<const char *>(valChar));
      }
    }
    rc = sqlite3_step(stmt);
  }

  // sqlite3 finalize
  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    return std::optional<std::string>();
  }

  return childId;
}

int SqliteModelIndex::rowCountBackend(int rowNum) {
  int rc = 0;
  int rowCountRet = 0;
  std::string whereParentId;

#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_rowCountBackend
  std::cout << BOOST_CURRENT_FUNCTION << " rowNum: " << rowNum << std::endl;
#endif

  // get the parent ID for the where clause
  if (rowNum < 0) {
    whereParentId = parentId;
  } else {
    auto rowIdOpt = getRowId(rowNum);
    if (!rowIdOpt) {
      return 0;
    }
    whereParentId = *rowIdOpt;
  }

  // Count the selected rows
  std::string sqlQuery = "SELECT COUNT(1) FROM `" + tableName + "`";
  sqlQuery.append(getWhereSQL(whereParentId));
  sqlQuery.append(";");

#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_rowCountBackend
  std::cout << BOOST_CURRENT_FUNCTION << " sqlQuery: " << sqlQuery << std::endl;
#endif

  /* sqlite3_prepare_v2, sqlite3_step, sqlite3_finalize is used
   * instead of sqlite3_exec because it allows more control over the
   * extracted data type. This method also allows progressive handling of the
   * queried data rather and avoids sqlite3_exec callback that will fragment the
   * logic too much.
   */

  // sqlite3 prepare statement
  sqlite3_stmt *stmt = nullptr;
  rc = sqlite3_prepare_v2(database.get(), sqlQuery.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
    return 0;

  // sqlite3 step loop
  rc = sqlite3_step(stmt);
  while (rc != SQLITE_DONE && rc != SQLITE_OK) {
    int colCount = sqlite3_column_count(stmt);
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      rowCountRet = sqlite3_column_int(stmt, colIndex);
    }
    rc = sqlite3_step(stmt);
  }

  // sqlite3 finalize
  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    return 0;
  }

  return rowCountRet;
}

std::string SqliteModelIndex::getWhereSQL(const std::string &parentId) const {
  std::string whereClause;
  if (parentId == "*") {
    whereClause.append("`" + columnMap->left.at("parentId") + "` IS NULL ");
  } else {
    whereClause.append("`" + columnMap->left.at("parentId") + "`='" + parentId +
                       "'");
  }

  std::string filterStrings;
  for (auto filterElement : *filterList) {
    auto fieldName = std::get<0>(filterElement);
    auto value = std::get<1>(filterElement);
    auto condition = std::get<2>(filterElement);
    std::string filterString;
    if (condition == "=") {
      filterString = "`" + fieldName + "` = '" + value + "'";
    } else if (condition == "match") {
      filterString = "`" + fieldName + "` like '%" + value + "%'";
    } else if (condition == "auto") {
    }
    filterStrings.append((filterString.empty() ? "" : " AND ") + filterString);
  }

  if (!whereClause.empty()) {
    if (!filterStrings.empty()) {
      whereClause = " AND ";
    }
  }
  whereClause += filterStrings;
  whereClause = whereClause.empty() ? "" : " WHERE " + whereClause;
  return whereClause;
}

std::string SqliteModelIndex::getOrderBySQL() const {
  std::string sortSQLClause;
  for (auto sortElement : *sortOrder) {
    std::string sortField = "`" + sortElement.first + "` " + sortElement.second;
    sortSQLClause.append((sortSQLClause.empty() ? "" : " , ") + sortField);
  }

  sortSQLClause = (sortSQLClause.empty() ? "" : " ORDER BY ") + sortSQLClause;
  return sortSQLClause;
}

int SqliteModelIndex::getRowNum() { return rowIndexNum; }
void SqliteModelIndex::setRowNum(int rowNum_) { rowIndexNum = rowNum_; }
int SqliteModelIndex::getColNum() { return colIndexNum; }
void SqliteModelIndex::setColNum(int colNum_) { colIndexNum = colNum_; }

int SqliteModelIndex::insertIndex(std::string id_,
                                  std::shared_ptr<SqliteModelIndex> indexPtr_) {
  indexMap.insert({id_, indexPtr_});
  return 0;
}

std::shared_ptr<SqliteModelIndex> SqliteModelIndex::findIndex(std::string id_) {
  auto findIt = indexMap.find(id_);
  if (findIt != indexMap.end()) {
    return findIt->second;
  }
  return std::shared_ptr<SqliteModelIndex>();
}

} // namespace widget
} // namespace bookfiler

#endif
