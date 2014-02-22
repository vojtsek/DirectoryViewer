
#include "mainhandler.h"
#include "mytreeview.h"

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
#include <QListView>
#include <QScrollBar>
#include <QToolBar>
#include <QObject>
#include <map>
#include <string>
#include "osinterface.h"

#ifndef TYPES_H
#define TYPES_H


class MainHandler;

template <class T>
struct ButtonHandle{
  QAbstractButton *btt;
  Qt::Key keycode;
  std::string label;
  void (T::*fnc)(void);
  ButtonHandle(Qt::Key k, std::string l, void (T::*f)(void)): keycode (k), label(l){ btt = new QPushButton(QString::fromStdString(l)); fnc = f;}
};


class AbstractView{
public:
  MyTreeView *content;
  std::string path, pattern;
  OSInterface osi;
  AbstractView(std::string, std::string);
  virtual ~AbstractView(){
    delete content;
  }
};

/*class MyTableView : public AbstractView{
private:
  std::string path;
public:
  explicit MyTableView(std::string, std::string);
  virtual ~MyTableView() {}
};*/

class MTree : public AbstractView{
private:
  std::string path;
  void buildTree(std::string, QStandardItem *it);
public:
  bool recursive;
  void init(std::string, std::string, bool);
  explicit MTree(std::string, std::string, bool);
  virtual ~MTree() {}
};


#endif //TYPES_H
