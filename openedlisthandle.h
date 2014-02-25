#ifndef OPENEDLISTHANDLE_H
#define OPENEDLISTHANDLE_H

#include "mainhandler.h"
#include "osinterface.h"
#include "mylineedit.h"

#include <dirent.h>
#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QListView>
#include <QScrollBar>
#include <QToolBar>
#include <QObject>
#include <QMainWindow>
#include <map>
#include <string>
#include <iostream>

class OpenedListHandle: public QWidget{
  Q_OBJECT
public:
  std::map<Qt::Key, ButtonHandle<OpenedListHandle>> tool_btts;
  QVBoxLayout *v_layout;
  QHBoxLayout *h_layout1, *h_layout2, *h_layout3;
  MyLineEdit *le1;
  ButtonHandle<OpenedListHandle> *up_btt;
  QLineEdit *le2;
  MyViewType *content;
  AbstractView *view;
  QToolBar *tb;
  QScrollBar *sb;
  bool in_layout;
  std::string path;
  enum {TREE, LIST, ICON };
  int view_type;

  OpenedListHandle() { initLayout(".");}
  OpenedListHandle(std::string, QWidget *parent = 0);
  OpenedListHandle(const OpenedListHandle &, QWidget *parent = 0);
  void delGraphics();
  void changeLayout(int);
  template <typename T>
  void connectSignals(){
    QObject::connect((T *)content, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(itemExpanded(QTreeWidgetItem *)));
    QObject::connect((T *)content, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(itemActivated(QTreeWidgetItem*,int)));
    QObject::connect((T *)content, SIGNAL(chlayout()), this, SLOT(chlayout()));
    QObject::connect((T *)content, &T::stepup, this, &OpenedListHandle::stepUp);
  }

  void initLayout(std::string, AbstractView *tree = 0);
  virtual ~OpenedListHandle();

public slots:
  void toTree();
  void toList();
  void toIcon();
  void chlayout();
  void stepUp();
  void patternChanged();
  void pathChanged();
  void setSelection(bool);
  //void itemPressed(QTreeWidgetItem *item);
  void itemExpanded(QTreeWidgetItem *item);
  void itemActivated(QTreeWidgetItem *item, int col );
};


#endif // OPENEDLISTHANDLE_H
