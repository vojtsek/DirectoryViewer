#include "myiconview.h"
#include "types.h"
#include "osinterface.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QBrush>
#include <iostream>
#include <string>

int MyIconView::getSelIdx(){
  return currentItem()->row() * columnCount() + currentItem()->column();
}

void MyIconView::focusInEvent(QFocusEvent *e){
  emit(focused());
  focus();
}

void MyIconView::focusOutEvent(QFocusEvent *e){
  unFocus();
}

void MyIconView::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
    emit(rebuild());
}

void MyIconView::setFocus(){ QWidget::setFocus(); }

std::string MyIconView::getSelected(){
  if(currentItem())
    return path + OSInterface::dir_sep + currentItem()->text().toStdString() ;
  else return "";
}

void MyIconView::changeSelection(){
  QBrush brb(QColor(150, 200, 255)), brw(Qt::white);
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
  QBrush brb(QColor(150, 200, 255));
  for(int i = 0; i < rowCount(); ++i){
      for(int j = 0; j < columnCount(); ++j){
          if(item(i, j) != nullptr){
            if(multi_selection.find(path + "/" + item(i, j)->text().toStdString()) != multi_selection.end())
              item(i, j)->setBackground(brb);
            }
        }
    }
  /*if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
    currentItem()->setBackground(brw);
  }else{
    multi_selection.insert(selected);
    currentItem()->setBackground(brb);
    }*/
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
