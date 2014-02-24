
#include "mainhandler.h"
#include "mytreeview.h"
#include "osinterface.h"

#include <dirent.h>
#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QStandardItem>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QGridLayout>
#include <QLineEdit>
#include <QScrollBar>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QObject>
#include <map>
#include <set>
#include <string>

#ifndef TYPES_H
#define TYPES_H


class MainHandler;
class OSInterface;

template <class T>
struct ButtonHandle{
  QAbstractButton *btt;
  Qt::Key keycode;
  std::string label;
  void (T::*fnc)(void);
  ButtonHandle(Qt::Key k, std::string l, void (T::*f)(void)): keycode (k), label(l){ btt = new QPushButton(QString::fromStdString(l)); fnc = f;}
};

typedef struct{
  std::string src_path;
  std::set<std::string> source_files, paths;
  std::map<std::string,std::set<std::string>> destination_files;
  enum {COPY, MOVE, VIEW };
  int cmd;
} cmd_info_T;

class MyViewType{
public:
  std::set<std::string> multi_selection;
  bool is_focused, marked;
  std::string path;
  virtual std::string getSelected() = 0;
  virtual int getSelIdx() = 0;
  virtual void setFocus() = 0;
  virtual void unFocus() = 0;
  virtual void focus() = 0;
  MyViewType(): is_focused(false), marked(false), path("") {}
  virtual ~MyViewType() {}
};

class AbstractView{
public:
  MyViewType *content;
  std::string path, pattern;
  OSInterface * osi;
  AbstractView(std::string, std::string);
  virtual AbstractView *clone() = 0;
  virtual ~AbstractView(){
    delete content;
  }
};

class MTree : public AbstractView{
public:
  void buildTree(std::string, QTreeWidgetItem *it);
  bool recursive;
  void init(std::string, std::string, bool);
  virtual AbstractView *clone();
  MTree(std::string, std::string, bool);
  virtual ~MTree() {}
};


#endif //TYPES_H
