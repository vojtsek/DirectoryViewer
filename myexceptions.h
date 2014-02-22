#ifndef MYEXCEPTIONS_H
#define MYEXCEPTIONS_H

#include <exception>
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
};

#endif // MYEXCEPTIONS_H
