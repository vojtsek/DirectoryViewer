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
#include <QPersistentModelIndex>

AbstractView::AbstractView(std::string p, std::string pat): path(p), pattern(pat){
  osi = new OSInterface();
  try{
    osi->getDirInfo(path, pattern);
  }catch(OSException *ex){
    ex->process();
  }
}

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
  MTree *tmp = new MTree(content->path, pattern, recursive);
  tmp->content->multi_selection = content->multi_selection;
  QBrush brr(QColor(0, 159, 255));
  for(int i = 0; i < ((QStandardItemModel *)((MyTreeView *)tmp->content)->model())->rowCount(); ++i){
      QStandardItem *it = ((QStandardItemModel *)((MyTreeView *)tmp->content)->model())->item(i);
      if(content->multi_selection.find(path + OSInterface::dir_sep + it->text().toStdString()) != content->multi_selection.end()){
        it->setForeground(brr);
        }
    }
  /*((MyTreeView *)tmp->content)->selectionModel()->select(QItemSelection (((MyTreeView *)tmp->content)->model()->index (5, 0), ((MyTreeView *)tmp->content)->model()->index (5, 2)), QItemSelectionModel::SelectCurrent);
  QPersistentModelIndex nextIndex = ((MyTreeView *)tmp->content)->indexAt(QPoint(5, 0));
  ((MyTreeView *)tmp->content)->setCurrentIndex(nextIndex);*/
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
  ((MyTreeView*)content)->setModel(model);
  ((MyTreeView*)content)->setSelectionBehavior(QTreeView::SelectRows);
  ((MyTreeView*)content)->setHeader(hv);
  ((MyTreeView*)content)->sortByColumn(1,Qt::AscendingOrder);
  ((MyTreeView*)content)->setColumnWidth(0, 300);
}

MTree::MTree(std::string p, std::string pat, bool rec): AbstractView(p, pat), recursive(rec){
  content = new MyTreeView();
  init(p, pat, rec);
}
