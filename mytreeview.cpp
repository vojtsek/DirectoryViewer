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
    emit(rebuild());
}

void MyTreeView::setFocus(){ QTreeWidget::setFocus(); }

std::string MyTreeView::getSelected(){
  if(currentItem())
  return path + OSInterface::dir_sep + currentItem()->text(0).toStdString() ;
  else return "";
}

void MyTreeView::changeSelection(){
  int idx = getSelIdx();
  QBrush brb(QColor(150, 200, 255)), brw(Qt::white);
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
    topLevelItem(idx)->setBackground(0, brw);
  }else{
    multi_selection.insert(selected);
    topLevelItem(idx)->setBackground(0, brb);
    }
}

void MyTreeView::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_M)
    mark(!marked);
  else if(e->key() == Qt::Key_S)
    changeSelection();
  else if(e->key() == Qt::Key_Backspace){
      emit(stepup());
      setFocus();
    }else if(e->key() == Qt::Key_F1)
      emit(chlayout());
    else
    QTreeWidget::keyPressEvent(e);
}

void MyTreeView::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
  QBrush brb(QColor(150, 200, 255));
  for(int i = 0; i < topLevelItemCount(); ++i){
      if(topLevelItem(i) != nullptr){
          if(multi_selection.find(path + "/" + topLevelItem(i)->text(0).toStdString()) != multi_selection.end())
            topLevelItem(i)->setBackground(0,brb);
        }
    }
}

void MyTreeView::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet(marked_list_style);
    }else{ //odznacit
      marked = false;
      setStyleSheet(focused_list_style);
    }
}

QWidget *MyTreeView::getContent(){
  return (QWidget *) this;
}

void MyTreeView::unFocus(){
  is_focused = false;
  if(!marked)
    setStyleSheet(unfocused_list_style);
  else mark(true);
}
