# Development Summary

I need a QT5 widget written in C++. Only the widget implementation needs to be written, not an app. There are two main components to this widget:
* The view, which inherits from `QTreeView`. This part needs to be designed to look like the reference image.
* The model, which inherits from `QAbstractItemModel` with a sqlite3 backend similar to `QSqlTableModel`
The implementation requires a sqlite3 database backend. The header files are already written so the implementation should be filled in. See the photos for how the tree view should look.

# SqliteModel component

The SqliteModel is a `QAbstractItemModel` with a sqlite3 backend similar to `QSqlTableModel`. This is the data model used with the view widget. It is specialized to use `sqlite3`.

## Why is `sqlite3` used and not `QSqlDatabase`?

The reason `sqlite3` is used and not `QSqlDatabase` is because other modules this widget is used with won't be linked against QT, so a `sqlite3` database is passed between modules instead. `QSqlDatabase` is not used because other modules don't link to QT, so using a sqlite3 database is more portable and a smaller dependency.

## How to I start implementing the `SqliteModel` component?

* The header file written for the `SqliteModel`: [/src/QModel/SqliteModel.hpp](/src/QModel/SqliteModel.hpp)
* The header file written for the `SqliteModelIndex`: [/src/QModel/SqliteModelIndex.hpp](/src/QModel/SqliteModelIndex.hpp)

`SqliteModel` is mostly implemented. The model indexes cells with a common `parentId` with a `SqliteModelIndex`. `SqliteModelIndex` provides caching for data retrieved from the sqlite3 database.

Needed fixes and changes:
* There is an issue with indexes where the children data won't show.
* Index cached data that is rarely used needs to be freed up

## Sorting Implementation

The user should be able to toggle the sort direction, by clicking on the column headers. Sorting is done without a proxy, so the cache must be refetched with the new order.

See `sort` and `setSort` methods in the header file [/src/QModel/SqliteModel.hpp](/src/QModel/SqliteModel.hpp) for my notes.

## Filtering Implementation

There must be a input field to filter columns. It will look like this:

![Tree View Design](/reference/tree-view-design-2.png?raw=true)

## How is the tree represented in the `sqlite3` table?

The `sqlite3` table must have the columns `id` and `parentId` (the name of the column can be different, but the columns must have the same purpose) so that the tree view children can be built off this. The `parentId` refers to the parent `id` row. This relationship establishes a parent-child relationship to make a tree.

The column names may be mapped differently by passing all mapped column names to the `SqliteModel` constructor. For example, the `id` column is actully named `guid` and the `parentId` column is actully named `guid_parent`:

```cpp
bookfiler::widget::SqliteModel *sqlModelPtr =
      new bookfiler::widget::SqliteModel(database, "tableName", {
      {"id", "guid"},
      {"parentId", "guid_parent"}});
```

# The Tree View component

The Tree Model using the following columns should look like the refrence image below.

`sqlite3` table columns: guid, parent_guid, Subject, Important, Attachment, From, Date, Size. Hidden columns: guid, parent_guid.

![Tree View Design](/reference/tree-view-design-1.png?raw=true)

The header file written for the `TreeView` documents how the methods should work: [/src/UI/TreeView.hpp](/src/UI/TreeView.hpp)

# Cell selection

The user should be able to select cells by clicking on them. They are able to select many cells by holding alt or shift while selecting. When copying and pasting a selection to a text pad, it should paste the tab delimited values of the copied cells. The selection should be able to be pasted into an excel sheet and retain the cell order.

# Usage Instructions

Example00 shows how the sqlite3 `QAbstractItemModel` is created and used. 
* [/src_example/example00/main.cpp](/src_example/example00/main.cpp)

# Libraries, Compiler, and compatability

The Libraries used are QT, Boost, and sqlite3. Other libraries need to be approved before use.

Program must compile on Windows and Linux. For Windows, use MinGW for compiling. For Linux use GCC. Use cmake as the build scipt. Personally, the IDE I use is QT Creator, but any should work as long as you can build with cmake.

# Collaboration

To begin development, you should fork this repository and begin implementing the source files at [/src/](/src). Make a pull request or zip up your forked repository and send it to me when delivering work.

## Coding Standards
Always use the standard library when possible. Use `std::shared_ptr` and `std::unique_ptr` instead of raw pointers whenever possible, except when creating widgets since the the QT UI will Immediately take control of the widget. use `boost` if some method does not exist in standard library. Finally use `QT5` as the last option. For example use `std::string` instead of `QString` so the code is more portable. Use `QString` only when necessary to pass to a QT function.

Separate all graphical GUI code into the `/src/UI/` directory. Anything with `QT` should be in the `/src/UI/` directory. All logic and non-GUI code goes into the `/src/core/` directory.

* Use camel case
* Use C++17 best coding practices
* Use the standard library when possible
* Use .hpp files for C++ headers and .cpp files for C++ source
* Use namespace bookfiler::widget for all `QWidget` and derivatives.

## Deliverables

* Clean and commented code that follows the general design already provided and discussed in the readme.
* Make the example that tests the widget works [/src_example/example00/main.cpp](/src_example/example00/main.cpp).
* Make sure you don't break existing functionality
* Fix the issue with indexes where the children data won't show.
* Make it so index cached data that is rarely used is freed up
* Add in user input filter boxes under the column headers
* Double clicking on a field will make it editable

