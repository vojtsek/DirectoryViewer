#include "myviewer.h"

#include "myiconview.h"
#include "types.h"
#include "osinterface.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTextEdit>
#include <QBrush>
#include <fstream>
#include <iostream>
#include <string>

/* vrati index oznaceneho prvku, v tomto pripade je to hodnota ulozena pri vzniku, kvuli navratu */

int MyViewer::getSelIdx(){
  return idx;
}

/* stane se neviditelny, zrusen bude pozdeji */

void MyViewer::die(){
  setFocusPolicy(Qt::NoFocus);
  setFixedSize(0, 0);
}

/* osetreni ziskani focusu */

void MyViewer::focusInEvent(QFocusEvent *e){
  emit(focused());
  focus();
}

void MyViewer::focusOutEvent(QFocusEvent *e){
  unFocus();
}

void MyViewer::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
}

void MyViewer::setFocus(){ QWidget::setFocus(); }

/* znovu vytvori obsah
 * jedna-li se o obrazek, proste ho nastavi jako pozadi
 * ostatni soubory cte jako text a pridava radek po radku
 * cursor nastavi na zacatek
 */

void MyViewer::rebuild(int idx){
    if(image){
        setStyleSheet(QString::fromStdString("QPlainTextEdit {background: #000 url(" + path + ") no-repeat center; }"));
        return;
    }
  std::ifstream in(path);
  std::string line;
  setReadOnly(true);
  while(in.good()){
      getline(in, line);
      appendPlainText(QString::fromStdString(line));
    }
  QTextCursor cursor(textCursor());
  cursor.movePosition(QTextCursor::Start);
  setTextCursor(cursor);
}

std::string MyViewer::getSelected(){
  return "";
}

/* osetreni stisku klavesy */

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

/* zmena vzhledu pri ziskani focusu */

void MyViewer::focus(){
  is_focused = true;
  //setStyleSheet(focused_list_style);
  if(image)
      setStyleSheet(QString::fromStdString("QPlainTextEdit {background: #000 url(" + path + ") no-repeat center; }"));
}

/* zmena vzhledu pri oznaceni */

void MyViewer::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet(marked_list_style);
    }else{ //odznacit
      marked = false;
      setStyleSheet(focused_list_style);
    }
}

/* zmena vzhledu pri ztrate focusu */

void MyViewer::unFocus(){
  is_focused = false;
  if(!marked){
    setStyleSheet(unfocused_list_style);
  }else mark(true);
}
