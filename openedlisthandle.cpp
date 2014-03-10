#include "openedlisthandle.h"
#include "mainhandler.h"
#include "functions.h"
#include "mylineedit.h"
#include "myiconview.h"
#include "mytreeview.h"
#include "archiveviewer.h"
#include "myviewer.h"
#include "osinterface.h"
#include "stylesheets.h"
#include "types.h"

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <unistd.h>
#include <wait.h>
#include <QLineEdit>
#include <QKeyEvent>
#include <QToolBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QThread>
#include <QObject>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <sstream>

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

void OpenedListHandle::clean(){
  for(auto a : to_del){
    delete a;
    }
  to_del.clear();
}

void OpenedListHandle::changeLayout(int type){
  clean();
  to_del.push_back(content);
  content->die();
  view_type = type;
  std::set<std::string> sel = content->multi_selection;
  bool was_focused = content->is_focused;
  bool was_marked = content->marked;
  QWidgetItem *myItem;
  myItem = dynamic_cast <QWidgetItem*>(h_layout2->itemAt(0));
  h_layout2->removeWidget(myItem->widget());
  if((view_type == TREE) || (view_type == LIST))
    QObject::disconnect((MyTreeView *)content, 0, this, 0);
  switch (type) {
  case TREE:
      content = new MyTreeView(path, le2->text().toStdString(), true, content->getSelIdx());
      h_layout2->addWidget((MyTreeView *)content);
      break;
  case LIST:
      content = new MyTreeView(path, le2->text().toStdString(), false, content->getSelIdx());
      h_layout2->addWidget((MyTreeView *)content);
      break;
  case ICON:
      content = new MyIconView(path, le2->text().toStdString(), content->getSelIdx());
      h_layout2->addWidget((MyIconView *) content);
      break;
  case VIEW:
      content = new MyViewer(path, le2->text().toStdString(), content->getSelIdx());
      h_layout2->addWidget((MyViewer *)content);
      break;
  case ARCHIVE:
      content = new ArchiveViewer(path, content->getSelIdx());
      h_layout2->addWidget((ArchiveViewer *)content);
      break;
    }
  highlightBtt();
  if(was_marked)
    content->mark(true);
  content->multi_selection = sel;
  if(was_focused){
    content->setFocus();
    content->focus();
    }
  if((view_type == TREE) || (view_type == LIST))
    connectSignals<MyTreeView>();
  else if (view_type == ICON)
    connectSignals<MyIconView>();
  else if (view_type == VIEW)
    connectSignals<MyViewer>();
  else if (view_type == ARCHIVE)
    connectSignals<ArchiveViewer>();
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
}

void OpenedListHandle::patternChanged(){
  changeLayout(view_type);
}

void OpenedListHandle::stepUp(){
  int it;
  std::string cont = le1->text().toStdString();
  if(cont == OSInterface::getPrefix()) return;
  cont = cont.substr(0, cont.size() - 1);
  if((it = cont.find_last_of(OSInterface::dir_sep)) == std::string::npos) return;
  cont = cont.substr(0, it + 1);
  if(!cont.empty())
    le1->setText(QString::fromStdString(cont));
  else
    le1->setText(QString::fromStdString(OSInterface::getPrefix()));
  content->setFocus();
}

void OpenedListHandle::pathChanged(){
  if(OSInterface::isDir(le1->text().toStdString())){
      path = le1->text().toStdString();
      content->path = path;
    content->rebuild();
    }
  if(view_type == VIEW) changeLayout(LIST);
}

void OpenedListHandle::setSelection(bool in){
  if(in){
      int it;
      std::string cont = le1->text().toStdString();
      if(cont == OSInterface::getPrefix()) return;
      if((it = cont.find_last_of(OSInterface::dir_sep)) == std::string::npos) return;
      le1->setSelection(it + 1,cont.size() - it );
    }else{
      le1->setText(QString::fromStdString(path));
    }
}


/*void OpenedListHandle::itemClicked( QTreeWidgetItem *item, int col ) {
  std::cout << "Item Text: " << item->text( col ).toStdString();
}*/

void OpenedListHandle::rebuildContent(){
  if(view_type == ICON){
      content->rebuild(content->getSelIdx());
    }
  tb2->setMaximumWidth(content->w);
}

void OpenedListHandle::processItem(std::string new_path){
  if(OSInterface::isDir(new_path)){
      if(le1->text().toStdString() == OSInterface::getPrefix()){
          int pos;
          std::string doubled;
          doubled.push_back(OSInterface::dir_sep);
          doubled.push_back(OSInterface::dir_sep);
          while((pos = new_path.find(doubled)) != std::string::npos){
              new_path = new_path.substr(0, pos) + new_path.substr(pos + 1, new_path.size());
          }
      }
      le1->setText(QString::fromStdString(new_path)); //signal
  }else if(getExtension(new_path) == "pdf"){
      pid_t pid;
      switch(pid = fork()){
      default:
          childs.push_back(pid);
          break;
      case 0:
          std::string cmd("/bin/mupdf");
          execl(cmd.c_str(), getBasename(cmd).c_str(), new_path.c_str());
          break;
      }
  }else if(getExtension(new_path) == "avi"){
    pid_t pid;
    switch(pid = fork()){
    default:
        childs.push_back(pid);
        break;
    case 0:
        std::string cmd("/bin/vlc");
        execl(cmd.c_str(), getBasename(cmd).c_str(), new_path.c_str());
        break;
    }
    }else if((OSInterface::isOpenable(new_path) && (!isArch(new_path)))){
      std::string old = path;
      path = new_path;
      changeLayout(VIEW);
      path = old;
  }else if(isArch(new_path)){
      std::string old = path;
      path = new_path;
      changeLayout(ARCHIVE);
      path = old;
  }
}

void OpenedListHandle::itemActivated(QTableWidgetItem *item){
      std::string new_path = content->getSelected();
      processItem(new_path);
}

void OpenedListHandle::selectionChanged(){
  updateLbl();
}

void OpenedListHandle::itemActivated(QTreeWidgetItem *item, int col){
  if(view_type == TREE){
      item->setExpanded(!item->isExpanded());
    }else{
      std::string new_path = content->getSelected();
      processItem(new_path);
    }
}

void OpenedListHandle::itemExpanded(QTreeWidgetItem *it){
  std::string p, tmp, path;
  QTreeWidgetItem *par = it->parent();
  while(par){
      p = par->text(0).toStdString() + OSInterface::dir_sep + p;
      par = par->parent();
    }
  tmp = p;
  p = content->path;
  if (content->path != OSInterface::getPrefix())
    p = p + OSInterface::dir_sep;
  if(!tmp.empty())
    p = p + tmp + OSInterface::dir_sep + it->text(0).toStdString();
  else
    p = p + it->text(0).toStdString();
  for(int r = 0; r < it->childCount(); ++r)
    {
      QTreeWidgetItem *chi = it->child(r);
      if((chi == nullptr) || (chi->childCount())) continue;
      path = p + OSInterface::dir_sep + chi->text(0).toStdString();
      if(OSInterface::isDir(path))
        ((MyTreeView *) content)->buildTree(path, chi, false);
    }
}

void OpenedListHandle::updateLbl(){
  std::stringstream ss;
  std::string file = content->getSelected();
  if(file.empty()) return;
  ss << getBasename(file);
  lbl->setText(QString::fromStdString(ss.str()));
  ss.str("");
  ss.clear();
  if((view_type == LIST) || ( view_type == ICON)){
      dirEntryT *t = content->osi->dirs[content->getSelIdx()];
      if(t != nullptr)
        ss << round((t->byte_size / pow(1024, size_in)));
      switch(size_in){
        case B:
          ss << " B";
          break;
        case KB:
          ss << " KB";
          break;
        case MB:
          ss << " MB";
          break;
        case GB:
          ss << " GB";
          break;
        }
      lbl2->setText(QString::fromStdString(ss.str()));
      ss.str("");
      ss.clear();
      ss << t->perms << " ";
      ss << t->mod_time;
    }
  lbl3->setText(QString::fromStdString(ss.str()));
}

void OpenedListHandle::initLayout(std::string p){
  v_layout = new QVBoxLayout();
  h_layout1 = new QHBoxLayout();
  h_layout2 = new QHBoxLayout();
  g_layout = new QGridLayout();
  v_layout2 = new QVBoxLayout();
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
  view_type = LIST;
  content = new MyTreeView(p, le2->text().toStdString(), false);
  tb = new QToolBar();
  lbl = new QLabel();
  lbl2 = new QLabel();
  lbl3 = new QLabel();
  tb = new QToolBar();
  tb2 = new QToolBar();
  tb2->addWidget(lbl);
  tb2->addSeparator();
  tb2->addWidget(lbl2);
  tb2->addSeparator();
  tb2->addWidget(lbl3);
  updateLbl();
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
      h_layout2->addWidget((MyIconView *)content);
    }
  for(auto &a : tool_btts){
      a.second.btt->setMaximumWidth(150);
      a.second.btt->setFocusPolicy(Qt::NoFocus);
      a.second.btt->setStyleSheet(btt_style);
      tb->addWidget(a.second.btt);
      QObject::connect(a.second.btt, &QPushButton::clicked, this, a.second.fnc);
    }
  v_layout2->addWidget(tb2);
  highlightBtt();
  v_layout2->addWidget(tb);
  v_layout->addLayout(h_layout1);
  v_layout->addLayout(h_layout2);
  v_layout->addLayout(v_layout2);
}

OpenedListHandle::OpenedListHandle(std::string p, unsigned int s, QWidget *parent):QWidget(parent),in_layout(false), view_type(LIST), path(p), size_in(s){
   // path = p;
    initLayout(p);
  }

void OpenedListHandle::delGraphics(){
  delete le1;
  delete le2;
  delete up_btt->btt;
  for(auto &a : tool_btts){
      delete a.second.btt;
    }
  delete tb;
  delete lbl;
  delete lbl2;
  delete lbl3;
  delete tb2;
  delete h_layout1;
  delete g_layout;
  delete h_layout2;
  delete v_layout2;
  delete v_layout;
}

OpenedListHandle::~OpenedListHandle(){
  clean();
  int status;
  if(childs.size()){
      QMessageBox msg_box;
      msg_box.setWindowTitle("Error!");
      msg_box.setText("Please close child windows.");
      msg_box.setStandardButtons(QMessageBox::Ok);
      msg_box.setDefaultButton(QMessageBox::Ok);
      msg_box.exec();
      wait(&status);
      msg_box.close();
  }

  for(auto &a: childs){
      wait(&status);
  }

  delete content;
  delGraphics();
}

/*OpenedListHandle::OpenedListHandle(const OpenedListHandle &o, QWidget *parent): QWidget(parent){
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
}*/
