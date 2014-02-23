#ifndef OSINTERFACE_H
#define OSINTERFACE_H

#include "myexceptions.h"
#include "types.h"
#include <map>
#include <exception>
#include <string>

typedef struct {
  std::string name, type_name;
  int type;
  enum {FILE, DIR, UNKNOWN};
  long byte_size;
} dirEntryT;

class OSInterface
{
public:
  std::map<std::string, dirEntryT*> dirs;
#ifdef __unix__
  static const char dir_sep = '/';
#endif
  OSInterface();
  void getDirInfo(std::string, std::string);
  static void copy(cmd_info_T &);
  static void doCopy(std::string &, std::string &);
  static std::string getCWD();
  static std::string getPrefix();
  static bool isDir(std::string);
};

class OSException: public MyException{
public:
  OSException(std::string w, std::string e): MyException(w, e) {}
};

#endif // OSINTERFACE_H
