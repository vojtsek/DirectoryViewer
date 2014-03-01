#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <string>

class MyDialog
{
public:
  MyDialog();
  static void MsgBox(std::string &);
  static std::string Prompt(std::string &);
};

#endif // MYDIALOG_H
