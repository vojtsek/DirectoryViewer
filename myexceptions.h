#ifndef MYEXCEPTIONS_H
#define MYEXCEPTIONS_H

#include <exception>
#include <QObject>
#include <string>

class MyException
{
private:
  std::string err;
public:
  MyException(std::string w, std::string e){
    err = "\"" + w + "\": " + e;
  }
  virtual const char *what();
  void process();
  virtual ~MyException() {}
};

#endif // MYEXCEPTIONS_H
