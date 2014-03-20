#ifndef OSINTERFACE_H
#define OSINTERFACE_H

#include "myexceptions.h"
#include "types.h"
#include <map>
#include <exception>
#include <string>
#include <QDate>

extern std::map<std::string, std::string> extern_programmes;

typedef struct {
  std::string name, type_name, ext_name, mod_time, perms;
  int type;
  enum {FILE, DIR, UNKNOWN, LINK, ARCHIVE, EXE};
  long long byte_size;
  long long show_size;
} dirEntryT;

class OSInterface
{
public:
  std::vector<dirEntryT*> dirs;
#ifdef __unix__
  static const char dir_sep = '/';
#endif

#ifdef __WIN32__
  static const char dir_sep = '\\';
#endif
  OSInterface();
  void getDirInfo(std::string, std::string);
  static void copy(cmd_info_T &);
  static void move(cmd_info_T &);
  static void remove(cmd_info_T &);
  static void create(std::string);
  static void rename(cmd_info_T &);
  static void doCopy(std::string &, std::string &);
  static void doMove(std::string &, std::string &);
  static void doRemove(std::string &);
  static void doRename(std::string &, std::string &);
  static void openFile(std::string &);
  static size_t computeDirSize(std::string);
//  static bool exists(std::string &);
  static std::string getCWD();
  static std::string getPrefix();
  static bool isDir(std::string);
  static size_t getSize(std::string);
  static bool isOpenable(std::string);
};

class OSException: public MyException{
public:
  OSException(std::string w, std::string e): MyException(w, e) {}
};

#endif // OSINTERFACE_H
