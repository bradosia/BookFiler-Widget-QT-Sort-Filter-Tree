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
#include "TreeItemEditor.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

/* The initializer list is required to keep the widget embedded in the cell
 * or else the widget appears as a new window
 */
TreeItemEditor::TreeItemEditor(QWidget *parent)
    : QComboBox(parent) {
  std::cout << "TreeItemEditor\n";
}

}
} // namespace FileTreePane
