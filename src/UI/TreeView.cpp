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
#include <QKeyEvent>
//#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

TreeView::TreeView() {
    setObjectName("BookFiler Tree Widget");
    setSelectionMode(MultiSelection);
    setSelectionBehavior(SelectRows);
};
TreeView::~TreeView(){};

int TreeView::update() {
    QAbstractItemModel *m = this->model();
    setModel(nullptr);
    setModel(m);
    return 0;
}

int TreeView::setItemEditorWidget(
    int columnNum,
    std::function<std::shared_ptr<QWidget>()> editorWidgetCreator) {
  return 0;
}

void TreeView::expand(const QModelIndex &index) {
    QTreeView::expand(index);
}

void TreeView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        QItemSelectionModel * selection = selectionModel();
        QModelIndexList indexes = selection->selectedIndexes();

        if(indexes.size() < 1)
            return;

        std::sort(indexes.begin(), indexes.end());

        QModelIndex previous = indexes.first();
        indexes.removeFirst();
        QString selected_text;
        for (auto current: indexes)
        {
            QVariant data = model()->data(previous);
            QString text = data.toString();
            selected_text.append(text);
            text.replace("\n","<br>");

            if (current.row() != previous.row()) {
                selected_text.append(QLatin1Char('\n'));
            } else {
                selected_text.append(QLatin1Char('\t'));
            }
            previous = current;
        }

        // add last element
        //selected_text.append(model()->data(current).toString());

        selected_text.append(QLatin1Char('\n'));
        QApplication::clipboard()->setText(selected_text);
    }
}

} // namespace widget
} // namespace bookfiler
