#include "mylineedit.h"
#include "osinterface.h"
#include <QKeyEvent>
#include <iostream>
#include <string>

void MyLineEdit::focusInEvent(QFocusEvent *e){
  emit(focused(true));
  QLineEdit::focusInEvent(e);
}

void MyLineEdit::focusOutEvent(QFocusEvent *e){
  is_focused = false;
  emit(focused(false));
  QLineEdit::focusOutEvent(e);
}

void MyLineEdit::keyPressEvent(QKeyEvent *e){
  std::string sep;
  sep.push_back(OSInterface::dir_sep);
  if((selectedText().toStdString() == sep) && text().toStdString() == sep) return;
  std::string cont = text().toStdString();
  char key = (char) e->key();
  if((cont[cont.size()-1] == OSInterface::dir_sep) && (key == OSInterface::dir_sep)) return;
  QLineEdit::keyPressEvent(e);
  if(e->key() == Qt::Key_Backspace){
    if(text().toStdString().empty())
      setText(QString::fromStdString(OSInterface::getPrefix()));
    emit(focused(true));
    }
}

void MyLineEdit::mousePressEvent(QMouseEvent *e){
  if(is_focused)
    QLineEdit::mousePressEvent(e);
  else{
    emit(focused(true));
      is_focused = true;
    }
}
