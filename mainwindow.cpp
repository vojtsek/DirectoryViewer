#include "mainwindow.h"
#include "mainhandler.h"
#include "myiconview.h"
#include "ui_mainwindow.h"
#include "osinterface.h"
#include "optiondialog.h"
#include "openedlisthandle.h"
#include "stylesheets.h"

#include <QPushButton>
#include <QShortcut>
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
  QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
  QObject::connect(ui->actionOption, SIGNAL(triggered()), this, SLOT(opt()));
  QShortcut *shortcut = new QShortcut(QKeySequence("F10"), this);
  QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(exit()));
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
      if(info.cmd == info.COPY)
        OSInterface::copy(info);
      else if(info.cmd == info.MOVE)
        OSInterface::move(info);
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
      if(info.cmd == info.REMOVE)
        OSInterface::remove(info);
      else if(info.cmd == info.RENAME)
        OSInterface::rename(info);
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
  ui->centralGridLayout->setHorizontalSpacing(20);
  QObject::connect(handler, &MainHandler::ch_list, this, &MainWindow::refreshMainLayout);
  QObject::connect(handler, &MainHandler::confirm1, this, &MainWindow::confirm1);
  QObject::connect(handler, &MainHandler::confirm2, this, &MainWindow::confirm2);
  QObject::connect(handler, &MainHandler::error, this, &MainWindow::error);

  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F2, ButtonHandle<MainHandler>(Qt::Key_F2, "F2 Add List", &MainHandler::list_added)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F3, ButtonHandle<MainHandler>(Qt::Key_F3, "F3 Remove List", &MainHandler::list_removed)));
  handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F4, ButtonHandle<MainHandler>(Qt::Key_F4, "F4 Create Folder", &MainHandler::create)));
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

  for(unsigned int i = 0; i < init_count; ++i){
      handler->list_added();
    }

  emit(handler->ch_list(false));

  handler->opened_lists[0]->content->setFocus();
}

void MainWindow::updateFocus(){
  for(auto &a : handler->opened_lists){
    a->content->unFocus();
    }
}

void MainWindow::exit(){
    if(QMessageBox::question(nullptr, "Exit?", "Do you wanna quit?", QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) == QMessageBox::No)
        return;
    QApplication::quit();
}

void MainWindow::opt(){
    OptionDialog dial;
    dial.exec();
    for(auto a : handler->opened_lists){
       ui->centralGridLayout->removeItem(a->v_layout);
       a->in_layout = false;
    }
    refreshMainLayout(false);
    for(auto a : handler->opened_lists)
        a->content->rebuild();
}

void MainWindow::handleTab(){
  QWidget *first, *second;
  QWidgetItem *myItem;
  first = second = nullptr;
  for(auto &a : handler->opened_lists){
      if(first == nullptr){
          myItem = dynamic_cast <QWidgetItem*>(a->h_layout2->itemAt(0));
          first = myItem->widget();
          continue;
        }
      myItem = dynamic_cast <QWidgetItem*>(a->h_layout2->itemAt(0));
      second = myItem->widget();
      QWidget::setTabOrder(first, second);
      first = second;
    }
}

void MainWindow::refreshMainLayout(bool removing){
  int row, col;
  unsigned int count;
  row = col = count = 0;
  if(removing){
      auto &a = *(handler->opened_lists.end()-1);
      ui->centralGridLayout->removeItem(a->v_layout);
      handler->opened_lists.pop_back();
      delete a;
      try{
        handler->opened_lists.at(handler->opened_lists.size() - 1)->content->setFocus();
      }catch(std::exception e) {}
  }
  for(auto &a : handler->opened_lists){
      if(!(col % col_count)){
          col = 0;
          ++row;
        }
      ++count;
      if(!a->in_layout){
          a->in_layout = true;
          if(!col && (count == handler->opened_lists.size()))
            // ui->centralGridLayout->addLayout(a.v_layout, row, col, 1, 2);
            ui->centralGridLayout->addLayout(a->v_layout, row, col, 1, 1);
          else
            ui->centralGridLayout->addLayout(a->v_layout, row, col, 1, 1);
        /*  if(handler->opened_lists.size() < handler->col_count)
            a->tb2->setMaximumWidth(ui->centralGridLayout / handler->opened_lists.size());
          else
            a->tb2->setMaximumWidth(ui->centralWidget->width() / handler->col_count);
            */
          a->content->setFocus();
          QObject::connect(a, &OpenedListHandle::updated, this, &MainWindow::handleTab);
          if((a->view_type == a->TREE) || (a->view_type == a->LIST))
            QObject::connect((MyTreeView *)a->content, &MyTreeView::focused, this, &MainWindow::updateFocus);
          else if(a->view_type == a->ICON)
            QObject::connect((MyIconView *)a->content, &MyIconView::tab, this, &MainWindow::handleTab);
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

