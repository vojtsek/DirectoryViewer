
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
  enum {COPY, MOVE, VIEW, RENAME, REMOVE };
  int cmd;
} cmd_info_T;

class MyViewType{
public:
  std::set<std::string> multi_selection;
  bool is_focused, marked;
  std::string path;
  int w;
  virtual std::string getSelected() = 0;
  virtual int getSelIdx() = 0;
  virtual void setFocus() = 0;
  virtual void unFocus() = 0;
  virtual void focus() = 0;
  virtual void mark(bool) = 0;
  virtual void die() = 0;
  virtual QWidget *getContent() = 0;
  MyViewType(): is_focused(false), marked(false), path("") {}
  virtual ~MyViewType() {}
};

class AbstractView{
public:
  MyViewType *content;
  std::string path, pattern;
  OSInterface *osi;
  int col_width;
  bool recursive;
  AbstractView(std::string, std::string);
 // virtual AbstractView *clone() = 0;
  virtual void rebuild(std::string, std::string) = 0;
  //virtual void init(std::string, std::string, bool);
  virtual ~AbstractView(){
    delete content;
  }
};

class MTree : public AbstractView{
public:
  void buildTree(std::string, QTreeWidgetItem *it, bool);
  void init(std::string, std::string, bool);
  //virtual AbstractView *clone();
  virtual void rebuild(std::string, std::string);
  MTree(std::string, std::string, bool);
  virtual ~MTree() {}
};

class MIcon : public AbstractView{
public:
  void init(std::string, std::string);
  //virtual AbstractView *clone();
  virtual void rebuild(std::string, std::string);
  MIcon(std::string, std::string);
  virtual ~MIcon() {}
};

class MView : public AbstractView{
public:
  void init(std::string, std::string pat = "");
  //virtual AbstractView *clone();
  virtual void rebuild(std::string, std::string pat = "");
  MView(std::string, std::string pat = "");
  virtual ~MView() {}
};
#endif //TYPES_H
