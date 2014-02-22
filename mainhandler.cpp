#include "mainhandler.h"
#include "osinterface.h"
#include <iostream>
#include <QStandardItemModel>
#include <QTreeView>

MainHandler::MainHandler(QObject *parent) :
  QObject(parent), col_count(2), max_lists(4)
{
}

void MainHandler::copy(){
  for(auto &a : opened_lists){
      if(a.content->is_focused){
          auto it = ((MyTreeView*) a.content)->getSelected();
          std::cout << it << std::endl;
        }
    }
}

void MainHandler::remove() {
}

void MainHandler::move() {

}

void MainHandler::rename() {

}

void MainHandler::view() {

}

void MainHandler::list_added(){
  if(opened_lists.size() < max_lists){
    opened_lists.emplace_back(OSInterface::getCWD());
    emit(ch_list(false));
  }
}

void MainHandler::list_removed(){
  if(opened_lists.size())
    emit(ch_list(true));
}

