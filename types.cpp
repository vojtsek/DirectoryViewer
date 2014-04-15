#include "types.h"
#include "osinterface.h"
#include "mytreeview.h"
#include "myviewer.h"
#include "myiconview.h"

#include <sstream>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <QLabel>
#include <QTableView>
#include <QTreeView>
#include <QPushButton>
#include <QObject>
#include <QLineEdit>
#include <QFocusEvent>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextEdit>
#include <QPersistentModelIndex>

std::shared_ptr<Data> Data::inst = nullptr;
std::shared_ptr<Data> Data::getInstance(){
    if(!inst)
        inst = std::shared_ptr<Data>(new Data);
    return inst;
}
