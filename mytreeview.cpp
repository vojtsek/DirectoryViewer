#include "mytreeview.h"
#include "osinterface.h"
#include "mydialog.h"
#include "functions.h"
#include "types.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPersistentModelIndex>
#include <QAbstractItemView>
#include <QBrush>
#include <QStandardItem>
#include <iostream>
#include <sstream>
#include <string>

int MyTreeView::getSelIdx(){
  return currentIndex().row();
}

void MyTreeView::focusInEvent(QFocusEvent *e){
  emit(focused());
  if(model()){
      if(selectedIndexes().empty()){
          QPersistentModelIndex sel_idx = indexAt(QPoint(0, 0));
          setCurrentIndex(sel_idx);
        }
    }
  focus();
}

void MyTreeView::buildTree(std::string root, QTreeWidgetItem *it, bool top){
  OSInterface os;
  try{
    os.getDirInfo(root, pattern);
  }catch(OSException *e){
    std::cout << e->what() << std::endl;
    return;
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
        item = new QTreeWidgetItem(this);
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

void MyTreeView::rebuild(){
  if(osi == nullptr) osi = new OSInterface();
  osi->dirs.clear();
  try{
    osi->getDirInfo(path, pattern);
  }catch(OSException *e){
    std::cout << e->what() << std::endl;
    stepup();
  }
  clear();
  buildTree(path, nullptr, true);
  setEditTriggers(QTreeWidget::NoEditTriggers);
  setAlternatingRowColors(false);
  setSortingEnabled(false);
  setSelectionBehavior(QTreeWidget::SelectRows);
  setHeaderHidden(true);
  setColumnWidth(0, 280);
  if(!osi->dirs.empty()){
      QPersistentModelIndex nextIndex = indexAt(QPoint(0, 0));
      selectionModel()->setCurrentIndex(nextIndex, QItemSelectionModel::SelectCurrent);
    }
  repaint();
}

void MyTreeView::focusOutEvent(QFocusEvent *e){
  unFocus();
}

void MyTreeView::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
    if(w)
      emit(rebuilded());
}

void MyTreeView::setFocus(){ QTreeWidget::setFocus(); }

std::string MyTreeView::getSelected(){
    repairPath(path);
        if(currentItem())
            return path + currentItem()->text(0).toStdString() ;
        else return "";
}

void MyTreeView::changeSelection(int idx){
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
  }else{
    multi_selection.insert(selected);
    }
  updateSelection();
}

void MyTreeView::die(){
  setFocusPolicy(Qt::NoFocus);
  setFixedSize(0, 0);
}

void MyTreeView::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_Insert)
    mark(!marked);
  else if(e->key() == Qt::Key_Shift){
      if(!recursive)
        changeSelection(getSelIdx());
      else{
          std::string msg("Selecting items is not allowed in tree mode.");
          MyDialog::MsgBox(msg);
        }
  }else if(e->key() == Qt::Key_Backspace){
      emit(stepup());
      setFocus();
    }else if(e->key() == Qt::Key_F1)
      emit(chlayout());
  else if((e->key() == Qt::Key_Down) || (e->key() == Qt::Key_Up)){
      QTreeWidget::keyPressEvent(e);
      if(e->modifiers() & Qt::ShiftModifier)
        changeSelection(getSelIdx());
    }
    else
    QTreeWidget::keyPressEvent(e);
}

void MyTreeView::updateSelection(){
  QBrush brb(QColor(150, 200, 255));
  QBrush brw;
  if(marked)
    brw = QBrush(QColor(238, 255, 238));
  else
    brw = QBrush(QColor(255, 255, 255));

  QFont sel_font, base_font;
  base_font.setFamily("Verdana");
  sel_font.setFamily("Helvetica");
  sel_font.setUnderline(true);
  sel_font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
  repairPath(path);
  for(int i = 0; i < topLevelItemCount(); ++i){
      if(topLevelItem(i) != nullptr){
          if(multi_selection.find(path + topLevelItem(i)->text(0).toStdString()) != multi_selection.end()){
              topLevelItem(i)->setBackground(0,brb);
              topLevelItem(i)->setFont(0, sel_font);
              topLevelItem(i)->setBackground(1,brb);
              topLevelItem(i)->setFont(1, sel_font);
              topLevelItem(i)->setBackground(2,brb);
              topLevelItem(i)->setFont(2, sel_font);
            }
          else{
              if(OSInterface::isDir(path + OSInterface::dir_sep + topLevelItem(i)->text(0).toStdString()))
                base_font.setBold(true);
              topLevelItem(i)->setBackground(0,brw);
              topLevelItem(i)->setBackground(1,brw);
              topLevelItem(i)->setBackground(2,brw);
              topLevelItem(i)->setFont(0, base_font);
              topLevelItem(i)->setFont(1, base_font);
              topLevelItem(i)->setFont(2, base_font);
              base_font.setBold(false);
            }
        }
    }
}

void MyTreeView::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
  updateSelection();
}

void MyTreeView::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet(marked_list_style);
    }else{ //odznacit
      marked = false;
      setStyleSheet(focused_list_style);
    }
  updateSelection();
}

QWidget *MyTreeView::getContent(){
  return (QWidget *) this;
}

void MyTreeView::unFocus(){
  is_focused = false;
  if(!marked){
      setStyleSheet(unfocused_list_style);
      updateSelection();
  }else mark(true);
}
