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

void MTree::buildTree(std::string root, QTreeWidgetItem *it){
  OSInterface os;
  try{
    os.getDirInfo(root, pattern);
  }catch(OSException *e){
    e->process();
    return;
  }
 // std::cout << root << std::endl;
  if(root[root.size() - 1] == OSInterface::dir_sep)
    root = root.substr(0,root.size() - 1);
  QTreeWidgetItem *item;
  for(auto &e : os.dirs){
      if(e->name.empty()) continue;
      if(e->type == e->LINK) continue;
      if((e->name == ".") || (e->name == "..")) continue;
      std::stringstream ss;
      item = new QTreeWidgetItem();
      item->setText(0, QString::fromStdString(e->name));

      /*if((e->type == e->DIR) && (e->name != ".") && (e->name != "..")){
          buildTree(root + "/" + e->name, item);
        }*/

      item->setText(1, QString::fromStdString(e->type_name));
      ss << e->byte_size;
      item->setText(2, QString::fromStdString(ss.str()));
      it->addChild(item);
      it->setExpanded(false);
    }
}

AbstractView *MTree::clone(){
  if(path.empty()) return nullptr;
  MTree *tmp = new MTree(content->path, pattern, recursive);
  tmp->content->multi_selection = content->multi_selection;
  QBrush brr(QColor(0, 159, 255));
  for(int i = 0; i < ((MyTreeView *)tmp->content)->topLevelItemCount(); ++i){
      QTreeWidgetItem *it = ((MyTreeView *) content)->topLevelItem(i);
      if(content->multi_selection.find(path + OSInterface::dir_sep + it->text(0).toStdString()) != content->multi_selection.end()){
        it->setForeground(0, brr);
        }
    }
  /*((MyTreeView *)tmp->content)->selectionModel()->select(QItemSelection (((MyTreeView *)tmp->content)->model()->index (5, 0), ((MyTreeView *)tmp->content)->model()->index (5, 2)), QItemSelectionModel::SelectCurrent);
  QPersistentModelIndex nextIndex = ((MyTreeView *)tmp->content)->indexAt(QPoint(5, 0));
  ((MyTreeView *)tmp->content)->setCurrentIndex(nextIndex);*/
  return tmp;
}

void MTree::init(std::string p, std::string pat, bool rec){
  content = new MyTreeView();
  ((MyTreeView *)content)->setColumnCount(3);
  recursive = rec;
  osi->dirs.clear();
  osi->getDirInfo(p, pat);
  content->path = p;
  QTreeWidgetItem *item;
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
      std::stringstream ss;
      item = new QTreeWidgetItem((MyTreeView *)content);
      item->setText(0, QString::fromStdString(e->name));
      if(e->type == e->DIR) item->setFont(0, dir_font);
      else if(e->type == e->LINK){
          item->setFont(0, link_font);
          item->setForeground(0, QBrush(QColor(255, 0, 0)));
      }else item->setFont(0, base_font);
      if(rec){
          if(e->type == e->DIR){
              buildTree(p + OSInterface::dir_sep + e->name, item);
            }
        }
      item->setText(1, QString::fromStdString(e->type_name));
      item->setFont(1, base_font);
      ss << e->byte_size;
      item->setText(2, QString::fromStdString(ss.str()));
      item->setFont(2, base_font);
      ++row;
    }
  ((MyTreeView*)content)->setSelectionBehavior(QTreeWidget::SelectRows);
  ((MyTreeView*)content)->setHeaderHidden(true);
  ((MyTreeView*)content)->setColumnWidth(0, 250);
}

MTree::MTree(std::string p, std::string pat, bool rec): AbstractView(p, pat), recursive(rec){
  init(p, pat, rec);
}
