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

There 

## Sorting and Filtering Implementation

I already wrote the header file for how each method on the derived `QTreeView` should be used [/src/UI/MainWidget.hpp](https://github.com/bradosia/BookFiler-Lib-Sort-Filter-Tree-Widget/blob/main/src/UI/MainWidget.hpp). You must finish implementing the widget using the source files provided in this repository. The example program is used to test the widget. [/src_example/example00/main.cpp](https://github.com/bradosia/BookFiler-Lib-Sort-Filter-Tree-Widget/blob/main/src_example/example00/main.cpp).

To begin development, you should fork this repository and begin implementing the source files [/src/](https://github.com/bradosia/BookFiler-Lib-Sort-Filter-Tree-Widget/tree/main/src). Make a pull request or zip up your forked repository and send it to me when delivering work.

# Cell selection

The user should be able to select cells by clicking on them. They are able to select many cells by holding alt or shift while selecting. When copying and pasting a selection to a text pad, it should paste the tab delimited values of the copied cells.

## Libraries, Compiler, and compatability

The Libraries used are QT, Boost, and sqlite3. Other libraries need to be approved before use.

Program must compile on Windows and Linux. For Windows, use MinGW for compiling. For Linux use GCC. Use cmake as the build scipt. Personally, the IDE I use is QT Creator, but any should work as long as you can build with cmake.

## `QTreeView` column dynamic sizing

The `QTreeView` columns must be dynamically created to be the same as the `sqlite3` table passed to the widget. You must use an SQL query to detect the columns in the table and dynamically create a view for it. 
Example SQL query for getting column names in a table:
```sql
SELECT name FROM pragma_table_info('QTreeTable');
```
`sqlite3` table must have the columns `guid` and `guid_parent` (the name of the column can be different, the columns must have the same purpose) so that the tree view children can be built off this. 

For example, I should be able to connect any database that is set up with columns `guid` and `guid_parent`:
```cpp
sqlite3 *dbPtr = nullptr;
sqlite3_open(":memory:", &dbPtr);
// Using shared pointers when possible
std::shared_ptr<sqlite3> database(nullptr);
database.reset(dbPtr, sqlite3_close);
```

Then I should be able to create the tree widget and set the database data.
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

## Coding Standards
Always use the standard library when possible. Use `std::shared_ptr` and `std::unique_ptr` instead of raw pointers whenever possible, except when creating widgets since the the QT UI will Immediately take control of the widget. use `boost` if some method does not exist in standard library. Finally use `QT5` as the last option. For example use `std::string` instead of `QString` so the code is more portable. Use `QString` only when necessary to pass to a QT function.

Separate all graphical GUI code into the `/src/UI/` directory. Anything with `QT` should be in the `/src/UI/` directory. All logic and non-GUI code goes into the `/src/core/` directory.

* Use camel case
* Use C++17 best coding practices
* Use the standard library when possible

## Deliverables

* Clean and commented code that follows the general design already provided and discussed in the readme.
* Make the example that tests the widget work [/src_example/example00/main.cpp](https://github.com/bradosia/BookFiler-Lib-Sort-Filter-Tree-Widget/blob/main/src_example/example00/main.cpp).
* Should be able to click the header column titles to re-order
* transaction cells are selectable and copyable as tab separated values in the clipboard.
* Double clicking on a field will make it editable

