#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>
#include <string>
#include <set>

class MyTreeView : public QTreeView
{
  Q_OBJECT
public:
  bool is_focused, marked;
  std::string path;
  std::set<std::string> multi_selection;
  explicit MyTreeView(QWidget *p = 0): is_focused(false), marked(false), path(""), QTreeView(p) {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  std::string getSelected();
  void unFocus();
  void focus();
  void mark(bool);
  void changeSelection();
  inline int getSelIdx();
signals:
  void focused();
  void unfocused();
  void stepup();
};

#endif // MYTREEVIEW_H
