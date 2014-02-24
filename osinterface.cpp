#include "osinterface.h"
#include "functions.h"
#include <algorithm>

OSInterface::OSInterface()
{
}


void OSInterface::copy(cmd_info_T &ci){
  for(auto src : ci.source_files){
      for(auto dstf : ci.destination_files[src]){
          try{
            doCopy(src, dstf);
          }catch(OSException *e){

          }
        }
    }
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

void OSInterface::doCopy(std::string &src, std::string &dst){
  std::cout << "Copy " << src << " to: " << dst << std::endl;
}

std::string OSInterface::getPrefix(){ return "/"; }

bool OSInterface::isDir(std::string path){
  DIR *dir;
  if((dir = opendir(path.c_str())) == NULL){
      return false;
   }
  return true;
}

/*bool operator < (const dirEntryT *d1, const dirEntryT *d2){
}
*/

void OSInterface::getDirInfo(std::string path, std::string pattern){
  DIR *dir;
  if((dir = opendir(path.c_str())) == NULL){
      throw new OSException(path, "failed to open dir.");
      return;
    }
  struct dirent *entry = new struct dirent();
  struct stat *finfo = new struct stat();
  std::string abs_path(path), name;
  dirEntryT *de;
  while((entry = readdir(dir))){
      name = entry->d_name;
      if((name == ".") || name == "..") continue;
      if(!matchExpression(name, pattern)) continue;
      de = new dirEntryT();
      lstat((abs_path + dir_sep + name).c_str(), finfo);
      if(finfo == NULL)
        throw new OSException(name, std::string("failed to read file info."));
      de->name = name;
      if(S_ISREG(finfo->st_mode)){
          de->type = de->FILE;
          de->type_name = "FILE";
        }else if(S_ISDIR(finfo->st_mode)){
          de->type = de->DIR;
          de->type_name = "DIR";
        }else if(S_ISLNK(finfo->st_mode)){
          de->type = de->LINK;
          de->type_name = "LINK";
        }else{
          de->type = de->UNKNOWN;
          de->type_name = "UNKNOWN";
        }
      de->byte_size = finfo->st_size;
      dirs.push_back(de);
    }
  closedir(dir);
  std::sort(dirs.begin(), dirs.end(),[=](dirEntryT *d1, dirEntryT *d2){
      if ((d1->type == d1->DIR) && (d2->type != d2->DIR))
        return true;
      else if ((d1->type != d1->DIR) && (d2->type == d2->DIR))
        return false;
      else
        return d1->name < d2->name; });
}

#endif // __unix__
