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
  return selectedIndexes().empty() ? 0 : selectedIndexes()[0].row();
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

void MyTreeView::setFocus(){ QTreeView::setFocus(); }


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
  else if(e->key() == Qt::Key_S)
    changeSelection();
  else if(e->key() == Qt::Key_Backspace){
      emit(stepup());
      setFocus();
    }
  else
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
