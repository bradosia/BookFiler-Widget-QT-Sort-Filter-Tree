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
#include "MainWidget.hpp"

/*
 * bookfiler - widget
 */
namespace bookfiler {
namespace widget {

TreeImpl::TreeImpl() { this->setObjectName("BookFiler Tree Widget"); };
TreeImpl::~TreeImpl(){};

int TreeImpl::setData(std::shared_ptr<sqlite3> database_,
                      std::string tableName_, std::string idColumn_,
                      std::string parentColumn_) {
  treeModel = std::make_shared<TreeModel>();
  treeModel->setData(database_, tableName_, idColumn_, parentColumn_);
  this->setModel(treeModel.get());
  return 0;
}

int TreeImpl::setRoot(std::string id) {
  return treeModel->setRoot(id);
}
int TreeImpl::update() { return 0; }
int TreeImpl::updateIdHint(std::vector<std::string> addedIdList,
                           std::vector<std::string> updatedIdList,
                           std::vector<std::string> deletedIdList) {
  return 0;
}
int TreeImpl::connectUpdateIdHint(
    std::function<void(std::vector<std::string>, std::vector<std::string>,
                       std::vector<std::string>)>
        slot) {
  updateSignal.connect(slot);
  return 0;
}
int TreeImpl::setItemEditorWidget(
    int columnNum,
    std::function<std::shared_ptr<QWidget>()> editorWidgetCreator) {
  return 0;
}

} // namespace widget
} // namespace bookfiler
