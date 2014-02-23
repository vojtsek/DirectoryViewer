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
#include <string>
#include <sstream>
#include <fstream>

MainHandler::MainHandler(QObject *parent) :
  QObject(parent), col_count(2), max_lists(4), init_count(2)
{
  int param;
  init_dir = OSInterface::getCWD();
  std::string line, option, d;
  try{
    std::ifstream in("dv.conf");
    while(1){
        getline(in, line);
        std::stringstream ss(line);
        if(line.empty()) break;
        ss >> option;
        if(option != "start_directory")
          ss >> param;
        if(option == "start_directory"){
            ss >> d;
            if(d == "cwd")
              init_dir = OSInterface::getCWD();
            else
              init_dir = d;
          }else if(option == "column_count") col_count = param;
        else if(option == "max_list_count") max_lists = param;
        else if(option == "init_list_count") init_count = param;
      }
  }catch(std::exception e) { std::cout << e.what() << std::endl; }
}

void MainHandler::prepare_cmd(cmd_info_T &cmd_info, bool &is_src, bool &is_dst){
  is_dst = is_src = false;
  std::string file;
  for(auto &a : opened_lists){
      if((a.content->marked) && (!a.content->is_focused)){
          is_dst = true;
          cmd_info.paths.insert(a.content->path);
        }
    }
  for(auto &a : opened_lists){
      if(a.content->is_focused){
          is_src = true;
          cmd_info.src_path = a.content->path;
          file = ((MyTreeView*) a.content)->getSelected();
          cmd_info.source_files.insert(file);
          for(auto &b : cmd_info.paths){
              cmd_info.destination_files[file].insert(b + OSInterface::dir_sep + getBasename(file));
            }
          for(auto src : ((MyTreeView*) a.content)->multi_selection){
              cmd_info.source_files.insert(src);
              for(auto &b : cmd_info.paths){
                  cmd_info.destination_files[src].insert(b + OSInterface::dir_sep + getBasename(src));
                }
            }
        }
    }
}

void MainHandler::copy(){
  cmd_info_T cmd_info;
  bool is_dst, is_src;
  prepare_cmd(cmd_info, is_src, is_dst);
  for(auto &a : cmd_info.paths){
    if(a == cmd_info.src_path){
        std::string err("Destination is the same.");
        emit(error(err));
        return;
      }
    }
  if(is_dst && is_src){
      emit(confirm("Copy",cmd_info));
    }else{
      if(!is_dst){
          std::string err("No selected destinations.");
          emit(error(err));
        }
    }
}

void MainHandler::remove() {
}

void MainHandler::move() {
  cmd_info_T cmd_info;
  bool is_dst, is_src;
  prepare_cmd(cmd_info, is_src, is_dst);
  for(auto &a : cmd_info.paths){
    if(a == cmd_info.src_path){
        std::string err("Destination is the same.");
        emit(error(err));
        return;
      }
    }
  if(is_dst && is_src){
      emit(confirm("Move",cmd_info));
    }else{
      if(!is_dst){
          std::string err("No selected destinations.");
          emit(error(err));
        }
    }
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

