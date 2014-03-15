#include "functions.h"
#include "osinterface.h"
#include <string>

void repairPath(std::string &path){
    if(path[path.size() - 1] != OSInterface::dir_sep)
        path.push_back(OSInterface::dir_sep);
}

bool matchExpression(std::string &expr, std::string &pattern){

  int pos = pattern.find('*');
  for(int i = 0; i < pos; ++i){
      if(expr[i] != pattern[i])
        return false;
    }

  for(unsigned int i = 0; i < pattern.size() - pos - 1; ++i){
      if(expr[expr.size() - 1 - i] != pattern[pattern.size() - 1 - i])
        return false;
    }
  return true;
}

std::string getBasename(std::string &path){
  int pos;
  if(path.empty()) return "";
  if((pos = path.find_last_of(OSInterface::dir_sep)) == -1) return "";
  return path.substr(pos + 1, path.size());
}

std::string getPath(std::string &path){
  int pos;
  if(path.empty()) return "";
  if((pos = path.find_last_of(OSInterface::dir_sep)) == -1) return "";
  return path.substr(0, pos);
}

std::string getExtension(std::string &path){
  if(path.empty()) return "";
  std::string fn = getBasename(path);
  int pos;
  if((pos = path.find_last_of('.')) == -1) return "";
  return path.substr(pos + 1, path.size());
}

bool isArch(std::string &path){
    std::string ext(getExtension(path));
  return ((ext == "gz") || (ext == "bz2") || (ext == "zip") || (ext == "rar"));
}

bool isImg(std::string &path){
    std::string ext(getExtension(path));
  return ((ext == "png") || (ext == "jpg") || (ext == "gif"));
}

bool isKnown(std::string &path){
    std::string ext(getExtension(path));
  return ((ext == "pdf") || (ext == "avi") || (ext == "doc") || (ext == "ppt"));
}

char isValidFn(std::string &path){
    std::string forbidden(" <>|\:()&;#?*");
    forbidden.push_back(OSInterface::dir_sep);
    for(char &a : forbidden){
        if(path.find(a) != std::string::npos)
            return a;
    }
    return 0;
}
