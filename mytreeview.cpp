#include "mytreeview.h"
#include "osinterface.h"
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

void MyTreeView::focusOutEvent(QFocusEvent *e){
  unFocus();
}

void MyTreeView::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
    if(w)
      emit(rebuild());
}

void MyTreeView::setFocus(){ QTreeWidget::setFocus(); }

std::string MyTreeView::getSelected(){
  if(currentItem())
  return path + OSInterface::dir_sep + currentItem()->text(0).toStdString() ;
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
  else if(e->key() == Qt::Key_Shift)
    changeSelection(getSelIdx());
  else if(e->key() == Qt::Key_Backspace){
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
  for(int i = 0; i < topLevelItemCount(); ++i){
      if(topLevelItem(i) != nullptr){
          if(multi_selection.find(path + OSInterface::dir_sep + topLevelItem(i)->text(0).toStdString()) != multi_selection.end()){
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
