
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
  bool is_focused, marked, recursive;
  std::string path, pattern;
  OSInterface *osi;
  int w, col_width, index;
  virtual std::string getSelected() = 0;
  virtual void rebuild(int idx = 0) = 0;
  virtual int getSelIdx() = 0;
  virtual int getIdxOf(std::string &) = 0;
  virtual void setFocus() = 0;
  virtual void unFocus() = 0;
  virtual void focus() = 0;
  virtual void mark(bool) = 0;
  virtual void die() = 0;
  MyViewType(std::string p, std::string pat): is_focused(false), marked(false), path(p), pattern(pat) {
    osi = nullptr;
  }
  virtual ~MyViewType() {}
};

#endif //TYPES_H
