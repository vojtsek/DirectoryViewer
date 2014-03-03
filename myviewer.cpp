#include "myviewer.h"

#include "myiconview.h"
#include "types.h"
#include "osinterface.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTextEdit>
#include <QBrush>
#include <iostream>
#include <string>

int MyViewer::getSelIdx(){
  return 0;
}

void MyViewer::die(){
  setFocusPolicy(Qt::NoFocus);
  setFixedSize(0, 0);
}

void MyViewer::focusInEvent(QFocusEvent *e){
  emit(focused());
  focus();
  //QPlainTextEdit::focusInEvent(e);
}

void MyViewer::focusOutEvent(QFocusEvent *e){
  unFocus();
  //QPlainTextEdit::focusOutEvent(e);
}

void MyViewer::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
  //  emit(rebuild());
}

void MyViewer::setFocus(){ QWidget::setFocus(); }

std::string MyViewer::getSelected(){
  return "";
}
/*
void MyViewer::updateSelection(){
  /*QBrush brb(QColor(150, 200, 255));
  for(int i = 0; i < rowCount(); ++i){
      for(int j = 0; j < columnCount(); ++j){
          if(item(i, j) != nullptr){
            if(multi_selection.find(path + "/" + item(i, j)->text().toStdString()) != multi_selection.end())
              item(i, j)->setBackground(brb);
            }
        }
    }
}

void MyViewer::changeSelection(){
  QBrush brb(QColor(150, 200, 255)), brw(Qt::white);
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
    //currentItem()->setBackground(brw);
  }else{
    multi_selection.insert(selected);
    //currentItem()->setBackground(brb);
    }
}
*/

void MyViewer::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_M)
    mark(!marked);
  else if(e->key() == Qt::Key_Backspace){
      emit(chlayout());
      setFocus();
    }else if(e->key() == Qt::Key_F1)
    emit(chlayout());
  else if(e->key() == Qt::Key_Tab){
      QWidget::keyPressEvent(e);
  }
 QPlainTextEdit::keyPressEvent(e);
}

void MyViewer::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
  //updateSelection();
}

void MyViewer::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet(marked_list_style);
    }else{ //odznacit
      marked = false;
      setStyleSheet(focused_list_style);
    }
}

void MyViewer::unFocus(){
  is_focused = false;
  if(!marked){
    setStyleSheet(unfocused_list_style);
   // updateSelection();
  }else mark(true);
}

QWidget * MyViewer::getContent(){
  return (QWidget *) this;
}

