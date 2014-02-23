#include "mainhandler.h"
#include "osinterface.h"
#include "functions.h"
#include "types.h"
#include <iostream>
#include <QStandardItemModel>
#include <vector>
#include <set>
#include <map>
#include <QTreeView>

MainHandler::MainHandler(QObject *parent) :
  QObject(parent), col_count(2), max_lists(4)
{
 // opened_lists.resize(4);
  opened_lists.clear();
}

void MainHandler::copy(){
  bool is_src, is_dst;
  is_dst = is_src = false;
  std::string file;
  cmd_info_T cmd_info;
  for(auto &a : opened_lists){
      if((a.content->marked) && (!a.content->is_focused)){
          is_dst = true;
          cmd_info.paths.insert(a.content->path + OSInterface::dir_sep);
        }
    }
  for(auto &a : opened_lists){
      if(a.content->is_focused){
          is_src = true;
          file = ((MyTreeView*) a.content)->getSelected();
          cmd_info.source_files.insert(file);
          for(auto &b : cmd_info.paths){
              cmd_info.destination_files[file].insert(b + getBasename(file));
            }
          for(auto src : ((MyTreeView*) a.content)->multi_selection){
              cmd_info.source_files.insert(src);
              for(auto &b : cmd_info.paths){
                  cmd_info.destination_files[src].insert(b + getBasename(src));
                }
            }
        }
    }
  if(is_dst && is_src)
    OSInterface::copy(cmd_info);
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

