#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>
#include <string>

class MyTreeView : public QTreeView
{
  Q_OBJECT
public:
  bool is_focused, marked;
  std::string path;
  explicit MyTreeView(QWidget *p = 0): is_focused(false), marked(false), path(""), QTreeView(p) {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  std::string getSelected();
  void unFocus();
  void focus();
  void mark(bool);
signals:
  void focused();
  void unfocused();
};

#endif // MYTREEVIEW_H
