/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief sqlite3 based tree widget.
 */

 /*
    @brief A container for items of data supplied by the simple tree model.
*/

// C++
#include <iostream>

// Local Project
#include "TreeItem.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

TreeItem::TreeItem(const QVector<QVariant> &data,
                               TreeItem *parent)
    : m_itemData(data), m_parentItem(parent) {}

TreeItem::~TreeItem() { qDeleteAll(m_childItems); }

void TreeItem::appendChild(TreeItem *item) {
  m_childItems.append(item);
}

void TreeItem::removeChild(int row, int count) {
  m_childItems.remove(row, count);
}

TreeItem *TreeItem::child(int row) {
  if (row < 0 || row >= m_childItems.size())
    return nullptr;
  return m_childItems.at(row);
}

int TreeItem::childCount() const { return m_childItems.count(); }

int TreeItem::columnCount() const { return m_itemData.count(); }

QVariant TreeItem::data(int column) const {
  if (column < 0 || column >= m_itemData.size())
    return QVariant();
  return m_itemData.at(column);
}
void TreeItem::setData(int column, QVariant value) {
  m_itemData[column] = value;
}
TreeItem *TreeItem::parentItem() { return m_parentItem; }

int TreeItem::row() const {
  if (m_parentItem)
    return m_parentItem->m_childItems.indexOf(
        const_cast<TreeItem *>(this));

  return 0;
}

}
} // namespace FileTreePane
