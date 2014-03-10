#ifndef MYICONVIEW_H
#define MYICONVIEW_H

#include "types.h"
#include "QTableWidget"

class MyIconView : public QTableWidget, public MyViewType
{
  Q_OBJECT
public:
  explicit MyIconView(QWidget *p = 0): QTableWidget(p), MyViewType("","") {
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setEditTriggers(QAbstractItemView::EditTriggers(0));
    w = width();
  }
  MyIconView(std::string pa, std::string pat, int idx = 0, QWidget *p = 0): QTableWidget(p), MyViewType(pa, pat) {
      setSelectionBehavior( QAbstractItemView::SelectItems );
      setSelectionMode( QAbstractItemView::SingleSelection );
      setEditTriggers(QAbstractItemView::EditTriggers(0));
      w = width();
      col_width = 80;
      rebuild(idx);
    }
 // MyTreeView(const MyTreeView&);
  virtual ~MyIconView() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void resizeEvent(QResizeEvent *e);
  virtual void rebuild(int idx = 0);
  std::string getSelected();
  virtual QWidget *getContent();
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  void changeSelection();
  void updateSelection();
  virtual int getSelIdx();
  virtual void die();
signals:
  void focused();
  void rebuilded();
  void unfocused();
  void stepup();
  void tab();
  void chlayout();
};

#endif // MYICONVIEW_H
