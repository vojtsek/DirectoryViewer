#include "osinterface.h"
#include "functions.h"
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>


std::string OSInterface::getCWD(){
  char buf[255];
  getcwd(buf, 255);
  return std::string(buf);
}

bool OSInterface::isDir(std::string path){
  DIR *dir;
  if((dir = opendir(path.c_str())) == NULL){
      return false;
   }
  return true;
}

void OSInterface::getDirInfo(std::string path, std::string pattern){
  DIR *dir;
  if(path == "/mnt") return;
  if(path == "/proc") return;
  if(path == "/run") return;
  if(path == "/root") return;
  if(path == "/sys") return;
  if((dir = opendir(path.c_str())) == NULL){
      throw new OSException(path, "failed to open dir.");
      return;
    }
  struct dirent *entry = new struct dirent();
  struct stat *finfo = new struct stat();
  std::string abs_path(path);
  std::map<std::string,struct dirent*> entries;
  dirEntryT *de;
  while((entry = readdir(dir))){
      if(!matchExpression(std::string(entry->d_name), pattern)) continue;
      de = new dirEntryT();
      stat((abs_path + "/" + std::string(entry->d_name)).c_str(), finfo);
      if(finfo == NULL)
        throw new OSException(std::string(entry->d_name), std::string("failed to read file info."));
      de->name = std::string(entry->d_name);
      if(S_ISLNK(finfo->st_mode)){
          continue;
      }else if(S_ISREG(finfo->st_mode)){
        de->type = de->FILE;
        de->type_name = "FILE";
      }else if(S_ISDIR(finfo->st_mode)){
        de->type = de->DIR;
        de->type_name = "DIR";
      }else{
          continue;
        de->type = de->UNKNOWN;
        de->type_name = "UNKNOWN";
        }
      de->byte_size = finfo->st_size;
      dirs.insert(std::pair<std::string, dirEntryT*>(de->name, de));
    }
  closedir(dir);
}

