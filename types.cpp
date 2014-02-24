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
  std::cout << root << std::endl;
  if(root[root.size() - 1] == OSInterface::dir_sep)
    root = root.substr(0,root.size() - 1);
  QStandardItem *item;
  QList<QStandardItem*> ql;
  int r = 0;
  for(auto &e : os.dirs){
      if(e->name.empty()) continue;
      if(e->type == e->LINK) continue;
      if((e->name == ".") || (e->name == "..")) continue;
      std::stringstream ss;
      item = new QStandardItem(QString::fromStdString(e->name));
      item->setEditable(false);
      it->setChild(r++, item);

      /*if((e->type == e->DIR) && (e->name != ".") && (e->name != "..")){
          buildTree(root + "/" + e->name, item);
        }*/
      ql.push_back(item);
      item = new QStandardItem(QString::fromStdString(e->type_name));
      item->setEditable(false);
      ql.push_back(item);
      ss << e->byte_size;
      item = new QStandardItem(QString::fromStdString(ss.str()));
      item->setEditable(false);
      ql.push_back(item);
      //it->appendRow(ql);
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
  QFont dir_font, base_font, link_font;
  link_font.setFamily("Verdana");
  link_font.setItalic(true);
  dir_font.setBold(true);
  dir_font.setFamily("Verdana");
  base_font.setFamily("Verdana");
  int row = 0;
  if(p[p.size() - 1] == '/')
    p = p.substr(0,p.size() - 1);
  for(auto &e : osi->dirs){
      if((e->name == ".") || (e->name == "..")) continue;
      qlis.clear();
      std::stringstream ss;
      std::string type;
      item = new QStandardItem(QString::fromStdString(e->name));
      item->setEditable(false);
      if(e->type == e->DIR) item->setFont(dir_font);
      else if(e->type == e->LINK){
          item->setFont(link_font);
          item->setForeground(QBrush(QColor(255, 0, 0)));
      }else item->setFont(base_font);
      if(rec){
          if(e->type == e->DIR){
              buildTree(p + OSInterface::dir_sep + e->name, item);
            }
        }
      model->setItem(row, 0, item);
      item = new QStandardItem(QString::fromStdString(e->type_name));
      item->setEditable(false);
      item->setFont(base_font);
      model->setItem(row, 1, item);
      ss << e->byte_size;
      item = new QStandardItem(QString::fromStdString(ss.str()));
      item->setFont(base_font);
      item->setEditable(false);
      model->setItem(row, 2, item);
      ++row;
    }
  ((MyTreeView*)content)->setModel(model);
  ((MyTreeView*)content)->setSelectionBehavior(QTreeView::SelectRows);
  ((MyTreeView*)content)->setHeaderHidden(true);
  ((MyTreeView*)content)->setColumnWidth(0, 250);
}

MTree::MTree(std::string p, std::string pat, bool rec): AbstractView(p, pat), recursive(rec){
  content = new MyTreeView();
  init(p, pat, rec);
}
