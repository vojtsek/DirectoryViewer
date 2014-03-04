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
    if(w)
      emit(rebuild());
}

void MyIconView::setFocus(){ QWidget::setFocus(); }

std::string MyIconView::getSelected(){
  if(currentItem())
    return path + OSInterface::dir_sep + currentItem()->text().toStdString() ;
  else return "";
}

void MyIconView::updateSelection(){
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
  for(int i = 0; i < rowCount(); ++i){
      for(int j = 0; j < columnCount(); ++j){
          if(item(i, j) != nullptr){
              if(multi_selection.find(path + OSInterface::dir_sep + item(i, j)->text().toStdString()) != multi_selection.end()){
                  item(i, j)->setBackground(brb);
                  item(i, j)->setFont(sel_font);
                }else{
                  if(OSInterface::isDir(path + OSInterface::dir_sep + item(i, j)->text().toStdString()))
                    base_font.setBold(true);
                  item(i, j)->setBackground(brw);
                  item(i, j)->setFont(base_font);
                  base_font.setBold(false);
                }
            }
        }
    }
}

void MyIconView::changeSelection(){
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
  }else{
    multi_selection.insert(selected);
    }
  updateSelection();
}

void MyIconView::die(){
  setFocusPolicy(Qt::NoFocus);
  setFixedSize(0, 0);
}

void MyIconView::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_Insert)
    mark(!marked);
  else if(e->key() == Qt::Key_Shift)
    changeSelection();
  else if(e->key() == Qt::Key_Backspace){
      emit(stepup());
      setFocus();
    }else if(e->key() == Qt::Key_F1)
    emit(chlayout());
  else if(e->key() == Qt::Key_Tab){
      QWidget::keyPressEvent(e);
  }else if((e->key() == Qt::Key_Down) || (e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Right) || (e->key() == Qt::Key_Left)){
      QTableWidget::keyPressEvent(e);
      if(e->modifiers() & Qt::ShiftModifier)
        changeSelection();
    }else
    QTableWidget::keyPressEvent(e);
}

void MyIconView::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
  updateSelection();
}

void MyIconView::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet(marked_list_style);
    }else{ //odznacit
      marked = false;
      setStyleSheet(focused_list_style);
    }
  updateSelection();
}

void MyIconView::unFocus(){
  is_focused = false;
  if(!marked){
    setStyleSheet(unfocused_list_style);
    updateSelection();
  }else mark(true);
}

QWidget * MyIconView::getContent(){
  return (QWidget *) this;
}
