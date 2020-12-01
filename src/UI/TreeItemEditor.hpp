/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief sqlite3 based tree widget.
 */

#ifndef BOOKFILER_LIBRARY_SORT_FILTER_TABLE_WIDGET_TREE_ITEM_EDITOR_H
#define BOOKFILER_LIBRARY_SORT_FILTER_TABLE_WIDGET_TREE_ITEM_EDITOR_H

// config
#include "../core/config.hpp"

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

class TreeItemEditor : public QComboBox {
  Q_OBJECT
signals:
  void editingFinished();

public:
  TreeItemEditor(QWidget *parent = nullptr);
  ~TreeItemEditor(){};
};

} // namespace widget
} // namespace bookfiler

#endif // BOOKFILER_LIBRARY_SORT_FILTER_TABLE_WIDGET_TREE_ITEM_EDITOR_H
