/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief sqlite3 based tree widget.
 */

#ifndef BOOKFILER_LIBRARY_SORT_FILTER_TREE_WIDGET_MODEL_H
#define BOOKFILER_LIBRARY_SORT_FILTER_TREE_WIDGET_MODEL_H

// config
#include "../core/config.hpp"

// C++
#include <iostream>
#include <memory>
#include <queue>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/signals2.hpp>

/* QT 5.13.2
 * License: LGPLv3
 */
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

// Local Project
#include "TreeItem.hpp"
#include "TreeItemDelegate.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

/*
 * @brief Provides a simple tree model to show how to create and use
 * hierarchical models.
 */
class TreeModel : public QAbstractItemModel {
  Q_OBJECT
private:
  std::shared_ptr<sqlite3> database;
  std::string tableName, idColumn, parentColumn, viewRootId;
  std::vector<QVariant> headerList;

public:
  explicit TreeModel(QObject *parent = nullptr);
  ~TreeModel();

  /* Sets the database to use for the model.
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

  /* Essential QAbstractItemModel methods
   *
   * https://doc.qt.io/qt-5/qabstractitemmodel.html
   * When subclassing QAbstractItemModel, at the very least you must implement
   * index(), parent(), rowCount(), columnCount(), and data().
   * These functions are used in all read-only models, and form the basis of
   * editable models.
   */
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  /* Copy and move operations methods
   */
  Qt::DropActions supportedDropActions() const override;
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;
  /* Other QAbstractItemModel methods
   */
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

private:
  TreeItem *rootItem;
};

} // namespace widget
} // namespace bookfiler

#endif // BOOKFILER_LIBRARY_SORT_FILTER_TREE_WIDGET_MODEL_H
