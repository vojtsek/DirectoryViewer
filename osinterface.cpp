#include "osinterface.h"
#include "functions.h"
OSInterface::OSInterface()
{
}

#ifdef __unix__

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

std::string OSInterface::getPrefix(){ return "/"; }

bool OSInterface::isDir(std::string path){
  DIR *dir;
  if((dir = opendir(path.c_str())) == NULL){
      return false;
   }
  return true;
}

void OSInterface::copy(cmd_info_T &ci){
  for(auto &src : ci.source_files){
      for(auto &a : ci.destination_files){
          for(auto &dstf : a){
              std::cout << "Copy " << src << " to: " << dstf << std::endl;
            }
        }
    }
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
  std::string abs_path(path), name;
  std::map<std::string,struct dirent*> entries;
  dirEntryT *de;
  while((entry = readdir(dir))){
      name = entry->d_name;
      if(!matchExpression(name, pattern)) continue;
      de = new dirEntryT();
      stat((abs_path + "/" + name).c_str(), finfo);
      if(finfo == NULL)
        throw new OSException(name, std::string("failed to read file info."));
      de->name = name;
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

#endif // __unix__
