/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief sqlite3 based tree widget.
 */

#ifndef BOOKFILER_WIDGET_SORT_FILTER_TREE_TREE_VIEW_H
#define BOOKFILER_WIDGET_SORT_FILTER_TREE_TREE_VIEW_H

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
#include "TreeItemDelegate.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

class TreeView : public QTreeView {
  Q_OBJECT
private:
  std::shared_ptr<TreeItemDelegate> treeItemDelegatePtr;

public:
  TreeView();
  ~TreeView();

  /* Called when the sqlite3 database is updated by another widget, thread, or
   * process. Need to rebuild the entire internal representation of the tree
   * because no hint at which rows were added, updated, or deleted is provided.
   * Internally update(const QModelIndex &index) is called to update the root
   * index and all child indexes
   * @return 0 on success, else error code
   */
  int update();

  /*
   * @param columnNum The column number that the editor widget will be used for
   * starting from 0
   * @param editorWidgetCreator A function to call that will create a new
   * std::shared_ptr for the item editor widget
   */
  int setItemEditorWidget(
      int columnNum,
      std::function<std::shared_ptr<QWidget>()> editorWidgetCreator);


  public slots:
      void expand(const QModelIndex &index);
};

} // namespace widget
} // namespace bookfiler

#endif
// end BOOKFILER_WIDGET_SORT_FILTER_TREE_TREE_VIEW_H
