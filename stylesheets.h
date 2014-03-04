#ifndef STYLESHEETS_H
#define STYLESHEETS_H

#include <QString>

const QString list_style ="selection-background-color:#09b;"
    "selection-color:#000;"
    "color:#888;";

const QString label_style = "QLabel {"
    "font-style:italic;"
    "font-weight:bold;"
    "color:#000; }";

const QString marked_btt_style = "QAbstractButton {"
    "border-radius: 6px;"
    "border: 1px solid #09b;"
    "padding:2;"
    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #09b, stop: 1 #dadbde);"
    "}";

const QString lineedit_style = "QLineEdit {"
    + list_style +
    "border: 1px solid #09b;"
    "color:#000; }";

const QString btt_style = "QAbstractButton {"
    "border: 1px solid #09b;"
    "border-radius: 6px;"
    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);"
    "padding:2;"
    "color:#000; }"
    "QPushButton:pressed {"
    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa); }";

const QString focused_list_style = "QPlainTextEdit, QTableView, QTreeView {"
    + list_style +
    "color:#000;"
    "border: 1px solid #000; }"
    "QStandardItem {margin:5;}";

const QString unfocused_list_style = "QPlainTextEdit, QTableView, QTreeView {"
    + list_style +
    "selection-background-color:#ccc;"
    "background:#ddd;"
    "border: none;}"
    "QStandardItem {margin:5;}";

const QString marked_list_style = "QPlainTextEdit, QTableView, QTreeView {"
    + list_style +
    "background:#efe;"
    "color:#000;border: 1px solid #09b;}";
#endif // STYLESHEETS_H
