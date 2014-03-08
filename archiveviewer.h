#ifndef ARCHIVEVIEWER_H
#define ARCHIVEVIEWER_H

#include "types.h"
#include <QTreeWidget>
#include <string>
#include <vector>

class ArchiveViewer : public QTreeWidget, public MyViewType
{
  Q_OBJECT
public:
  explicit ArchiveViewer(QWidget *p = 0): QTreeWidget(p), MyViewType("", "") {
    setSelectionBehavior(QAbstractItemView::SelectRows);
  }
  ArchiveViewer(std::string pa, QWidget *p = 0): QTreeWidget(p), MyViewType(pa, "*") {
    setColumnCount(3);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    rebuild();
  }
  std::vector<std::string> items;
  virtual ~ArchiveViewer() {}
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void resizeEvent(QResizeEvent *);
  virtual void rebuild();
  virtual std::string getSelected();
  virtual int getSelIdx();
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  virtual QWidget *getContent();
  void readArch();
  void buildTree(QTreeWidgetItem *, int lvl);
  virtual void die();
signals:
  void rebuilded();
  void focused();
  void unfocused();
  void stepup();
  void chlayout();
};

#endif // ARCHIVEVIEWER_H
