/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @version 1.00
 * @license MIT
 * @brief sqlite3 based tree widget.
 */

#ifndef BOOKFILER_LIBRARY_SORT_FILTER_TABLE_WIDGET_TREE_MODEL_INDEX_H
#define BOOKFILER_LIBRARY_SORT_FILTER_TABLE_WIDGET_TREE_MODEL_INDEX_H

// config
#include "config.hpp"

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
 * @brief A container for items of data supplied by the simple tree model.
 */
class TreeModelIndex {
private:
  TreeModelIndex *parentIndex;

public:
  TreeModelIndex();
  ~TreeModelIndex();

  int setParent(TreeModelIndex *parentIndex);
  int run();
};

} // namespace widget
} // namespace bookfiler

#endif // BOOKFILER_LIBRARY_SORT_FILTER_TABLE_WIDGET_TREE_MODEL_INDEX_H
