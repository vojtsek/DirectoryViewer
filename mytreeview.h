#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include "types.h"
#include <QTreeWidget>
#include <string>
#include <set>

class MyTreeView : public QTreeWidget, public MyViewType
{
  Q_OBJECT
public:
  explicit MyTreeView(QWidget *p = 0): QTreeWidget(p), MyViewType("", "") {
    setSelectionBehavior(QAbstractItemView::SelectRows);
  }
  MyTreeView(std::string pa, std::string pat, bool rec, int idx = 0, QWidget *p = 0): QTreeWidget(p), MyViewType(pa, pat) {
    setColumnCount(3);
    recursive = rec;
    setSelectionBehavior(QAbstractItemView::SelectRows);
    index = idx;
    rebuild(idx);
  }
//  MyTreeView(const MyTreeView&);
  virtual ~MyTreeView() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void resizeEvent(QResizeEvent *);
  virtual void rebuild(int idx = 0);
  virtual std::string getSelected();
  virtual int getSelIdx();
  virtual int getIdxOf(std::string &);
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  void changeSelection();
  void updateSelection();
  void buildTree(std::string, QTreeWidgetItem *, bool top);
  virtual void die();
signals:
  void rebuilded();
  void focused();
  void unfocused();
  void stepup();
  void chlayout();
};

#endif // MYTREEVIEW_H
