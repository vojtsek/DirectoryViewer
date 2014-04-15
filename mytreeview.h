#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include "types.h"

#include <QTreeWidget>
#include <string>
#include <set>

/*
#ifndef det
#define det
typedef struct {
  std::string name, type_name, ext_name, mod_time, perms;
  int type;
  enum {FILE, DIR, UNKNOWN, LINK, ARCHIVE, EXE};
  long long byte_size;
  long long show_size;
} dirEntryT;
#endif
*/
class MyTreeView : public QTreeWidget, public MyViewType
{
  Q_OBJECT
public:
  explicit MyTreeView(OSInterface *osi, QWidget *p = 0): QTreeWidget(p), MyViewType("", "", osi) {
    setSelectionBehavior(QAbstractItemView::SelectRows);
  }
  MyTreeView(std::string pa, std::string pat, bool rec, OSInterface *osi, int idx = 0, QWidget *p = 0): QTreeWidget(p), MyViewType(pa, pat, osi) {
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
  void addItem(QTreeWidgetItem *, dirEntryT *);
  void buildTree(std::string, QTreeWidgetItem *, bool top);
  virtual void die();
signals:
  void rebuilded();
  void focused();
  void unfocused();
  void stepup();
  void chlayout();
  void refresh();
};

#endif // MYTREEVIEW_H
