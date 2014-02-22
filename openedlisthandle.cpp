
#include "openedlisthandle.h"
#include "mainhandler.h"
#include "mylineedit.h"
#include "osinterface.h"
#include "types.h"

#include <dirent.h>
#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QTreeView>
#include <QListView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QToolBar>
#include <QModelIndex>
#include <QObject>
#include <map>
#include <string>
#include <iostream>

void OpenedListHandle::changeLayout(int type){
  view_type = type;
  QObject::disconnect(content, 0, this, 0);
  view->content->deleteLater();
  switch (type) {
    case TREE:      
      ((MTree *) view)->init(path, le2->text().toStdString(), true);
      content = view->content;
      h_layout2->removeWidget(sb);
      h_layout2->addWidget(content);
      h_layout2->addWidget(sb);
      break;
     case LIST:
      ((MTree *) view)->init(path, le2->text().toStdString(), false);
      content = view->content;
      h_layout2->removeWidget(sb);
      h_layout2->addWidget(content);
      h_layout2->addWidget(sb);
      break;
    default:
      break;
    }
  if(content->is_focused)
    content->setFocus();
  QObject::connect(content, SIGNAL(activated(const QModelIndex &)), this, SLOT(itemPressed(const QModelIndex &)));
  //v_layout->update();
}

void OpenedListHandle::toTree(){
  changeLayout(TREE);
}

void OpenedListHandle::toIcon(){
  changeLayout(ICON);
}

void OpenedListHandle::toList(){
  changeLayout(LIST);
}

void OpenedListHandle::patternChanged(){
  changeLayout(view_type);
}

void OpenedListHandle::stepUp(){
  int it;
  std::string cont = le1->text().toStdString();
  if(cont == OSInterface::getPrefix()) return;
  if((it = cont.find_last_of(OSInterface::dir_sep)) == -1) return;
  cont = cont.substr(0, it);
  if(!cont.empty())
    le1->setText(QString::fromStdString(cont));
  else
    le1->setText(QString::fromStdString(OSInterface::getPrefix()));
}

void OpenedListHandle::pathChanged(){
  if(OSInterface::isDir(le1->text().toStdString())){
      path = le1->text().toStdString();
    changeLayout(view_type);
    }
}

void OpenedListHandle::setSelection(bool in){
  if(in){
      int it;
      std::string cont = le1->text().toStdString();
      if(cont == OSInterface::getPrefix()) return;
      if((it = cont.find_last_of(OSInterface::dir_sep)) == -1) return;
      le1->setSelection(it,cont.size()-it);
    }else{
      le1->setText(QString::fromStdString(path));
    }
}

void OpenedListHandle::itemPressed(const QModelIndex &mi){
  if(((MTree*)view)->recursive) return;
  QStandardItem *item = ((QStandardItemModel*)((QTreeView*) content)->model())->item(mi.row());
  if(OSInterface::isDir(path + OSInterface::dir_sep + item->text().toStdString())){
      if(path[path.size() - 1] == '/')
        path = path.substr(0, path.size() - 1);
      path = path + OSInterface::dir_sep + item->text().toStdString();
      le1->setText(QString::fromStdString(path)); //signal
    }
}



void OpenedListHandle::initLayout(std::string p){
  v_layout = new QVBoxLayout();
  h_layout1 = new QHBoxLayout();
  h_layout2 = new QHBoxLayout();
  h_layout3 = new QHBoxLayout();
  tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F2, ButtonHandle<OpenedListHandle>(Qt::Key_F2, "Tree view", &OpenedListHandle::toTree)));
  tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F3, ButtonHandle<OpenedListHandle>(Qt::Key_F3, "List view", &OpenedListHandle::toList)));
  tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F4, ButtonHandle<OpenedListHandle>(Qt::Key_F4, "Icon view", &OpenedListHandle::toIcon)));
  up_btt = new ButtonHandle<OpenedListHandle>(Qt::Key_F10, "UP", &OpenedListHandle::stepUp);
  up_btt->btt->setMaximumWidth(30);
  up_btt->btt->setFocusPolicy(Qt::ClickFocus);
  le1 = new MyLineEdit();
  le1->setFocusPolicy(Qt::ClickFocus);
  le2 = new QLineEdit();
  le2->setFocusPolicy(Qt::ClickFocus);
  le1->setText(QString::fromStdString(p));
  le2->setMaximumWidth(70);
  le2->setText("*");
  view = new MTree(p,le2->text().toStdString(), false);
  view_type = LIST;
  content = view->content;
 // QObject::connect(content, &MyTreeView::focused, this, &OpenedListHandle::focusedNew);
 // QObject::connect(content, &MyTreeView::unfocused, this, &OpenedListHandle::focusedNew);
  tb = new QToolBar();
  sb = new QScrollBar();
  h_layout1->addWidget(le1);
  h_layout1->addWidget(le2);
  h_layout1->addWidget(up_btt->btt);
  QObject::connect(up_btt->btt, &QPushButton::clicked, this, up_btt->fnc);
  QObject::connect(le2, &QLineEdit::textChanged, this, &OpenedListHandle::patternChanged);
  QObject::connect(le1, &QLineEdit::textChanged, this, &OpenedListHandle::pathChanged);
  QObject::connect(le1, &MyLineEdit::focused, this, &OpenedListHandle::setSelection);
  QObject::connect(content, SIGNAL(activated(const QModelIndex &)), this, SLOT(itemPressed(const QModelIndex &)));
  h_layout2->addWidget(content);
  h_layout2->addWidget(sb);
  for(auto &a : tool_btts){
      a.second.btt->setMaximumWidth(150);
      a.second.btt->setFocusPolicy(Qt::NoFocus);
        h_layout3->addWidget(a.second.btt);
        QObject::connect(a.second.btt, &QPushButton::clicked, this, a.second.fnc);
    }
  v_layout->addLayout(h_layout1);
  v_layout->addLayout(h_layout2);
  v_layout->addLayout(h_layout3);
}

OpenedListHandle::OpenedListHandle(std::string p, QWidget *parent):QWidget(parent),in_layout(false), view_type(LIST){
    path = p;
    initLayout(p);
  }

void OpenedListHandle::delGraphics(){
  delete sb;
  delete tb;
  delete le1;
  delete le2;
  delete view;
  delete up_btt->btt;
  for(auto &a : tool_btts){
      delete a.second.btt;
    }
  delete h_layout1;
  delete h_layout2;
  delete h_layout3;
  delete v_layout;
}

OpenedListHandle::~OpenedListHandle(){
  delGraphics();
}

OpenedListHandle::OpenedListHandle(const OpenedListHandle &o, QWidget *parent): QWidget(parent){
  in_layout = false; //musi se pridat znovu
  view_type = o.view_type;
  path = o.path;
  initLayout(path);
  content->is_focused = o.content->is_focused;
  content->marked = o.content->marked;
  if(content->is_focused)
    content->setFocus();
}



