#ifndef OPENEDLISTHANDLE_H
#define OPENEDLISTHANDLE_H

#include "mainhandler.h"
#include "osinterface.h"
#include "mylineedit.h"
#include "mytreeview.h"
#include "myiconview.h"

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
#include <QTableWidgetItem>
#include <QListView>
#include <QScrollBar>
#include <QToolBar>
#include <QLabel>
#include <QObject>
#include <QMainWindow>
#include <map>
#include <vector>
#include <string>
#include <iostream>

class MyTreeView;
class MyIconView;

class OpenedListHandle: public QWidget{
  Q_OBJECT
public:
  std::map<Qt::Key, ButtonHandle<OpenedListHandle>> tool_btts;
  std::vector<MyViewType *> to_del;
  QVBoxLayout *v_layout, *v_layout2;
  QHBoxLayout *h_layout1, *h_layout2;
  QGridLayout *g_layout;
  MyLineEdit *le1;
  ButtonHandle<OpenedListHandle> *up_btt;
  QLineEdit *le2;
  QLabel *lbl, *lbl2, *lbl3;
  MyViewType *content;
  QToolBar *tb, *tb2;
  bool in_layout;
  std::string path;
  enum {TREE, LIST, ICON, VIEW};
  int view_type;
  unsigned int size_in;
  enum {B, KB, MB, GB};

  OpenedListHandle() { initLayout(".");}
  OpenedListHandle(std::string, unsigned int, QWidget *parent = 0);
 // OpenedListHandle(const OpenedListHandle &, QWidget *parent = 0);
  void delGraphics();
  void changeLayout(int);
  void highlightBtt();
  void clean();
  void updateLbl();
  void processItem(std::string);
  template <typename T>
  void connectSignals(){
    if(typeid(T) == typeid(MyTreeView)){
        QObject::connect((T *)content, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(itemExpanded(QTreeWidgetItem *)));
        QObject::connect((T *)content, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(itemActivated(QTreeWidgetItem*,int)));
      }else if(typeid(T) == typeid(MyIconView)){
        QObject::connect((T *)content, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(itemActivated(QTableWidgetItem *)));
      }
    QObject::connect((T *)content, SIGNAL(rebuilded()), this, SLOT(rebuildContent()));
    QObject::connect((T *)content, SIGNAL(chlayout()), this, SLOT(chlayout()));
    QObject::connect((T *)content, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    QObject::connect((T *)content, &T::stepup, this, &OpenedListHandle::stepUp);
  }

  void initLayout(std::string);
  virtual ~OpenedListHandle();

signals:
  void updated();
public slots:
  void toTree();
  void toList();
  void toIcon();
  void chlayout();
  void stepUp();
  void patternChanged();
  void pathChanged();
  void setSelection(bool);
  void rebuildContent();
  //void itemPressed(QTreeWidgetItem *item);
  void itemExpanded(QTreeWidgetItem *item);
  void selectionChanged();
  void itemActivated(QTreeWidgetItem *item, int col );
  void itemActivated(QTableWidgetItem *item);
};


#endif // OPENEDLISTHANDLE_H
