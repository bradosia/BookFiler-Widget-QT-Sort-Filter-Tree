/*
 * @name BookFiler Widget - Sqlite Model
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief QAbstractItemModel with a sqlite3 backend.
 */

#if DEPENDENCY_SQLITE
#ifndef BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_H
#define BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_H

// config
#include "../core/config.hpp"

// C++
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/bimap.hpp>

/* sqlite3 3.33.0
 * License: PublicDomain
 */
#include <sqlite3.h>

/* QT 5.13.2
 * License: LGPLv3
 */
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QVariant>
#include <QVector>

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

/*
 * @brief An index meant to be used with SqliteModel. Implements some caching
 * for queries to be used for the virtual data() function  from
 * QAbstractItemModel
 */
class SqliteModelIndex {
private:
  std::shared_ptr<sqlite3> database;
  SqliteModelIndex *parentIndex = nullptr;
  std::string parentId, tableName;
  int rowIndexNum, colIndexNum;
  std::unordered_map<int, std::unordered_map<int, QVariant>> data;
  /* A list of updated ID
   * This updated data is temporary and must be committed to the database
   */
  std::vector<std::string> updateIdList;

  std::shared_ptr<std::list<std::pair<std::string, std::string>>> sortOrder;
  std::shared_ptr<std::list<std::tuple<std::string, std::string, std::string>>>
      filterList;

  std::string getWhereSQL(const std::string &parentId) const;
  std::string getOrderBySQL() const;

  /* map the code column name to the sqlite3 column name
   */
  std::shared_ptr<boost::bimap<std::string, std::string>> columnMap;
  /* map the default column position to the actual column position
   */
  std::shared_ptr<boost::bimap<int, int>> columnNumMap;
  /* map the code column name to the default column position
   */
  std::shared_ptr<boost::bimap<std::string, int>> columnToNumMap;

  /* index map maps parentId->index
   */
  std::unordered_map<std::string, std::shared_ptr<SqliteModelIndex>> indexMap;
  /* index child map maps id->rowNum->childId
   * column is not used for indexes
   */
  std::map<std::string, std::map<int, std::string>> indexChildMap;

public:
  SqliteModelIndex(
      std::shared_ptr<sqlite3> database_, std::string tableName_,
      std::shared_ptr<boost::bimap<std::string, std::string>> columnMap_,
      std::shared_ptr<boost::bimap<int, int>> columnNumMap_,
      std::shared_ptr<boost::bimap<std::string, int>> columnToNumMap_);
  ~SqliteModelIndex();

  int setParent(SqliteModelIndex *parentIndex);
  SqliteModelIndex *getParent();
  int setSortOrder(
      std::shared_ptr<std::list<std::pair<std::string, std::string>>>
          sortOrder);
  int setFilterList(
      std::shared_ptr<
          std::list<std::tuple<std::string, std::string, std::string>>>
          filterList);
  /* returns the parent ID
   * @return parentId
   */
  std::string getParentId();
  /* set the parent ID
   * @return 0 on sucess, else error code
   */
  int setParentId(std::string parentId);
  /* returns the parent ID
   * @return parentId
   */
  std::optional<std::string> getParentIdBackend();
  /* get data from the cache
   */
  QVariant getDataCell(int rowNum, int columnNum);
  /* set data to the cache
   * @return 0 on sucess, else error code
   */
  int setDataCell(int rowNum, int columnNum, QVariant value);
  /* get data from the database to the cache
   * @return 0 on sucess, else error code
   */
  int getDataCellBackend(int rowNum, int columnNum);
  /* set data to the database from the cache
   * @return 0 on sucess, else error code
   */
  int setDataCellBackend(int rowNum, int columnNum);
  /* get all data from the backend and update cache
   * wipes all temporary data
   * @return 0 on sucess, else error code
   */
  int getDataBackend();
  /* row number to ID using cache
   */
  std::optional<std::string> getRowId(int rowNum);
  /* row number to ID using sqlite3
   */
  std::optional<std::string> getRowIdBackend(int rowNum);
  /* get row count using sqlite3
   * @return 0 on sucess, else error code
   */
  int rowCountBackend(int rowNum = -1);
  /* getters and setters for the row and column number that the index exists at
   */
  int getRowNum();
  void setRowNum(int rowNum_);
  int getColNum();
  void setColNum(int colNum_);
  /* insert the index into the index cache
   */
  int insertIndex(std::string id_, std::shared_ptr<SqliteModelIndex> indexPtr_);
  /* find index in the index cache
   */
  std::shared_ptr<SqliteModelIndex> findIndex(std::string id_);
};

} // namespace widget
} // namespace bookfiler

#endif // BOOKFILER_QMODEL_SQLITE_MODEL_INDEX_H
#endif
