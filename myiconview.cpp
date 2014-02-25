#include "myiconview.h"
#include "types.h"
#include "osinterface.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QBrush>
#include <iostream>
#include <string>

int MyIconView::getSelIdx(){
  return 1;
}

void MyIconView::focusInEvent(QFocusEvent *e){
  emit(focused());
  focus();
}

void MyIconView::setFocus(){ QWidget::setFocus(); }

std::string MyIconView::getSelected(){
  //co kdyz je empty
  return path + OSInterface::dir_sep;
}

void MyIconView::changeSelection(){
  int idx = getSelIdx();
  QBrush brr(QColor(0, 159, 255)), brb(Qt::black);
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
    //topLevelItem(idx)->setForeground(0, brb);
  }else{
    multi_selection.insert(selected);
    //topLevelItem(idx)->setForeground(0, brr);
    }
}

void MyIconView::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_M)
    mark(!marked);
  else if(e->key() == Qt::Key_S)
    changeSelection();
  else if(e->key() == Qt::Key_Backspace){
      emit(stepup());
      setFocus();
    }else if(e->key() == Qt::Key_F1)
    emit(chlayout());
  else if(e->key() == Qt::Key_Tab){
      QWidget::keyPressEvent(e);
  }else
    QTableWidget::keyPressEvent(e);
}

void MyIconView::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
}

void MyIconView::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet(marked_list_style);
    }else{ //odznacit
      marked = false;
      setStyleSheet(focused_list_style);
    }
}

void MyIconView::unFocus(){
  is_focused = false;
  if(!marked)
    setStyleSheet(unfocused_list_style);
  else mark(true);
}

