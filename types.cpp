#include "types.h"
#include "osinterface.h"
#include "mytreeview.h"

#include <sstream>
#include <map>
#include <string>
#include <iostream>
#include <QTableView>
#include <QTreeView>
#include <QPushButton>
#include <QObject>
#include <QLineEdit>
#include <QFocusEvent>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QApplication>

AbstractView::AbstractView(std::string p, std::string pat): path(p), pattern(pat){
  osi = new OSInterface();
  try{
    osi->getDirInfo(path, pattern);
  }catch(OSException *ex){
    ex->process();
  }
}

/*MyTableView::MyTableView(std::string p, std::string pat): AbstractView(p, pat){
  content = new QTableView();
  ((QTableView*)content)->setShowGrid(false);
  ((QTableView*)content)->horizontalHeader()->hide();
  QStandardItemModel *model = new QStandardItemModel(0, 3);
  QStandardItem *item;
  int row = 0;
  for(auto &e : osi.dirs){
      std::stringstream ss;
      std::string type;
      item = new QStandardItem(QString::fromStdString(e.second->name));
      item->setEditable(false);
      model->setItem(row, 0, item);
      item = new QStandardItem(QString::fromStdString(e.second->type_name));
      item->setEditable(false);
      model->setItem(row, 1, item);
      ss << e.second->byte_size;
      item = new QStandardItem(QString::fromStdString(ss.str()));
      item->setEditable(false);
      model->setItem(row, 2, item);
      ++row;
    }
  ((QTableView*)content)->setModel(model);
  ((QTableView*)content)->setColumnWidth(0, 300);
}*/

void MTree::buildTree(std::string root, QStandardItem *it){
  OSInterface os;
  try{
    os.getDirInfo(root, pattern);
  }catch(OSException *e){
    e->process();
    return;
  }
  if(root[root.size() - 1] == '/')
    root = root.substr(0,root.size() - 1);
  QStandardItem *item;
  QList<QStandardItem*> ql;
  for(auto &e : os.dirs){
      if(e.second->name.empty()) continue;
      if((e.second->name == ".") || (e.second->name == "..")) continue;
      std::stringstream ss;
      item = new QStandardItem(QString::fromStdString(e.second->name));
      item->setEditable(false);
      if((e.second->type == e.second->DIR) && (e.second->name != ".") && (e.second->name != "..")){
          buildTree(root + "/" + e.second->name, item);
        }
      ql.push_back(item);
      item = new QStandardItem(QString::fromStdString(e.second->type_name));
      item->setEditable(false);
      ql.push_back(item);
      ss << e.second->byte_size;
      item = new QStandardItem(QString::fromStdString(ss.str()));
      item->setEditable(false);
      ql.push_back(item);
      it->appendRow(ql);
      ql.clear();
    }
}

AbstractView *MTree::clone(){
  if(path.empty()) return nullptr;
  MTree *tmp = new MTree(path, pattern, recursive);
  return tmp;
}

void MTree::init(std::string p, std::string pat, bool rec){
  recursive = rec;
  osi->dirs.clear();
  osi->getDirInfo(p, pat);
  content->path = p;
  QStandardItemModel *model = new QStandardItemModel(0, 3);
  QStandardItem *item;
  QList<QStandardItem*> qlis;
  int row = 0;
  if(p[p.size() - 1] == '/')
    p = p.substr(0,p.size() - 1);
  for(auto &e : osi->dirs){
      if((e.second->name == ".") || (e.second->name == "..")) continue;
      qlis.clear();
      std::stringstream ss;
      std::string type;
      item = new QStandardItem(QString::fromStdString(e.second->name));
      item->setEditable(false);
      if(rec){
          if(e.second->type == e.second->DIR){
              buildTree(p + "/" + e.second->name, item);
            }
        }
      model->setItem(row, 0, item);
      item = new QStandardItem(QString::fromStdString(e.second->type_name));
      item->setEditable(false);
      model->setItem(row, 1, item);
      ss << e.second->byte_size;
      item = new QStandardItem(QString::fromStdString(ss.str()));
      item->setEditable(false);
      model->setItem(row, 2, item);
      ++row;
    }
  QHeaderView *hv = new QHeaderView(Qt::Horizontal);
  ((QTreeView*)content)->setModel(model);
  ((QTreeView*)content)->setSelectionBehavior(QTreeView::SelectRows);
  ((QTreeView*)content)->setHeader(hv);
  ((QTreeView*)content)->sortByColumn(1,Qt::AscendingOrder);
  ((QTreeView*)content)->setColumnWidth(0, 300);
}

MTree::MTree(std::string p, std::string pat, bool rec): AbstractView(p, pat), recursive(rec){
  content = new MyTreeView();
  init(p, pat, rec);
}
