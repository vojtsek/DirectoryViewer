
#include "openedlisthandle.h"
#include "mainhandler.h"
#include "mylineedit.h"
#include "osinterface.h"
#include "stylesheets.h"
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
#include <QStandardItemModel>
#include <QToolBar>
#include <QModelIndex>
#include <QObject>
#include <map>
#include <string>
#include <iostream>

void OpenedListHandle::changeLayout(int type){
  view_type = type;
  bool was_focused = content->is_focused;
  if((view_type == TREE) || (view_type == LIST))
    QObject::disconnect((MyTreeView *)content, 0, this, 0);
  delete ((MyTreeView *) view->content)->model();
  switch (type) {
    case TREE:      
      ((MTree *) view)->init(path, le2->text().toStdString(), true);
      content = view->content;
      h_layout2->addWidget((MyTreeView *)content);
      break;
     case LIST:
      ((MTree *) view)->init(path, le2->text().toStdString(), false);
      content = view->content;
      h_layout2->addWidget((MyTreeView *)content);
      break;
    default:
      break;
    }
  if(was_focused){
    content->setFocus();
    content->focus();
    }
  if((view_type == TREE) || (view_type == LIST)){
      QObject::connect((MyTreeView *)content, SIGNAL(activated(const QModelIndex &)), this, SLOT(itemPressed(const QModelIndex &)));
      QObject::connect((MyTreeView *)content, &MyTreeView::stepup, this, &OpenedListHandle::stepUp);
    }
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
  content->setFocus();
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


void OpenedListHandle::itemClicked( QTreeWidgetItem *item, int col ) {
  std::cout << "Item Text: " << item->text( col ).toStdString();
}

void OpenedListHandle::itemPressed(const QModelIndex &mi){
  if(((MTree*)view)->recursive){
      std::string p;
      //std::cout <<  ((QStandardItemModel *)(((MyTreeView *)content)->model()))->item(((MyTreeView*)content)->currentIndex().row())->text().toStdString() << std::endl;
      QStandardItem *it = ((QStandardItemModel *)(((MyTreeView *)content)->model()))->item(mi.row(),mi.column());
      QStandardItem *par = it->parent();
      while(par){
          p = OSInterface::dir_sep + par->text().toStdString();
          par = par->parent();
        }
      p = content->path + OSInterface::dir_sep + it->text().toStdString() + OSInterface::dir_sep + p;
      for(int r = 0; r < it->rowCount(); ++r)
        {
          QStandardItem *chi = it->child(r, 0);
      //    std::cout << chi->text().toStdString() << std::endl;
          ((MTree *)view)->buildTree( p + OSInterface::dir_sep + chi->text().toStdString(), chi);
          //((MyTreeView *)content)->update();
        }
    }else{
      std::string new_path = content->getSelected();
      if(OSInterface::isDir(new_path)){
          if(le1->text().toStdString() == OSInterface::getPrefix())
            new_path = new_path.substr(1, new_path.size());
          le1->setText(QString::fromStdString(new_path)); //signal
        }
    }
}

void OpenedListHandle::initLayout(std::string p, MTree *tree){
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
  up_btt->btt->setStyleSheet(btt_style);
  le1 = new MyLineEdit();
  le1->setFocusPolicy(Qt::ClickFocus);
  le1->setStyleSheet(lineedit_style);
  le2 = new QLineEdit();
  le2->setStyleSheet(lineedit_style);
  le2->setFocusPolicy(Qt::ClickFocus);
  le1->setText(QString::fromStdString(p));
  le2->setMaximumWidth(70);
  le2->setText("*");
  tb = new QToolBar();
  view_type = LIST;
  if(!tree)
      view = new MTree(p,le2->text().toStdString(), false);
  else
    view = tree;
  content = view->content;
  if((view_type == TREE) || (view_type == LIST))
    QObject::connect((MyTreeView *)content, &MyTreeView::stepup, this, &OpenedListHandle::stepUp);
  tb = new QToolBar();
  h_layout1->addWidget(le1);
  h_layout1->addWidget(le2);
  h_layout1->addWidget(up_btt->btt);
  QObject::connect(up_btt->btt, &QPushButton::clicked, this, up_btt->fnc);
  QObject::connect(le2, &QLineEdit::textChanged, this, &OpenedListHandle::patternChanged);
  QObject::connect(le1, &QLineEdit::textChanged, this, &OpenedListHandle::pathChanged);
  QObject::connect(le1, &MyLineEdit::focused, this, &OpenedListHandle::setSelection);
  if((view_type == TREE) || (view_type == LIST)){
    QObject::connect((MyTreeView *)content, SIGNAL(activated(const QModelIndex &)), this, SLOT(itemPressed(const QModelIndex &)));
    QObject::connect( (MyTreeView *)content, SIGNAL( itemClicked( QTreeWidgetItem *, int ) ), this, SLOT( itemClicked( QTreeWidgetItem *, int )));

    h_layout2->addWidget((MyTreeView *)content);
    }
  for(auto &a : tool_btts){
      a.second.btt->setMaximumWidth(150);
      a.second.btt->setFocusPolicy(Qt::NoFocus);
      a.second.btt->setStyleSheet(btt_style);
        tb->addWidget(a.second.btt);
        QObject::connect(a.second.btt, &QPushButton::clicked, this, a.second.fnc);
    }
  h_layout3->addWidget(tb);
  v_layout->addLayout(h_layout1);
  v_layout->addLayout(h_layout2);
  v_layout->addLayout(h_layout3);
}

OpenedListHandle::OpenedListHandle(std::string p, QWidget *parent):QWidget(parent),in_layout(false), view_type(LIST){
    path = p;
    initLayout(p);
  }

void OpenedListHandle::delGraphics(){
  delete le1;
  delete le2;
  delete view;
  delete up_btt->btt;
  for(auto &a : tool_btts){
      delete a.second.btt;
    }
  delete tb;
  delete h_layout1;
  delete h_layout2;
  delete h_layout3;
  delete v_layout;
}

OpenedListHandle::~OpenedListHandle(){
  delGraphics();
}

OpenedListHandle::OpenedListHandle(const OpenedListHandle &o, QWidget *parent): QWidget(parent){
  in_layout = false;
  view_type = o.view_type;
  path = o.path;
  MTree *new_view = (MTree *) o.view->clone();
  initLayout(path, new_view);
  content->is_focused = o.content->is_focused;
  content->marked = o.content->marked;
  if(content->is_focused)
    content->setFocus();
}



