#include "mytreeview.h"
#include "osinterface.h"
#include <QKeyEvent>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <iostream>
#include <string>

void MyTreeView::focusInEvent(QFocusEvent *e){
  emit(focused());
  focus();
  QTreeView::focusInEvent(e);
}

std::string MyTreeView::getSelected(){
  //co kdyz je empty
  int idx = selectedIndexes().empty() ? 0 : selectedIndexes()[0].row();
  return path + OSInterface::dir_sep + ((QStandardItemModel*) model())->item(idx)->text().toStdString();
}

void MyTreeView::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_M)
    mark(!marked);
  QTreeView::keyPressEvent(e);
}

void MyTreeView::focus(){
  is_focused = true;
  setStyleSheet("QTreeView{color:#000;border: 1px solid #000;}");
}

void MyTreeView::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet("QTreeView{color:#000;border: 1px solid #f00;}");
    }else{ //odznacit
      marked = false;
      setStyleSheet("QTreeView{color:#000;border: 1px solid #000;}");
    }
}

void MyTreeView::unFocus(){
  is_focused = false;
  if(!marked)
    setStyleSheet("QTreeView{color:#999;border:none;}");
  else mark(true);
}
