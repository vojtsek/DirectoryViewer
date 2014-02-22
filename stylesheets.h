#ifndef STYLESHEETS_H
#define STYLESHEETS_H

#include <QString>

const QString list_style = "selection-background-color:#09b;"
    "selection-color:#000;"
    "color:#999;";

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

const QString focused_list_style = "QTreeView {"
    + list_style +
    "color:#000;"
    "border: 1px solid #000; }";

const QString unfocused_list_style = "QTreeView {"
    + list_style +
    "selection-background-color:#fff;"
    "border: none;}";

const QString marked_list_style = "QTreeView {"
    + list_style +
    "selection-background-color:#fff;"
    "background:#efe;"
    "color:#000;border: 1px solid #09b;}";
#endif // STYLESHEETS_H
