#include "types.h"
#include "osinterface.h"
#include "mytreeview.h"
#include "myiconview.h"

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

void MTree::buildTree(std::string root, QTreeWidgetItem *it, bool top){
  OSInterface os;
  try{
    os.getDirInfo(root, pattern);
  }catch(OSException *e){
    //e->process();
    //return;
  }
  QIcon dir_icon("directory.png");
  QIcon ar_icon("archive.png");
  QIcon base_icon("file.png");
  QFont base_font, bold_font, italic_font;
  italic_font.setFamily("Verdana");
  italic_font.setItalic(true);
  bold_font.setBold(true);
  bold_font.setFamily("Verdana");
  base_font.setFamily("Verdana");
  if(root[root.size() - 1] == OSInterface::dir_sep)
    root = root.substr(0,root.size() - 1);
  QTreeWidgetItem *item;
  for(auto &e : os.dirs){
      if(e->name.empty()) continue;
      if((e->name == ".") || (e->name == "..")) continue;
      std::stringstream ss;
      if(it != nullptr)
        item = new QTreeWidgetItem();
      else
        item = new QTreeWidgetItem((MyTreeView *) content);
      item->setText(0, QString::fromStdString(e->name));
      item->setIcon(0, base_icon);
      if(e->type == e->DIR){
          item->setIcon(0, dir_icon);
          item->setFont(0, bold_font);
      }else if(e->type == e->LINK){
          item->setFont(0, italic_font);
          item->setForeground(0, QBrush(QColor(255, 0, 0)));
      }else if(e->type == e->ARCHIVE){
          item->setIcon(0, ar_icon);
          item->setFont(0, bold_font);
          item->setForeground(0, QBrush(QColor(255, 0, 255)));
      }else item->setFont(0, base_font);

      item->setText(1, QString::fromStdString(e->type_name));
      item->setFont(1, italic_font);
      if(e->type == e->DIR) item->setFont(0, bold_font);
      ss << e->byte_size;
      item->setText(2, QString::fromStdString(ss.str()));
      if(recursive && top){
          if(e->type == e->DIR){
              buildTree(root + OSInterface::dir_sep + e->name, item, false);
            }
        }
      if(it != nullptr){
      it->addChild(item);
      it->setExpanded(false);
        }
    }
}

AbstractView *MTree::clone(){
  if(path.empty()) return nullptr;
  MTree *tmp = new MTree(content->path, pattern, recursive);
  tmp->content->multi_selection = content->multi_selection;
  QBrush brr(QColor(0, 159, 255));
  for(int i = 0; i < ((MyTreeView *)tmp->content)->topLevelItemCount(); ++i){
      QTreeWidgetItem *it = ((MyTreeView *)tmp->content)->topLevelItem(i);
      if(content->multi_selection.find(path + OSInterface::dir_sep + it->text(0).toStdString()) != content->multi_selection.end()){
        it->setForeground(0, brr);
        }
    }
  return tmp;
}

AbstractView *MIcon::clone(){
  if(path.empty()) return nullptr;
  MIcon *tmp = new MIcon(content->path, pattern);
  tmp->content->multi_selection = content->multi_selection;
  QBrush brr(QColor(0, 159, 255));
 /* for(int i = 0; i < ((MyTreeView *)tmp->content)->topLevelItemCount(); ++i){
      QTreeWidgetItem *it = ((MyTreeView *)tmp->content)->topLevelItem(i);
      if(content->multi_selection.find(path + OSInterface::dir_sep + it->text(0).toStdString()) != content->multi_selection.end()){
        it->setForeground(0, brr);
        }
    }*/
  return tmp;
}

void MIcon::rebuild(std::string path, std::string pattern){
  ((MyIconView *) content)->clear();
  osi->dirs.clear();
  if(!osi->dirs.size())
  osi->getDirInfo(path, pattern);
  int w = ((MyIconView *)content)->width();
  int cols = w / col_width;
  ((MyIconView *)content)->setRowCount(osi->dirs.size() / cols + 1);
  ((MyIconView *)content)->setColumnCount(cols);
  ((MyIconView *)content)->setIconSize(QSize(25,25));
  for(int i = 0; i < cols; ++i){
      ((MyIconView*)content)->setColumnWidth(i, col_width - 1);
    }
  content->path = path;
  QTableWidgetItem *item, *icon_item;
  QIcon dir_icon("directory.png");
  QIcon arch_icon("archive.png");
  QIcon base_icon("file.png");
  QFont base_font, bold_font, italic_font;
  italic_font.setFamily("Verdana");
  italic_font.setItalic(true);
  bold_font.setBold(true);
  bold_font.setFamily("Verdana");
  base_font.setFamily("Verdana");
  int row = 0, col = 0;
  if(path[path.size() - 1] == '/')
    path = path.substr(0,path.size() - 1);
  for(auto &e : osi->dirs){
      if((e->name == ".") || (e->name == "..")) continue;
      std::stringstream ss;
      item = new QTableWidgetItem(QString::fromStdString(e->name));
      //icon_item = new QTableWidgetItem();
      item->setTextAlignment(0);
      item->setIcon(base_icon);

      if(e->type == e->DIR){
          item->setIcon(dir_icon);
          item->setFont(bold_font);
      }else if(e->type == e->LINK){
          item->setFont(italic_font);
          item->setForeground(QBrush(QColor(255, 0, 0)));
      }else if(e->type == e->ARCHIVE){
          item->setIcon(arch_icon);
        item->setFont(bold_font);
        item->setForeground(QBrush(QColor(255, 0, 255)));
      }else item->setFont(base_font);
      ((MyIconView*)content)->setItem(row, col, item);
      col = (col + 1) % cols;
      if(!col)
        ++row;
    }
  ((MyIconView*)content)->horizontalHeader()->setVisible(false);
  ((MyIconView*)content)->verticalHeader()->setVisible(false);
  //((MyIconView*)content)->setIconSize(QSize(50,50));
  ((MyIconView*) content)->setShowGrid(false);
  if(!osi->dirs.empty()){
      QPersistentModelIndex nextIndex = ((MyTreeView*)content)->indexAt(QPoint(0, 0));
      ((MyTreeView*)content)->selectionModel()->setCurrentIndex(nextIndex, QItemSelectionModel::SelectCurrent);
    }
}


void MTree::rebuild(std::string p, std::string pat){
  osi->dirs.clear();
  osi->getDirInfo(p, pat);
  content->path = p;
  int count = ((MyTreeView *)content)->topLevelItemCount();
  for(int i = 0; i < count; ++i){
      delete ((MyTreeView *) content)->takeTopLevelItem(0);
    }
  buildTree(p, nullptr, true);
  ((MyTreeView*)content)->setSelectionBehavior(QTreeWidget::SelectRows);
  ((MyTreeView*)content)->setHeaderHidden(true);
  ((MyTreeView*)content)->setColumnWidth(0, 250);
  if(!osi->dirs.empty()){
      QPersistentModelIndex nextIndex = ((MyTreeView*)content)->indexAt(QPoint(0, 0));
      ((MyTreeView*)content)->selectionModel()->setCurrentIndex(nextIndex, QItemSelectionModel::SelectCurrent);
    }
}

void MTree::init(std::string p, std::string pat, bool rec){
  content = new MyTreeView();
  ((MyTreeView *)content)->setColumnCount(4);
  recursive = rec;
  rebuild(p, pat);
}

MTree::MTree(std::string p, std::string pat, bool rec): AbstractView(p, pat){
  init(p, pat, rec);
}

MIcon::MIcon(std::string path, std::string pattern): AbstractView(path, pattern) {
  init(path, pattern);
}

void MIcon::init(std::string path, std::string pattern){
  col_width = 120;
  content = new MyIconView();
  rebuild(path, pattern);
}
