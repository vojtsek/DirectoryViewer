#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>

class MyLineEdit : public QLineEdit
{
  Q_OBJECT
public:
  bool is_focused;
  explicit MyLineEdit(QWidget *p = 0): QLineEdit(p), is_focused(false) {}
  virtual void keyPressEvent(QKeyEvent *e);
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void mousePressEvent(QMouseEvent *);
signals:
  void focused(bool);
};

#endif // MYLINEEDIT_H
