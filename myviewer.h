#ifndef MYVIEWER_H
#define MYVIEWER_H

#include "types.h"
#include <QPlainTextEdit>
#include <QLineEdit>

class MyViewer : public QPlainTextEdit, public MyViewType
{
  Q_OBJECT
public:
  MyViewer(QWidget *parent = 0): QPlainTextEdit(parent), MyViewType() {}
  virtual ~MyViewer() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void resizeEvent(QResizeEvent *e);
  std::string getSelected();
  virtual QWidget *getContent();
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  virtual int getSelIdx();
  virtual void die();
signals:
  void focused();
  void rebuild();
  void unfocused();
  void itemSelectionChanged();
  void stepup();
  void tab();
  void chlayout();
};

#endif // MYVIEWER_H
