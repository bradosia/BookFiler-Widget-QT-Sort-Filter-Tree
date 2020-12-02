# Development Summary

I need a QT5 widget written in C++. Only the widget implementation needs to be written, not an app. There are two main components to this widget:
* The view, which inherits from `QTreeView`. This part needs to be designed to look like the reference image.
* The model, which inherits from `QAbstractItemModel`. This widget uses `sqlite3`. 
I already wrote some of the code to help with the implementation.

# The Tree View component

The Tree Model using the following columns should look like the refrence image below.

`sqlite3` table columns: guid, parent_guid, Subject, Important, Attachment, From, Date, Size. Hidden columns: guid, parent_guid.

![Tree View Design](/reference/tree-view-design-1.png?raw=true)

The header file written for the `TreeView` documents how the methods should work: [/src/UI/TreeView.hpp](/src/UI/TreeView.hpp)

# The Tree Model component

The Tree Model is similar to `QSqlTableModel`, except it is specialized to use `sqlite3`. The reason `sqlite3` is used and not `QSqlDatabase` is because other modules this widget is used with won't be linked against QT, so a `sqlite3` database is passed between modules instead.

The header file written for the `TreeModel` documents how the methods should work: [/src/UI/TreeModel.hpp](/src/UI/TreeModel.hpp)

There is already a partial implementation for the tree model to give an example of how the column names are pulled out of a `sqlite3` database table. Database query results need to somehow be cached in the model. One design idea may be to cache queries in a custom table index that will be used with the `QModelIndex`. A mostly empty file for this is at [/src/core/TreeIModelIndex.hpp](/src/core/TreeIModelIndex.hpp).

## Sorting and Filtering Implementation

Most of the time, in memory `sqlite3` databases will be used so using sql queries to refetch the `sqlite3` database table should be quick. Similar to `QSqlTableModel`, the `TreeModel` also has `setSort` and `setFilter` methods, but the usage is different. See the header file [/src/UI/TreeModel.hpp](/src/UI/TreeModel.hpp) for my notes on these methods.

## How is the tree represented in the `sqlite3` table?

The `sqlite3` table must have the columns `guid` and `guid_parent` (the name of the column can be different, the columns must have the same purpose) so that the tree view children can be built off this. The `guid_parent` refers to the parent `guid` row. This relationship establishes a parent-child relationship to make a tree.

# Cell selection

The user should be able to select cells by clicking on them. They are able to select many cells by holding alt or shift while selecting. When copying and pasting a selection to a text pad, it should paste the tab delimited values of the copied cells.

# Libraries, Compiler, and compatability

The Libraries used are QT, Boost, and sqlite3. Other libraries need to be approved before use.

Program must compile on Windows and Linux. For Windows, use MinGW for compiling. For Linux use GCC. Use cmake as the build scipt. Personally, the IDE I use is QT Creator, but any should work as long as you can build with cmake.

# Simple Usage

Here is a simple example to show how a `sqlite3` database is opened and passed to the `TreeModel`
```cpp
sqlite3 *dbPtr = nullptr;
sqlite3_open("data.db", &dbPtr);
// Using shared pointers when possible
std::shared_ptr<sqlite3> database(nullptr);
database.reset(dbPtr, sqlite3_close);
```

This is a simple example to show the widget in a small program. See how the view and the model are decoupled. 
```cpp
  // Create View and Model
  std::shared_ptr<bookfiler::widget::TreeView> treeViewPtr =
      std::make_shared<bookfiler::widget::TreeView>();
  std::shared_ptr<bookfiler::widget::TreeModel> treeModelPtr =
      std::make_shared<bookfiler::widget::TreeModel>();
  qtMainWindow.setCentralWidget(treeViewPtr.get());
  qtMainWindow.show();

  // Setup Model
  treeModelPtr->setData(database, "testTable", "guid", "parent_guid");
  treeModelPtr->setRoot("*");

  // Setup View
  treeViewPtr->setModel(treeModelPtr.get());
  treeViewPtr->update();
```

See [/src_example/example00/main.cpp](/src_example/example00/main.cpp) for the full code to test your widget after implementing it.

# Collaboration

To begin development, you should fork this repository and begin implementing the source files at [/src/](/src). Make a pull request or zip up your forked repository and send it to me when delivering work.

## Coding Standards
Always use the standard library when possible. Use `std::shared_ptr` and `std::unique_ptr` instead of raw pointers whenever possible, except when creating widgets since the the QT UI will Immediately take control of the widget. use `boost` if some method does not exist in standard library. Finally use `QT5` as the last option. For example use `std::string` instead of `QString` so the code is more portable. Use `QString` only when necessary to pass to a QT function.

Separate all graphical GUI code into the `/src/UI/` directory. Anything with `QT` should be in the `/src/UI/` directory. All logic and non-GUI code goes into the `/src/core/` directory.

* Use camel case
* Use C++17 best coding practices
* Use the standard library when possible

## Deliverables

* Clean and commented code that follows the general design already provided and discussed in the readme.
* Make the example that tests the widget work [/src_example/example00/main.cpp](/src_example/example00/main.cpp).
* Should be able to click the header column titles to re-order
* transaction cells are selectable and copyable as tab separated values in the clipboard.
* Double clicking on a field will make it editable

