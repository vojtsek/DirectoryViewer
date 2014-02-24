#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include "types.h"
#include <QTreeView>
#include <string>
#include <set>

class MyTreeView : public QTreeView, public MyViewType
{
  Q_OBJECT
public:
  explicit MyTreeView(QWidget *p = 0): MyViewType(), QTreeView(p) {
    setSelectionBehavior(QAbstractItemView::SelectRows);
  }
  MyTreeView(const MyTreeView&);
  virtual ~MyTreeView() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  std::string getSelected();
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  void mark(bool);
  void changeSelection();
  virtual int getSelIdx();
signals:
  void focused();
  void unfocused();
  void stepup();
};

#endif // MYTREEVIEW_H
