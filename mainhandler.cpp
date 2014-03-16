#include "mainhandler.h"
#include "osinterface.h"
#include "mydialog.h"
#include "functions.h"
#include "types.h"
#include <iostream>
#include <QStandardItemModel>
#include <QInputDialog>
#include <vector>
#include <set>
#include <map>
#include <QTreeView>
#include <string>
#include <sstream>
#include <fstream>

MainHandler::MainHandler(QObject *parent) :
  QObject(parent)
{
  int param;
  size_in = KB;
  init_dir = OSInterface::getCWD();
  home_path = OSInterface::getCWD();
  std::string line, option, d;
  try{
    std::ifstream in(home_path + OSInterface::dir_sep + "dv.conf");
    while(1){
        getline(in, line);
        std::stringstream ss(line);
        if(line.empty()) break;
        ss >> option;
        if((option != "start_directory") && (option != "home_path")){
          ss >> param;
          if(option == "column_count") col_count = param;
          else if(option == "max_list_count") max_lists = param;
          else if(option == "init_list_count") init_count = param;
          else if(option == "size_in") size_in = param;
        } else{
            ss >> d;
            if(option == "start_directory"){
                if(d == "cwd")
                    init_dir = OSInterface::getCWD();
                else
                    init_dir = d;
            }else if(option == "home_path")
                home_path = d;
        }
      }
  }catch(std::exception e) { std::cout << e.what() << std::endl; }
}

void MainHandler::prepare_cmd(cmd_info_T &cmd_info, bool &is_src, bool &is_dst, bool add_dst, OpenedListHandle * &source){
  is_dst = is_src = false;
  std::string file;
  cmd_info.paths.clear();
  if(add_dst){
      for(auto &a : opened_lists){
          if(((a->content->marked) || opened_lists.size() == 2) && (!a->content->is_focused)){
              is_dst = true;
              cmd_info.paths.insert(a->content->path);
            }
        }
    }
  for(auto &a : opened_lists){
      if(a->content->is_focused){
          cmd_info.src_path = a->content->path;
          for(auto src :  a->content->multi_selection){
              is_src = true;
              source = a;
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
  cmd_info.cmd = cmd_info.COPY;
  bool is_dst, is_src;
  OpenedListHandle *src;
  prepare_cmd(cmd_info, is_src, is_dst, true, src);
  for(auto &a : cmd_info.paths){
    if(a == cmd_info.src_path){
        std::string err("Destination is the same.");
        emit(error(err));
        return;
      }
    }
  if(is_dst && is_src){
      emit(confirm1("Copy",cmd_info));
      src->content->multi_selection.clear();
      src->content->rebuild();
      for(auto &a : opened_lists)
        if((a->content->marked) || opened_lists.size() == 2) a->content->rebuild();
    }else{
      if(!is_dst){
          std::string err("No selected destinations.");
          emit(error(err));
        }
      if(!is_src){
          std::string err("No selected source files.");
          emit(error(err));
        }
    }
}

void MainHandler::remove() {
  cmd_info_T cmd_info;
  cmd_info.cmd = cmd_info.REMOVE;
  bool is_dst, is_src;
  OpenedListHandle *src;
  prepare_cmd(cmd_info, is_src, is_dst, false, src);
  if(is_src){
      emit(confirm2("Remove",cmd_info));
      src->content->multi_selection.clear();
      src->content->rebuild();
    }else{
      std::string err("No source file selected.");
      emit(error(err));
    }
}

void MainHandler::move() {
  cmd_info_T cmd_info;
  cmd_info.cmd = cmd_info.MOVE;
  bool is_dst, is_src;
  OpenedListHandle *src;
  prepare_cmd(cmd_info, is_src, is_dst, true, src);
  for(auto &a : cmd_info.paths){
    if(a == cmd_info.src_path){
        std::string err("Destination is the same.");
        emit(error(err));
        return;
      }
    }
  if(is_dst && is_src){
      emit(confirm1("Move",cmd_info));
      src->content->multi_selection.clear();
      src->content->rebuild();
      for(auto &a : opened_lists)
        if((a->content->marked) || opened_lists.size() == 2) a->content->rebuild();
    }else{
      if(!is_dst){
          std::string err("No selected destinations.");
          emit(error(err));
        }
      if(!is_src){
          std::string err("No selected source files.");
          emit(error(err));
        }
    }
}

void MainHandler::rename() {
  cmd_info_T cmd_info;
  bool is_dst, is_src;
  OpenedListHandle *src;
  prepare_cmd(cmd_info, is_src, is_dst, false, src);
  if(is_src){
      for(auto &a : cmd_info.source_files){
          std::string file = a;
          QString lbl("Enter new name:");
          QString txt("new name for \n");
          txt.append(QString::fromStdString(getBasename(file)));
          std::string str = cmd_info.src_path;
          str.push_back(OSInterface::dir_sep);
          std::string name = QInputDialog::getText(nullptr, lbl, txt).toStdString();
          str.append(name);
          if(name.empty()){
              std::string err("Empty names not allowed.");
              emit(error(err));
              break;
            }
          cmd_info.destination_files[a].insert(str);
        }
      OSInterface::rename(cmd_info);
      src->content->multi_selection.clear();
      src->content->rebuild();
    }else{
      std::string err("No selected source files.");
      emit(error(err));
    }
}

void MainHandler::create() {
    char inval = 0;
    for(auto &a : opened_lists){
        if(a->content->is_focused){
            QString lbl("Enter new name:");
            QString txt("Name of new folder:\n");
            std::string name = QInputDialog::getText(nullptr, lbl, txt).toStdString();
            if(name.empty()){
                std::string err("Empty names not allowed.");
                emit(error(err));
                break;
              }
            if((inval = isValidFn(name)) != 0){
                std::string err("Invalid character '");
                err.push_back(inval);
                err.push_back('\'');
                emit(error(err));
                break;
              }
            repairPath(a->content->path);
            try{
                OSInterface::create(a->content->path + name);
            }catch(OSException *e) {e->process();}
            a->content->rebuild();
        }
    }
}

void MainHandler::list_added(){
  if(opened_lists.size() < max_lists){
    opened_lists.push_back(new OpenedListHandle(init_dir, size_in));
    emit(ch_list(false));
  }
}

void MainHandler::list_removed(){
  if(opened_lists.size())
    emit(ch_list(true));
}

