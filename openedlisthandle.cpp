#include "openedlisthandle.h"
#include "mainhandler.h"
#include "mylineedit.h"
#include "myiconview.h"
#include "osinterface.h"
#include "stylesheets.h"
#include "types.h"

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QKeyEvent>
#include <QToolBar>
#include <QModelIndex>
#include <QObject>
#include <map>
#include <string>
#include <iostream>

void OpenedListHandle::highlightBtt(){
  for(auto &a : tool_btts){
      a.second.btt->setStyleSheet(btt_style);
      if((view_type == TREE) && (a.second.label == "Tree view"))
        a.second.btt->setStyleSheet(marked_btt_style);
      if((view_type == ICON) && (a.second.label == "Icon view"))
        a.second.btt->setStyleSheet(marked_btt_style);
      if((view_type == LIST) && (a.second.label == "List view"))
        a.second.btt->setStyleSheet(marked_btt_style);
    }
}

void OpenedListHandle::changeLayout(int type){
  view_type = type;
  bool was_focused = content->is_focused;
  bool was_marked = content->marked;
  if((view_type == TREE) || (view_type == LIST))
    QObject::disconnect((MyTreeView *)content, 0, this, 0);
  //delete ((MyTreeView *) view->content)->model();
  switch (type) {
    case TREE:
      delete view;
      view = new MTree(path, le2->text().toStdString(), true);
      content = view->content;
      h_layout2->addWidget((MyTreeView *)content);
      break;
     case LIST:
      delete view;
      view = new MTree(path, le2->text().toStdString(), false);
      content = view->content;
      h_layout2->addWidget((MyTreeView *)content);
      break;
    case ICON:
      delete view;
      view = new MIcon(path, le2->text().toStdString());
      view->rebuild(path, le2->text().toStdString());
      content = view->content;
      h_layout2->addWidget((MyIconView *)content);
      ((MyIconView *) content)->resizeColumnsToContents();
      view->rebuild(path, le2->text().toStdString());
    default:
      break;
    }
  highlightBtt();
  if(was_marked)
    content->mark(true);
  if(was_focused){
    content->setFocus();
    content->focus();
    }
  if((view_type == TREE) || (view_type == LIST))
    connectSignals<MyTreeView>();
  else if (view_type == ICON)
    connectSignals<MyIconView>();
  emit(updated());
  h_layout2->update();
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

void OpenedListHandle::chlayout(){
  if(view_type == LIST)
    changeLayout(TREE);
  else if(view_type == TREE)
    changeLayout(ICON);
  else
    changeLayout(LIST);
  //view->rebuild(path, "*");
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
    view->rebuild(path, "*");
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


/*void OpenedListHandle::itemClicked( QTreeWidgetItem *item, int col ) {
  std::cout << "Item Text: " << item->text( col ).toStdString();
}*/

void OpenedListHandle::itemActivated(QTableWidgetItem *item){
      std::string new_path = content->getSelected();
      if(OSInterface::isDir(new_path)){
          if(le1->text().toStdString() == OSInterface::getPrefix())
            new_path = new_path.substr(1, new_path.size());
          le1->setText(QString::fromStdString(new_path)); //signal
        }
}

void OpenedListHandle::itemActivated(QTreeWidgetItem *item, int col){
  if(view_type == TREE){
      item->setExpanded(!item->isExpanded());
    }else{
      std::string new_path = content->getSelected();
      if(OSInterface::isDir(new_path)){
          if(le1->text().toStdString() == OSInterface::getPrefix())
            new_path = new_path.substr(1, new_path.size());
          le1->setText(QString::fromStdString(new_path)); //signal
        }
    }
}

void OpenedListHandle::itemExpanded(QTreeWidgetItem *it){
  std::string p, path;
  QTreeWidgetItem *par = it->parent();
  while(par){
      p = par->text(0).toStdString() + OSInterface::dir_sep + p;
      par = par->parent();
    }
  p = content->path + OSInterface::dir_sep + p + OSInterface::dir_sep + it->text(0).toStdString();
  for(int r = 0; r < it->childCount(); ++r)
    {
      QTreeWidgetItem *chi = it->child(r);
      path = p + OSInterface::dir_sep + chi->text(0).toStdString();
      if(OSInterface::isDir(path))
        ((MTree *)view)->buildTree(path, chi, false);
    }
}

void OpenedListHandle::initLayout(std::string p, AbstractView *tree){
  v_layout = new QVBoxLayout();
  h_layout1 = new QHBoxLayout();
  h_layout2 = new QHBoxLayout();
  h_layout3 = new QHBoxLayout();
  tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F2, ButtonHandle<OpenedListHandle>(Qt::Key_F2, "List view", &OpenedListHandle::toList)));
  tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F3, ButtonHandle<OpenedListHandle>(Qt::Key_F3, "Tree view", &OpenedListHandle::toTree)));
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
  tb = new QToolBar();
  h_layout1->addWidget(le1);
  h_layout1->addWidget(le2);
  h_layout1->addWidget(up_btt->btt);
  QObject::connect(up_btt->btt, &QPushButton::clicked, this, up_btt->fnc);
  QObject::connect(le2, &QLineEdit::textChanged, this, &OpenedListHandle::patternChanged);
  QObject::connect(le1, &QLineEdit::textChanged, this, &OpenedListHandle::pathChanged);
  QObject::connect(le1, &MyLineEdit::focused, this, &OpenedListHandle::setSelection);
  if((view_type == TREE) || (view_type == LIST)){
    connectSignals<MyTreeView>();
    h_layout2->addWidget((MyTreeView *)content);
    }else if(view_type == ICON){
      connectSignals<MyIconView>();
      h_layout2->addWidget((MyTreeView *)content);
    }
  for(auto &a : tool_btts){
      a.second.btt->setMaximumWidth(150);
      a.second.btt->setFocusPolicy(Qt::NoFocus);
      a.second.btt->setStyleSheet(btt_style);
      tb->addWidget(a.second.btt);
      QObject::connect(a.second.btt, &QPushButton::clicked, this, a.second.fnc);
    }
  highlightBtt();
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
  AbstractView *new_view;
  if((view_type == LIST) || (view_type == TREE))
    new_view = ((MTree *) o.view)->clone();
  else if(view_type == ICON)
    new_view = ((MIcon *) o.view)->clone();
  initLayout(path, new_view);
  content->is_focused = o.content->is_focused;
  content->marked = o.content->marked;
  content->multi_selection = o.content->multi_selection;
  if(content->is_focused)
    content->setFocus();
}
