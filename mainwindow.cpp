#include "mainwindow.h"
#include "mainhandler.h"
#include "myiconview.h"
#include "ui_mainwindow.h"
#include "osinterface.h"
#include "openedlisthandle.h"
#include "stylesheets.h"

#include <QPushButton>
#include <QComboBox>
#include <typeinfo>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QListView>
#include <QScrollBar>
#include <QToolBar>
#include <QKeyEvent>
#include <QTreeView>
#include <QHeaderView>
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  handler(new MainHandler), focus_idx(0)
{
  ui->setupUi(this);
  prepareLayout();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::confirm1(std::string action, cmd_info_T &info){
  QMessageBox msg_box;
  char buf[10];
  sprintf(buf, "%d", info.source_files.size());
  QString text = QString::fromStdString(action);
  text.append(" selected ");
  text.append(buf);
  text.append(" file(s) \n");
  text.append("From:\n");
  text.append(QString::fromStdString(info.src_path));
  text.append("\nTo:\n");
  for(auto &a : info.paths){
    text.append(QString::fromStdString(a));
    text.append("\n");
    }
  text.append("?");
  msg_box.setWindowTitle("Confirm");
  msg_box.setText(text);
  msg_box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
  msg_box.setDefaultButton(QMessageBox::Yes);
  if(msg_box.exec() == QMessageBox::Yes){
    OSInterface::copy(info);
    }
}

void MainWindow::confirm2(std::string action, cmd_info_T &info){
  QMessageBox msg_box;
  char buf[10];
  sprintf(buf, "%d", info.source_files.size());
  QString text = QString::fromStdString(action);
  text.append(" selected ");
  text.append(buf);
  text.append(" file(s)\n");
  text.append("From:\n");
  text.append(QString::fromStdString(info.src_path));
  text.append("?");
  msg_box.setWindowTitle("Confirm");
  msg_box.setText(text);
  msg_box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
  msg_box.setDefaultButton(QMessageBox::Yes);
  if(msg_box.exec() == QMessageBox::Yes){
    OSInterface::copy(info);
    }
}

void MainWindow::error(std::string &err){
  QMessageBox msg_box;
  msg_box.setWindowTitle("Error!");
  msg_box.setText(QString::fromStdString(err));
  msg_box.setStandardButtons(QMessageBox::Ok);
  msg_box.setDefaultButton(QMessageBox::Ok);
  msg_box.exec();
}

void MainWindow::prepareLayout(){
  QObject::connect(handler, &MainHandler::ch_list, this, &MainWindow::refreshMainLayout);
  QObject::connect(handler, &MainHandler::confirm1, this, &MainWindow::confirm1);
  QObject::connect(handler, &MainHandler::confirm2, this, &MainWindow::confirm2);
  QObject::connect(handler, &MainHandler::error, this, &MainWindow::error);

  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F2, ButtonHandle<MainHandler>(Qt::Key_F2, "F2 Add List", &MainHandler::list_added)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F3, ButtonHandle<MainHandler>(Qt::Key_F3, "F3 Remove List", &MainHandler::list_removed)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F4, ButtonHandle<MainHandler>(Qt::Key_F4, "F4 View", &MainHandler::view)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F5, ButtonHandle<MainHandler>(Qt::Key_F5, "F5 Copy", &MainHandler::copy)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F6, ButtonHandle<MainHandler>(Qt::Key_F6, "F6 Move", &MainHandler::move)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F7, ButtonHandle<MainHandler>(Qt::Key_F7, "F7 Rename", &MainHandler::rename)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F8, ButtonHandle<MainHandler>(Qt::Key_F8, "F8 Remove", &MainHandler::remove)));


  for(auto &a : handler->tool_btts){
      ui->mainToolBar->addWidget(a.second.btt);
      a.second.btt->setFocusPolicy(Qt::ClickFocus);
      a.second.btt->setStyleSheet(btt_style);
      QObject::connect(a.second.btt, &QPushButton::clicked, handler, a.second.fnc);
  }

  for(unsigned int i = 0; i < handler->init_count; ++i){
      handler->opened_lists.emplace_back(handler->init_dir);
    }

  emit(handler->ch_list(false));

  handler->opened_lists[0].content->setFocus();
 /*
  MyTreeView *mtv = new MyTreeView(".", "*");
  mtv->content->move(100, 100);
  mtv->content->setFixedSize(400,400);
  ((QTreeView*)mtv->content)->header()->hide();
  mtv->content->show();
  */

}

void MainWindow::updateFocus(){
  for(auto &a : handler->opened_lists){
    a.content->unFocus();
    }
}


void MainWindow::handleTab(){
  QWidget *first, *second;
  QWidgetItem *myItem;
  first = second = nullptr;
  for(auto &a : handler->opened_lists){
      if(first == nullptr){
          myItem = dynamic_cast <QWidgetItem*>(a.h_layout2->itemAt(0));
          first = myItem->widget();
          continue;
        }
      myItem = dynamic_cast <QWidgetItem*>(a.h_layout2->itemAt(0));
      second = myItem->widget();
      QWidget::setTabOrder(first, second);
      first = second;
    }
  //for (int i = 0; i < layout->count(); ++i) {
}

void MainWindow::refreshMainLayout(bool removing){
  int row, col;
  unsigned int count;
  row = col = count = 0;
  if(removing){
      auto &a = *(handler->opened_lists.end()-1);
      ui->centralGridLayout->removeItem(a.v_layout);
      handler->opened_lists.pop_back();
      try{
        handler->opened_lists.at(handler->opened_lists.size() - 1).content->setFocus();
      }catch(std::exception e) {}
  }
  for(auto &a : handler->opened_lists){
      if(!(col % handler->col_count)){
          col = 0;
          ++row;
        }
      ++count;
      if(!a.in_layout){
          a.in_layout = true;
          if(!col && (count == handler->opened_lists.size()))
            // ui->centralGridLayout->addLayout(a.v_layout, row, col, 1, 2);
            ui->centralGridLayout->addLayout(a.v_layout, row, col, 1, 1);
          else
            ui->centralGridLayout->addLayout(a.v_layout, row, col, 1, 1);
          a.content->setFocus();
          QObject::connect(&a, &OpenedListHandle::updated, this, &MainWindow::handleTab);
          if((a.view_type == a.TREE) || (a.view_type == a.LIST))
            QObject::connect((MyTreeView *)a.content, &MyTreeView::focused, this, &MainWindow::updateFocus);
          else if(a.view_type == a.ICON)
            QObject::connect((MyIconView *)a.content, &MyIconView::tab, this, &MainWindow::handleTab);
        }
      ++col;

  }
}

void MainWindow::keyPressEvent(QKeyEvent *k){
  try{
    ButtonHandle<MainHandler> &tbtt = handler->tool_btts.at((Qt::Key)k->key());
    emit(tbtt.btt->clicked());
  }catch(std::out_of_range &e){
    QMainWindow::keyPressEvent(k);
  }
}

