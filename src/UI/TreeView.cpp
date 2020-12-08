/*
 * @name BookFiler Library - Sort Filter Tree Widget
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief Super fast tree sorting and filtering tree widget.
 */

/*
 * piwebapi-ucdavis 1.0
 */
#include "TreeView.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

TreeView::TreeView() { this->setObjectName("BookFiler Tree Widget"); };
TreeView::~TreeView(){};

int TreeView::update() {
    QTreeView::repaint();
    return 0;
}

int TreeView::setItemEditorWidget(
    int columnNum,
    std::function<std::shared_ptr<QWidget>()> editorWidgetCreator) {
  return 0;
}

void TreeView::expand(const QModelIndex &index)
{
    QTreeView::expand(index);
}

} // namespace widget
} // namespace bookfiler
