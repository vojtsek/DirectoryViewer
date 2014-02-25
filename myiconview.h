#ifndef MYICONVIEW_H
#define MYICONVIEW_H

#include "types.h"
#include "QTableWidget"

class MyIconView : public QTableWidget, public MyViewType
{
  Q_OBJECT
public:
  explicit MyIconView(QWidget *p = 0): QTableWidget(p), MyViewType() {}
 // MyTreeView(const MyTreeView&);
  virtual ~MyIconView() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  std::string getSelected();
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  void changeSelection();
  virtual int getSelIdx();
signals:
  void focused();
  void unfocused();
  void stepup();
  void chlayout();
};

#endif // MYICONVIEW_H
