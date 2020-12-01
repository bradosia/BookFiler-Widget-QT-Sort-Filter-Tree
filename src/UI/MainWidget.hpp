/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief sqlite3 based tree widget.
 */

#ifndef BOOKFILER_LIBRARY_SORT_FILTER_TREE_WIDGET_TREE_H
#define BOOKFILER_LIBRARY_SORT_FILTER_TREE_WIDGET_TREE_H

// config
#include "../core/config.hpp"

// C++17
//#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/signals2.hpp>

/* sqlite3 3.33.0
 * License: PublicDomain
 */
#include <sqlite3.h>

/* QT 5.13.2
 * License: LGPLv3
 */
#include <QTreeView>
#include <QWidget>

// Local Project
#include "TreeModel.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

class TreeImpl : public QTreeView {
  Q_OBJECT
private:
  std::shared_ptr<TreeModel> treeModel;
  boost::signals2::signal<void(std::vector<std::string>,
                               std::vector<std::string>,
                               std::vector<std::string>)>
      updateSignal;

public:
  TreeImpl();
  ~TreeImpl();

  /* Sets the database to use for the view widget.
   * @param database mysqlite3 database that this tree widget will be synced
   * with
   * @param tableName the table name
   * @param idColumn The name of the ID column in the database table.
   * for example the sql schema may be: "guid" text(32) NOT NULL
   * * @param idColumn The name of the parent ID column in the database table.
   * for example the sql schema may be: "parent_guid" text(32)
   * @return 0 on success, else error code
   */
  int setData(std::shared_ptr<sqlite3> database, std::string tableName,
              std::string idColumn, std::string parentColumn);
  /* @param id the view root. "*" to view all rows with a NULL parent
   * @return 0 on success, else error code
   */
  int setRoot(std::string id);
  /* Called when the sqlite3 database is updated by another widget, thread, or
   * process. Need to rebuild the entire internal representation of the tree
   * because no hint at which rows were added, updated, or deleted is provided.
   * @return 0 on success, else error code
   */
  int update();
  /* Called when the sqlite3 database is updated by another widget, thread, or
   * process.
   * @param addedIdList a list of id that were added. Only the
   * row id provided was added, not the children, unless the child id is
   * also listed
   * @param updatedIdList a list of id that were updated. Only the
   * row id provided was updated, not the children, unless the child id is
   * also listed
   * @param deletedIdList a list of id that were deleted. Only the
   * row id provided was deleted, not the children, unless the child id is
   * also listed
   * @return 0 on success, else error code
   */
  int updateIdHint(std::vector<std::string> addedIdList,
                   std::vector<std::string> updatedIdList,
                   std::vector<std::string> deletedIdList);
  /* Connect a function that will be signaled when the database is updated by
   * this widget
   * @param addedIdList a list of id that were added. Only the
   * row id provided was added, not the children, unless the child id is
   * also listed
   * @param updatedIdList a list of id that were updated. Only the
   * row id provided was updated, not the children, unless the child id is
   * also listed
   * @param deletedIdList a list of id that were deleted. Only the
   * row id provided was deleted, not the children, unless the child id is
   * also listed
   * @return 0 on success, else error code
   */
  int connectUpdateIdHint(
      std::function<void(std::vector<std::string>, std::vector<std::string>,
                         std::vector<std::string>)>);
  /*
   * @param columnNum The column number that the editor widget will be used for
   * starting from 0
   * @param editorWidgetCreator A function to call that will create a new
   * std::shared_ptr for the item editor widget
   */
  int setItemEditorWidget(
      int columnNum,
      std::function<std::shared_ptr<QWidget>()> editorWidgetCreator);
};

} // namespace widget
} // namespace bookfiler

#endif
// end BOOKFILER_LIBRARY_SORT_FILTER_TREE_WIDGET_TREE_H
