#ifndef MYICONVIEW_H
#define MYICONVIEW_H

#include "types.h"
#include "QTableWidget"

class MyIconView : public QTableWidget, public MyViewType
{
  Q_OBJECT
public:
  explicit MyIconView(OSInterface *osi, QWidget *p = 0): QTableWidget(p), MyViewType("","", osi) {
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setEditTriggers(QAbstractItemView::EditTriggers(0));
    w = width();
  }
  MyIconView(std::string pa, std::string pat, OSInterface *osi, int idx = 0, QWidget *p = 0): QTableWidget(p), MyViewType(pa, pat, osi) {
      setSelectionBehavior( QAbstractItemView::SelectItems );
      setSelectionMode( QAbstractItemView::SingleSelection );
      setEditTriggers(QAbstractItemView::EditTriggers(0));
      w = width();
      col_width = 100;
      index = idx;
      rebuild(idx);
    }
  virtual ~MyIconView() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void resizeEvent(QResizeEvent *e);
  virtual void rebuild(int idx = 0);
  std::string getSelected();
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  void addItem(int &, int &, dirEntryT *);
  void changeSelection();
  void updateSelection();
  void build(int &);
  virtual int getSelIdx();
  virtual int getIdxOf(std::string &);
  virtual void die();
signals:
  void focused();
  void rebuilded();
  void unfocused();
  void stepup();
  void tab();
  void refresh();
  void chlayout();
};

#endif // MYICONVIEW_H
