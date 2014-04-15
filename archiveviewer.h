#ifndef ARCHIVEVIEWER_H
#define ARCHIVEVIEWER_H

#include "types.h"
#include <QTreeWidget>
#include <string>
#include <vector>

class ArchiveViewer : public QTreeWidget, public MyViewType
{
    int idx;
  Q_OBJECT
public:
  explicit ArchiveViewer(QWidget *p = 0): QTreeWidget(p), MyViewType("", "", nullptr) {
    setSelectionBehavior(QAbstractItemView::SelectRows);
  }
  ArchiveViewer(std::string pa, int i = 0, QWidget *p = 0): QTreeWidget(p), MyViewType(pa, "*", nullptr), idx(i) {
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
  virtual void rebuild(int idx = 0);
  virtual std::string getSelected();
  virtual int getSelIdx();
  virtual int getIdxOf(std::string &) { return 0; }
  virtual void unFocus();
  virtual void setFocus();
  virtual void focus();
  virtual void mark(bool);
  void readArch();
  void buildTree(QTreeWidgetItem *,unsigned int);
  virtual void die();
signals:
  void rebuilded();
  void focused();
  void unfocused();
  void stepup();
  void chlayout();
  void refresh();
};

#endif // ARCHIVEVIEWER_H
