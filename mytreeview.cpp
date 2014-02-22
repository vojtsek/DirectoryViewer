#include "mytreeview.h"
#include "osinterface.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QTreeView>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QBrush>
#include <QStandardItem>
#include <iostream>
#include <string>

int MyTreeView::getSelIdx(){
  return selectedIndexes().empty() ? 0 : selectedIndexes()[0].row();
}

void MyTreeView::focusInEvent(QFocusEvent *e){
  emit(focused());
  if(selectedIndexes().empty()){
  setSelectionBehavior(QAbstractItemView::SelectRows);
  selectionModel()->select(QItemSelection (model()->index (0, 0), model()->index (0, 2)), QItemSelectionModel::Select);
    }
  focus();
}

std::string MyTreeView::getSelected(){
  //co kdyz je empty
  int idx = getSelIdx();
  return path + OSInterface::dir_sep + ((QStandardItemModel*) model())->item(idx)->text().toStdString();
}

void MyTreeView::changeSelection(){
  int idx = getSelIdx();
  QBrush brr(QColor(0, 159, 255)), brb(Qt::black);
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
    ((QStandardItemModel*) model())->item(idx)->setForeground(brb);
  }else{
    multi_selection.insert(selected);
    ((QStandardItemModel*) model())->item(idx)->setForeground(brr);
    }
}

void MyTreeView::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_M)
    mark(!marked);
  if(e->key() == Qt::Key_S)
    changeSelection();
  if(e->key() == Qt::Key_Backspace){
      emit(stepup());
      setFocus();
    }
  QTreeView::keyPressEvent(e);
}

void MyTreeView::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
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

void MyTreeView::unFocus(){
  is_focused = false;
  if(!marked)
    setStyleSheet(unfocused_list_style);
  else mark(true);
}
