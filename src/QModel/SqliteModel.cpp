/*
 * @name BookFiler Widget - Sqlite Model
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief QAbstractItemModel with a sqlite3 backend.
 */

#if DEPENDENCY_SQLITE

/* QT 5.13.2
 * License: LGPLv3
 */
#include <QStringList>

// Local Project
#include "SqliteModel.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

SqliteModel::SqliteModel(
    std::shared_ptr<sqlite3> database_, std::string tableName_,
    std::vector<boost::bimap<std::string, std::string>::value_type> columnMap_,
    QObject *parent)
    : QAbstractItemModel(parent) {
  sortOrder =
      std::make_shared<std::list<std::pair<std::string, std::string>>>();
  filterList = std::make_shared<
      std::list<std::tuple<std::string, std::string, std::string>>>();
  columnMap = std::make_shared<boost::bimap<std::string, std::string>>();
  columnNumMap = std::make_shared<boost::bimap<int, int>>();
  columnToNumMap = std::make_shared<boost::bimap<std::string, int>>();
  setData(database_, tableName_, columnMap_);
  // create root index
  rootIndex = std::make_shared<SqliteModelIndex>(database, tableName, columnMap,
                                                 columnNumMap, columnToNumMap);
  rootIndex->setSortOrder(sortOrder);
  rootIndex->setFilterList(filterList);
  rootIndex->setParentId("*");

  // Perform a full fetch for data and cache
  rootIndex->getDataBackend();
}

SqliteModel::~SqliteModel() {}

/* Custom methods
 *
 *
 *
 *
 */

int SqliteModel::setData(
    std::shared_ptr<sqlite3> database_, std::string tableName_,
    std::vector<boost::bimap<std::string, std::string>::value_type>
        columnMap_) {
  int rc = 0;

  // Set sqlite database information
  database = database_;
  tableName = tableName_;

  // Use default column map if none provided
  if (!columnMap_.empty()) {
    columnMap = std::make_shared<boost::bimap<std::string, std::string>>(
        columnMap_.begin(), columnMap_.end());
  }

  /* Get the table headers */
  std::string sqlQuery =
      "SELECT name FROM pragma_table_info('" + tableName + "');";

  sqlite3_stmt *stmt = nullptr;
  rc = sqlite3_prepare_v2(database.get(), sqlQuery.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
    return rc;

  int rowCount = 0;
  rc = sqlite3_step(stmt);
  while (rc != SQLITE_DONE && rc != SQLITE_OK) {
    int colCount = sqlite3_column_count(stmt);
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      int type = sqlite3_column_type(stmt, colIndex);
      const char *columnName = sqlite3_column_name(stmt, colIndex);
      const unsigned char *valueUChar = sqlite3_column_text(stmt, colIndex);
      std::string valueStr =
          std::string(reinterpret_cast<const char *>(valueUChar));
      headerList.push_back(reinterpret_cast<const char *>(valueUChar));
      columnToNumMap->insert({valueStr, rowCount});
      // use the database column names if no column map provided
      if (columnMap_.empty()) {
        columnMap->insert({valueStr, valueStr});
      }
#if BOOKFILER_QMODEL_SQLITE_MODEL_setData
      std::cout << BOOST_CURRENT_FUNCTION << " row: " << rowCount
                << ", col: " << colCount << ", colName: " << columnName
                << ", valueUChar: " << valueUChar << std::endl;
#endif
    }
    rowCount++;
    rc = sqlite3_step(stmt);
  }

  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    return rc;
  }

  /* Fill out the column num map at the same size as actual columns
   */
  for (int i = 0; i < rowCount; i++) {
    columnNumMap->insert({i, i});
  }

  return 0;
}

int SqliteModel::setRoot(std::string id) {
  viewRootId = std::make_shared<std::string>(id);
  rootIndex->setParentId(*viewRootId);
  return 0;
}

int SqliteModel::setColumnNumMap(
    std::vector<boost::bimap<int, int>::value_type> columnNumMap_) {
  columnNumMap = std::make_shared<boost::bimap<int, int>>(columnNumMap_.begin(),
                                                          columnNumMap_.end());
  return 0;
}

int SqliteModel::connectUpdateIdHint(
    std::function<void(std::vector<std::string>, std::vector<std::string>,
                       std::vector<std::string>)>
        slot) {
  updateSignal.connect(slot);
  return 0;
}

/* Base methods for the view
 *
 *
 *
 */

int SqliteModel::columnCount(const QModelIndex &index) const {
#if BOOKFILER_QMODEL_SQLITE_MODEL_COUNT
  auto startTimePoint = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);
  std::cout << "["
            << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S %X")
            << "] ";
  std::cout << BOOST_CURRENT_FUNCTION << " row: " << index.row()
            << ", col: " << index.column();
  if (index.internalPointer()) {
    SqliteModelIndex *parentIndexPtr =
        static_cast<SqliteModelIndex *>(index.internalPointer());
    std::cout << ", parentId: " << parentIndexPtr->getParentId();
  }
  std::cout << std::endl;
#endif
  return columnNumMap->size();
}

QVariant SqliteModel::data(const QModelIndex &index, int role) const {
#if BOOKFILER_QMODEL_SQLITE_MODEL_DATA
  auto startTimePoint = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);
  std::cout << "["
            << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S %X")
            << "] ";
  std::cout << BOOST_CURRENT_FUNCTION << " index.row: " << index.row()
            << ", index.col: " << index.column() << ", role: " << role;
  if (index.internalPointer()) {
    SqliteModelIndex *parentIndexPtr =
        static_cast<SqliteModelIndex *>(index.internalPointer());
    std::cout << ", parentId: " << parentIndexPtr->getParentId();
  }
  std::cout << std::endl;
#endif

  if (!index.isValid())
    return QVariant();

  SqliteModelIndex *modelIndexPtr =
      static_cast<SqliteModelIndex *>(index.internalPointer());

  QVariant value = modelIndexPtr->getDataCell(index.row(), index.column());

#if BOOKFILER_QMODEL_SQLITE_MODEL_DATA
  std::cout << BOOST_CURRENT_FUNCTION
            << " value: " << value.toString().toStdString() << std::endl;
#endif

  // Normal data display
  if (role == Qt::DisplayRole) {
    return value;
  }
  // Data displayed in the edit box
  else if (role == Qt::EditRole) {
    return value;
  }

  // for all else
  return QVariant();
}

Qt::ItemFlags SqliteModel::flags(const QModelIndex &index) const {
#if BOOKFILER_QMODEL_SQLITE_MODEL_FLAGS
  std::cout << BOOST_CURRENT_FUNCTION << " row: " << index.row()
            << ", col: " << index.column();
  if (index.internalPointer()) {
    SqliteModelIndex *parentIndexPtr =
        static_cast<SqliteModelIndex *>(index.internalPointer());
    std::cout << ", parentId: " << parentIndexPtr->getParentId();
  }
  std::cout << std::endl;
#endif
  if (!index.isValid())
    return Qt::NoItemFlags;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable |
         Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QVariant SqliteModel::headerData(int columnNum, Qt::Orientation orientation,
                                 int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return headerList.at(columnNum);
  }

  return QVariant();
}

QModelIndex SqliteModel::index(int rowNum, int colNum,
                               const QModelIndex &parent) const {
  SqliteModelIndex *parentIndexPtr = nullptr;
  if (parent.internalPointer()) {
    parentIndexPtr = static_cast<SqliteModelIndex *>(parent.internalPointer());
  }
#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX
  auto startTimePoint = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);
  std::cout << "["
            << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S %X")
            << "] ";
  std::cout << BOOST_CURRENT_FUNCTION << " parent.row: " << parent.row()
            << ", parent.col: " << parent.column();
  std::cout << ", row: " << rowNum << ", col: " << colNum;
  if (parentIndexPtr) {
    std::cout << ", parentId: " << parentIndexPtr->getParentId();
  }
  std::cout << std::endl;
#endif

  if (!hasIndex(rowNum, colNum, parent))
    return QModelIndex();

  if (!parentIndexPtr) {
    return createIndex(rowNum, colNum, rootIndex.get());
    //parentIndexPtr = rootIndex.get();
  }

  auto rowIdOpt = parentIndexPtr->getRowId(rowNum);
  if (rowIdOpt) {
#if BOOKFILER_QMODEL_SQLITE_MODEL_INDEX
    std::cout << BOOST_CURRENT_FUNCTION << " rowIdOpt: " << *rowIdOpt
              << std::endl;
#endif
    // Find if index was already cached
    std::shared_ptr<SqliteModelIndex> childIndexPtr =
        parentIndexPtr->findIndex(*rowIdOpt);
    if (childIndexPtr) {
      return createIndex(rowNum, colNum, childIndexPtr.get());
    }

    // Create new index
    childIndexPtr = std::make_shared<SqliteModelIndex>(
        database, tableName, columnMap, columnNumMap, columnToNumMap);

    /* QAbstractItemModel overrided methods are const
     * so we can not store indexes in a map in this object
     * Instead indexes are stored as children
     */
    parentIndexPtr->insertIndex(*rowIdOpt, childIndexPtr);

    childIndexPtr->setSortOrder(sortOrder);
    childIndexPtr->setFilterList(filterList);
    childIndexPtr->setRowNum(rowNum);
    childIndexPtr->setColNum(colNum);
    childIndexPtr->setParentId(*rowIdOpt);
    childIndexPtr->setParent(parentIndexPtr);

    // Perform a full fetch for data and cache
    childIndexPtr->getDataBackend();

    return createIndex(rowNum, colNum, childIndexPtr.get());
  }
  return QModelIndex();
}

QModelIndex SqliteModel::parent(const QModelIndex &index) const {
  SqliteModelIndex *childIndexPtr;
  if (index.internalPointer()) {
    childIndexPtr = static_cast<SqliteModelIndex *>(index.internalPointer());
  }
#if BOOKFILER_QMODEL_SQLITE_MODEL_PARENT
  auto startTimePoint = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);
  std::cout << "["
            << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S %X")
            << "] ";
  std::cout << BOOST_CURRENT_FUNCTION << " parent.row: " << index.row()
            << ", parent.col: " << index.column();
  if (childIndexPtr) {
    std::cout << ", parentId: " << childIndexPtr->getParentId();
  }
  std::cout << std::endl;
#endif

  // never return a model index corresponding to the root item
  if (!childIndexPtr)
    return QModelIndex();

  if (childIndexPtr->getParentId() == "*")
    return QModelIndex();

  SqliteModelIndex *parentIndexPtr = childIndexPtr->getParent();
  if (!parentIndexPtr) {
    return QModelIndex();
  }

  return createIndex(parentIndexPtr->getRowNum(), 0, parentIndexPtr);
}

int SqliteModel::rowCount(const QModelIndex &parent) const {
  int rowCountRet = 0;
  SqliteModelIndex *parentIndexPtr = nullptr;
  if (parent.internalPointer()) {
    parentIndexPtr = static_cast<SqliteModelIndex *>(parent.internalPointer());
  }
#if BOOKFILER_QMODEL_SQLITE_MODEL_ROW_COUNT
  auto startTimePoint = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);
  std::cout << "["
            << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S %X")
            << "] ";
  std::cout << BOOST_CURRENT_FUNCTION << " parent.row: " << parent.row()
            << ", parent.col: " << parent.column();
  if (parentIndexPtr) {
    std::cout << ", parentId: " << parentIndexPtr->getParentId();
  }
  std::cout << std::endl;
#endif

  if (!parentIndexPtr) {
    parentIndexPtr = rootIndex.get();
  }

  rowCountRet = parentIndexPtr->rowCountBackend(parent.row());

#if BOOKFILER_QMODEL_SQLITE_MODEL_ROW_COUNT
  std::cout << BOOST_CURRENT_FUNCTION << " rowCountRet: " << rowCountRet
            << std::endl;
#endif

  return rowCountRet;
}

bool SqliteModel::setData(const QModelIndex &index, const QVariant &value,
                          int role) {
  if (role == Qt::EditRole) {
    if (!index.isValid())
      return false;

    SqliteModelIndex *modelIndexPtr =
        static_cast<SqliteModelIndex *>(index.internalPointer());

    int rc = modelIndexPtr->setDataCell(index.row(), index.column(), value);
    if (rc != 0) {
      return false;
    }
  }
  return true;
}

Qt::DropActions SqliteModel::supportedDropActions() const {
  return Qt::CopyAction | Qt::MoveAction;
}

bool SqliteModel::removeRows(int row, int count, const QModelIndex &parent) {
  // TODO
  return true;
}

int SqliteModel::setSort(
    std::list<std::pair<std::string, std::string>> sortOrderList) {
  // Push each sort field to front and remove duplicates
  for (auto sortField : sortOrderList) {
    for (std::list<std::pair<std::string, std::string>>::iterator it =
             sortOrder->begin();
         it != sortOrder->end(); ++it) {
      // delete pairs with the same column code name
      if (it->first == sortField.first) {
        sortOrder->erase(it);
      }
    }
    sortOrder->push_front(sortField);
  }
  return 0;
}

int SqliteModel::setFilter(
    std::list<std::tuple<std::string, std::string, std::string>> filterList_) {
  filterList = std::make_shared<
      std::list<std::tuple<std::string, std::string, std::string>>>(
      filterList_);
  return 0;
}

void SqliteModel::sort(int columnActualNum, Qt::SortOrder order) {
#if BOOKFILER_QMODEL_SQLITE_MODEL_SORT
  auto startTimePoint = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);
  std::cout << "["
            << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S %X")
            << "] ";
  std::cout << BOOST_CURRENT_FUNCTION << " columnActualNum: " << columnActualNum
            << ", Qt::SortOrder: " << order << std::endl;
#endif

  std::pair<std::string, std::string> sortField;
  // get the default column num from the actual column num
  auto findIt = columnNumMap->right.find(columnActualNum);
  if (findIt == columnNumMap->right.end()) {
    return;
  }
  int defaultColumnNum = findIt->second;
  // get the code column name from the default column num
  auto findIt2 = columnToNumMap->right.find(defaultColumnNum);
  if (findIt2 == columnToNumMap->right.end()) {
    return;
  }
  std::string columnCodeName = findIt2->second;
  sortField.first = columnCodeName;
  switch (order) {
  case Qt::SortOrder::AscendingOrder:
    sortField.second = "ASC";
    break;
  case Qt::SortOrder::DescendingOrder:
    sortField.second = "DESC";
    break;
  }
#if BOOKFILER_QMODEL_SQLITE_MODEL_SORT
  std::cout << BOOST_CURRENT_FUNCTION << " columnCodeName: " << columnCodeName
            << ", Order: " << sortField.second << std::endl;
#endif
  std::list<std::pair<std::string, std::string>> sortOrderList{sortField};
  setSort(sortOrderList);

  // Perform a full fetch for data and cache
  rootIndex->getDataBackend();

  emit layoutChanged();
}

} // namespace widget
} // namespace bookfiler

#endif
