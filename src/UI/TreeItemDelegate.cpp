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
#include "TreeItemDelegate.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

TreeItemDelegate::TreeItemDelegate() {
  std::cout << "TreeItemDelegate\n";
}

QWidget *
TreeItemDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const {
  std::cout << "createEditor() index.row=" << index.row()
            << " index.column=" << index.column() << "\n";
  if (index.data().canConvert<QString>()) {
    TreeItemEditor *editor = new TreeItemEditor(parent);
    const int row = index.row();
    editor->addItem(QString("one in row %1").arg(row));
    editor->addItem(QString("two in row %1").arg(row));
    editor->addItem(QString("three in row %1").arg(row));
    connect(editor, &TreeItemEditor::editingFinished, this,
            &TreeItemDelegate::commitAndCloseEditor);
    return editor;
  }
  return QStyledItemDelegate::createEditor(parent, option, index);
}

void TreeItemDelegate::setEditorData(QWidget *editor,
                                           const QModelIndex &index) const {
  TreeItemEditor *cb = qobject_cast<TreeItemEditor *>(editor);
  // get the index of the text in the combobox that matches the current value of
  // the item
  const QString currentText = index.data(Qt::EditRole).toString();
  const int cbIndex = cb->findText(currentText);
  // if it is valid, adjust the combobox
  if (cbIndex >= 0)
    cb->setCurrentIndex(cbIndex);
}

void TreeItemDelegate::setModelData(QWidget *editor,
                                          QAbstractItemModel *model,
                                          const QModelIndex &index) const {
  TreeItemEditor *cb = qobject_cast<TreeItemEditor *>(editor);
  model->setData(index, cb->currentText(), Qt::EditRole);
}

void TreeItemDelegate::commitAndCloseEditor() {
  TreeItemEditor *editor = qobject_cast<TreeItemEditor *>(sender());
  emit commitData(editor);
  emit closeEditor(editor);
}

}
} // namespace FileTreePane
