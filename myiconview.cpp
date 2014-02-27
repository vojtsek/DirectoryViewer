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

void MyIconView::focusOutEvent(QFocusEvent *e){
  unFocus();
}

void MyIconView::setFocus(){ QWidget::setFocus(); }

std::string MyIconView::getSelected(){
  //co kdyz je empty
  //std::cout << this->currentItem()->text().toStdString() << std::endl;
  return path + OSInterface::dir_sep + this->currentItem()->text().toStdString();
}

void MyIconView::changeSelection(){
  QBrush brb(QColor(0, 200, 255)), brw(Qt::white);
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
    currentItem()->setBackground(brw);
  }else{
    multi_selection.insert(selected);
    currentItem()->setBackground(brb);
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

QWidget * MyIconView::getContent(){
  return (QWidget *) this;
}
