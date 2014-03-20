#ifndef MYVIEWER_H
#define MYVIEWER_H

#include "types.h"
#include "functions.h"
#include <QPlainTextEdit>
#include <QLineEdit>

class MyViewer : public QPlainTextEdit, public MyViewType
{
  Q_OBJECT
    bool image;
    int idx;
public:
  explicit MyViewer(QWidget *parent = 0): QPlainTextEdit(parent), MyViewType("","") {}
  MyViewer(std::string pa, std::string pat, int i = 0, QWidget *p = 0): QPlainTextEdit(p), MyViewType(pa, pat), image(false), idx(i) {
      if(isImg(path))
          image = true;
    rebuild();
  }
  virtual ~MyViewer() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void resizeEvent(QResizeEvent *e);
  virtual void rebuild(int idx = 0);
  std::string getSelected();
  virtual void unFocus();
  virtual int getIdxOf(std::string &) { return 0;}
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  virtual int getSelIdx();
  virtual void die();
signals:
  void focused();
  void rebuilded();
  void unfocused();
  void itemSelectionChanged();
  void stepup();
  void tab();
  void chlayout();
};

#endif // MYVIEWER_H
