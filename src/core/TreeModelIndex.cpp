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
#include "TreeModelIndex.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

TreeModelIndex::TreeModelIndex() {}

TreeModelIndex::~TreeModelIndex() {}

int TreeModelIndex::setParent(TreeModelIndex *parentIndex_) {
  parentIndex = parentIndex_;
  return 0;
}
int TreeModelIndex::run() {

    return 0;
}

} // namespace widget
} // namespace bookfiler
